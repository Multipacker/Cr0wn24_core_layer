#ifndef D3D11_H
#define D3D11_H

#define COBJMACROS

#include <d3d11.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>
#include <dxgidebug.h>

#pragma comment (lib, "gdi32")
#pragma comment (lib, "user32")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "d3d11")
#pragma comment (lib, "d3dcompiler")

#define AssertHR(hr) Assert(SUCCEEDED(hr))

typedef struct D3D11Texture
{
	ID3D11ShaderResourceView* view;
	ID3D11Texture2D* texture;
} D3D11Texture;

typedef struct D3D11State
{
	IDXGISwapChain1 *swap_chain;
	ID3D11Device *device;
	ID3D11DeviceContext *context;

	ID3D11RenderTargetView *rtview;
	ID3D11DepthStencilView *dsview;

	ID3D11Buffer *v_buffer;
	ID3D11Buffer *ubuffer;

	ID3D11InputLayout *layout;
	ID3D11VertexShader *v_shader;
	ID3D11PixelShader *p_shader;

	ID3D11InputLayout *line_layout;
	ID3D11VertexShader *line_vertex_shader;
	ID3D11PixelShader *line_pixel_shader;

	ID3D11DepthStencilState *depthState;
	ID3D11RasterizerState* rasterizerState;
	ID3D11BlendState* blend_state;
	ID3D11BlendState* line_blend_state;
	ID3D11SamplerState* sampler;

	DWORD current_width;
	DWORD current_height;

	OS_Window *window;
} D3D11State;

internal void D3D11_End();

#endif