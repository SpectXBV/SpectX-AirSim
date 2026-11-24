# AirSide v2.3.0

Release date: 2026-11-24
Platform: Linux x86_64
Engine: Unreal Engine 5.8.2
Configuration: Shipping

## New features

- Configurable stochastic wind model.
- Reference wind speed and NED direction.
- Smooth stochastic gust transitions.
- Random wind direction changes.
- Altitude-dependent power-law wind shear.
- Kaimal spectral turbulence with standalone inverse FFT.
- Deterministic random seeds.
- Cylinder-based aerodynamic forces.
- Attitude-dependent projected aerodynamic areas.
- Configurable center-of-pressure torque.
- Optional vertical drag and maximum force limiting.
- Backward compatibility with standard AirSim wind.

## Documentation

- Detailed wind-model-guide.md.
- Configuration parameters and JSON examples.
- Mathematical equations and model limitations.
- Five numerical validation plots.

## Validation

- Standalone wind model tests passed.
- All seven aerodynamic test groups passed.
- Target turbulence standard deviation: 1.0 m/s.
- Measured turbulence standard deviation: 0.996482 m/s.
- Fixed-seed reproducibility verified.
- AirLib rebuilt using Unreal Clang 20.1.8.
- Unreal Editor compilation and linking passed.
- Linux Shipping packaging completed successfully.

## Default release configuration

The supplied settings.json enables constant 5 m/s eastward
wind with gusts, turbulence and shear disabled.

See wind-model-guide.md for complete stochastic configurations.

## Known limitations

Aerodynamic coefficients and equivalent geometry require
vehicle-specific calibration.

Packaged application started and reached PX4 connection initialization.
Integrated ROS2/PX4 flight validation remains pending.
