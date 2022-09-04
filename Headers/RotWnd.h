#pragma once
#define CINTERFACE
#define COBJMACROS
#include <Windows.h>
#include <math.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <Resources.h>

/* Kinda become a habit of mine to blend multiple (spoken) languages together in a project. Don't be surprised if there's more than this struct */
extern struct Fejalink {
	HMODULE ProcessModule;
	ATOM ClientWndClass, OutputWndClass;
	HWND OutputWindow, ClientWindow;
	struct {
		ID3D11Device* Device;
		ID3D11DeviceContext* DC;
		ID3D11Texture2D* PreRotateRender, *PostRotateRender;
		ID3D11RenderTargetView* RenderTarget;
	} DX;
} Fejalink;

void CreateWindows(void);
void InitDX(void);