import sys
import time
from datetime import datetime, timezone

sys.path.insert(
    0,
    r"C:\SpectX\SpectX-AirSim\PythonClient"
)

import cosysairsim


TURBINE = "WTG_03"
TICKS_PER_SECOND = 10_000_000
EPOCH = datetime(1970, 1, 1, tzinfo=timezone.utc)


def to_ticks(value):
    dt = datetime.strptime(
        value,
        "%Y-%m-%d %H:%M"
    ).replace(tzinfo=timezone.utc)

    delta = dt - EPOCH

    seconds = (
        delta.days * 86400
        + delta.seconds
    )

    return (
        seconds * TICKS_PER_SECOND
        + delta.microseconds * 10
    )


def normalize_dict(d):
    result = {}

    for k, v in d.items():
        if isinstance(k, bytes):
            k = k.decode("utf-8")

        result[k] = list(v)

    return result


def orientation(client):
    result = normalize_dict(
        client.simGetTurbineOrientations(
            TURBINE
        )
    )

    if "rotor_local" not in result:
        raise RuntimeError(
            "rotor_local missing. Keys: "
            + str(sorted(result.keys()))
        )

    return result


def angular_difference(a, b):
    return abs(
        ((b - a + 180.0) % 360.0)
        - 180.0
    )


def rotation_difference(a, b):
    return max(
        angular_difference(x, y)
        for x, y in zip(a, b)
    )


def measure_motion(client, seconds=0.70):
    before = orientation(client)["rotor_local"]

    time.sleep(seconds)

    after = orientation(client)["rotor_local"]

    delta = rotation_difference(
        before,
        after
    )

    return before, after, delta


def set_sim_time(client, value):
    ticks = to_ticks(value)

    client.simSetTimeOfDay(
        True,
        ticks,
        1.0,
        0.1,
        True
    )

    time.sleep(0.25)

    actual = client.simGetTimeOfDay()

    print(
        f"TimeOfDay {value}: "
        f"requested={ticks}, "
        f"actual={actual}"
    )


client = cosysairsim.MultirotorClient()
client.confirmConnection()

print("\n=== ORIENTATION API ===")

data = orientation(client)

for key in sorted(data.keys()):
    print(f"{key:20s} {data[key]}")

expected = {
    "actor_world",
    "tower_local",
    "tower_world",
    "nacelle_local",
    "nacelle_world",
    "rotor_local",
    "rotor_world",
    "hub_local",
    "hub_world",
    "ring_local",
    "ring_world",
    "blade_1_local",
    "blade_1_world",
    "blade_2_local",
    "blade_2_world",
    "blade_3_local",
    "blade_3_world",
}

missing = expected - set(data.keys())

if missing:
    raise RuntimeError(
        f"Missing orientation keys: {sorted(missing)}"
    )

print("\nOrientation API: PASS")


print("\n=== VALIDATION TESTS ===")

bad_turbine = client.simSetTurbineDowntime(
    "WTG_99",
    "2026-10-20 13:30",
    "2026-10-23 09:00"
)

bad_date = client.simSetTurbineDowntime(
    TURBINE,
    "2026-99-20 13:30",
    "2026-10-23 09:00"
)

bad_order = client.simSetTurbineDowntime(
    TURBINE,
    "2026-10-23 09:00",
    "2026-10-20 13:30"
)

print("Invalid turbine:", bad_turbine)
print("Invalid date:   ", bad_date)
print("Invalid order:  ", bad_order)

assert bad_turbine is False
assert bad_date is False
assert bad_order is False

print("Validation: PASS")


print("\n=== MULTI-DAY DOWNTIME ===")

accepted = client.simSetTurbineDowntime(
    TURBINE,
    "2026-10-20 13:30",
    "2026-10-23 09:00"
)

print("Schedule accepted:", accepted)

assert accepted is True


print("\n--- BEFORE START: should rotate ---")

set_sim_time(
    client,
    "2026-10-20 13:29"
)

a, b, delta = measure_motion(client)

print("before:", a)
print("after: ", b)
print("delta: ", delta)

assert delta > 5.0, (
    "Rotor did not move before downtime"
)

print("Before-start rotation: PASS")


print("\n--- EXACT START: should stop ---")

set_sim_time(
    client,
    "2026-10-20 13:30"
)

a, b, delta = measure_motion(client)

print("before:", a)
print("after: ", b)
print("delta: ", delta)

assert delta < 0.1, (
    "Rotor moved at downtime start"
)

print("Start-inclusive stop: PASS")


print("\n--- INSIDE WINDOW: should remain stopped ---")

set_sim_time(
    client,
    "2026-10-22 12:00"
)

a, b, delta = measure_motion(client)

print("before:", a)
print("after: ", b)
print("delta: ", delta)

assert delta < 0.1, (
    "Rotor moved inside downtime"
)

print("Multi-day stop: PASS")


print("\n--- EXACT END: should resume ---")

set_sim_time(
    client,
    "2026-10-23 09:00"
)

a, b, delta = measure_motion(client)

print("before:", a)
print("after: ", b)
print("delta: ", delta)

assert delta > 5.0, (
    "Rotor did not resume at downtime end"
)

print("End-exclusive resume: PASS")


print("\n==============================")
print("ALL TURBINE API TESTS PASSED")
print("==============================")
