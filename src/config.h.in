//-----------------------------------------------------------------------//
//A GPU based particle engine                                            //
//Copyright (C) 2013 Joseph Dillon                                       //
//-----------------------------------------------------------------------//
//This program is free software: you can redistribute it and/or modify   //
//it under the terms of the GNU General Public License as published by   //
//the Free Software Foundation, either version 3 of the License, or      //
//(at your option) any later version.                                    //
//                                                                       //
//This program is distributed in the hope that it will be useful,        //
//but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//GNU General Public License for more details.                           //
//                                                                       //
//You should have received a copy of the GNU General Public License      //
//along with this program.  If not, see <http://www.gnu.org/licenses/>.  //
//-----------------------------------------------------------------------//
#define VERSION_MAJOR @gpgpu-particle-engine_VERSION_MAJOR@
#define VERSION_MINOR @gpgpu-particle-engine_VERSION_MINOR@

#if (defined LINUX) && (defined WIN32)
#error Multiple platforms defined!
#endif
#if !((defined LINUX) || (defined WIN32))
#error No platforms defined
#endif

#if defined LINUX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#define clContextProp(window, platform) {\
  CL_GL_CONTEXT_KHR,   (cl_context_properties)glfwGetGLXContext(window),\
  CL_GLX_DISPLAY_KHR,  (cl_context_properties)glfwGetX11Display(),\
  CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),\
  0\
}
#elif defined WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define clContextProp(window, platform) {\
  CL_GL_CONTEXT_KHR,   (cl_context_properties)glfwGetWGLContext(window),\
  CL_WGL_HDC_KHR,      (cl_context_properties)wglGetCurrentDC(),\
  CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),\
  0\
}
#else
#define clContextProp(window, platform)
#endif
