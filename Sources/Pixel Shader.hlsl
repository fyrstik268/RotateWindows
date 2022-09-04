#include "..\Headers\Shader Headers.hlsli"

SamplerState Sampler;
Texture2D Texture;

float4 Main(PixelShaderInput Input): SV_Target {
    return Texture.Sample(Sampler, Input.TexCoords);
}