FROM ubuntu:22.04

# Install dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y tzdata

# Set timezone (e.g., Asia/Ho_Chi_Minh)
ENV TZ=Asia/Ho_Chi_Minh
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtk-3-dev \
    libopencv-dev

# Create app directory
WORKDIR /app

# Copy your project files
COPY . /app

# Build your project
RUN mkdir build && cd build && cmake .. && make

# Run your app (adjust executable name)
CMD ["./build/main"]
