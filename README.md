# PoissonSurfer: Screened Poisson Surface Reconstruction (M1/M2 Mac Port)

This repository is a specialized port of the world-renowned **Screened Poisson Surface Reconstruction** algorithm, originally developed by **Michael Kazhdan** and **Matthew Bolitho**.

##  Apple Silicon (M1/M2) Port
This version has been meticulously updated and optimized to run natively on **Apple Silicon (M1/M2/M3)** architecture. 
- **Native Performance:** Fully compatible with `arm64` architecture, leveraging the efficiency of Apple's M-series chips.
- **OpenMP Integration:** Includes refined multi-threading support compatible with modern Clang/LLVM compilers on macOS.
- **Architecture Fixes:** Resolved specific alignment and bit-shifting issues that previously hindered compilation on ARM-based Unix systems.

## Key Contributions & Modernization
Beyond the initial port, this repository introduces several engineering enhancements to make the codebase more robust and developer-friendly:

*   **CMake Build System:** Replaced legacy Makefiles and MSVC solution files with a modern, cross-platform **CMake** configuration. This allows for seamless builds on macOS, Linux, and Windows with a single command.
*   **Comprehensive Test Suite:** Introduced a dedicated testing framework (located in `/tests`) covering:
    *   **Mathematical Primitives:** Unit tests for Vectors, Polynomials, and Sparse Matrices.
    *   **Geometric Logic:** Validation of Marching Cubes implementation and spatial transformations.
    *   **End-to-End Reconstruction:** Automated integration tests that verify the reconstruction of primitive shapes (Spheres, Cubes) from point clouds.
*   **Code Robustness:** 
    *   Added a testing mock layer (`test_mocks.h`) to decouple library logic from CLI-specific output for better embeddability.
    *   Fixed critical default argument bugs and template instantiation issues found in the original source.
    *   Improved memory safety with updated pointer management.

## Why Poisson Reconstruction Still Matters
Even decades after its introduction, Screened Poisson Reconstruction remains the **industry standard** for surface generation from oriented point clouds. Its importance stems from:
1.  **Global Optimization:** Unlike local methods (like Ball Pivoting), it treats reconstruction as a global Poisson problem, making it incredibly resilient to noise and non-uniform sampling.
2.  **Watertight Results:** It naturally produces "watertight" meshes, which is a hard requirement for 3D printing and physical simulations.
3.  **Screening Logic:** The "Screened" variant allows the surface to faithfully follow the input points while maintaining the smoothness provided by the underlying PDE.
4.  **Scalability:** The octree-based implementation allows it to process hundreds of millions of points on consumer hardware.

## Modern Developments
The field of surface reconstruction is rapidly evolving, and this codebase serves as a foundation for modern workflows:
- **Neural Implicit Surfaces:** While AI-based methods (like Neural Radiance Fields or DeepSDF) are rising, Poisson Recon is often used as the final "extraction" step to convert neural probability fields into usable polygonal meshes.
- **Real-time Scanning:** Optimized versions of this algorithm are the backbone of many mobile LiDAR apps and handheld 3D scanners.
- **Hybrid Workflows:** Modern pipelines frequently combine this algorithm's reliability with machine learning-based normal estimation to achieve near-perfect reconstructions from raw, unoriented sensor data.

---

## Quick Start (Build & Test)

### Requirements
- CMake 3.10+
- A C++20 compatible compiler (Clang/GCC/MSVC)
- OpenMP (Available via `brew install libomp` on macOS)

### Building
```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests
```bash
make run_tests
./run_tests
```

### Usage
```bash
./PoissonRecon --in input.ply --out output.ply --depth 10
```

---

## Parameter Reference

### 1. PoissonRecon Parameters
The core engine for surface reconstruction.

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `--in` | `string` | **Required.** Input point cloud (`.ply`, `.bnpts`, or ASCII). Normals are required. |
| `--out` | `string` | Output mesh filename in `.ply` format. |
| `--depth` | `int` | Maximum octree depth (resolution). $2^d$. Higher = more detail (and RAM). |
| `--pointWeight` | `float` | "Screening" weight. Stick to points vs. surface smoothness. (0 = original Poisson). |
| `--samplesPerNode`| `float` | Noise control. Use 1.0-5.0 for clean data, 15.0-20.0 for noisy data. |
| `--scale` | `float` | Bounding box buffer. Default is 1.1. |
| `--fullDepth` | `int` | Depth at which the octree stops being adaptive. Default is 5. |
| `--iters` | `int` | Number of Gauss-Seidel iterations per level. Default is 8. |
| `--threads` | `int` | Parallel processing threads. Defaults to all CPU cores. |
| `--density` | `flag` | Export sampling density at vertices. (Required for `SurfaceTrimmer`). |
| `--voxel` | `string` | Save the implicit function as a binary voxel grid. |
| `--polygonMesh` | `flag` | Output polygons instead of triangulating the mesh. |
| `--confidence` | `flag` | Use normal magnitudes as confidence information. |

### 2. SurfaceTrimmer Parameters
Poisson Recon produces "watertight" meshes. Use this tool to trim away regions with low point density (e.g., to keep holes open).

| Parameter | Type | Description |
| :--- | :--- | :--- |
| `--in` | `string` | Input mesh from `PoissonRecon` (must have been run with `--density`). |
| `--trim` | `float` | Density threshold. Regions lower than this are discarded. |
| `--out` | `string` | Filename for the trimmed mesh. |
| `--smooth` | `int` | Number of smoothing passes on the density signal before trimming. |
| `--aRatio` | `float` | Area ratio for removing small disconnected "islands." |

---
*Original algorithm by Michael Kazhdan, Matthew Bolitho, and Hugues Hoppe.*
