// --- Inputs ---
// float4 Color
// float2 PixelSize

float2 SceneColor = GetDefaultSceneTextureUV(Parameters, 1);

// Viewport 관련 정보
float2 ViewportSize     = View.ViewSizeAndInvSize.xy;
float2 InvViewportSize  = View.ViewSizeAndInvSize.zw;
float  ViewportRatio    = ViewportSize.y / ViewportSize.x;

// 픽셀 크기 조정 (비율 보정)
float2 PixelViewportSize = PixelSize * float2(1.0, ViewportRatio);

// 현재 픽셀이 속한 정규화된 픽셀 중심 위치 구하기
float2 RelativeUvToPixel = floor(SceneColor * PixelViewportSize) + float2(0.5, 0.5);
float2 TargetUv          = RelativeUvToPixel / PixelViewportSize;

// UV를 정확한 텍셀 중심으로 정렬
float2 TexelSize     = InvViewportSize;
float2 HalfTexelSize = TexelSize * 0.5;
TargetUv             = TargetUv - fmod(TargetUv, TexelSize) + HalfTexelSize;

// 최종 결과 출력
return SceneTextureLookup(TargetUv, 14, false).xyz;
