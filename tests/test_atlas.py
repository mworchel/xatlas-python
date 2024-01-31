import os

import numpy as np
import pytest
import trimesh
import xatlas

# current working directory
cwd = os.path.abspath(os.path.expanduser(os.path.dirname(__file__)))


def test_add_mesh():
    mesh = trimesh.load_mesh(os.path.join(cwd, "data", "00190663.obj"))

    atlas = xatlas.Atlas()

    # Positions have wrong shape (number of dimensions)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(np.random.rand(1), mesh.faces)
    assert "Nx3" in str(e.value)

    # Positions have wrong shape (second dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(np.random.rand(1, 1), mesh.faces)
    assert "Nx3" in str(e.value)

    # Indices have wrong shape (number of dimensions)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(mesh.vertices, np.random.rand(1))
    assert "Nx3" in str(e.value)

    # Indices have wrong shape (second dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(mesh.vertices, np.random.rand(1, 1))
    assert "Nx3" in str(e.value)

    # Normals have wrong shape (number of dimensions)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(mesh.vertices, mesh.faces, np.random.rand(1))
    assert "Nx3" in str(e.value)

    # Normals have wrong shape (second dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(mesh.vertices, mesh.faces, np.random.rand(1, 1))
    assert "Nx3" in str(e.value)

    # Normals have wrong shape (first dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(mesh.vertices, mesh.faces, np.random.rand(1, 3))
    assert "first dimension" in str(e.value)

    # UVs have wrong shape (number of dimensions)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(
            mesh.vertices, mesh.faces, mesh.vertex_normals, np.random.rand(1)
        )
    assert "Nx2" in str(e.value)

    # UVs have wrong shape (second dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(
            mesh.vertices, mesh.faces, mesh.vertex_normals, np.random.rand(1, 1)
        )
    assert "Nx2" in str(e.value)

    # UVs have wrong shape (first dimension)
    with pytest.raises(ValueError) as e:
        atlas.add_mesh(
            mesh.vertices, mesh.faces, mesh.vertex_normals, np.random.rand(1, 2)
        )
    assert "first dimension" in str(e.value)

    # Index array references out-of-bounds vertices
    with pytest.raises(RuntimeError) as e:
        atlas.add_mesh(np.random.rand(1, 3), mesh.faces)
    assert "out of range" in str(e.value)


def test_generate():
    mesh = trimesh.load_mesh(os.path.join(cwd, "data", "00190663.obj"))

    atlas = xatlas.Atlas()
    atlas.add_mesh(mesh.vertices, mesh.faces, mesh.vertex_normals)

    atlas.generate()

    assert atlas.mesh_count == 1
    assert len(atlas) == 1

    vmapping, indices, uvs = atlas.get_mesh(0)
    assert vmapping.shape == (18996,)
    assert indices.shape == (32668, 3)
    assert uvs.shape == (18996, 2)

    vmapping, indices, uvs = atlas[0]
    assert vmapping.shape == (18996,)
    assert indices.shape == (32668, 3)
    assert uvs.shape == (18996, 2)

    assert atlas.chart_count == 70
    assert atlas.width >= 900
    assert atlas.height >= 900


def test_get_mesh():
    mesh = trimesh.load_mesh(os.path.join(cwd, "data", "00190663.obj"))

    atlas = xatlas.Atlas()

    # No mesh in an empty atlas
    with pytest.raises(IndexError) as e:
        atlas.get_mesh(0)
    assert "out of bounds" in str(e.value)

    atlas.add_mesh(mesh.vertices, mesh.faces, mesh.vertex_normals)
    atlas.generate()

    # One mesh in the atlas
    _, _, _ = atlas.get_mesh(0)

    with pytest.raises(IndexError) as e:
        atlas.get_mesh(1)
    assert "out of bounds" in str(e.value)
