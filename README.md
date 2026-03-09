ROS 2 Autonomous Safety Controller

This repository contains a safety-critical monitoring system for a two-wheel differential drive robot. The system is fully integrated into the robot's launch lifecycle, ensuring that the safety monitor is always active when the simulation starts.

Core Functionality

The safety controller acts as a fail-safe layer between the user (teleop) and the robot's hardware. It continuously audits the surrounding environment and overrides movement commands if the robot's proximity to any object falls below the safety threshold.

Proximity Detection: Monitors the /scan topic (LaserScan) for obstacles within a 0.5m radius.

Emergency Braking: Automatically publishes zero-velocity commands to /cmd_vel when a collision is imminent.

Signal Filtering: Robustly handles non-numerical sensor data, specifically filtering out inf and NaN values.

Design Philosophy

Integrated Safety (Plug-and-Play): The safety node is launched automatically via robot.launch.py. This ensures that the robot is never "unprotected" and simplifies the user workflow.

Fail-Safe Override: Instead of filtering existing commands, the node publishes a high-priority zero-velocity signal upon detection, ensuring a reliable stop even during active manual driving.

Global Safety Bubble: Monitors the full LIDAR field to ensure safety regardless of the robot's orientation or movement direction.

Build & Run Instructions

1. Setup Workspace

Ensure you have a ROS 2 workspace (e.g., ros2_ws).

cd ~/ros2_ws
colcon build --packages-select my_diff_robot
source install/setup.bash


2. Launch Simulation & Safety Node

The following command starts the Gazebo simulation environment, the robot model, and the Safety Controller simultaneously:

ros2 launch my_diff_robot robot.launch.py


3. Test with Teleop

Open a new terminal to drive the robot manually and verify that the safety system overrides your commands near obstacles:

source install/setup.bash
python3 src/my_diff_robot/scripts/simple_teleop.py
