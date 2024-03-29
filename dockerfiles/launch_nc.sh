#!/bin/bash

# Define the path to the directory you want to mount into the container
HOST_DIRECTORY="/home/ktb/git/neurachip_deploy"
CONTAINER_DIRECTORY="/root/neurachip_deploy"
HOST_DIRECTORY2="/home/ktb/git/neurachip_deploy/Dashboard/template"
CONTAINER_DIRECTORY2="/var/www/html"



#CONTAINER_DIRECTORY="/var/www/html"

# Define the name of your container
CONTAINER_NAME="nc_gui"

# Define the port mapping (host:container)
PORT_MAPPING1="80:80"
PORT_MAPPING2="443:443"

# Check if the container already exists
if [ $(docker ps -a -f name=^/${CONTAINER_NAME}$ --format "{{.Names}}") == $CONTAINER_NAME ]; then
    echo "Container $CONTAINER_NAME already exists, removing it."
    docker rm -f $CONTAINER_NAME
fi

# Run the container with the specified settings
docker run -d --name $CONTAINER_NAME -p $PORT_MAPPING1 -p $PORT_MAPPING2 -v $HOST_DIRECTORY:$CONTAINER_DIRECTORY -v $HOST_DIRECTORY2:$CONTAINER_DIRECTORY2 nc_image

echo "Container $CONTAINER_NAME launched successfully."

