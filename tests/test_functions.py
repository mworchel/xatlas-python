import os
import trimesh
import xatlas

# current working directory
cwd = os.path.abspath(os.path.expanduser(os.path.dirname(__file__)))


def test_parametrize():
    mesh = trimesh.load_mesh(os.path.join(cwd, "data", "00190663.obj"))

    vmapping, indices, uvs = xatlas.parametrize(
        mesh.vertices, mesh.faces, mesh.vertex_normals
    )
    assert vmapping.shape == (18996,)
    assert indices.shape == (32668, 3)
    assert uvs.shape == (18996, 2)
