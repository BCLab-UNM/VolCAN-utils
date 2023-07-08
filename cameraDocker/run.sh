#!/bin/bash

docker network inspect ros-net >/dev/null 2>&1 || \
    docker network create ros-net --subnet=172.18.0.0/16

docker run -it \
    --network host \
    --env="DISPLAY" \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v ./images:/images \
    --volume="$HOME/.Xauthority:/home/developer/.Xauthority:rw" \
    dragonfly-camera:latest 
    
    
