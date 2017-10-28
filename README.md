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
git clone https://github.com/bwiberg/conflagrant.git
cd conflagrant
mkdir build && cd build
cmake ..
make
```

### MacOS

**_Not tested yet_**

```shell
brew install glm jsoncpp assimp
git clone https://github.com/bwiberg/conflagrant.git
cd conflagrant
mkdir build && cd build
cmake ..
make
```

### Windows...

Install these libraries (preferred way is to build from source). Add their base directories to the [Path](https://stackoverflow.com/a/28545224) env. variable.
* [GLEW](http://glew.sourceforge.net/index.html)
* [GLFW3](http://www.glfw.org/)
* [GLM](https://glm.g-truc.net/0.9.8/index.html)
* [Assimp](http://assimp.sourceforge.net/main_downloads.html) - needs to be built from source
* [json-cpp](https://github.com/open-source-parsers/jsoncpp/releases) - needs to be built from source


## Running examples

```shell
cd conflagrant/build/examples
./exampleN
```
