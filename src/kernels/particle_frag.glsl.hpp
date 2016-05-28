#include "compiler_magic.hpp"
constexpr const char* particle_frag_glsl =
  #include "particle_frag.glsl"
  ;
constexpr size_t particle_frag_glsl_len = com_length(particle_frag_glsl);
