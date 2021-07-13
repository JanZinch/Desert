#pragma once
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

class TerrainMesh
{
private:

	struct Vertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texcoord;
	};

	bool state;

	DWORD indicesBufSize;

	ID3D11Device* c_pd3dDevice;
	ID3D11DeviceContext* c_pImmediateContext;

	ID3D11Buffer* c_pVertexBuffer;
	ID3D11Buffer* c_pIndexBuffer;

public:

	TerrainMesh(ID3D11Device* pd3dDevice, ID3D11DeviceContext* context);
	bool isInitialized();
	void Draw();
	~TerrainMesh();

};

