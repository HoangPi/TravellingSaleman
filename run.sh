#!/bin/bash

# Set image name
IMAGE_NAME="travellingsaleman-traveling_salesman_problem"

# Check if image already exists
if docker image inspect $IMAGE_NAME > /dev/null 2>&1; then
    echo "✅ Docker image '$IMAGE_NAME' already exists. Skipping build."
else
    # Check if Dockerfile exists
    if [ ! -f Dockerfile ]; then
        echo "❌ Dockerfile not found in current directory."
        exit 1
    fi

    # Build the Docker image
    echo "🚧 Building Docker image: $IMAGE_NAME..."
    docker build -t $IMAGE_NAME .

    # Check if build succeeded
    if [ $? -ne 0 ]; then
        echo "❌ Docker build failed."
        exit 1
    fi
fi

# Check if x11-xserver-utils is installed
if ! dpkg -s x11-xserver-utils >/dev/null 2>&1; then
    echo "Installing x11-xserver-utils..."
    sudo apt-get update
    sudo apt-get install -y x11-xserver-utils
else
    echo "x11-xserver-utils is already installed."
fi

# Allow Docker to access X11
echo "Running xhost +local:root..."
xhost +local:root

# Run the Docker container
echo "Launching Docker container..."
docker run -it \
  --env DISPLAY=$DISPLAY \
  --volume /tmp/.X11-unix:/tmp/.X11-unix \
  travellingsaleman-traveling_salesman_problem
