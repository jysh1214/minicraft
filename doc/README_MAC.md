# Mac Build Instructions #

## Dependencies ##

Several packages/libraries are needed to build the project.

- [CMake >= 3.21](https://cmake.org/download/)
- [Ninja >= 1.12.0](https://ninja-build.org/)
- [glew-2.2.0](https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz)
- [glfw-3.4](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)
- [glm-1.0.1](https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip)
- [opencv-4.9.0](https://github.com/opencv/opencv/archive/refs/tags/4.9.0.tar.gz)


## Prerequisites ##

Build and install dependencies.

### CMake ###
Build and install [CMake](https://cmake.org/download/).

### Ninja ###

Download `Ninja` and extract it.
```
wget https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-mac.zip
unzip ninja-mac.zip
```

To build and install `Ninja`:
```
# in ninja directory
cmake \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install"

cmake --build ./build-mac --target install
```

### GLEW ###

Download `GLEW` and extract it.
```
wget https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz
tar zxvf glew-2.2.0.tgz
```

To build and install `GLEW`:
```
# in glew directory
cmake \
  -G Ninja \
  -S ./build/cmake \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D BUILD_SHARED_LIBS=OFF \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install"

cmake --build ./build-mac --target install
```

### GLFW ###

Download `GLFW` and extract it.
```
wget https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
unzip glfw-3.4.zip
```

To build and install `GLFW`:
```
# in glfw directory
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D GLFW_BUILD_EXAMPLES=FALSE \
  -D GLFW_BUILD_TESTS=FALSE \
  -D GLFW_BUILD_DOCS=FALSE \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" \
  -D BUILD_SHARED_LIBS=OFF

cmake --build ./build-mac --target install
```

### GLM ###

Download `GLM` and extract it.
```
wget https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
unzip 1.0.1.zip
```

To build and install `GLM`:
```
# in glm directory
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" \
  -D BUILD_SHARED_LIBS=OFF \
  -D GLM_BUILD_TESTS=OFF

cmake --build ./build-mac --target install
```

### OpenCV ###

Download `OpenCV` and extract it.
```
wget https://github.com/opencv/opencv/archive/refs/tags/4.9.0.tar.gz
tar zxvf 4.9.0.tar.gz
```

To build and install `OpenCV`:
```
# in opencv directory
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" \
  -D BUILD_SHARED_LIBS=OFF \
  -D BUILD_EXAMPLES=OFF \
  -D BUILD_TESTS=OFF \
  -D BUILD_DOCS=OFF \
  -D BUILD_opencv_python2=OFF \
  -D BUILD_opencv_python3=OFF \
  -D WITH_OPENEXR=OFF
  
cmake --build ./build-mac --target install
```


## Build & Usage ##

After building the dependencies, you need to add the install directories of `GLEW`, `GLFW`, `GLM` and `OpenCV` to the `CMAKE_PREFIX_PATH`. This helps `CMake` find these libraries when building your project.
```
export CMAKE_PATH="path/to/glew-2.2.0/build-mac/install;path/to/glfw-3.4/build-mac/install;path/to/glm-1.0.1/build-mac/install;path/to/opencv-4.9.0/build-mac/install"
```

Run the following command to configure and build the `minicraft` project:
```
# in minicraft directory
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Debug \
  -D BUILD_SHARED_LIBS=OFF \
  -D CMAKE_PREFIX_PATH=${CMAKE_PATH} \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install"

cmake --build ./build-mac --target install
```

Execute the program:
```
# in minicraft directory
./build-mac/install/bin/minicraft
```
