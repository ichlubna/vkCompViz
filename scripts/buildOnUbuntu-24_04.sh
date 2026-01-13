#!/bin/bash
set -e

# When used in Docker as root
if [ "$1" == "docker" ]; then
    if [ -d "vkCompViz" ]; then
        echo "The directory vkCompViz already exists."
        exit 0
    fi
    if ! command -v sudo &> /dev/null; then
        apt update
        apt install -y sudo
    fi
fi
# Installing basic dev tools
export DEBIAN_FRONTEND=noninteractive
sudo ln -fs /usr/share/zoneinfo/Europe/Prague /etc/localtime
echo "Europe/Prague" | sudo tee /etc/timezone
sudo apt update
sudo apt install software-properties-common -y
sudo add-apt-repository ppa:ubuntuhandbook1/ffmpeg7 -y; sudo apt update
sudo apt install sudo ninja-build git wget gnupg software-properties-common tzdata libglfw3-dev ffmpeg libavfilter-dev libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libavdevice-dev build-essential -y

# Installing Clang 20
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo gpg --dearmor > /usr/share/keyrings/llvm-archive-keyring.gpg
echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/noble/ llvm-toolchain-noble-20 main" | sudo tee -a /etc/apt/sources.list.d/llvm.list
sudo apt update; sudo apt install -y clang-20 libc++-20-dev libc++abi-20-dev

# Cloning the project in Docker
if [ "$1" == "docker" ]; then
    git clone --recursive https://github.com/ichlubna/vkCompViz.git
    cd vkCompViz/
fi

# Preparing submodules and build dir
git submodule update --init --recursive
mkdir build; cd build

# Here are some external libs
mkdir buildTools; cd buildTools

# Using specific CMake version to ensure the support for modules
wget https://github.com/Kitware/CMake/releases/download/v4.1.0/cmake-4.1.0-linux-x86_64.tar.gz; tar -xvzf cmake-4.1.0-linux-x86_64.tar.gz
rm ./*.gz

# Getting latest VMA
git clone https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
cd VulkanMemoryAllocator; mkdir install
../cmake-4.1.0-linux-x86_64/bin/cmake ./ -DCMAKE_INSTALL_PREFIX=./install -G "Ninja" -DCMAKE_CXX_COMPILER="/usr/bin/clang-20"
../cmake-4.1.0-linux-x86_64/bin/cmake --build . --target install 
mv ./install ../vma
cd ..
rm -rf ./VulkanMemoryAllocator
VMA_PATH=$(realpath ./vma)

# Getting tested Slang release
wget https://github.com/shader-slang/slang/releases/download/v2025.14.2/slang-2025.14.2-linux-x86_64.tar.gz
mkdir slang; tar -xvzf slang-2025.14.2-linux-x86_64.tar.gz -C ./slang
SLANG_PATH=$(realpath ./slang)
rm ./*.gz

# Installing Vulkan SDK
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-noble.list http://packages.lunarg.com/vulkan/lunarg-vulkan-noble.list
sudo apt update; sudo apt install vulkan-sdk -y

# Compiling the project
cd ..
./buildTools/cmake-4.1.0-linux-x86_64/bin/cmake .. -G Ninja -DCMAKE_PREFIX_PATH="$VMA_PATH;$SLANG_PATH" -DCMAKE_CXX_COMPILER="/usr/bin/clang-20" -DCMAKE_C_COMPILER="/usr/bin/clang-20" -DCMAKE_CXX_FLAGS="-stdlib=libc++ -I/usr/include/c++/v1 -resource-dir=/usr/lib/llvm-20/lib/clang/20 -I/usr/include/x86_64-linux-gnu" -DCMAKE_EXE_LINKER_FLAGS="-L/usr/lib/llvm-20/lib -Wl,-rpath,/usr/lib/llvm-20/lib -lm -lc++abi -lc++"
ninja
