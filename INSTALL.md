# Installation
The repository needs to be cloned with its submodules first:
```
git clone --recursive https://github.com/ichlubna/vkCompViz.git
```

The library is a standard C++ project, using C++26 standard. The required dependencies are:  
- [CMake](https://cmake.org) 4.0 and higher, supporting C++ modules and STD import.
- [Ninja](https://ninja-build.org) build system.
- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) and GPU with drivers supporting Vulkan.
- [Vulkan Memory Allocator](https://gpuopen.com/vulkan-memory-allocator/) for memory management.
- [GLFW](https://www.glfw.org) window creation library.
- [Slang](https://shader-slang.org) shading language API and compiler.
- [FFmpeg](https://ffmpeg.org) media encoding and decoding library.

These dependencies can be either manually downloaded from the links above, or installed via the OS package manager, such as [Pacman](https://wiki.archlinux.org/title/Pacman) or [APT](https://help.ubuntu.com/kubuntu/desktopguide/C/apt-get.html).  

After cloning the repository, the following commands are recommended to compile the library and the examples:  
```
cd vkCompViz
mkdir build
cmake .. -G "Ninja"
ninja
```

This approach assumes using [GCC](https://gcc.gnu.org) compiler. When switching to, for example [Clang](https://clang.llvm.org), a command like this can be used:  
```
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER="/usr/bin/clang-20" -DCMAKE_C_COMPILER="/usr/bin/clang-20" -DCMAKE_CXX_FLAGS="-stdlib=libc++ -I/usr/include/c++/v1 -resource-dir=/usr/lib/llvm-20/lib/clang/20 -I/usr/include/x86_64-linux-gnu" -DCMAKE_EXE_LINKER_FLAGS="-L/usr/lib/llvm-20/lib -Wl,-rpath,/usr/lib/llvm-20/lib -lm -lc++abi -lc++"
ninja
```

This project uses [GitHub workflow](/.github/workflows/linuxBuild.yml) to test to compilation after every commit. This workflow uses an Ubuntu compilation [script](/scripts/buildOnUbuntu-24_04.sh) which can be run, for example in clean Ubuntu Docker image. The script also demonstrates the installation of the required dependencies with the package manager and also manually.

# Usage
To run the examples, simply use:  
```
cd ./examples/exampleName/
./exampleName
```

Each example has its own README file with example run command with necessary parameters. To decide which example to include or exclude in the compilation, the easiest way is to use CMake GUI or CCMake to open the compilation parameters where each example can be enabled or disabled.  

The easiest way to implement a custom program with the library is to fork the project and add the code as another example. Simply copy one of the existing examples, edit, and add the new example into the root [CMakeLists](/CMakeLists.txt) file as:
```
addExample("nameOfTheExampleDirectory" "Description")
```

vkCompViz is compiled as a shared library so it can be included in other projects in the same way as other C++ libraries, including its [interface](/vkCompViz/include/vkCompViz.cppm) and adding the compiled library to the linker.
