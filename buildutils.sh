#!/bin/bash
sudo apt-add-repository -y ppa:ubuntu-sdk-team/ppa
sudo apt-get update
sudo apt-get install cmake libglew-dev libglm-dev x11proto-xf86vidmode-dev xorg-dev libdevil-dev build-essential cppcheck nvidia-opencl-dev
curl -L http://sourceforge.net/projects/glfw/files/glfw/3.0.1/glfw-3.0.1.zip/download -o glfw-3.0.1.zip
unzip glfw-3.0.1.zip
cd glfw-3.0.1
cmake -DBUILD_SHARED_LIBS=YES .
make
sudo make install
cd ..
curl -L http://sourceforge.net/projects/ogl-math/files/glm-0.9.4.4/glm-0.9.4.4.zip/download -o glm-0.9.4.4.zip
unzip glm-0.9.4.4.zip
mkdir build
mv glm-0.9.4.4/glm build
curl -L http://www.cmake.org/files/v2.8/cmake-2.8.11.2.tar.gz -o cmake-2.8.11.2.tar.gz
tar -xvf cmake-2.8.11.2.tar.gz
cd cmake-2.8.11.2
mkdir build
cd build
cmake ..
make
sudo make install
cd ../..
sudo apt-get remove cmake
