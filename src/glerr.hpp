#ifdef DEBUG
inline bool CHECK_decode(GLenum err)
{
  switch(err)
  {
  case GL_INVALID_ENUM:
    std::cout << "GL_INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    std::cout << "GL_INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    std::cout << "GL_INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    std::cout << "GL_STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    std::cout << "GL_STACK_UNDERFLOW";
    break;
  }
  return err != GL_NO_ERROR;
}
#define CHECK ;if(CHECK_decode(glGetError()))std::cout << " " << __FILE__ << ":" << __LINE__ << std::endl
#endif
