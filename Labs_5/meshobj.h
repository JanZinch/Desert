//---------------------------------------------------------------------------
// Meshobj.h
// Загрузка объекта из мешей    
//---------------------------------------------------------------------------
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <fstream>

using namespace std;

//---------------------------------------------------------------------------
// Задаем максимальное количество вершин и индексов. Это значение
// можно увеличить если требуется загружать большие меши       
//---------------------------------------------------------------------------
const int buffermax = 16384;
//---------------------------------------------------------------------------
// Базовый класс для загрузки и хранения мешей. Создайте несколько экземпляров
// данного класса для загрузки в ваше приложение нескольких мешей
//---------------------------------------------------------------------------
class MeshFromObj{

public:

	MeshFromObj(ID3D11Device * pd3dDevice, ID3D11DeviceContext* context, char * fname);
	void Draw();
	~MeshFromObj();

protected:
	
	ID3D11Device*               c_pd3dDevice;
	ID3D11DeviceContext*		c_pImmediateContext;
	ID3D11Buffer*               c_pVertexBuffer;
	ID3D11Buffer*               c_pIndexBuffer;
	int						    verticesCount;
	int						    indicesCount;
	bool						meshloaded;
	void LoadMeshFromObj(char * fname);
	HRESULT CreateVertexAndIndexBuffers();
};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Формат структуры VERTEX должен совпадать с форматом вершинного буфера 
//---------------------------------------------------------------------------
struct VERTEX
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
};
//---------------------------------------------------------------------------
// Временный буфер для хранения вершин и индексов
//---------------------------------------------------------------------------
struct {
	VERTEX vertices[buffermax];
	DWORD  indices[buffermax];
	int    verticesI;
	int    indicesI;
} buffer;
//---------------------------------------------------------------------------
// Определение методов класса MeshFromObj
//---------------------------------------------------------------------------
MeshFromObj::MeshFromObj(ID3D11Device * pd3dDevice, ID3D11DeviceContext * context, char * fname)
{
	c_pd3dDevice = pd3dDevice;
	c_pImmediateContext = context;
	c_pVertexBuffer = NULL;
	c_pIndexBuffer = NULL;
	meshloaded = false;
	LoadMeshFromObj(fname); if (buffer.verticesI == 0) return;
	HRESULT hr = CreateVertexAndIndexBuffers();
	if (!FAILED(hr)) meshloaded = true;
}
//---------------------------------------------------------------------------
// Деструктор объекта MeshFromObj
//---------------------------------------------------------------------------
MeshFromObj::~MeshFromObj()
{
	if (c_pVertexBuffer) c_pVertexBuffer->Release();
	if (c_pIndexBuffer) c_pIndexBuffer->Release();
}
//---------------------------------------------------------------------------
// Фукнция создания вершинного и индексного буферов, 
// эта функция должна вызываться после функции LoadMeshFromObj
//---------------------------------------------------------------------------
HRESULT MeshFromObj::CreateVertexAndIndexBuffers()
{
	//определяем количество вершин и количество индексов
	verticesCount = buffer.verticesI;
	indicesCount = buffer.indicesI;

	//Hresult
	HRESULT hr;


	// Заполняем вершинный буфер
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX) * (verticesCount + 1);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = buffer.vertices;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pVertexBuffer);
	if (FAILED(hr))
		return hr;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * indicesCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = buffer.indices;
	hr = c_pd3dDevice->CreateBuffer(&bd, &InitData, &c_pIndexBuffer);
	if (FAILED(hr))
		return hr;
}
//---------------------------------------------------------------------------
void MeshFromObj::Draw()
{
	if (meshloaded)
	{
		// Установка вершинного буфера
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		c_pImmediateContext->IASetVertexBuffers(0, 1, &c_pVertexBuffer, &stride, &offset);

		// Установка индексного буфера
		c_pImmediateContext->IASetIndexBuffer(c_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Установка топологии буфера
		c_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		c_pImmediateContext->DrawIndexed(indicesCount, 0, 0);
	}
}
//---------------------------------------------------------------------------
// Функции для добавления вершин и индексов
//---------------------------------------------------------------------------
int AddVertex(int vertexI, VERTEX vertex)
{
	int res = -1;
	//Поиск существующей вершины
	for (int i = 0; i < buffer.verticesI; i++)
		if (memcmp(&buffer.vertices[i], &vertex, sizeof(VERTEX)) == 0) res = i;
	//Добавление
	if (res < 0) {
		buffer.vertices[buffer.verticesI++] = vertex; res = buffer.verticesI - 1;
	}
	return res;
}
//---------------------------------------------------------------------------
void AddIndex(int index)
{
	buffer.indices[buffer.indicesI++] = index;
}
//---------------------------------------------------------------------------
// Функция осуществляет проход файла obj и загрузку из него данных
// вершинного и индексного буфера
//---------------------------------------------------------------------------
void MeshFromObj::LoadMeshFromObj(char * fname)
{
	// Очистка данных
	buffer.verticesI = 0;
	buffer.indicesI = 0;

	//Создание временного хранилища входящих данных, как только данные будут
	//загружены в подходящий формат скопируем эти данные в вершинный и индексный буферы
	XMFLOAT3 * Positions = (XMFLOAT3*)malloc(buffermax * sizeof(XMFLOAT3));
	XMFLOAT2 * TexCoords = (XMFLOAT2*)malloc(buffermax * sizeof(XMFLOAT2));
	XMFLOAT3 * Normals = (XMFLOAT3*)malloc(buffermax * sizeof(XMFLOAT3));

	// Индексы для массивов
	int PositionsI = 0;
	int TexCoordsI = 0;
	int NormalsI = 0;

	// Ввод данных из файла
	//char fname[256];
	//strcpy(fname,Edit1->Text.c_str());
	WCHAR strCommand[256] = { 0 };
	wifstream InFile(fname);

	if (!InFile) return;

	while (true){

		InFile >> strCommand;
		if (!InFile) break;

		if (0 == wcscmp(strCommand, L"#"))
		{
			// Комментарий
		}
		else if (0 == wcscmp(strCommand, L"v"))
		{
			// Координаты
			float x, y, z;
			InFile >> x >> y >> z; float c = 0.05f;
			Positions[PositionsI++] = XMFLOAT3(x*c, y*c, z*c);
		}
		else if (0 == wcscmp(strCommand, L"vt"))
		{
			// Текстурные координаты
			float u, v;
			InFile >> u >> v;
			TexCoords[TexCoordsI++] = XMFLOAT2(u, -v);
		}
		else if (0 == wcscmp(strCommand, L"vn"))
		{
			// Нормали
			float x, y, z;
			InFile >> x >> y >> z;
			Normals[NormalsI++] = XMFLOAT3(x, y, z);
		}
		else if (0 == wcscmp(strCommand, L"f"))
		{
			// Face
			UINT iPosition, iTexCoord, iNormal;
			VERTEX vertex;

			for (UINT iFace = 0; iFace < 3; iFace++)
			{
				ZeroMemory(&vertex, sizeof(VERTEX));

				// OBJ формат использует массивы с индексами от 1
				InFile >> iPosition;
				vertex.position = Positions[iPosition - 1];

				if ('/' == InFile.peek())
				{
					InFile.ignore();

					if ('/' != InFile.peek())
					{
						// Опционно текстурные координаты
						InFile >> iTexCoord;
						vertex.texcoord = TexCoords[iTexCoord - 1];
					}

					if ('/' == InFile.peek())
					{
						InFile.ignore();

						// Опционно нормали
						InFile >> iNormal;
						vertex.normal = Normals[iNormal - 1];
					}
				}

				//Добавляем вершину и индекс
				int index = AddVertex(iPosition, vertex);
				AddIndex(index);

			}
		}
	}
	InFile.close();

	//Очистка временных массивов
	free(Positions);
	free(TexCoords);
	free(Normals);
}
//---------------------------------------------------------------------------