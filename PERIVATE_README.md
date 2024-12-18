# minicraft #



## Dependencies ##

- [CMake >= 3.21](https://cmake.org/download/)
- [glew-2.2.0](https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz)
- [glfw-3.4](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip)
- [glm-1.0.1](https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip)

References
- [GLEW](https://glew.sourceforge.net/install.html)
- [GLFW](https://www.glfw.org/docs/latest/quick.html)
- [GLM](https://github.com/g-truc/glm)

## Build windows ##

```
C:\CMake\bin\cmake.exe `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D CMAKE_PREFIX_PATH="C:\glew-2.1.0\build-windows\install;C:\glfw-3.4\build-windows\install;C:\glm-1.0.1\build-windows\install"
```


## Build GLFW & GLEW ##

### windows ###

```
# glew
C:\CMake\bin\cmake.exe `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -S ./build/cmake `
  -B ./build-windows `
  -D CMAKE_BUILD_TYPE=Release `
  -D BUILD_SHARED_LIBS=OFF `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install"
```

```
# glfw
C:\CMake\bin\cmake.exe `
  -G "Visual Studio 17 2022" `
  -A x64 `
  -B ./build-windows `
  -D CMAKE_BUILD_TYPE=Release `
  -D BUILD_SHARED_LIBS=OFF `
  -D CMAKE_INSTALL_PREFIX="./build-windows/install" `
  -D GLFW_BUILD_EXAMPLES=FALSE `
  -D GLFW_BUILD_TESTS=FALSE `
  -D GLFW_BUILD_DOCS=FALSE `
  -D USE_MSVC_RUNTIME_LIBRARY_DLL=1
```



## mac ##

```
module load CMake/3.24 Ninja

export CMAKE_PATH="/Users/alex/Modules/glew-2.2.0/build-mac/install;/Users/alex/Modules/glfw-3.4/build-mac/install;/Users/alex/Modules/glm-1.0.1/build-mac/install"

rm -rf ./build-mac && \
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Debug \
  -D BUILD_SHARED_LIBS=OFF \
  -D CMAKE_PREFIX_PATH=${CMAKE_PATH} \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" && \
cmake --build ./build-mac --target install && \
./build-mac/install/bin/minicraft

# glew
wget https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz

tar zxvf glew-2.2.0.tgz

cmake \
  -G Ninja \
  -S ./build/cmake \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D BUILD_SHARED_LIBS=OFF \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" && \
cmake --build ./build-mac --target install


wget https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip

unzip glfw-3.4.zip

# glfw
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D GLFW_BUILD_EXAMPLES=FALSE \
  -D GLFW_BUILD_TESTS=FALSE \
  -D GLFW_BUILD_DOCS=FALSE \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" \
  -D BUILD_SHARED_LIBS=OFF && \
cmake --build ./build-mac --target install

wget https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip

unzip 1.0.1.zip

# glm
cmake \
  -G Ninja \
  -B ./build-mac \
  -D CMAKE_BUILD_TYPE=Release \
  -D CMAKE_INSTALL_PREFIX="./build-mac/install" \
  -D BUILD_SHARED_LIBS=OFF \
  -D GLM_BUILD_TESTS=OFF && \
cmake --build ./build-mac --target install
```