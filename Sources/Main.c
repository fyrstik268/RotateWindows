#include <RotWnd.h>

struct Fejalink Fejalink;

/* DWMWA_CLOAK: Hides window, still compositioned in DWM, input of type BOOL (32-bit) */
/* Set up VBO, IBO, Texture, Copy client window to texture, redraw texture rotated into layered output window, etc... */

struct Vertex {
	float Position[2];
	float TexCoord[2];
};

__declspec(noreturn) void __cdecl MainEntrypoint(void) {
	MessageBoxW(NULL, L"NB! Project is still in development, you need to kill the process in task manager.", L"Preliminary Notice", MB_OK | MB_ICONINFORMATION);
	Fejalink.ProcessModule = GetModuleHandleW(NULL);
	CreateWindows();
	InitDX();

	/* Test Render Time! */
	SetLayeredWindowAttributes(Fejalink.OutputWindow, RGB(0xCF, 0x37, 0xFE), 0xFF, LWA_COLORKEY);
	RECT WndRect;
	GetWindowRect(Fejalink.ClientWindow, &WndRect);
	const float Formula = sqrtf(powf((float)(WndRect.right - WndRect.left), 2.f) / 2) + sqrtf(powf((float)(WndRect.bottom - WndRect.top), 2.f) / 2.f);
	ShowWindow(Fejalink.ClientWindow, SW_SHOW);

	static const D3D11_BUFFER_DESC VBODesc = {
		.ByteWidth = sizeof(struct Vertex) * 4,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0
	};
	const float VertPoint = sqrtf(powf((float)(WndRect.right - WndRect.left), 2) / 2);
	const struct Vertex Verticies[4] = {
		[0].Position = {-(VertPoint / (Formula / 2.f) - 1.f), 1.f},
		[0].TexCoord = {0.f, 1.f},
		[1].Position = {-1.f, (VertPoint / (Formula / 2.f) - 1.f)},
		[1].TexCoord = {0.f, 0.f},
		[2].Position = {VertPoint / (Formula / 2.f) - 1.f, -1.f},
		[2].TexCoord = {1.f, 0.f},
		[3].Position = {1.f, -(VertPoint / (Formula / 2.f) - 1.f)},
		[3].TexCoord = {1.f, 1.f},
	};
	D3D11_SUBRESOURCE_DATA InitData = {
		.pSysMem = Verticies,
		.SysMemPitch = sizeof(Verticies)
	};
	ID3D11Buffer* restrict VBO;
	ID3D11Device_CreateBuffer(Fejalink.DX.Device, &VBODesc, &InitData, &VBO);
	UINT VertexStride = sizeof(struct Vertex), VertexOffset = 0;
	ID3D11DeviceContext_IASetVertexBuffers(Fejalink.DX.DC, 0, 1, &VBO, &VertexStride, &VertexOffset);

	static const D3D11_BUFFER_DESC IBODesc = {
		.ByteWidth = sizeof(WORD) * 6,
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0
	};
	static const WORD IBOData[6] = {0, 1, 2, 2, 0, 3};
	InitData.pSysMem = IBOData;
	InitData.SysMemPitch = sizeof(IBOData);
	ID3D11Buffer* restrict IBO;
	ID3D11Device_CreateBuffer(Fejalink.DX.Device, &IBODesc, &InitData, &IBO);
	ID3D11DeviceContext_IASetIndexBuffer(Fejalink.DX.DC, IBO, DXGI_FORMAT_R16_UINT, 0);

	D3D11_VIEWPORT Viewport = {
		.Width = Formula,
		.Height = Formula
	};
	ID3D11DeviceContext_RSSetViewports(Fejalink.DX.DC, 1, &Viewport);

	D3D11_TEXTURE2D_DESC TextureDesc = {
		.Width = Formula,
		.Height = Formula,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
		.SampleDesc.Count = 1,
		.SampleDesc.Quality = 0,
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET,
		.CPUAccessFlags = 0,
		.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE
	};
	ID3D11Device_CreateTexture2D(Fejalink.DX.Device, &TextureDesc, NULL, &Fejalink.DX.PostRotateRender);

	TextureDesc.Width = WndRect.right - WndRect.left;
	TextureDesc.Height = WndRect.bottom - WndRect.top;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.MiscFlags = 0;
	ID3D11Device_CreateTexture2D(Fejalink.DX.Device, &TextureDesc, NULL, &Fejalink.DX.PreRotateRender);
	ID3D11ShaderResourceView* restrict SRV;
	ID3D11Device_CreateShaderResourceView(Fejalink.DX.Device, Fejalink.DX.PreRotateRender, NULL, &SRV);
	ID3D11DeviceContext_PSSetShaderResources(Fejalink.DX.DC, 0, 1, &SRV);
	ID3D11ShaderResourceView_Release(SRV);

	MSG Message;
	while(TRUE) {
		while(PeekMessageW(&Message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessageW(&Message);
		}
		InvalidateRect(Fejalink.ClientWindow, NULL, TRUE);
		HDC const DC = CreateCompatibleDC(GetDC(Fejalink.ClientWindow));
		BITMAPINFO BitmapInfo = {
			.bmiHeader.biSize = sizeof(BITMAPINFOHEADER),
			.bmiHeader.biWidth = 660,
			.bmiHeader.biHeight = 523,
			.bmiHeader.biPlanes = 1,
			.bmiHeader.biBitCount = 32,
			.bmiHeader.biCompression = BI_RGB
		};
		BYTE* Bits;
		HBITMAP Bitmap = CreateDIBSection(DC, &BitmapInfo, DIB_RGB_COLORS, &Bits, NULL, 0);
		SelectObject(DC, Bitmap);
		PrintWindow(Fejalink.ClientWindow, DC, 0);
		ID3D11DeviceContext_UpdateSubresource(Fejalink.DX.DC, Fejalink.DX.PreRotateRender, 0, NULL, Bits, 660 * 4, 0);
		ReleaseDC(Fejalink.ClientWindow, DC);
		DeleteObject(Bitmap);
		DeleteDC(DC);

		ID3D11Device_CreateRenderTargetView(Fejalink.DX.Device, (ID3D11Resource*)Fejalink.DX.PostRotateRender, NULL, &Fejalink.DX.RenderTarget);
		ID3D11DeviceContext_OMSetRenderTargets(Fejalink.DX.DC, 1, &Fejalink.DX.RenderTarget, NULL);
		float ClearColour[4] = {(float)0xCF / 0xFF, (float)0x37 / 0xFF, (float)0xFE / 0xFF, 1.f};
		ID3D11DeviceContext_ClearRenderTargetView(Fejalink.DX.DC, Fejalink.DX.RenderTarget, ClearColour);
		ID3D11DeviceContext_DrawIndexed(Fejalink.DX.DC, 6, 0, 0);
		ID3D11RenderTargetView_Release(Fejalink.DX.RenderTarget);

		InvalidateRect(Fejalink.OutputWindow, NULL, FALSE);
		Sleep(20);
	}

	ExitProcess(0);
}