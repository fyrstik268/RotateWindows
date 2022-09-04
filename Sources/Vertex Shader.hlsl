#include "..\Headers\Shader Headers.hlsli"

PixelShaderInput Main(float2 Position: POSITION, float2 TexCoords: TEXCOORD) {
	PixelShaderInput Output;
	Output.Position = float4(Position, 0.f, 1.f);
	Output.TexCoords = TexCoords;
	return Output;
}