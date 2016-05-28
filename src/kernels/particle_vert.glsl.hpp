#include "compiler_magic.hpp"
constexpr const char* particle_vert_glsl =
  #include "particle_vert.glsl"
  ;
constexpr size_t particle_vert_glsl_len = com_length(particle_vert_glsl);
