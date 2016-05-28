#include "compiler_magic.hpp"
constexpr const char* particle_cl =
  #include "particle.cl"
  ;
constexpr size_t particle_cl_len = com_length(particle_cl);
