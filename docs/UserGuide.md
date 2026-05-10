# PoissonSurfer: An Undergraduate Guide to Surface Reconstruction

Welcome to the world of Computational Geometry and Computer Graphics! This guide is designed to help you understand, build, and master the **Screened Poisson Surface Reconstruction** algorithm using the **PoissonSurfer** toolkit.

---

## Preface: Why Surface Reconstruction?

Imagine you have a handheld 3D scanner. As you move it around an object, it captures millions of individual points $(x, y, z)$ in space. These points are just "dust"—they don't have a surface, they don't have "insides" or "outsides," and you certainly can't 3D print them.

**Surface Reconstruction** is the mathematical process of turning that "dust" into a continuous, smooth, "watertight" mesh (like a skin). The Poisson method is the gold standard for this task because it doesn't just connect the dots; it solves a global optimization problem.

---

## Chapter 1: The Mathematical Foundation

### 1.1 The Poisson Equation
At the heart of this tool is a classic partial differential equation (PDE) from physics. If we represent the surface as an implicit function $\chi$, where $\chi(p) = 1$ inside the object and $\chi(p) = 0$ outside, the surface is the boundary where the value changes.

The algorithm solves:
$$\nabla^2 \chi = \nabla \cdot \vec{V}$$
Where $\vec{V}$ is a vector field defined by the oriented normals of your input points. In simpler terms, we are looking for a solid shape whose "gradients" match your input "normals."

### 1.2 The Octree: Spatial Intelligence
Processing 100 million points directly is impossible. PoissonSurfer uses an **Octree**—a hierarchical data structure that subdivides 3D space. 
- A large empty area is represented by one big cube.
- A high-detail area (like a face or a corner) is subdivided into smaller and smaller cubes.
- The `--depth` parameter controls how many times we can subdivide.

---

## Chapter 2: Laboratory Setup

### 2.1 Prerequisites
Before we begin, ensure your environment is ready. This port is optimized for **Apple Silicon (M1/M2/M3)** but works on any modern Unix-like system.

1.  **Compiler:** A C++20 compatible compiler (Clang or GCC).
2.  **CMake:** The architect of our build process.
3.  **OpenMP:** For multi-core acceleration. 
    *   On macOS: `brew install libomp`

### 2.2 Building the Project
Open your terminal and execute:

```bash
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

This will produce two main executables: `surfrecon` (the engine) and `surftrimmer` (the scalpel).

---

## Chapter 3: Your First Reconstruction

To run a basic reconstruction, you need an oriented point cloud in `.ply` format. "Oriented" means every point must have a normal vector $(nx, ny, nz)$ telling the computer which way is "out."

```bash
./surfrecon --in dragon_points.ply --out dragon_mesh.ply --depth 8
```

### What just happened?
1.  **Voxelization:** The points were mapped into an Octree.
2.  **Solver:** The Poisson equation was solved across the grid.
3.  **Iso-surface Extraction:** The "Marching Cubes" algorithm walked through the grid to find the $0.5$ boundary and generated triangles.

---

## Chapter 4: Mastering the Parameters

As a researcher, you will spend most of your time tuning these "knobs":

### 4.1 `--depth` (The Resolution)
This is the most critical parameter.
- **Depth 6-7:** Very fast, "blobby" results. Good for testing.
- **Depth 9-10:** Standard for high-quality meshes.
- **Depth 12+:** Extreme detail, requires significant RAM (16GB+).

### 4.2 `--pointWeight` (The "Screening" Strength)
- **Low (0-2):** Produces very smooth, rounded surfaces. Good for noisy data.
- **High (10-20):** Forces the surface to go exactly through your points. Can lead to "spiky" artifacts if the data is noisy.

### 4.3 `--samplesPerNode` (Noise Filter)
Think of this as a "minimum density" requirement. 
- For clean synthetic data: `1.0`.
- For noisy real-world scans: `15.0` or higher.

---

## Chapter 5: Post-Processing with Surface Trimmer

Poisson Reconstruction creates **watertight** meshes. If you scan a flat piece of paper, it will try to "close" it into a balloon. To fix this, we use `surftrimmer`.

1.  **Run Recon with Density:**
    ```bash
    ./surfrecon --in scan.ply --out mesh.ply --density
    ```
2.  **Trim Low-Density Regions:**
    ```bash
    ./surftrimmer --in mesh.ply --trim 5.0 --out trimmed.ply
    ```
This removes parts of the mesh where there were very few input points (the "guessed" parts).

---

## Chapter 6: Troubleshooting

| Issue | Likely Cause | Solution |
| :--- | :--- | :--- |
| **Inside-Out Mesh** | Normals are flipped. | Check your scanner software or use a tool to flip normals. |
| **Crashes at High Depth** | Out of Memory. | Decrease `--depth` or increase swap space. |
| **"Bubbly" Surface** | Too much noise. | Increase `--samplesPerNode`. |
| **Holes are closed** | Intended behavior. | Use `surftrimmer` as shown in Chapter 5. |

---

## Glossary

- **Normals:** Vectors perpendicular to the surface.
- **Marching Cubes:** An algorithm that turns a grid of values into a triangle mesh.
- **Divergence:** A vector operator that measures the "outward flow" from a point.
- **Manifold:** A surface that is "well-behaved" (no self-intersections, no impossible edges).

---
*Happy Reconstructing! If you find bugs, please report them to the project maintainers.*
