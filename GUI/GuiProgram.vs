$input a_position, a_normal, a_texcoord0, a_texcoord1
$output v_texcoord0, v_normal

#include <common.sh>

uniform vec4 u_widgetSize;

void main()
{
    v_texcoord0 = a_texcoord0;
    v_normal = a_normal;

    vec3 position = a_position;
    position.xy += vec2(a_texcoord1.x * u_widgetSize.x, a_texcoord1.y * u_widgetSize.y);

    gl_Position = mul(u_modelViewProj, vec4(position, 1.0));
}
