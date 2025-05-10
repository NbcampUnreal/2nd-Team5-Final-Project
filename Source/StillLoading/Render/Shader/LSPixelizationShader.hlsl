float pixelSize = in_pixelSize;

float2 viewportUv = GetDefaultSceneTextureUV(Parameters, 1);

float2 viewportSize = View.ViewSizeAndInvSize.xy;
float viewportRatio = viewportSize.y / viewportSize.x;
float2 pixelViewportSize = pixelSize * float2(1.0, viewportRatio);

float2 relativeUvToPixel = floor(viewportUv * pixelViewportSize) + float2(0.5, 0.5);
float2 uv = relativeUvToPixel / pixelViewportSize;

float2 texelSize = View.ViewSizeAndInvSize.zw;
float2 halfTexelSize = texelSize * 0.5;

uv = uv - fmod(uv, texelSize) + halfTexelSize;

return SceneTextureLookup(uv, 14,false).xyz;
