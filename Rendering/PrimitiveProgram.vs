$input a_position, a_normal, a_texcoord0
$output v_texcoord0, v_normal

#include <common.sh>

void main()
{
    v_texcoord0 = a_texcoord0;
    v_normal = a_normal;

    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
