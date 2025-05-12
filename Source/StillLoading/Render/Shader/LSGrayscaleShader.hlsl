// ---Input ---
// float4 Color
// float Brightness  // (0.0 ~ 2.0 정도 추천, 기본값 1.0)

float Gray = dot(Color.rgb, float3(0.3, 0.59, 0.11));
Gray *= Brightness; // 밝기 조절

Gray = saturate(Gray); // 0~1로 클램핑

return float4(Gray, Gray, Gray, Color.a);
