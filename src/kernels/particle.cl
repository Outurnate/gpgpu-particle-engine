#define MIN_BOUND -1.001f
#define MAX_BOUND  1.001f

inline float2 point_force(float2 p1, float2 p2, float inv_mag, float strength)
{
  float2 ret;
  float theta = atan2(p1.y - p2.y, p1.x - p2.x);
  ret.x = cos(theta) / inv_mag;
  ret.y = sin(theta) / inv_mag;
  return (ret * strength);
}

__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *col, __global float3 *pointf, __global float *mass)
{
  unsigned i = get_global_id(0);
  float2 p = pos[i];
  float3 c = col[i];
  float2 v = vel[i];

  v += point_force(pointf[0].xy, p, 3000.0f, pointf[0].z) * mass[i];

  v += point_force(0.0f, p, 5000.0f, -1.0f) * mass[i];
  /*v.x += ((0.0f - p.x) / 5000.0f) * -1.0f * mass[i];
  v.y += ((0.0f - p.y) / 5000.0f) * -1.0f * mass[i];*/

  p += vel[i].xy;

  if (!(p.x < MAX_BOUND && p.x > MIN_BOUND))
    v.x *= -0.5f;
  if (!(p.y < MAX_BOUND && p.y > MIN_BOUND))
    v.y *= -0.5f;

  v *= 0.997f;

  p.x = clamp(p.x, MIN_BOUND, MAX_BOUND);
  p.y = clamp(p.y, MIN_BOUND, MAX_BOUND);

  pos[i] = p;
  col[i] = c;
  vel[i] = v;
}