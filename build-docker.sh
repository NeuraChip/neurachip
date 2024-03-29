set -e

USER_ID=$(id -u)
GROUP_ID=$(id -g)

# These first builds use a smaller context
docker build -f ./dockerfiles/Dockerfile.base dockerfiles \
    --target builder -t neurachip/neurachip:latest-devel

# This build uses the full context
# docker build -f ./dockerfiles/Dockerfile.base . \
#     --target dev-user -t neurachip/neurachip:latest-devel-$USER_ID \
#     --build-arg USER_ID=$USER_ID \
#     --build-arg GROUP_ID=$GROUP_ID 

docker build -f ./dockerfiles/Dockerfile.base . \
    --target dev-user -t neurachip/neurachip:latest-devel-artifact