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
#include "config.h"

#include "frame.hpp"

#include <iostream>
#include <cstdio>
#include <sstream>
#include <CL/cl.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

static void error(int error, const char* description)
{
  std::cout << "Error: " << error << ": " << description << std::endl;
}

int main()
{
  std::cout <<
"<program>  Copyright (C) <year>  <name of author>\n\
This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n\
This is free software, and you are welcome to redistribute it\n\
under certain conditions; type `show c' for details.\n" << std::endl;

  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (platforms.size() == 0)
  {
    std::cerr << "No platforms available." << std::endl;
    return EXIT_FAILURE;
  }
  for (unsigned i = 0; i < platforms.size(); ++i)
  {
    std::string name, profile, vendor, version, extensions;
    platforms[i].getInfo(CL_PLATFORM_EXTENSIONS, &extensions);
    platforms[i].getInfo(CL_PLATFORM_NAME, &name);
    platforms[i].getInfo(CL_PLATFORM_PROFILE, &profile);
    platforms[i].getInfo(CL_PLATFORM_VENDOR, &vendor);
    platforms[i].getInfo(CL_PLATFORM_VERSION, &version);
    printf("Platform #%i:\nName:\t\t%s\nProfile:\t%s\nVendor:\t\t%s\nVersion:\t%s\nExtensions:\t%s\n\n",
	   i, name.c_str(), profile.c_str(), vendor.c_str(), version.c_str(), extensions.c_str());
  }
  cl::Platform platform;
  if (platforms.size() > 1)
  {
    std::string res;
    int choice;
    for (;;)
    {
      std::cout << "Multiple platforms exist.  Choice? ";
      std::getline(std::cin, res);
      if (!(std::stringstream(res) >> choice) || !(0 <= choice && choice < platforms.size()))
      {
	std::cerr << "Invalid choice" << std::endl;
	continue;
      }
      else break;
    }
    platform = platforms[choice];
  }
  else platform = platforms[0];

  std::vector<cl::Device> devices;
  platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
  if (devices.size() == 0)
  {
    std::cerr << "No devices available." << std::endl;
    return EXIT_FAILURE;
  }
  for (unsigned i = 0; i < devices.size(); ++i)
  {
    std::string name, profile, vendor, driver_version, device_version, extensions, cversion;
    devices[i].getInfo(CL_DEVICE_NAME, &name);
    devices[i].getInfo(CL_DEVICE_PROFILE, &profile);
    devices[i].getInfo(CL_DEVICE_VENDOR, &vendor);
    devices[i].getInfo(CL_DRIVER_VERSION, &driver_version);
    devices[i].getInfo(CL_DEVICE_VERSION, &device_version);
    devices[i].getInfo(CL_DEVICE_EXTENSIONS, &extensions);
    devices[i].getInfo(CL_DEVICE_OPENCL_C_VERSION, &cversion);
    printf("Device #%i:\nName:\t\t%s\nProfile:\t%s\nVendor:\t\t%s\nDriver Version:\t%s\nDevice Version:\t%s\nExtensions:\t%s\nC Version:\t%s\n\n",
	   i, name.c_str(), profile.c_str(), vendor.c_str(), driver_version.c_str(), device_version.c_str(), extensions.c_str(), cversion.c_str());
  }
  cl::Device device;
  if (devices.size() > 1)
  {
    std::string res;
    int choice;
    for (;;)
    {
      std::cout << "Multiple devices exist.  Choice? ";
      std::getline(std::cin, res);
      if (!(std::stringstream(res) >> choice) || !(0 <= choice && choice < devices.size()))
      {
	std::cerr << "Invalid choice" << std::endl;
	continue;
      }
      else break;
    }
    device = devices[choice];
  }
  else device = devices[0];
  std::cout << std::endl;

  GLFWwindow *window;

  glfwSetErrorCallback(error);
  if (!glfwInit())
    return EXIT_FAILURE;

  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

  if (!(window = glfwCreateWindow(800, 600, "GPU Particles", NULL, NULL)))
  {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

  cl_context_properties properties[] = clContextProp(window, platform);

  cl::Context context({ device }, properties);

  printf("GL Version:\t%s\nVendor:\t\t%s\nRenderer:\t%s\n", glGetString(GL_VERSION), glGetString(GL_VENDOR), glGetString(GL_RENDERER));
  Frame frame;
  frame.Init();

  while (!glfwWindowShouldClose(window))
  {
    frame.Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  frame.Destroy();

  glfwTerminate();
  return EXIT_SUCCESS;
}
