// ---Input ---
// float4 Color

float4 GrayScale = dot(UVs, float3(0.3, 0.59, 0.11));
return GrayScale;