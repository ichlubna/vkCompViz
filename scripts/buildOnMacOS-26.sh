#!/bin/bash
# When running on a clean MacOS 26 Tahoe, the following needs to be installed
# Can be triggered by adding a parameter "clean" to the script
# Xcode Commandline Tools
# type "clang" and proceed with installing the tools as instructed in the terminal
# Homebrew
# /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
# Clang 20
# brew install llvm@20
# Ninja
# brew install ninja

set -x

if [ "$1" == "clean" ]; then
    clang
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    brew install llvm@20
    brew install ninja 
    brew install pkgconf
fi

LLVM_PATH=$(brew --prefix llvm@20)

mkdir -p buildTools; cd buildTools

# CMake
curl -LO https://github.com/Kitware/CMake/releases/download/v4.2.1/cmake-4.2.1-macos-universal.dmg
hdiutil attach cmake-4.2.1-macos-universal.dmg
cp -R /Volumes/cmake-4.2.1-macos-universal/ ./cmake
# Workaround to fix the modules file path
# https://www.reddit.com/r/cpp/comments/1eafzjs/anyone_having_success_with_c23_modules_and_cmake/
# https://gitlab.kitware.com/cmake/cmake/-/issues/25965#note_1523575
sed -i '' "s|\${_clang_modules_json_impl}.modules.json|${LLVM_PATH}/lib/c++/libc++.modules.json|g" \
./cmake/CMake.app/Contents/share/cmake-4.2/Modules/Compiler/Clang-CXX-CXXImportStd.cmake

# VMA
git clone https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
cd VulkanMemoryAllocator; mkdir install
../cmake/CMake.app/Contents/bin/cmake ./ -DCMAKE_INSTALL_PREFIX=./install -G "Ninja" -DCMAKE_CXX_COMPILER="$LLVM_PATH/bin/clang"
../cmake/CMake.app/Contents/bin/cmake --build . --target install 
mv ./install ../vma
cd ..
rm -rf ./VulkanMemoryAllocator
VMA_PATH=$(realpath ./vma)

# Getting tested Slang release
curl -LO https://github.com/shader-slang/slang/releases/download/v2025.14.2/slang-2025.14.2-macos-x86_64.tar.gz
mkdir slang; tar -xvzf slang-2025.14.2-macos-x86_64.tar.gz -C ./slang
SLANG_PATH=$(realpath ./slang)
rm ./*.gz

# Vulkan
LUNAR_VERSION="1.4.350.0"
curl -LO https://sdk.lunarg.com/sdk/download/$LUNAR_VERSION/mac/vulkansdk-macos-$LUNAR_VERSION.zip
unzip vulkansdk-macos-$LUNAR_VERSION.zip
sudo ./vulkansdk-macos-$LUNAR_VERSION.app/Contents/MacOS/vulkansdk-macos-$LUNAR_VERSION --root ~/VulkanSDK/$LUNAR_VERSION --accept-licenses --default-answer --confirm-command install
export VULKAN_SDK=~/VulkanSDK/$LUNAR_VERSION/macOS
export PATH=$VULKAN_SDK/bin:$PATH
export DYLD_LIBRARY_PATH=$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH

# FFMPEG
brew install ffmpeg

# GLFW
brew install glfw

# Compilation
cd ..
# Get the project
if [ "$1" == "clean" ]; then
    git clone --recursive https://github.com/ichlubna/vkCompViz.git
    cd vkCompViz/
fi
git submodule update --init --recursive
mkdir build; cd build

SDKROOT=$(xcrun --sdk macosx --show-sdk-path)
FFMPEG_PATH=$(brew --prefix ffmpeg)

../buildTools/cmake/CMake.app/Contents/bin/cmake .. \
  -G Ninja \
  -DCMAKE_PREFIX_PATH="$VMA_PATH;$SLANG_PATH;$VULKAN_SDK" \
  -DCMAKE_C_COMPILER="$LLVM_PATH/bin/clang" \
  -DCMAKE_CXX_COMPILER="$LLVM_PATH/bin/clang++" \
  -DCMAKE_OSX_SYSROOT="$SDKROOT" \
  -DCMAKE_CXX_FLAGS="-stdlib=libc++ -resource-dir=$LLVM_PATH/lib/clang/20" \
  -DCMAKE_EXE_LINKER_FLAGS="-L$LLVM_PATH/lib -Wl,-rpath,$LLVM_PATH/lib -lc++ -lc++abi -L$FFMPEG_PATH/lib -Wl,-rpath,$FFMPEG_PATH/lib -lswscale"

ninja
