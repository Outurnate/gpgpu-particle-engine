#ifndef CMAGIC
#define CMAGIC

#include <cstddef>

std::size_t constexpr com_length(const char* str)
{
  auto orig = str;
  while (*(str++));
  return str - orig;
}

#endif
