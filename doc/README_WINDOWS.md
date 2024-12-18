# Windows Build Instructions #

## Dependencies ##

Several packages/libraries are needed to build the project.

- [CMake >= 3.21](https://cmake.org/download/)
- [Visual Studio 17 2022](https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-notes)
- [glew-2.2.0](https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz)
- [glfw-3.4](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)
- [glm-1.0.1](https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip)
- [opencv-4.9.0](https://github.com/opencv/opencv/archive/refs/tags/4.9.0.tar.gz)


## Prerequisites ##

Build and install dependencies.

**How to build project with `Visual Studio 17 2022`?**

1. Download and extract the source code.
1. Open a terminal at the root directory of the project, and run the cmake command to configure and generate the project.
2. Open `.sln` file in Visual Studio, set the configuration, and build the `ALL_BUILD` and `INSTALL` targets.

### CMake ###
Download and install [CMake](https://cmake.org/download/).

### Visual Studio 17 2022 ###
Download and install [Visual Studio 17 2022](https://learn.microsoft.com/en-us/visualstudio/releases/2022/release-notes).

### GLEW ###

To build and install `GLEW`:
```
# in glew directory
cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -S ./build/cmake `
  -B ./build-windows `
  -D BUILD_SHARED_LIBS=OFF `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install"
```

### GLFW ###

To build and install `GLFW`:
```
# in glfw directory
cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D BUILD_SHARED_LIBS=OFF `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D GLFW_BUILD_EXAMPLES=FALSE `
  -D GLFW_BUILD_TESTS=FALSE `
  -D GLFW_BUILD_DOCS=FALSE
```

### GLM ###

To build and install `GLM`:
```
# in glm directory
cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D BUILD_SHARED_LIBS=OFF `
  -D GLM_BUILD_TESTS=OFF
```

### OpenCV ###

To build and install `OpenCV`:
```
# in opencv directory
cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D BUILD_SHARED_LIBS=OFF `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D BUILD_EXAMPLES=OFF `
  -D BUILD_TESTS=OFF `
  -D BUILD_DOCS=OFF `
  -D BUILD_opencv_python2=OFF `
  -D BUILD_opencv_python3=OFF `
  -D BUILD_JAVA=OFF
```


## Build & Usage ##

After building the dependencies, you need to add the install directories of `GLEW`, `GLFW`, `GLM` and `OpenCV` to the `CMAKE_PREFIX_PATH`. This helps `CMake` find these libraries when building your project.

Suppose you've installed all these libraries directly under your `C:\`. In that case, your `CMAKE_PREFIX_PATH` would be configured as follows:
```
CMAKE_PREFIX_PATH="C:\glew-2.1.0\build-windows\install;C:\glfw-3.4\build-windows\install;C:\glm-1.0.1\build-windows\install"
```

To use static linked `OpenCV`, add the following path to `CMAKE_PREFIX_PATH`:
```
CMAKE_PREFIX_PATH="C:\glew-2.1.0\build-windows\install;C:\glfw-3.4\build-windows\install;C:\glm-1.0.1\build-windows\install
;C:\opencv-4.9.0\build-windows\install\x64\vc17\staticlib"
```

Open a terminal at the `minicraft` directory, and run the following command to configure and generate the `minicraft` project:
```
# in minicraft directory
cmake `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D CMAKE_PREFIX_PATH="..."
  -D PYTHON_EXEC=python
```

Open `build-windows/minicraft.sln` in Visual Studio, and build the `ALL_BUILD` and `INSTALL` targets.

Set `minicraft` target as setup projest, and click the start buttom.
