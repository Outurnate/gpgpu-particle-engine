R"(
#version 420 core

in vec2 position;
in vec3 color;
out vec3 ocolor;

void main()
{
  gl_Position = vec4(position, 0.0, 1.0);
  ocolor = color;
}
)"