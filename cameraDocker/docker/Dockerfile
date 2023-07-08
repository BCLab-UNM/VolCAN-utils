FROM ros:galactic-ros-base

# Install dependencies
RUN apt-get -qq update && \
    apt-get -qq upgrade && \
    apt-get -qq install ros-galactic-image-view 
    
RUN apt-get -qq install xauth

RUN rm /bin/sh && ln -s /bin/bash /bin/sh

# Clear apt cache to save on space
RUN rm -rf /var/lib/apt/lists/*

# Setup Entrypoint
COPY ./docker/entrypoint.sh /
COPY ./cyclonedds.xml /

ENTRYPOINT ["/entrypoint.sh"]
