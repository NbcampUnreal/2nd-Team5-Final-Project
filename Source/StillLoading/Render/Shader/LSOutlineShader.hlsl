// --- Input ---
// float4 UVs
// float KERNEL_SIZE
// float2 NormalStepRange
// float2 DepthStepRange

float2 KernelUVs = GetDefaultSceneTextureUV(Parameters, 1);
float2 TexelSize = GetSceneTextureViewSize(1).zw;
float2 PixelUVs;

float3 LaplacianFilter_Normal = float3(0.0, 0.0, 0.0);
float LaplacianFilter_Depth = 0.0;
float CenterWeight = 0.0;

float HALF_KERNEL_SIZE = floor(KERNEL_SIZE / 2.0);
float HALF_KERNEL_SIZE_SQ = KERNEL_SIZE * KERNEL_SIZE / 4.0;

for (float y = -HALF_KERNEL_SIZE; y <= HALF_KERNEL_SIZE; y++)
{
	for (float x = -HALF_KERNEL_SIZE; x <= HALF_KERNEL_SIZE; x++)
	{
		if(x * x + y * y > HALF_KERNEL_SIZE_SQ)continue;

		CenterWeight++;
		PixelUVs = KernelUVs + TexelSize * float2(x, y);

		LaplacianFilter_Normal -= SceneTextureLookup(PixelUVs, 8, false).rgb;
		LaplacianFilter_Depth -= SceneTextureLookup(PixelUVs, 1, false).r;
	}
}

LaplacianFilter_Normal += SceneTextureLookup(KernelUVs, 8, false).rgb * (CenterWeight);
LaplacianFilter_Depth += SceneTextureLookup(KernelUVs, 1, false).r * (CenterWeight);

CenterWeight--;

CenterWeight = 1.0 / CenterWeight;

LaplacianFilter_Normal *= CenterWeight;
LaplacianFilter_Depth *= CenterWeight;

float MaxNormal = max(max(LaplacianFilter_Normal.r, LaplacianFilter_Normal.g), LaplacianFilter_Normal.b);
float StepNormal = (MaxNormal - NormalStepRange.x) / (NormalStepRange.y - NormalStepRange.x);
float StepDepth = (LaplacianFilter_Depth - DepthStepRange.x) / (DepthStepRange.y - DepthStepRange.x);
float Result =  1 - max(StepNormal, StepDepth);

return UVs * Result;