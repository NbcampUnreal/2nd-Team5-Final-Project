// ---Input ---
// float4 Color
// float Brightness

float Gray = dot(Color.rgb, float3(0.3, 0.59, 0.11));
Gray *= Brightness;

Gray = saturate(Gray);

return float4(Gray, Gray, Gray, Color.a);
