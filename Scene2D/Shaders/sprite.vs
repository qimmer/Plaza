$input a_position, a_texcoord0
$output v_texcoord0

#include <common.sh>

uniform vec4 uvOffsetScale;

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 0.0, 1.0) );

	v_texcoord0 = uvOffsetScale.xy + (a_texcoord0 * uvOffsetScale.zw);
}
