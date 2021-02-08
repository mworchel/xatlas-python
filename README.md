# Python bindings for xatlas

(Unofficial) Python bindings for [xatlas](https://github.com/jpcy/xatlas), a library that generates texture coordinates for triangle meshes.

## Installation

### From source

```bash
git clone --recursive https://github.com/mworchel/xatlas-python.git
pip install ./xatlas-python
```

### Using Pip

```bash
pip install xatlas
```

## Usage

### Parametrize a mesh and export it

```python
import trimesh
import xatlas

# We use trimesh (https://github.com/mikedh/trimesh) to load a mesh but you can use any library.
mesh = trimesh.load_mesh("input.obj")

# The parametrization potentially duplicates vertices.
# `vmapping` contains the original vertex index for each new vertex (shape N, type uint32).
# `indices` contains the vertex indices of the new triangles (shape Fx3, type uint32)
# `uvs` contains texture coordinates of the new vertices (shape Nx2, type float32)
vmapping, indices, uvs = xatlas.parametrize(mesh.vertices, mesh.faces)

# Trimesh does not correctly export the parametrized mesh, so we 
# can use the `export` helper function to export the mesh as obj.
xatlas.export("output.obj", mesh.vertices[vmapping], indices, uvs)

# Both `xatlas.parametrize` and `xatlas.export` also accept vertex normals
```

### Parametrize multiple meshes using one atlas

```python
mesh1 = trimesh.load_mesh("input1.obj")
mesh2 = trimesh.load_mesh("input2.obj")

atlas = xatlas.Atlas()

atlas.add_mesh(mesh1.vertices, mesh1.faces)
atlas.add_mesh(mesh2.vertices, mesh2.faces)

# Optionally parametrize the generation with
# `xatlas.ChartOptions` and `xatlas.PackOptions`.
atlas.generate()

vmapping1, indices1, uvs1 = atlas[0]
vmapping2, indices2, uvs2 = atlas[1]
```

### Query the atlas

```python
atlas.mesh_count  # Number of meshes
len(atlas)        # Convenience binding for `atlas.mesh_count`
atlas.get_mesh(i) # Data for the i-th mesh
atlas[i]          # Convenience binding for `atlas.get_mesh`

atlas.width       # Width of the atlas 
atlas.height      # Height of the atlas

...               # See xatlas documentation for all properties
```

## License

The xatlas Python bindings are provided under a MIT license. By using, distributing, or contributing to this project, you agree to the terms and conditions of this license.

## References

Test model taken from the [ABC dataset](https://deep-geometry.github.io/abc-dataset/)
