inline float2 point_force(float2 p1, float2 p2, float inv_mag, float strength)
{
  float2 ret;
  float theta = atan2(p1.y - p2.y, p1.x - p2.x);
  ret.x = cos(theta) / inv_mag;
  ret.y = sin(theta) / inv_mag;
  return (ret * strength);
}

__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *pointf, __global float *mass)
{
  unsigned i = get_global_id(0);
  float2 p = pos[i];
  float2 v = vel[i];

  v += (point_force(pointf[0].xy, p, 3000.0f, pointf[0].z)
     +  point_force(0.0f, p, 5000.0f, -1.0f)) * mass[i];

  p += vel[i].xy;

  v *= 0.997f;

  p = clamp(p, -1.001f, 1.001f);
  v = clamp(v, -0.05f, 0.05f);

  pos[i] = p;
  vel[i] = v;
}