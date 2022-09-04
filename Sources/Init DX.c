#include <RotWnd.h>
#include <Vertex Shader.h>
#include <Pixel Shader.h>

void InitDX(void) {
	static const D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, FeatureLevels, ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION, &Fejalink.DX.Device, NULL, &Fejalink.DX.DC);

	static const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[] = {
		[0] .SemanticName = "POSITION",
		[0].SemanticIndex = 0,
		[0].Format = DXGI_FORMAT_R32G32_FLOAT,
		[0].InputSlot = 0,
		[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
		[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		[0].InstanceDataStepRate = 0,
		[1].SemanticName = "TEXCOORD",
		[1].SemanticIndex = 0,
		[1].Format = DXGI_FORMAT_R32G32_FLOAT,
		[1].InputSlot = 0,
		[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT,
		[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
		[1].InstanceDataStepRate = 0
	};
	ID3D11InputLayout* restrict InputLayout;
	ID3D11Device_CreateInputLayout(Fejalink.DX.Device, InputLayoutDesc, ARRAYSIZE(InputLayoutDesc), VertexShaderBytecode, sizeof(VertexShaderBytecode), &InputLayout);
	ID3D11DeviceContext_IASetInputLayout(Fejalink.DX.DC, InputLayout);
	ID3D11InputLayout_Release(InputLayout);
	ID3D11DeviceContext_IASetPrimitiveTopology(Fejalink.DX.DC, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11VertexShader* restrict VertexShader;
	ID3D11Device_CreateVertexShader(Fejalink.DX.Device, VertexShaderBytecode, sizeof(VertexShaderBytecode), NULL, &VertexShader);
	ID3D11DeviceContext_VSSetShader(Fejalink.DX.DC, VertexShader, NULL, 0);
	ID3D11VertexShader_Release(VertexShader);

	ID3D11PixelShader* restrict PixelShader;
	ID3D11Device_CreatePixelShader(Fejalink.DX.Device, PixelShaderBytecode, sizeof(PixelShaderBytecode), NULL, &PixelShader);
	ID3D11DeviceContext_PSSetShader(Fejalink.DX.DC, PixelShader, NULL, 0);
	ID3D11PixelShader_Release(PixelShader);

	static const D3D11_SAMPLER_DESC SamplerDesc = {
		.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
		.MipLODBias = 0.f,
		.MaxAnisotropy = 1,
		.ComparisonFunc = D3D11_COMPARISON_ALWAYS,
		.BorderColor = {0xCF, 0x37, 0xFE},
		.MinLOD = -(D3D11_FLOAT32_MAX),
		.MaxLOD = D3D11_FLOAT32_MAX
	};
	ID3D11SamplerState* restrict SamplerState;
	ID3D11Device_CreateSamplerState(Fejalink.DX.Device, &SamplerDesc, &SamplerState);
	ID3D11DeviceContext_PSSetSamplers(Fejalink.DX.DC, 0, 1, &SamplerState);
	ID3D11SamplerState_Release(SamplerState);

	static const D3D11_RASTERIZER_DESC RasterizerDesc = {
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_NONE,
		.FrontCounterClockwise = FALSE,
		.DepthBias = 0,
		.DepthBiasClamp = 0.f,
		.SlopeScaledDepthBias = 0.f,
		.DepthClipEnable = FALSE,
		.ScissorEnable = FALSE,
		.MultisampleEnable = FALSE,
		.AntialiasedLineEnable = FALSE
	};
	ID3D11RasterizerState* restrict RasterizerState;
	ID3D11Device_CreateRasterizerState(Fejalink.DX.Device, &RasterizerDesc, &RasterizerState);
	ID3D11DeviceContext_RSSetState(Fejalink.DX.DC, RasterizerState);
	ID3D11RasterizerState_Release(RasterizerState);

	return;
}