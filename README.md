# Cosys-AirSim

Cosys-AirSim is a simulator for drones, cars and more, with extensive API support, built on [Unreal Engine](https://www.unrealengine.com/). It is open-source, cross platform, and supports hardware-in-loop with popular flight controllers such as PX4 for physically and visually realistic simulations. It is developed as an Unreal plugin that can simply be dropped into any Unreal environment. 

This fork is based on last public AirSim release from Microsoft's GitHub.
Cosys-Lab made extensive modifications to the AirSim platform to support multiple projects and research goals. 
Please contact a Cosys-Lab researcher to get more in depth information on our work or if you wish to collaborate. 
The [original AirSim MIT license](https://github.com/Cosys-Lab/Cosys-AirSim/blob/main/LICENSE) applies to all native AirSim source files. 
Please note that we use that same [MIT license](https://github.com/Cosys-Lab/Cosys-AirSim/blob/main/LICENSE) as which applies to all changes made by Cosys-Lab in case you plan to do anything within this repository.
Do note that this repository is provided as is, will not be actively updated and comes without warranty or support. 
Please contact a Cosys-Lab researcher to get more in depth information on which branch or version is best for your work.

This [main branch](https://github.com/Cosys-Lab/Cosys-AirSim/tree/main) is for the latest supported Unreal Version v5.8, maintained for support, and is available for builds in the [releases](https://github.com/Cosys-Lab/Cosys-AirSim/releases).
Unreal [5.2.1](https://github.com/Cosys-Lab/Cosys-AirSim/tree/5.2.1) is also available for long term support.

## Associated publications

- [Cosys-AirSim: A Real-Time Simulation Framework Expanded for Complex Industrial Applications](https://arxiv.org/abs/2303.13381)
```
@inproceedings{cosysairsim2023jansen,
  author={Jansen, Wouter and Verreycken, Erik and Schenck, Anthony and Blanquart, Jean-Edouard and Verhulst, Connor and Huebel, Nico and Steckel, Jan},
  booktitle={2023 Annual Modeling and Simulation Conference (ANNSIM)}, 
  title={COSYS-AIRSIM: A Real-Time Simulation Framework Expanded for Complex Industrial Applications}, 
  year={2023},
  doi={https://doi.org/10.48550/arXiv.2303.13381}}
```

You can also find the presentation of the live tutorial of Cosys-AirSim at ANNSIM '23 conference [here](https://github.com/Cosys-Lab/Cosys-AirSim/tree/main/docs/annsim23_tutorial) together with the associated videos.


- [Physical LiDAR Simulation in Real-Time Engine](https://arxiv.org/abs/2208.10295)
```
@inproceedings{lidarsim2022jansen,
  author={Jansen, Wouter and Huebel, Nico and Steckel, Jan},
  booktitle={2022 IEEE Sensors}, 
  title={Physical LiDAR Simulation in Real-Time Engine}, 
  year={2022},
  pages={1-4},
  doi={10.1109/SENSORS52175.2022.9967197}}
}
```
- [Simulation of Pulse-Echo Radar for Vehicle Control and SLAM](https://www.mdpi.com/1424-8220/21/2/523)
```
@Article{echosim2021schouten,
  author={Schouten, Girmi and Jansen, Wouter and Steckel, Jan},
  title={Simulation of Pulse-Echo Radar for Vehicle Control and SLAM},
  JOURNAL={Sensors},
  volume={21},
  year={2021},
  number={2},
  article-number={523},
  doi={10.3390/s21020523}
}
```

## Cosys-Lab Modifications
* Updated for Unreal 5.
* Added [multi-layer annotation](https://cosys-lab.github.io/Cosys-AirSim/annotation) for groundtruth label generation with RGB, greyscale and texture options. Extensive API integration and available for camera and GPU-LiDAR sensors.
* Added [Instance Segmentation](https://cosys-lab.github.io/Cosys-AirSim/instance_segmentation). 
* Added [Echo sensor type](https://cosys-lab.github.io/Cosys-AirSim/echo) for simulation of sensors like sonar and radar.
* Added [GPU LIDAR sensor type](https://cosys-lab.github.io/Cosys-AirSim/gpulidar): Uses GPU acceleration to simulate a LiDAR sensor. Can support much higher point density then normal LiDAR and behaves more authentic and has realistic intensity generation.
* Added [skid steering SimMode and vehicle type](https://cosys-lab.github.io/Cosys-AirSim/skid_steer_vehicle). ClearPath Husky and Pioneer P3DX implemented as vehicle types using this new vehicle model. 
* Added [Matlab API Client](https://cosys-lab.github.io/Cosys-AirSim/matlab) implementation as an easy to install Matlab toolbox.
* Added various [random but deterministic dynamic object types and world configuration options](https://cosys-lab.github.io/Cosys-AirSim/dynamic_objects).
* Added [Artificial Lights](https://cosys-lab.github.io/Cosys-AirSim/lights). 
* Added BoxCar vehicle model to the Car SimMode to have a smaller vehicle to use in indoor spaces.
* Added a new image type called [Lighting](https://cosys-lab.github.io/Cosys-AirSim/image_apis) which only shows the light information and no materials.
* Updated [ComputerVision mode](https://cosys-lab.github.io/Cosys-AirSim/image_apis#computer-vision-mode-1): Now has full API and Simulation just like other vehicle types. It mostly means it can now have sensors attached (outside of IMU). Improved handling and camera operation.
* Updated [LIDAR sensor type](https://cosys-lab.github.io/Cosys-AirSim/lidar): Fixed not tracing correctly, added ground truth (point labels) generation, added range-noise generation. Improved API pointcloud delivery to be full scan instead of being frame-rate dependent and partial.
* Updated the camera, Echo and (GPU-)LiDAR sensors to be uncoupled from the vehicle and be placed as external world sensors.
* Updated sensors like cameras, Echo sensor and GPU-LiDAR to ignore certain objects with the _MarkedIgnore_ Unreal tag and enabling the "IgnoreMarked" setting in [the settings file](https://cosys-lab.github.io/Cosys-AirSim/settings).
* Updated cameras sensor with more distortion features such as chromatic aberration, motion blur and lens distortion. 
* Updated Python [ROS implementation](https://cosys-lab.github.io/Cosys-AirSim/ros_python) with completely new implementation and feature set.
* Updated C++ [ROS2 implementation](https://cosys-lab.github.io/Cosys-AirSim/ros2) to support custom Cosys-AirSim features.
* Dropped support for Unity Environments.

Some more details on our changes can be found in the [changelog](https://github.com/Cosys-Lab/Cosys-AirSim/blob/main/CHANGELOG.md).

## How to Get It
#### Download and run from packaged binary - Windows/Linux
* [Download and run it](https://cosys-lab.github.io/Cosys-AirSim/run_packaged)
#### Download and install from precompiled plugin - Windows/Linux
* [Download and install it](https://cosys-lab.github.io/Cosys-AirSim/install_precompiled)
#### Install and use from source - Windows
* [Install/Build it](https://cosys-lab.github.io/Cosys-AirSim/install_windows)
#### Install and use from source - Linux
* [Install/Build it](https://cosys-lab.github.io/Cosys-AirSim/install_linux)

## How to Use It

### Documentation

View our [detailed documentation](https://cosys-lab.github.io/Cosys-AirSim/) on all aspects of Cosys-AirSim.

## Original AirSim Paper

More technical details are available in [AirSim paper (FSR 2017 Conference)](https://arxiv.org/abs/1705.05065). Please cite this as:
```
@inproceedings{airsim2017fsr,
  author = {Shital Shah and Debadeepta Dey and Chris Lovett and Ashish Kapoor},
  title = {AirSim: High-Fidelity Visual and Physical Simulation for Autonomous Vehicles},
  year = {2017},
  booktitle = {Field and Service Robotics},
  eprint = {arXiv:1705.05065},
  url = {https://arxiv.org/abs/1705.05065}
}
```

## License

This project is released under the MIT License. Please review the [License file](https://github.com/Cosys-Lab/Cosys-AirSim/blob/main/LICENSE) for more details.

<!-- AIRSIDE_WTG_API_START -->

## AirSide runtime APIs

AirSide extends the Python client with simulator TimeOfDay control and
wind-turbine runtime APIs.

### Connect

```python
import cosysairsim

client = cosysairsim.MultirotorClient()
client.confirmConnection()
```

### Simulator TimeOfDay

TimeOfDay uses Unix epoch time in 100-nanosecond ticks.

There are `10,000,000` ticks per second.

Set the simulator time:

```python
from datetime import datetime, timezone

dt = datetime(2026, 10, 19, 13, 30, tzinfo=timezone.utc)
ticks = int(dt.timestamp() * 10_000_000)

client.simSetTimeOfDay(
    True,
    ticks,
    celestial_clock_speed=1,
    update_interval_secs=60,
    move_sun=True
)
```

Read the current simulator time:

```python
ticks = client.simGetTimeOfDay()

if ticks:
    dt = datetime.fromtimestamp(
        ticks / 10_000_000,
        tz=timezone.utc
    )
    print(dt)
```

Disable TimeOfDay:

```python
client.simSetTimeOfDay(False)
```

`simGetTimeOfDay()` returns `0` when TimeOfDay is disabled.

> Note: simulator API timestamps are Unix/UTC based. Unreal's on-screen
> clock may display local time depending on the host configuration.

### Wind turbine IDs

The Offshore Wind Farm map exposes 35 turbines:

```text
WTG_01 ... WTG_35
```

### Schedule turbine downtime

```python
ok = client.simSetTurbineDowntime(
    "WTG_04",
    "2026-10-20 13:30",
    "2026-10-23 09:00"
)

print(ok)
```

Datetime format:

```text
YYYY-MM-DD HH:MM
```

Downtime semantics are:

```text
start <= simulator_time < end
```

The turbine stops at the start time and resumes exactly at the end time.

Same-day, overnight, and multi-day downtime intervals are supported.

Downtime scheduling uses the simulator TimeOfDay clock. In the current
implementation, the supplied calendar values correspond to UTC/Unix
simulator time; Unreal's displayed clock may appear in local time.

### Read turbine orientations

```python
orientations = client.simGetTurbineOrientations("WTG_04")

for name, value in orientations.items():
    print(name, value)
```

The returned dictionary includes orientation values for turbine
components such as:

```text
actor
tower
nacelle
rotor
hub
ring
blade_1
blade_2
blade_3
```

Both local and world orientations are available.

Orientation vectors use:

```text
[roll, pitch, yaw]
```

with values expressed in degrees.

Euler-angle representations can wrap or flip while describing the same
physical orientation, so component-wise Euler differences should not be
used as the only test for physical rotation.

<!-- AIRSIDE_WTG_API_END -->

## Vessel motion API

AirSide exposes runtime vessel control through the Python RPC client. Vessel latitude, longitude, altitude, and velocity are reported from the center of the vessel charging pad / helideck.

`simSetVesselTarget(latitude, longitude, speed_mps)` commands a geographic target and desired horizontal speed. It returns `True` when the target and route are accepted.

`simGetVesselState()` returns the current vessel position, velocity, speed, heading, target, distance to target, route state, and arrival state.

Example: move the vessel 100 m north at 3 m/s:

```python
import math
import time
import cosysairsim

client = cosysairsim.MultirotorClient()
client.confirmConnection()

state = client.simGetVesselState()

if state.get("valid") != 1.0:
    raise RuntimeError("Vessel is not available")

target_latitude = state["latitude"] + math.degrees(
    100.0 / 6378137.0
)
target_longitude = state["longitude"]

accepted = client.simSetVesselTarget(
    target_latitude,
    target_longitude,
    3.0
)

if not accepted:
    raise RuntimeError("Vessel target was rejected")

while True:
    state = client.simGetVesselState()

    print(
        f"speed={state['speed_mps']:.2f} m/s "
        f"north={state['velocity_north_mps']:.2f} m/s "
        f"east={state['velocity_east_mps']:.2f} m/s "
        f"remaining={state['distance_to_target_m']:.1f} m "
        f"arrived={state['arrived']:.0f}"
    )

    if state["arrived"] == 1.0:
        break

    time.sleep(1.0)
```

The vessel charging pad is the authoritative GNSS reference point. The physical ship, charging pad, and collision plane move together.
