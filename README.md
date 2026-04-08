# rctech

This is a long‑term experimental playground for graphics and modern C++ design. The project explores ideas rather than aiming for a finished engine, using Vulkan and forward‑looking C++ features as they become available. It evolves continuously, and while the repository as a whole is experimental, its components are intentionally written to be standalone and freely usable elsewhere.

## Project Directory Structure

* base - Fundamental generic types.
* core - Base types related to the engin.
* ext - Third party (external) dependencies.
* generic - Generic platform types (related to a platform specific types)
* hlsl - HLSL embedded shaders
* renderer - Vulkan renderer
* res - Embedded resources library (containing e.g. HLSL shaders from /hlsl)
* tools - Tool needed to build the project.
* vulkan - Vulkan loader (vulkan C++ abstraction layer).
* windows - Windows-specific types.