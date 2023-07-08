#!/bin/bash
set -e

source /opt/ros/galactic/setup.bash
export CYCLONEDDS_URI="file:///cyclonedds.xml"
ros2 daemon stop
ros2 daemon start
cd /images
ros2 run image_view image_view --ros-args --remap image:=/image_raw
