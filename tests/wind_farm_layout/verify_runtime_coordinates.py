from pathlib import Path
import cosysairsim
import csv
import math

client = cosysairsim.MultirotorClient()
client.confirmConnection()

with open(Path(__file__).resolve().parents[2] / "docs/wind_farm_layout/turbine_coordinates.csv") as f:
    expected = {
        int(r["id"]): (float(r["x"])/100, float(r["y"])/100)
        for r in csv.DictReader(f)
    }

def decode(s):
    return s.decode() if isinstance(s, bytes) else str(s)

objects = client.simListSceneObjectsTags("Turbine.*")
turbines = {}

for name, tag in objects:
    name, tag = decode(name), decode(tag)

    if tag.startswith("Turbine") and tag[7:].isdigit():
        n = int(tag[7:])
        assert n not in turbines, f"Duplicate turbine: {n}"
        turbines[n] = name

print("Detected turbines:", len(turbines))
assert set(turbines) == set(range(1, 37))

positions = {}

for n, name in turbines.items():
    p = client.simGetObjectPose(name, True).position
    positions[n] = (p.x_val, p.y_val, p.z_val)

    assert all(math.isfinite(v) for v in positions[n])

origin = positions[4]
errors = []

print("WTG-04 global NED:", origin)
print()
print("ID      North(m)     East(m)    Error(m)")

for n in sorted(positions):
    p = positions[n]

    x = p[0] - origin[0]
    y = p[1] - origin[1]

    ex, ey = expected[n]
    error = math.hypot(x-ex, y-ey)
    errors.append(error)

    print(f"WTG-{n:02d} {x:12.3f} {y:11.3f} {error:11.6f}")

print()
print("Maximum XY error:", max(errors), "m")
print("Mean XY error:", sum(errors)/len(errors), "m")

if max(errors) < 0.05:
    print("RUNTIME COORDINATE VERIFICATION PASSED")
else:
    print("RUNTIME COORDINATES REQUIRE INVESTIGATION")
