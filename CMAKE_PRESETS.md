# CMake Presets Guide

This project includes CMake presets for building with different compilers: MSVC, Clang, and GCC.

## Available Presets

### Configure Presets
- **msvc-debug** - Microsoft Visual C++ (Debug)
- **msvc-release** - Microsoft Visual C++ (Release)
- **clang-debug** - Clang/LLVM (Debug)
- **clang-release** - Clang/LLVM (Release)
- **gcc-debug** - GCC (Debug)
- **gcc-release** - GCC (Release)

### Build Presets
Build presets correspond to each configure preset above.

### Test Presets
Test presets correspond to each configure preset above.

## Usage

### List Available Presets
```bash
cmake --list-presets
```

### Configure Project
```bash
# Using MSVC
cmake --preset msvc-debug
cmake --preset msvc-release

# Using Clang (requires Ninja generator and clang++ installed)
cmake --preset clang-debug
cmake --preset clang-release

# Using GCC (requires Ninja generator and g++/gcc installed)
cmake --preset gcc-debug
cmake --preset gcc-release
```

### Build Project
```bash
# Build using MSVC Debug preset
cmake --build --preset msvc-debug

# Build using Clang Release preset
cmake --build --preset clang-release

# Build using GCC Debug preset
cmake --build --preset gcc-debug
```

### Run Tests
```bash
# Run tests with MSVC Debug build
ctest --preset msvc-debug

# Run tests with Clang Release build
ctest --preset clang-release

# Run tests with GCC Release build
ctest --preset gcc-release
```

## Preset Details

### MSVC (Visual Studio 17 2022)
- **Generator**: Visual Studio 17 2022
- **Build Type**: Multi-configuration (Debug/Release selected at build time)
- **Requirements**: Visual Studio 2022 Community/Professional/Enterprise
- **Output Directory**: `build/msvc-debug` or `build/msvc-release`

### Clang/LLVM
- **Generator**: Ninja
- **Build Type**: Debug or Release (selected at configure time)
- **Requirements**: 
  - Ninja build system
  - Clang/LLVM toolchain (clang++, clang)
- **Output Directory**: `build/clang-debug` or `build/clang-release`

### GCC
- **Generator**: Ninja
- **Build Type**: Debug or Release (selected at configure time)
- **Requirements**:
  - Ninja build system
  - GCC toolchain (g++, gcc)
- **Output Directory**: `build/gcc-debug` or `build/gcc-release`

## Installing Required Tools

### Clang/LLVM on Windows
Download from: https://releases.llvm.org/download.html
Or use MSVC's built-in Clang support

### GCC on Windows
- MinGW-w64: https://www.mingw-w64.org/
- Or use Windows Subsystem for Linux (WSL)

### Ninja
- From: https://github.com/ninja-build/ninja/releases
- Or via Chocolatey: `choco install ninja`
- Or via pip: `pip install ninja`

## Environment Setup

### Optional: Setting Compiler Path
If CMake cannot find your compiler, you can set it explicitly:

```bash
# For GCC
cmake --preset gcc-debug -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# For Clang
cmake --preset clang-debug -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
```

## Features

All presets include:
- C++17 standard
- Compile commands export (`compile_commands.json`) for IDE integration
- Qt6 integration
- Google Test framework (auto-downloaded)
- Cross-compiler support

## Notes

- MSVC presets use Visual Studio's multi-configuration generator, so Debug/Release selection happens during build
- Clang and GCC presets use Ninja and single-configuration generators, so Debug/Release is selected at configure time
- All presets output to separate directories (`build/` subfolder) to avoid conflicts
- Compile commands are always exported for IDE integration (LSP, IntelliSense, etc.)
