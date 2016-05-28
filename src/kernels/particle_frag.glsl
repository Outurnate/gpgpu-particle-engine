R"(
#version 420 core

in vec3 ocolor;
out vec4 colorOut;
uniform sampler2D pointsprite;

void main()
{
  vec4 texCol = texture(pointsprite, gl_PointCoord);
  colorOut = vec4(texCol.rgb * vec4(ocolor, 1.0).rgb, texCol.a);
}
)"