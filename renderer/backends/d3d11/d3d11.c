global D3D11State d3d11_state;

R_Handle D3D11_LoadTexture(void *data, S32 width, S32 height);

internal void
D3D11_Init(OS_Window *window)
{
	r_state = OS_AllocMem(sizeof(R_State));
	r_state->GPULoadTexture = D3D11_LoadTexture;

	Assert(window);

	d3d11_state.window = window;

	HRESULT hr;

	// NOTE(hampus): Create D3D11 device and context
	{
		U32 flags = 0;
#ifndef NDEBUG
		// NOTE(hampus): Enables VERY USEFUL debug messages
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};
		hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, levels, ArrayCount(levels),
													 D3D11_SDK_VERSION, &d3d11_state.device, 0, &d3d11_state.context);
		AssertHR(hr);
	}

#ifndef NDEBUG
	// NOTE(hampus): For debug builds enable VERY USEFUL debug break on API errors
	{
		ID3D11InfoQueue *info;
		ID3D11Device_QueryInterface(d3d11_state.device, &IID_ID3D11InfoQueue, (void **)&info);
		ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
		ID3D11InfoQueue_Release(info);
	}

	// NOTE(hampus): Enable debug break for DXGI too
	{
		IDXGIInfoQueue *dxgi_info;
		hr = DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, (void **)&dxgi_info);
		AssertHR(hr);
		IDXGIInfoQueue_SetBreakOnSeverity(dxgi_info, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		IDXGIInfoQueue_SetBreakOnSeverity(dxgi_info, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
		IDXGIInfoQueue_Release(dxgi_info);
	}

#endif

	// NOTE(hampus): Create DXI swap chain
	{
		// NOTE(hampus): Get DXGI device from D3D11 device
		IDXGIDevice *dxgi_device;
		hr = ID3D11Device_QueryInterface(d3d11_state.device, &IID_IDXGIDevice, (void **)&dxgi_device);
		AssertHR(hr);

		// NOTE(hampus): Get DXGI adapter for DXGI device
		IDXGIAdapter *dxgi_adapter;
		hr = IDXGIDevice_GetAdapter(dxgi_device, &dxgi_adapter);
		AssertHR(hr);

		// NOTE(hampus): Get DXGI factory from DXGI adapter
		IDXGIFactory2 *factory;
		hr = IDXGIAdapter_GetParent(dxgi_adapter, &IID_IDXGIFactory2, (void **)&factory);
		AssertHR(hr);

		DXGI_SWAP_CHAIN_DESC1 desc =
		{
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.SampleDesc = {1, 0},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = 2,
			.Scaling = DXGI_SCALING_NONE,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
		};

		hr = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown *)d3d11_state.device, window->handle, &desc, 0, 0, &d3d11_state.swap_chain);

		IDXGIFactory_MakeWindowAssociation(factory, window->handle, DXGI_MWA_NO_ALT_ENTER);

		IDXGIFactory2_Release(factory);
		IDXGIAdapter_Release(dxgi_adapter);
		IDXGIDevice_Release(dxgi_device);
	}

	{

		D3D11_BUFFER_DESC desc =
		{
			.ByteWidth = MEGABYTES(512),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
		};

		ID3D11Device_CreateBuffer(d3d11_state.device, &desc, 0, &d3d11_state.v_buffer);
	}

	{
		// NOTE(hampus): Rect shader
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"MIN", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(Rect, min), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"MAX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(Rect, max), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"MIN_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(Rect, min_uv), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"MAX_UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(Rect, max_uv), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MemberOffset(Rect, color), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"CORNER_RADIUS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MemberOffset(Rect, corner_radius), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"EXTRA_PARAMS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MemberOffset(Rect, edge_softness), D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

#include "core_layer/renderer/backends/d3d11/d3d11_vshader.h"
#include "core_layer/renderer/backends/d3d11/d3d11_pshader.h"

		ID3D11Device_CreateVertexShader(d3d11_state.device, d3d11_vshader, sizeof(d3d11_vshader), 0, &d3d11_state.v_shader);
		ID3D11Device_CreatePixelShader(d3d11_state.device, d3d11_pshader, sizeof(d3d11_pshader), 0, &d3d11_state.p_shader);
		ID3D11Device_CreateInputLayout(d3d11_state.device, desc, ArrayCount(desc), d3d11_vshader, sizeof(d3d11_vshader), &d3d11_state.layout);
	}

	// NOTE(hampus): Line shader
	{
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{"INST_POS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(LineVertex, inst_pos), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"DIR", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(LineVertex, dir), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"THICKNESS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, MemberOffset(LineVertex, thickness), D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, MemberOffset(LineVertex, color), D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};

#include "core_layer/renderer/backends/d3d11/d3d11_line_vshader.h"
#include "core_layer/renderer/backends/d3d11/d3d11_line_pshader.h"

		ID3D11Device_CreateVertexShader(d3d11_state.device, d3d11_line_vshader, sizeof(d3d11_line_vshader), 0, &d3d11_state.line_vertex_shader);
		ID3D11Device_CreatePixelShader(d3d11_state.device, d3d11_line_pshader, sizeof(d3d11_line_pshader), 0, &d3d11_state.line_pixel_shader);
		ID3D11Device_CreateInputLayout(d3d11_state.device, desc, ArrayCount(desc), d3d11_line_vshader, sizeof(d3d11_line_vshader), &d3d11_state.line_layout);
	}

	{
		D3D11_BUFFER_DESC desc =
		{
			// space for 4x4 float matrix (cbuffer0 from pixel shader)
			.ByteWidth = 4 * 4 * sizeof(F32),
			.Usage = D3D11_USAGE_DYNAMIC,
			.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		};
		ID3D11Device_CreateBuffer(d3d11_state.device, &desc, NULL, &d3d11_state.ubuffer);
	}

	{
		D3D11_SAMPLER_DESC desc =
		{
			.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
			.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
			.MaxAnisotropy = 1,
			.MinLOD = -FLT_MAX,
			.MaxLOD = +FLT_MAX,
		};

		ID3D11Device_CreateSamplerState(d3d11_state.device, &desc, &d3d11_state.sampler);
	}

	// NOTE(hampus): Rect 
	{
		// enable alpha blending
		D3D11_BLEND_DESC desc =
		{
			.RenderTarget[0] =
			{
				.BlendEnable = TRUE,
				.SrcBlend = D3D11_BLEND_SRC1_COLOR,
				.DestBlend = D3D11_BLEND_INV_SRC1_COLOR,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
				.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			},
		};
		ID3D11Device_CreateBlendState(d3d11_state.device, &desc, &d3d11_state.blend_state);
	}

	// NOTE(hampus): Line 
	{
		// enable alpha blending
		D3D11_BLEND_DESC desc =
		{
				.RenderTarget[0] =
				{
						.BlendEnable = TRUE,
						.SrcBlend = D3D11_BLEND_SRC_ALPHA,
						.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
						.BlendOp = D3D11_BLEND_OP_ADD,
						.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
						.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
						.BlendOpAlpha = D3D11_BLEND_OP_ADD,
						.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
				},
		};
		ID3D11Device_CreateBlendState(d3d11_state.device, &desc, &d3d11_state.line_blend_state);
	}

	{
		// disable culling
		D3D11_RASTERIZER_DESC desc =
		{
			.FillMode = D3D11_FILL_SOLID,
			.CullMode = D3D11_CULL_NONE,
			.ScissorEnable = TRUE,
		};
		ID3D11Device_CreateRasterizerState(d3d11_state.device, &desc, &d3d11_state.rasterizerState);
	}

	{
		// disable depth & stencil test
		D3D11_DEPTH_STENCIL_DESC desc =
		{
			.DepthEnable = FALSE,
			.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D11_COMPARISON_LESS,
			.StencilEnable = FALSE,
			.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
			.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
		};
		ID3D11Device_CreateDepthStencilState(d3d11_state.device, &desc, &d3d11_state.depthState);
	}
}

internal void
D3D11_End(Vec4F32 clear_color)
{
	HRESULT hr;

	U32 width, height;
	OS_QueryWindowSize(d3d11_state.window, (S32 *)&width, (S32 *)&height);

	// NOTE(hampus): Resize swap chain if needed
	if (d3d11_state.rtview == 0 || width != d3d11_state.current_width || height != d3d11_state.current_height)
	{
		if (d3d11_state.rtview)
		{
			ID3D11DeviceContext_ClearState(d3d11_state.context);
			ID3D11RenderTargetView_Release(d3d11_state.rtview);
			ID3D11DepthStencilView_Release(d3d11_state.dsview);
			d3d11_state.rtview = 0;
		}

		// NOTE(hampus): Resize to new size for non-zero size
		if (width != 0 && height != 0)
		{
			hr = IDXGISwapChain1_ResizeBuffers(d3d11_state.swap_chain, 0, width, height, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr))
			{
				Assert(!"Failed to resize swap chain!");
			}

			// create RenderTarget view for new backbuffer texture
			ID3D11Texture2D* backbuffer;
			IDXGISwapChain1_GetBuffer(d3d11_state.swap_chain, 0, &IID_ID3D11Texture2D, (void**)&backbuffer);
			ID3D11Device_CreateRenderTargetView(d3d11_state.device, (ID3D11Resource*)backbuffer, 0, &d3d11_state.rtview);
			ID3D11Texture2D_Release(backbuffer);
			D3D11_TEXTURE2D_DESC depthDesc =
			{
				.Width = width,
				.Height = height,
				.MipLevels = 1,
				.ArraySize = 1,
				.Format = DXGI_FORMAT_D32_FLOAT, // or use DXGI_FORMAT_D32_FLOAT_S8X24_UINT if you need stencil
				.SampleDesc = { 1, 0 },
				.Usage = D3D11_USAGE_DEFAULT,
				.BindFlags = D3D11_BIND_DEPTH_STENCIL,
			};

			// create new depth stencil texture & DepthStencil view
			ID3D11Texture2D	*depth;
			ID3D11Device_CreateTexture2D(d3d11_state.device, &depthDesc, NULL, &depth);
			ID3D11Device_CreateDepthStencilView(d3d11_state.device, (ID3D11Resource*)depth, NULL, &d3d11_state.dsview);
			ID3D11Texture2D_Release(depth);
		}
		d3d11_state.current_width = width;
		d3d11_state.current_height = height;
	}

	// clear screen
	FLOAT color[] = {clear_color.r, clear_color.g, clear_color.b, clear_color.a};
	ID3D11DeviceContext_ClearRenderTargetView(d3d11_state.context, d3d11_state.rtview, color);
	ID3D11DeviceContext_ClearDepthStencilView(d3d11_state.context, d3d11_state.dsview, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	if (d3d11_state.rtview)
	{
		for (Batch2DNode *node = r_state->render_data.batch_list->first;
				 node != 0;
				 node = node->next)
		{
			Batch2D *batch = node->batch;

			switch (batch->inst_kind)
			{
				case Batch2DInstKind_Rect:
				{

					// output viewport covering all client area of window
					D3D11_VIEWPORT viewport =
					{
						.TopLeftX = 0,
						.TopLeftY = 0,
						.Width = (FLOAT)width,
						.Height = (FLOAT)height,
						.MinDepth = 0,
						.MaxDepth = 1,
					};

					{
						D3D11_MAPPED_SUBRESOURCE mapped;
						ID3D11DeviceContext_Map(d3d11_state.context, (ID3D11Resource*)d3d11_state.v_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
						memcpy(mapped.pData, batch->data, batch->inst_count * sizeof(Rect));
						ID3D11DeviceContext_Unmap(d3d11_state.context, (ID3D11Resource*)d3d11_state.v_buffer, 0);
					}

					{
						Mat4x4F32 proj = Ortho(0, (F32)width, (F32)height, 0, -1.0f, 1.0f);

						D3D11_MAPPED_SUBRESOURCE mapped;
						ID3D11DeviceContext_Map(d3d11_state.context, (ID3D11Resource*)d3d11_state.ubuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
						memcpy(mapped.pData, &proj, sizeof(proj));
						ID3D11DeviceContext_Unmap(d3d11_state.context, (ID3D11Resource*)d3d11_state.ubuffer, 0);
					}

					// Input Assembler
					ID3D11DeviceContext_IASetInputLayout(d3d11_state.context, d3d11_state.layout);
					ID3D11DeviceContext_IASetPrimitiveTopology(d3d11_state.context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
					UINT stride = sizeof(Rect);
					UINT offset = 0;
					ID3D11DeviceContext_IASetVertexBuffers(d3d11_state.context, 0, 1, &d3d11_state.v_buffer, &stride, &offset);

					// Vertex Shader
					ID3D11DeviceContext_VSSetConstantBuffers(d3d11_state.context, 0, 1, &d3d11_state.ubuffer);
					ID3D11DeviceContext_VSSetShader(d3d11_state.context, d3d11_state.v_shader, NULL, 0);

					D3D11_RECT rect;
					rect.left = (LONG)batch->clip_rect.x0;
					rect.top = (LONG)batch->clip_rect.y0;
					rect.right = (LONG)batch->clip_rect.x1;
					rect.bottom = (LONG)batch->clip_rect.y1;

					if (rect.right > viewport.Width)
					{
						rect.right = (LONG)viewport.Width;
					}

					if (rect.bottom > viewport.Height)
					{
						rect.bottom = (LONG)viewport.Height;
					}

					// Rasterizer Stage
					ID3D11DeviceContext_RSSetViewports(d3d11_state.context, 1, &viewport);
					ID3D11DeviceContext_RSSetState(d3d11_state.context, d3d11_state.rasterizerState);
					ID3D11DeviceContext_RSSetScissorRects(d3d11_state.context, 1, &rect);

					ID3D11ShaderResourceView *view = PtrFromInt(batch->tex.handle.a);
					// Pixel Shader
					ID3D11DeviceContext_PSSetSamplers(d3d11_state.context, 0, 1, &d3d11_state.sampler);
					ID3D11DeviceContext_PSSetShaderResources(d3d11_state.context, 0, 1, &view);
					ID3D11DeviceContext_PSSetShader(d3d11_state.context, d3d11_state.p_shader, NULL, 0);

					// Output Merger
					ID3D11DeviceContext_OMSetBlendState(d3d11_state.context, d3d11_state.blend_state, NULL, ~0U);

					ID3D11DeviceContext_OMSetDepthStencilState(d3d11_state.context, d3d11_state.depthState, 0);
					ID3D11DeviceContext_OMSetRenderTargets(d3d11_state.context, 1, &d3d11_state.rtview, d3d11_state.dsview);

					ID3D11DeviceContext_DrawInstanced(d3d11_state.context, 4, batch->inst_count, 0, 0);
				} break;

				case Batch2DInstKind_Line:
				{
					// output viewport covering all client area of window
					D3D11_VIEWPORT viewport =
					{
						.TopLeftX = 0,
						.TopLeftY = 0,
						.Width = (FLOAT)width,
						.Height = (FLOAT)height,
						.MinDepth = 0,
						.MaxDepth = 1,
					};

					{
						D3D11_MAPPED_SUBRESOURCE mapped;
						ID3D11DeviceContext_Map(d3d11_state.context, (ID3D11Resource*)d3d11_state.v_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
						memcpy(mapped.pData, batch->data, batch->inst_count * sizeof(LineVertex));
						ID3D11DeviceContext_Unmap(d3d11_state.context, (ID3D11Resource*)d3d11_state.v_buffer, 0);
					}

					{
						Mat4x4F32 proj = Ortho(0, (F32)width, (F32)height, 0, -1.0f, 1.0f);

						D3D11_MAPPED_SUBRESOURCE mapped;
						ID3D11DeviceContext_Map(d3d11_state.context, (ID3D11Resource*)d3d11_state.ubuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
						memcpy(mapped.pData, &proj, sizeof(proj));
						ID3D11DeviceContext_Unmap(d3d11_state.context, (ID3D11Resource*)d3d11_state.ubuffer, 0);
					}

					// Input Assembler
					ID3D11DeviceContext_IASetInputLayout(d3d11_state.context, d3d11_state.line_layout);
					ID3D11DeviceContext_IASetPrimitiveTopology(d3d11_state.context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					UINT stride = sizeof(LineVertex);
					UINT offset = 0;
					ID3D11DeviceContext_IASetVertexBuffers(d3d11_state.context, 0, 1, &d3d11_state.v_buffer, &stride, &offset);

					// Vertex Shader
					ID3D11DeviceContext_VSSetConstantBuffers(d3d11_state.context, 0, 1, &d3d11_state.ubuffer);
					ID3D11DeviceContext_VSSetShader(d3d11_state.context, d3d11_state.line_vertex_shader, NULL, 0);

					D3D11_RECT rect;
					rect.left = (LONG)batch->clip_rect.x0;
					rect.top = (LONG)batch->clip_rect.y0;
					rect.right = (LONG)batch->clip_rect.x1;
					rect.bottom = (LONG)batch->clip_rect.y1;

					if (rect.right > viewport.Width)
					{
						rect.right = (LONG)viewport.Width;
					}

					if (rect.bottom > viewport.Height)
					{
						rect.bottom = (LONG)viewport.Height;
					}

					// Rasterizer Stage
					ID3D11DeviceContext_RSSetViewports(d3d11_state.context, 1, &viewport);
					ID3D11DeviceContext_RSSetState(d3d11_state.context, d3d11_state.rasterizerState);
					ID3D11DeviceContext_RSSetScissorRects(d3d11_state.context, 1, &rect);

					ID3D11ShaderResourceView *view = PtrFromInt(batch->tex.handle.a);
					// Pixel Shader
					ID3D11DeviceContext_PSSetSamplers(d3d11_state.context, 0, 1, &d3d11_state.sampler);
					ID3D11DeviceContext_PSSetShaderResources(d3d11_state.context, 0, 0, 0);
					ID3D11DeviceContext_PSSetShader(d3d11_state.context, d3d11_state.line_pixel_shader, NULL, 0);

					// Output Merger
					ID3D11DeviceContext_OMSetBlendState(d3d11_state.context, d3d11_state.line_blend_state, NULL, ~0U);

					ID3D11DeviceContext_OMSetDepthStencilState(d3d11_state.context, d3d11_state.depthState, 0);
					ID3D11DeviceContext_OMSetRenderTargets(d3d11_state.context, 1, &d3d11_state.rtview, d3d11_state.dsview);

					ID3D11DeviceContext_DrawInstanced(d3d11_state.context, 6, batch->inst_count, 0, 0);
				} break;

				InvalidCase;
			}
		}
	}

	// change to FALSE to disable vsync
	BOOL vsync = TRUE;
	hr = IDXGISwapChain1_Present(d3d11_state.swap_chain, vsync ? 1 : 0, 0);
	if (hr == DXGI_STATUS_OCCLUDED)
	{
		// window is minimized, cannot vsync - instead sleep a bit
		if (vsync)
		{
			Sleep(10);
		}
	}
	else if (FAILED(hr))
	{
		Assert(!"Failed to present swap chain! Device lost?");
	}
}

R_Handle
D3D11_LoadTexture(void *data, S32 width, S32 height)
{
	Assert(data);
	Assert(width != 0);
	Assert(height != 0);

	R_Handle result = {0};

	D3D11Texture d3d11_tex = {0};

	{
		D3D11_TEXTURE2D_DESC desc =
		{
			.Width = width,
			.Height = height,
			.MipLevels = 1,
			.ArraySize = 1,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.SampleDesc = { 1, 0 },
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_SHADER_RESOURCE,
			.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		};

		D3D11_SUBRESOURCE_DATA sub_data;
		sub_data.pSysMem = data;
		sub_data.SysMemPitch = width * sizeof(U32);

		ID3D11Device_CreateTexture2D(d3d11_state.device, &desc, &sub_data, &d3d11_tex.texture);
		ID3D11Device_CreateShaderResourceView(d3d11_state.device, (ID3D11Resource*)d3d11_tex.texture, NULL, &d3d11_tex.view);
	}

	result.a = IntFromPtr(d3d11_tex.view);
	result.b = IntFromPtr(d3d11_tex.texture);

	return result;
}