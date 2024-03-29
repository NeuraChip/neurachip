set -e

USER_ID=$(id -u):$(id -g)
USER=$(id -u)

# DEV_IMAGE_NAME="neurachip/neurachip:latest-devel-$USER"

# docker run --rm --privileged -it \
#     --user=$USER_ID \
#     -v ${PWD}:/working_dir -w /working_dir \
#     $DEV_IMAGE_NAME \
#     /bin/bash

DEV_IMAGE_NAME="neurachip/neurachip:latest-devel-artifact"

docker run --rm --privileged -it \
    $DEV_IMAGE_NAME \
    /bin/bash
