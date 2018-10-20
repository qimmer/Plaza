$input a_position, a_normal, a_texcoord0, a_texcoord1
$output v_texcoord0, v_normal

#include <common.sh>

uniform ivec2 u_widgetSize;

void main()
{
    v_texcoord0 = a_texcoord0;
    v_normal = a_normal;

    vec3 position = a_position;
    //position.xy += mul(vec2(u_widgetSize), a_texcoord1);
    position.z = a_position.z;

    gl_Position = mul(u_modelViewProj, vec4(position, 1.0));
}
