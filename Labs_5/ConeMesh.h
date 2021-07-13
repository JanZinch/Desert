#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <stdio.h>
#include <debugapi.h>

class ConeMesh {

private:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};

	ID3D11Device* c_pd3dDevice;
	ID3D11DeviceContext* c_pImmediateContext;

	ID3D11Buffer* c_pVertexBuffer;
	ID3D11Buffer* c_pIndexBuffer;

	int size;

	bool state;

public:


	ConeMesh(ID3D11Device * pd3dDevice, ID3D11DeviceContext* context);
	bool GetState() { return state; }
	void Draw();
	~ConeMesh();
	
};


ConeMesh::ConeMesh(ID3D11Device * device, ID3D11DeviceContext * context)
{
	HRESULT hr;
	c_pd3dDevice = device;
	c_pImmediateContext = context;
	c_pVertexBuffer = NULL;
	c_pIndexBuffer = NULL;

	const unsigned short segmentsCount = 16;
	const unsigned short vertexCount = segmentsCount * 3;

	unsigned short i, j, k;

	Vertex v_arr[vertexCount * 2];

	const float theta = XM_2PI / segmentsCount;
	float radius = 1.0f;
	i = j = k = 0;
	
	

	float dx, dz, dy = 1.5f;

	XMFLOAT3 normal;
	normal.x = normal.z = 1.0f;
	normal.y = 0.0f;

	char buf[90];

	while (i < vertexCount) {
	
		// osn 3

		if (i <= ((float)vertexCount * 0.25f) - 1) {

			normal.x = 1;
		}
		else if (i == ((float)vertexCount * 0.25f) - 1) {

			normal.z = 0;
		}
		else if (i <= ((float)vertexCount * 0.5f) - 1) {

			normal.z = -1;
		}
		else if (i == ((float)vertexCount * 0.5f) - 1) {

			normal.x = 1;
		}
		else if (i <= ((float)vertexCount * 0.75f) - 1) {

			normal.x = -1;
		}
		else if (i == ((float)vertexCount * 0.75f) - 1) {

			normal.z = 0;
		}
		else if (i <= ((float)vertexCount - 1)) {

			normal.z = 1;
		}
		else if (i == ((float)vertexCount - 1)) {

			normal.x = 0;
		}




		v_arr[i].position = XMFLOAT3(0.0f, dy, 0.0f);	
		v_arr[i].normal = normal;
		v_arr[i].texcoord = XMFLOAT2(0.0f, 0.0f);

		OutputDebugStringA("dx: 0   dz: 0  ------- ");
		sprintf(buf, "x: %.2f   y: %.2f  z: %.2f  \n", normal.x, normal.y, normal.z);
		OutputDebugStringA(buf);

		i++;

		for (k = 0; k < 2; k++) {

			dx = sin(theta * j);
			dz = cos(theta * j);
			sprintf(buf, "dx: %.2f   dz: %.2f   ", dx, dz);
			OutputDebugStringA(buf);

			v_arr[i].position = XMFLOAT3(dx, 0.0f, dz);
			v_arr[i].normal = normal;

			if (k == 0) {
			
				v_arr[i].texcoord = XMFLOAT2(1.0f, 0.0f);
			}
			else {
			
				v_arr[i].texcoord = XMFLOAT2(0.0f, 1.0f);
			}
			
			sprintf(buf, "x: %.2f   y: %.2f  z: %.2f  \n", normal.x, normal.y, normal.z);
			OutputDebugStringA(buf);
			j++;
			i++;	
		}	

	
		//j = 0;
	
		

		// promez 3

		if (i <= ((float)vertexCount * 0.25f) - 1) {

			normal.x = normal.z = 1;
		}
		else if (i == ((float)vertexCount * 0.25f) - 1) {

			normal.z = 0;
		}
		else if (i <= ((float)vertexCount * 0.5f) - 1) {

			normal.z = -1;
		}
		else if (i == ((float)vertexCount * 0.5f) - 1) {

			normal.x = 1;
		}
		else if (i <= ((float)vertexCount * 0.75f) - 1) {

			normal.x = -1;
		}
		else if (i == ((float)vertexCount * 0.75f) - 1) {

			normal.z = 0;
		}
		else if (i <= ((float)vertexCount - 1)) {

			normal.z = 1;
		}
		else if (i == ((float)vertexCount - 1)) {

			normal.x = 0;
		}

		v_arr[i].position = XMFLOAT3(0.0f, dy, 0.0f);
		v_arr[i].normal = normal;
		v_arr[i].texcoord = XMFLOAT2(0.0f, 0.0f);
		i++;
		OutputDebugStringA("dx: 0   dz : 0  ------ - ");
		sprintf(buf, "x: %.2f   y: %.2f  z: %.2f  \n", normal.x, normal.y, normal.z);
		OutputDebugStringA(buf);

		for (k = 0; k < 2; k++) {

			if (k == 0) {
			
				v_arr[i].texcoord = XMFLOAT2(1.0f, 0.0f);
			}
			else {
			
				dx = sin(theta * j);
				dz = cos(theta * j);
				v_arr[i].texcoord = XMFLOAT2(0.0f, 1.0f);
			}
		
			v_arr[i].position = XMFLOAT3(dx, 0.0f, dz);
			v_arr[i].normal = normal;
			sprintf(buf, "dx: %.2f   dz: %.2f  ", dx, dz);
			OutputDebugStringA(buf);
			sprintf(buf, "x: %.2f   y: %.2f  z: %.2f  \n", normal.x, normal.y, normal.z);
			OutputDebugStringA(buf);
			i++;

		}


	}

	j = vertexCount;
	
	k = 0;

	for (i = 0; i < vertexCount; i++) {


		if (k == 0) {

			v_arr[j].texcoord = XMFLOAT2(0.0f, 0.0f);
			k++;

		}
		else if (k == 1) {
		
			v_arr[j].texcoord = XMFLOAT2(1.0f, 0.0f);
			k++;
		}
		else if (k == 2) {
		
			v_arr[j].texcoord = XMFLOAT2(0.0f, 1.0f);
			k = 0;
		}
		

		v_arr[j].position = v_arr[i].position;
		v_arr[j].position.y = 0.0f;
		v_arr[j].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
		
		
		sprintf(buf, "x: %.2f   y: %.2f   z: %.2f  \n", v_arr[i].position.x, v_arr[i].position.y, v_arr[i].position.z);
		OutputDebugStringA(buf);

		
		j++;		

	}


	DWORD i_arr[vertexCount * 2];
	size = sizeof(i_arr) / sizeof(DWORD);

	for (i = 0; i < vertexCount; i++) {
	
		i_arr[i] = i;
	}

	k = vertexCount * 2 - 1;

	for (i = vertexCount; i < vertexCount * 2; i++) {

		i_arr[i] = k;
		k--;
	}

	k = vertexCount;


	state = true;


	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v_arr);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = v_arr;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pVertexBuffer);
	if (FAILED(hr)) state = false;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(i_arr);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = i_arr;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pIndexBuffer);
	if (FAILED(hr)) state = false;


}


void ConeMesh::Draw()
{
	if (state){

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		c_pImmediateContext->IASetVertexBuffers(0, 1, &c_pVertexBuffer, &stride, &offset);
		c_pImmediateContext->IASetIndexBuffer(c_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		c_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		c_pImmediateContext->DrawIndexed(size, 0, 0);
	}
}

ConeMesh::~ConeMesh()
{
	if (c_pVertexBuffer) c_pVertexBuffer->Release();
	if (c_pIndexBuffer) c_pIndexBuffer->Release();
}