__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *col)
{
  unsigned i = get_global_id(0);
  /*float2 p = pos[i];
  float2 v = vel[i];

  p += v;

  pos[i] = p;
  vel[i] = v;*/
  pos[i].x = 1;
}