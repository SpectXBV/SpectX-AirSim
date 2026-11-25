import cosysairsim

client = cosysairsim.MultirotorClient()
client.confirmConnection()

passed = []
failed = []

for i in range(1, 37):
    name = f"WTG_{i:02d}"

    try:
        result = client.simGetTurbineOrientations(name)

        keys = [
            k.decode() if isinstance(k, bytes) else k
            for k in result.keys()
        ]

        if "rotor_local" not in keys:
            raise RuntimeError(f"Missing rotor_local: {keys}")

        print(f"{name}: PASS")
        passed.append(name)

    except Exception as e:
        print(f"{name}: FAIL - {e}")
        failed.append(name)

print("\n========== SUMMARY ==========")
print(f"Passed: {len(passed)}/36")
print(f"Failed: {len(failed)}/36")
print(f"Failed IDs: {failed}")
