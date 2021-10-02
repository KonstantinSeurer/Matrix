# Matrix

Matrix is a framework for rendering 3D graphics in real time. The current features are:

- A scene graph optimized for iterating over many components
- A graphics API abstraction with ray tracing support
- GLTF loading
- Scene rendering
- Atmospheric scattering
- Mouse and keyboard input
- A math API

## Dependencies

- LunarG Vulkan SDK

### Building submodules

Linux:

```bash
./dependencies/clean.sh # Only when updating submodules
./dependencies/build.sh
```

Windows:

```powershell
# TODO: implement build scripts for Windows
```

## Building

Linux:

```bash
mkdir build
cmake -B build
cd build
make -j 16
```

Windows:

```powershell
# TODO: add build instructions for Windows
```

