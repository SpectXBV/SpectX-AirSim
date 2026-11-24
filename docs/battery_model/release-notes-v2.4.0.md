# AirSide v2.4.0

## Calibrated Battery Model and Stochastic Wind

### New Features

- Configurable calibrated battery energy consumption.
- Dynamic battery voltage, current and state of charge.
- Configurable internal battery resistance.
- Configurable initial state of charge.
- Optional residual climb power correction.
- Simplified CC-CV charging.
- Backward-compatible legacy battery model.

### Reference Battery Configuration

- Battery: 6S6P Li-ion.
- Rated capacity: 30 Ah / 648 Wh.
- Nominal voltage: 21.6 V.
- Maximum voltage: 25.2 V.
- Recommended charging rate: 0.5C / 15 A.
- Operational SOC reserve: 15%.
- Reference electrical demand: approximately 1182 W.
- Target operational endurance: approximately 28 minutes.

### Retained Features

- Stochastic wind model.
- Wind shear and stochastic gusts.
- Kaimal turbulence.
- Drone aerodynamic wind forces and torques.
- Vessel following and docking.
- Existing PX4 integration and runtime APIs.

### Build and Validation

- Unreal Engine 5.8.2.
- Linux x86_64 Shipping configuration.
- Clang 20.1.8 and libc++.
- 26 battery numerical checks passed.
- Stochastic wind regression results retained.
- All 34 wind aerodynamic checks passed.
- Combined Development game build successful.
- Shipping game packaging successful.

### Documentation

- Updated README with battery model equations.
- Complete battery JSON configuration.
- Reference battery discharge graph.
- Charging characteristics and model limitations.

### Important Notes

The calibrated battery model is opt-in through settings.json.

The 28-minute endurance is a conservative simulation
design target, not measured real-world aircraft endurance.

Full closed-loop flight and charging validation remain
necessary.

### Packaged Game Runtime Validation

- Linux Shipping executable initialized successfully.
- MAVLink TCP listeners initialized for all three drones
  on ports 4560, 4561 and 4562.
- Game process returned exit code 0.
- Distributed settings enable the calibrated 648 Wh
  battery model for all three drones.
- PX4 connection, closed-loop flight, full endurance
  and docking/charging validation remain outstanding.
