#pragma once

#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>

#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")

class TextWriter {

private:

	ID3D10Device1 *d3d101Device;
	IDXGIKeyedMutex *keyedMutex11;
	IDXGIKeyedMutex *keyedMutex10;
	ID2D1RenderTarget *D2DRenderTarget;
	
	ID2D1SolidColorBrush *Brush;
	ID3D11Texture2D *BackBuffer11;
	ID3D11Texture2D *sharedTex11;
	ID3D11Buffer *d2dVertBuffer;
	ID3D11Buffer *d2dIndexBuffer;
	ID3D11ShaderResourceView *d2dTexture;
	
	IDWriteFactory *DWriteFactory;
	IDWriteTextFormat *TextFormat;
	wstring printText;

	struct Vertex {

		XMFLOAT3 pos;
		XMFLOAT2 texCoord;

		Vertex() {}
		Vertex(float x, float y, float z, float u, float v) : pos(x, y, z), texCoord(u, v) {}
		
	};


	int Width;
	int Height;

	//

	ID3D11Device* d3d11Device;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11SamplerState* TexSamplerState;

public:

	TextWriter();
	HRESULT InitTW(ID3D11Device*, ID3D11DeviceContext*, ID3D11SamplerState*, int, int);
	bool InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter);
	void InitD2DScreenTexture();
	void RenderText(ID3D11BlendState*, wstring);
	~TextWriter();

};


TextWriter::TextWriter() {

	d3d11Device = NULL;
	d3d11DevCon = NULL;
	TexSamplerState = NULL;
	Width = NULL;
	Height = NULL;

}

HRESULT TextWriter::InitTW(ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon, ID3D11SamplerState* TexSamplerState, int width, int height) {

	this->d3d11Device = d3d11Device;
	this->d3d11DevCon = d3d11DevCon;
	this->TexSamplerState = TexSamplerState;
	Width = width;
	Height = height;

	return S_OK;
}

bool TextWriter::InitD2D_D3D101_DWrite(IDXGIAdapter1 *Adapter){

	HRESULT hr;

	hr = D3D10CreateDevice1(Adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &d3d101Device);

	D3D11_TEXTURE2D_DESC sharedTexDesc;

	ZeroMemory(&sharedTexDesc, sizeof(sharedTexDesc));

	sharedTexDesc.Width = Width;
	sharedTexDesc.Height = Height;
	sharedTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDesc.MipLevels = 1;
	sharedTexDesc.ArraySize = 1;
	sharedTexDesc.SampleDesc.Count = 1;
	sharedTexDesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = d3d11Device->CreateTexture2D(&sharedTexDesc, NULL, &sharedTex11);
	hr = sharedTex11->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex11);

	IDXGIResource *sharedResource10;
	HANDLE sharedHandle10;

	hr = sharedTex11->QueryInterface(__uuidof(IDXGIResource), (void**)&sharedResource10);
	hr = sharedResource10->GetSharedHandle(&sharedHandle10);

	sharedResource10->Release();

	IDXGISurface1 *sharedSurface10;
	hr = d3d101Device->OpenSharedResource(sharedHandle10, __uuidof(IDXGISurface1), (void**)(&sharedSurface10));
	hr = sharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&keyedMutex10);

	ID2D1Factory *D2DFactory;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&D2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;

	ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));

	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = D2DFactory->CreateDxgiSurfaceRenderTarget(sharedSurface10, &renderTargetProperties, &D2DRenderTarget);

	sharedSurface10->Release();
	D2DFactory->Release();
	
	hr = D2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &Brush);

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&DWriteFactory));

	hr = DWriteFactory->CreateTextFormat(L"Script", NULL, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_FONT_STYLE_ITALIC, DWRITE_FONT_STRETCH_NORMAL,
		36.0f, L"en-us", &TextFormat);

	hr = TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	d3d101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	return true;
}

void TextWriter::InitD2DScreenTexture(){

	HRESULT hr;
	
	Vertex vertices[] =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
		Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
		Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	};

	DWORD indices[] = {

		0,  1,  2,
		0,  2,  3,
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &d2dIndexBuffer);


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &d2dVertBuffer);

	
	d3d11Device->CreateShaderResourceView(sharedTex11, NULL, &d2dTexture);

}

void TextWriter::RenderText(ID3D11BlendState* Transparency, wstring text){

	keyedMutex11->ReleaseSync(0);
	keyedMutex10->AcquireSync(0, 5);	
	D2DRenderTarget->BeginDraw();

	D2DRenderTarget->Clear(D2D1::ColorF(1.0f, 0.3f, 0.0f, 0.0f));

	wostringstream printString;
	printString << text;
	printText = printString.str();

	D2D1_COLOR_F FontColor = D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f);
	Brush->SetColor(FontColor);

	D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, Width, Height);
	D2DRenderTarget->DrawText(printText.c_str(), wcslen(printText.c_str()), TextFormat, layoutRect, Brush);
	D2DRenderTarget->EndDraw();

	keyedMutex10->ReleaseSync(1);
	keyedMutex11->AcquireSync(1, 5);

	d3d11DevCon->OMSetBlendState(Transparency, NULL, 0xffffffff);
	d3d11DevCon->IASetIndexBuffer(d2dIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &d2dVertBuffer, &stride, &offset);

	/*WVP = XMMatrixIdentity() * XMMatrixTranslation(0.5f, -0.5f, 0.0f);
	cbPerObj.WVP = XMMatrixTranspose(WVP);
	d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
	d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);*/
	d3d11DevCon->PSSetShaderResources(0, 1, &d2dTexture);
	/*d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

	d3d11DevCon->RSSetState(CWcullMode);*/
	//Draw the second cube
	d3d11DevCon->DrawIndexed(6, 0, 0);


}

TextWriter::~TextWriter() {

	d3d101Device->Release();
	keyedMutex11->Release();
	keyedMutex10->Release();
	D2DRenderTarget->Release();

	Brush->Release();
	BackBuffer11->Release();
	sharedTex11->Release();
	d2dVertBuffer->Release();
	d2dIndexBuffer->Release();
	d2dTexture->Release();

	DWriteFactory->Release();
	TextFormat->Release();
}

