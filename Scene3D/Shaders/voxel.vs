$input a_xyz_vindex, a_palette_ao_height
$output v_uv, v_ao_palette, v_normal

#include <common.sh>

uniform vec4 voxel_face_vertex_positions_us[36];
uniform vec4 voxel_face_vertex_normals_vs[36];

void main()
{
    int vertex_index = int(a_xyz_vindex.a * 255.0);
    int paletteIndex = int(a_palette_ao_height.x * 255.0);
    float ao = a_palette_ao_height.z;
    float height = float(a_palette_ao_height.w) / 255.0;

    vec3 position = voxel_face_vertex_positions_us[vertex_index].xyz + (a_xyz_vindex.xyz * 255.0);
    position.y = position.y * height;

    v_normal = voxel_face_vertex_normals_vs[vertex_index].xyz;
    v_uv = vec2(voxel_face_vertex_positions_us[vertex_index].w, voxel_face_vertex_normals_vs[vertex_index].w);
    v_ao_palette = vec2(ao, floor(paletteIndex) / 256.0);

    gl_Position = mul(u_modelViewProj, vec4(position, 1.0));
}
