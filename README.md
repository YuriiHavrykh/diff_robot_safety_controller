# ROS 2 Autonomous Safety Controller

This repository contains a **safety-critical monitoring system** for a two-wheel differential drive robot.  
The system is fully integrated into the robot's launch lifecycle, ensuring that the safety monitor is **always active when the simulation starts**.

---

# Core Functionality

The safety controller acts as a **fail-safe layer** between the user (**teleop**) and the robot's hardware.  
It continuously audits the surrounding environment and **overrides movement commands** if the robot's proximity to any object falls below the safety threshold.

### Features

- **Proximity Detection**  
  Monitors the `/scan` topic (`LaserScan`) for obstacles within a **0.5 m radius**.

- **Emergency Braking**  
  Automatically publishes **zero-velocity commands** to `/cmd_vel` when a collision is imminent.

- **Signal Filtering**  
  Robustly handles non-numerical sensor data, specifically filtering out `inf` and `NaN` values.

---

# Design Philosophy

### Integrated Safety (Plug-and-Play)

The safety node is launched automatically via `robot.launch.py`.  
This ensures that the robot is **never "unprotected"** and simplifies the user workflow.

### Fail-Safe Override

Instead of filtering existing commands, the node **publishes a high-priority zero-velocity signal** upon detection, ensuring a reliable stop even during active manual driving.

### Global Safety Bubble

Monitors the **full LIDAR field** to ensure safety regardless of the robot's orientation or movement direction.

---

# Build & Run Instructions
## Setup

Ensure you have a ROS 2 workspace (e.g., ros2_ws).

## Launch and Teleop

```bash
cd ~/ros2_ws
colcon build --packages-select my_diff_robot
source install/setup.bash

# Launch simulation + robot + safety controller
ros2 launch my_diff_robot robot.launch.py

# Open a new terminal for teleoperation
source install/setup.bash
python3 src/my_diff_robot/scripts/simple_teleop.py


