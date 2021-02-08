import pytest
import trimesh
import xatlas

def test_parametrize():
    mesh = trimesh.load_mesh("tests/data/00190663.obj")

    vmapping, indices, uvs = xatlas.parametrize(mesh.vertices, mesh.faces, mesh.vertex_normals)
    assert vmapping.shape == (18996,)
    assert indices.shape == (32668, 3)
    assert uvs.shape == (18996, 2)