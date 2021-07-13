#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include <algorithm>

#include <stdio.h>
#include <debugapi.h>

inline FLOAT dot(const XMFLOAT2 &a, const XMFLOAT2& b)
{
    return a.x * b.x + a.y * b.y;
}


inline FLOAT lerp(const FLOAT& lo, const FLOAT& hi, const FLOAT& t)
{
    return lo * (1 - t) + hi * t;
}

inline float smoothstep(const float& t)
{
    return t * t * (3 - 2 * t);
}



float RandomFloat()
{
    return rand() % 1024 / 1023.0;
}


class PerlinNoise {

    static const int tableSize = 256;
    static const int tableSizeMask = tableSize - 1;
    XMFLOAT2 gradients[tableSize];
    int permutationTable[tableSize * 2]; 

    static const int groundSizeZ = 50;               // z   50
    static const int groundSizeX = 50;                  //x

    const float GroundMinX = -25.0f, GroundMinZ = -25.0f;  // -25
    const float GroundStepX = 1.0f, GroundStepZ = 1.0f;


    struct Vertex
    {
        XMFLOAT3 position;
        XMFLOAT3 normal;
        XMFLOAT2 texcoord;
    };

    //int n, m;
    XMFLOAT3** ground;

    bool state;

    DWORD indicesBufSize;

    ID3D11Device* c_pd3dDevice;
    ID3D11DeviceContext* c_pImmediateContext;

    ID3D11Buffer* c_pVertexBuffer;
    ID3D11Buffer* c_pIndexBuffer;


    float evaluation(const XMFLOAT2& p)
    {
        int xi0 = ((int)floor(p.x)) & tableSizeMask;
        int yi0 = ((int)floor(p.y)) & tableSizeMask;

        int xi1 = (xi0 + 1) & tableSizeMask;
        int yi1 = (yi0 + 1) & tableSizeMask;

        float tx = p.x - ((int)floor(p.x));
        float ty = p.y - ((int)floor(p.y));

        float u = smoothstep(tx);
        float v = smoothstep(ty);

        // generate vectors going from the grid points to p
        float x0 = tx, x1 = tx - 1;
        float y0 = ty, y1 = ty - 1;

        XMFLOAT2 p00 = XMFLOAT2(x0, y0);
        XMFLOAT2 p10 = XMFLOAT2(x1, y0);
        XMFLOAT2 p01 = XMFLOAT2(x0, y1);
        XMFLOAT2 p11 = XMFLOAT2(x1, y1);

        // gradients at the corner of the cell
        const XMFLOAT2& c00 = gradients[hash(xi0, yi0)];
        const XMFLOAT2& c10 = gradients[hash(xi1, yi0)];
        const XMFLOAT2& c01 = gradients[hash(xi0, yi1)];
        const XMFLOAT2& c11 = gradients[hash(xi1, yi1)];

        // linear interpolation
        float a = lerp(dot(c00, p00), dot(c10, p10), u);
        float b = lerp(dot(c01, p01), dot(c11, p11), u);
        return lerp(a, b, v); // g 
    }

    inline int hash(int x, int y)
    {
        return permutationTable[permutationTable[x] + y];
    }

public:


    PerlinNoise(ID3D11Device* device, ID3D11DeviceContext* context) {
        
        c_pd3dDevice = device;
        c_pImmediateContext = context;
        c_pVertexBuffer = NULL;
        c_pIndexBuffer = NULL;

        state = true;

        int i;
        float M_2PI = atan(1.0) * 8;

        for (i = 0; i < tableSize; ++i) {

            // better
            //float theta = acos(2 * RandomFloat() - 1);
            float phi = RandomFloat() * M_2PI;
            float x = cos(phi); // * sin(theta);
            float y = sin(phi); // * sin(theta);
            //float z = cos(theta);
            gradients[i] = XMFLOAT2(x, y);

            permutationTable[i] = i;
        }

        // create permutation table
        for (i = 0; i < tableSize; ++i) {

            std::swap(permutationTable[i], permutationTable[rand() & tableSizeMask]);

        }
        // extend the permutation table in the index range [256:512]
        for (i = 0; i < tableSize; ++i) {

            permutationTable[tableSize + i] = permutationTable[i];

        }

    }

    // ќпределение высоты над поверхностью грунта
    float Altitude(XMFLOAT3 pos)
    {
        float gAverage = 0;
        
        // ќпределение индексов в матрице
        float x = (pos.x - GroundMinX) / GroundStepX;
        float z = (pos.z - GroundMinZ) / GroundStepZ;
        
        //float x = pos.x;
        //float z = pos.z;
        int j = x;
        int i = z;
        
        if (i >= 0 && j >= 0 && j < groundSizeX - 1 && i < groundSizeZ - 1)
        {
            // Ћинейна€ интерпол€ци€ высоты по 4 ближайшим точкам карты
            x -= j;	// ƒробна€ часть
            z -= i;
            gAverage = ground[i][j].y * (1 - x) * (1 - z) + ground[i + 1][j].y * (1 - x) * z + ground[i][j + 1].y  * x * (1 - z) + ground[i + 1][j + 1].y * x * z;
        }

        return gAverage;

        //return pos.y - gAverage;
    }

    void CreateRandomMap() {

        HRESULT hr;

        const float frequency = 0.1f;
        const float amplitude = 8;

        int i, j;

        ground = new XMFLOAT3* [groundSizeZ];

        for (i = 0; i < groundSizeZ; i++) {

            ground[i] = new XMFLOAT3[groundSizeX];
        }

        float x = GroundMinX, z = GroundMinZ;
        const float step = 1.0f;

        for (i = 0; i < groundSizeZ; ++i) {

            for (j = 0; j < groundSizeX; ++j) {

                ground[i][j].x = x;
                ground[i][j].y = evaluation(XMFLOAT2(i * frequency, j * frequency)) * amplitude;	// generate a float in the range [0:1]            
                ground[i][j].z = z;
                x += step;
            }
        
            x = GroundMinX;
            z += step;
        
        }

        OutputDebugStringA("\nPerlin map\n");
        char buf[80];

        for (i = 0; i < groundSizeZ; i++) {

            for (j = 0; j < groundSizeX; j++) {


                sprintf_s(buf, "%.2f ", ground[i][j].y);
                OutputDebugStringA(buf);
            }

            OutputDebugStringA("\n");
        }

        OutputDebugStringA("\nEnd\n");


        const int vertexCount = (groundSizeZ - 1) * (groundSizeX - 1) * 4;
        
        //Vertex *vertices = new Vertex[vertexCount];

        Vertex vertices[vertexCount];

        const int indicesCount = (groundSizeZ - 1) * (groundSizeX - 1) * 6;
        //DWORD *indices = new DWORD[indicesCount];

        DWORD indices[indicesCount];

        indicesBufSize = indicesCount;

        int k = 0;
        

        for (i = 0; i < groundSizeZ - 1; i++) {

            for (j = 0; j < groundSizeX - 1; j++) {


                 vertices[k].position = ground[i][j];
                 vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                 vertices[k].texcoord = XMFLOAT2(0.0f, 0.0f);
                 sprintf_s(buf, "%.2f |", vertices[k].position.y);
                 OutputDebugStringA(buf);
                 k++;

                 vertices[k].position = ground[i][j + 1];
                 vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                 vertices[k].texcoord = XMFLOAT2(0.0f, 1.0f);
                 sprintf_s(buf, "%.2f |", vertices[k].position.y);
                 OutputDebugStringA(buf);
                 k++;

                 vertices[k].position = ground[i + 1][j];
                 vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                 vertices[k].texcoord = XMFLOAT2(1.0f, 0.0f);
                 sprintf_s(buf, "%.2f |", vertices[k].position.y);
                 OutputDebugStringA(buf);
                 k++;



                 vertices[k].position = ground[i + 1][j + 1];
                 vertices[k].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                 vertices[k].texcoord = XMFLOAT2(1.0f, 1.0f);
                 sprintf_s(buf, "%.2f |", vertices[k].position.y);
                 OutputDebugStringA(buf);
                 k++;


                
            }
            
            OutputDebugStringA("\n");

        }

        j = 0;

        for (i = 0; i < vertexCount; i += 4) {

            indices[j] = i + 2; j++;
            indices[j] = i + 1; j++;
            indices[j] = i; j++;

            indices[j] = i + 3; j++;
            indices[j] = i + 1; j++;
            indices[j] = i + 2; j++;
        }

        OutputDebugStringA("----IndExes-----\n");

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

    void Draw() {
    
    
        if (state) {

            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            c_pImmediateContext->IASetVertexBuffers(0, 1, &c_pVertexBuffer, &stride, &offset);
            c_pImmediateContext->IASetIndexBuffer(c_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
            c_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            c_pImmediateContext->DrawIndexed(indicesBufSize, 0, 0);
        }
    
    
    
    }



    ~PerlinNoise() {
    
    
        for (int i = 0; i < groundSizeZ; i++) {

            delete ground[i];
        }

        delete ground;
    
    
    }


};






