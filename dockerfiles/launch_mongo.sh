#!/bin/bash

# Define the container name
CONTAINER_NAME="mongo_container"

# Define the port mapping, if you want to access the MongoDB instance from the host
# For example, to map MongoDB's default port 27017 to the same port on the host
PORT_MAPPING="27017:27017"

HOST_DIR="/home/ktb/git/neurachip_deploy/mongo_data"
CONTAINER_DIR="/data/db"

# Check if a container with the same name already exists
if [ "$(docker ps -aq -f name=^/${CONTAINER_NAME}$)" ]; then
	    echo "Container with name $CONTAINER_NAME already exists. Removing it..."
	        docker rm -f $CONTAINER_NAME
fi

# Run the MongoDB container
echo "Launching MongoDB container named $CONTAINER_NAME..."
docker run -d --name $CONTAINER_NAME -v $HOST_DIR:$CONTAINER_DIR -p $PORT_MAPPING mongo_image

echo "MongoDB container launched successfully."

