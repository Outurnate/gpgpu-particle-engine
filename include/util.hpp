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
#ifndef UTILH
#define UTILH

#include <GL/glew.h>
#include <GL/gl.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>

inline void showShaderLog(GLuint object, PFNGLGETSHADERIVPROC glGet__iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
{
    GLint length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &length);
    log = (char*)malloc(length);
    glGet__InfoLog(object, length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

#endif
