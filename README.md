# :fire: conflagrant :fire:
The Conflagrant graphics engine (OpenGL 4.0+).

## Installation

### Linux (Arch)

Depending on your GPU you first need to run one of these commands:
* NVIDIA - `pacaur -S cuda`
* Intel - `pacaur -S intel-opencl-sdk`
* AMD - `pacaur -S amdapp-sdk`


```shell
pacaur -S ocl-icd opencl-headers glew glm jsoncpp assimp
git clone --recursive -j8 https://github.com/bwiberg/conflagrant.git
cd conflagrant
mkdir build && cd build
cmake ..
make
```

### MacOS

**_Does not work at the moment, but the following should be a good start_**

```shell
brew install glm jsoncpp assimp
git clone --recursive -j8 https://github.com/bwiberg/conflagrant.git
cd conflagrant
mkdir build && cd build
cmake ..
make
```

### Windows...

**_Does not work at the moment, but the following should be a good start_**

Install these libraries (preferred way is to build from source). Add their base directories to the [Path](https://stackoverflow.com/a/28545224) env. variable.
* [GLEW](http://glew.sourceforge.net/index.html)
* [GLFW3](http://www.glfw.org/)
* [GLM](https://glm.g-truc.net/0.9.8/index.html)
* [Assimp](http://assimp.sourceforge.net/main_downloads.html) - needs to be built from source
* [json-cpp](https://github.com/open-source-parsers/jsoncpp/releases) - needs to be built from source

Using [Git for Windows](https://git-for-windows.github.io/) (or similar), run the following:

```shell
git clone --recursive -j8 https://github.com/bwiberg/conflagrant.git
```

Next step is to build using CMake using one of the alternatives below:
* Use [CLion](https://www.jetbrains.com/clion/) as IDE (_my preferred solution_) 
* Use `cmake-gui.exe` to create a project solution (Code::Blocks, Visual Studio, Unix Makefiles etc.)

## Running examples

```shell
cd conflagrant/build/examples
./exampleN
```

## Controls

### DeferredRenderer

* V: Toggle VCT pipeline
    * 0: Toggle direct rendering
    * 1: Toggle direct lighting
    * 2: Toggle indirect diffuse lighting
    * 3: Toggle indirect specular lighting
* Left Shift + F: Toggle first-person camera controls
* A: Start all animations
* B: Stop all animations
