#include "TerrainMesh.h"
#include <iostream>
#include <fstream>
#include <debugapi.h>

using namespace std;

bool TerrainMesh::isInitialized() {

	return state;
}


TerrainMesh::TerrainMesh(ID3D11Device* device, ID3D11DeviceContext* context) {

	HRESULT hr;
	c_pd3dDevice = device;
	c_pImmediateContext = context;
	c_pVertexBuffer = NULL;
	c_pIndexBuffer = NULL;

	state = true;

	const int height = 9;
	const int width = 9;
	const float step = 0.5f;


	const int vertexCount = (height - 1) * (width - 1) * 4;

	char buf[90];
	int i, j, k = 0;
	 
	ifstream file;
	file.open("Terrain.txt");

	if (!file.is_open()) { OutputDebugStringA("Error!\n");  return; }
	else { OutputDebugStringA("Success!\n"); }

	Vertex vertices_2d[height][width];
	Vertex vertices[vertexCount];
	
	float y;
	float x = 0.0f, z = 0.0f;

	for (i = 0; i < height; i++) {
	
		for (j = 0; j < width; j++) {
		
			file >> y;
			vertices_2d[i][j].position.y = y;
			vertices_2d[i][j].position.x = x;
			vertices_2d[i][j].position.z = z;
			x += step;
			
		}

		x = 0.0f;					
		z += step;
	}



	file.close();

	OutputDebugStringA("-------Terrain------\n");

	for (i = 0; i < height - 1; i++) {
	
		for (j = 0; j < width - 1; j++) {

			vertices[k] = vertices_2d[i][j];
			vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[k].texcoord = XMFLOAT2(0.0f, 0.0f);
			sprintf_s(buf, "%.0f ", vertices[k].position.y);
			OutputDebugStringA(buf);
			k++;


			vertices[k] = vertices_2d[i][j + 1];
			vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[k].texcoord = XMFLOAT2(0.0f, 1.0f);
			sprintf_s(buf, "%.0f ", vertices[k].position.y);
			OutputDebugStringA(buf);
			k++;

			vertices[k] = vertices_2d[i + 1][j];
			vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[k].texcoord = XMFLOAT2(1.0f, 0.0f);
			sprintf_s(buf, "%.0f ", vertices[k].position.y);
			OutputDebugStringA(buf);
			k++;	

			

			vertices[k] = vertices_2d[i + 1][j + 1];
			vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[k].texcoord = XMFLOAT2(1.0f, 1.0f);
			sprintf_s(buf, "%.0f ", vertices[k].position.y);
			OutputDebugStringA(buf);
			k++;

			
		}

		OutputDebugStringA("\n");
	
	}


	for (i = 0; i < vertexCount; i++) {

		
		sprintf_s(buf, "| %.0f |", vertices[i].position.y);
		OutputDebugStringA(buf);

	}


	//DWORD first, second, third, fourth;

	const int indicesCount = (height - 1) * (width - 1) * 6;

	indicesBufSize = indicesCount;

	DWORD indices[indicesCount];

	j = i = 0;

	/*while (i < vertexCount) {
		
		indices[j] = i + 2; j++;
		indices[j] = i + 1; j++;
		indices[j] = i; j++;

		indices[j] = i + 3; j++;
		indices[j] = i + 1; j++;
		indices[j] = i + 2; j++;

		i += 4;
	}*/

	for (i = 0; i < vertexCount; i += 4) {
	
		indices[j] = i + 2; j++;
		indices[j] = i + 1; j++;
		indices[j] = i; j++;

		indices[j] = i + 3; j++;
		indices[j] = i + 1; j++;
		indices[j] = i + 2; j++;
	}


	OutputDebugStringA("----Indexes-----\n");

	for (i = 0; i < indicesCount; i++) {
	
		sprintf_s(buf, "%d ", indices[i]);
		OutputDebugStringA(buf);		
	}


	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = vertices;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pVertexBuffer);
	if (FAILED(hr)) state = false;

	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pIndexBuffer);
	if (FAILED(hr)) state = false;

}

void TerrainMesh::Draw()
{
	if (state) {

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		c_pImmediateContext->IASetVertexBuffers(0, 1, &c_pVertexBuffer, &stride, &offset);
		c_pImmediateContext->IASetIndexBuffer(c_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		c_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		c_pImmediateContext->DrawIndexed(indicesBufSize, 0, 0);
	}


}


TerrainMesh::~TerrainMesh()
{
	if (c_pVertexBuffer) c_pVertexBuffer->Release();
	if (c_pIndexBuffer) c_pIndexBuffer->Release();
}