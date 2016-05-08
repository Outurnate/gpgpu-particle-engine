#include "frame.hpp"

#include <iostream>

ParticleKernel::ParticleKernel(cl::Context context, cl::Device device)
  : context(context)
{
  cl::Program::Sources sources;

  std::string kernelSource(
"inline float2 point_force(float2 p1, float2 p2, float inv_mag, float strength)"
"{"
"  float2 ret;"
"  float theta = atan2(p1.y - p2.y, p1.x - p2.x);"
"  ret.x = cos(theta) / inv_mag;"
"  ret.y = sin(theta) / inv_mag;"
"  return (ret * strength);"
"}"
"__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *pointf, __global float *mass)"
"{"
"  unsigned i = get_global_id(0);"
"  float2 p = pos[i];"
"  float2 v = vel[i];"
"  v += (point_force(pointf[0].xy, p, 3000.0f, pointf[0].z)) * mass[i];"
"  p += vel[i].xy;"
"  v *= 0.997f;"
"  p = clamp(p, -1.001f - mass[i], 1.001f + mass[i]);"
"  v = clamp(v, -0.05f, 0.05f);"
"  pos[i] = p;"
"  vel[i] = v;"
"}");
  
  sources.push_back({ kernelSource.c_str(), kernelSource.size() });
  cl::Program program(context, sources);

  std::vector<cl::Device> devices;
  devices.push_back(device);
  try
  {
    program.build(devices);
  }
  catch(cl::Error)
  {
    std::cerr << "CL build error: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
    exit(EXIT_FAILURE);
  }
  
  queue = cl::CommandQueue(context, device);
  kernel = cl::Kernel(program, "particlePhysics");
}

void ParticleKernel::RunSystem(const ParticleSystem& ps)
{
  std::vector<cl::Memory> vbo({ ps.clVBO });
  kernel.setArg(0, ps.clVBO);
  kernel.setArg(1, ps.velocityBuffer);
  kernel.setArg(2, ps.forceBuffer);
  kernel.setArg(3, ps.massBuffer);
  queue.finish();
  queue.enqueueAcquireGLObjects(&vbo);
  queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(ps.particleCount), cl::NullRange);
  queue.enqueueReleaseGLObjects(&vbo);
  queue.finish();
}

cl::Context& ParticleKernel::GetContext()
{
  return context;
}

cl::CommandQueue& ParticleKernel::GetQueue()
{
  return queue;
}
