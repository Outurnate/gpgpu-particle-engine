__kernel void particlePhysics(__global float2 *pos, __global float2 *vel, __global float3 *col, __global float3 *pointf)
{
  unsigned i = get_global_id(0);
  float2 p = pos[i];
  float3 c = col[i];
  float2 v = vel[i];

  v.x += ((pointf[0].x - p.x) / 5000.0f) * pointf[0].z * (((i % 5) + 1) / 6.0f)/* * vel[i].z*/;
  v.y += ((pointf[0].y - p.y) / 5000.0f) * pointf[0].z * (((i % 5) + 1) / 6.0f)/* * vel[i].z*/;

  v.y -= 0.000015f;

  p += vel[i].xy;

  p = clamp(p, -1, 1);

  pos[i] = p;
  col[i] = c;
  vel[i] = v;
}