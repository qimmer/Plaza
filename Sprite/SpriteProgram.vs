$input a_position, a_normal, a_texcoord0
$output v_texcoord0, v_normal

#include <common.sh>

void main()
{
    v_texcoord0 = a_texcoord0;
    v_normal = a_normal;

    vec3 position = a_position;
    gl_Position = mul(u_modelViewProj, vec4(position, 1.0));
}
