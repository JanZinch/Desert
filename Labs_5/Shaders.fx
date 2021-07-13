//--------------------------------------------------------------------------------------
// File: Shaders.fx
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Переменные константных буферов
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register( t0 );        // Буфер текстуры
TextureCube txCube : register( t1 );
SamplerState samLinear : register( s0 );     // Буфер образца

 
// Буфер с информацией о матрицах
cbuffer ConstantBufferMatrixes : register( b0 )
{
    matrix World;            // Матрица мира
    matrix View;             // Матрица вида
    matrix Projection;       // Матрица проекции
};

 
// Буфер с информацией о свете
cbuffer ConstantBufferLight : register( b1 )
{
    float4 vLightDir[2];    // Направление источника света
    float4 vLightColor[2];  // Цвет источника света
    float4 vOutputColor;    // Активный цвет
};


cbuffer ConstantBufferMetal : register( b2 )
{
	matrix EmptyMatrix;
	float4 CameraPos;
	float4 CommonColor;
	float Pale;
	float3 LightDir;		
	float4 LightColor;
	float4 MetalAmbientColor;
	float SpotSize;

};


//--------------------------------------------------------------------------------------

struct VS_INPUT                   // Входящие данные вершинного шейдера
{
    float4 Pos : POSITION;        // Позиция по X, Y, Z
    float2 Tex : TEXCOORD0;       // Координаты текстуры по tu, tv
    float3 Norm : NORMAL;         // Нормаль по X, Y, Z
};

struct VS_INPUT4
{
    float4 Pos : POSITION;
	float3 Norm : NORMAL;
				// Семантика - желательно заглавными латинскими буквами!
};
 
struct PS_INPUT                   // Входящие данные пиксельного шейдера
{
    float4 Pos : SV_POSITION;     // Позиция пикселя в проекции (экранная)
    float2 Tex : TEXCOORD0;       // Координаты текстуры по tu, tv
    float3 Norm : TEXCOORD1;      // Относительная нормаль пикселя по tu, tv
	float3 WorldPos : TEXCOORD2;
};

struct PS_INPUT4
{
    float4 Pos : SV_POSITION;
    float4 PosW : RWORLD;		// Позиция относительно сцены (позиция в виртуальном пространстве)
    float4 PosC : RCAMERA;	// Позиция относительно камеры
	float3 Norm : NORMAL;	// Нормаль к поверхности
				// Семантика - желательно заглавными латинскими буквами!
};

//--------------------------------------------------------------------------------------
// Вершинный шейдер
//--------------------------------------------------------------------------------------

PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = mul( input.Norm, World );
    output.Tex = input.Tex;
	output.WorldPos = mul( input.Pos, World );
    return output;
}

//--------------------------------------------------------------------------------------
// Vertex Shader №4 (для металла)
//--------------------------------------------------------------------------------------
PS_INPUT4 VS4( VS_INPUT input ) 
{
    PS_INPUT4 output = (PS_INPUT4)0;
	output.PosW = mul( input.Pos, World ); // Передаем позицию относительно сцены в пиксельный шейдер (с интерполяцией при растеризации)
    output.PosC = mul( output.PosW, View );  // Передаем позицию относительно камеры в пиксельный шейдер (с интерполяцией при растеризации)
    output.Pos = mul( output.PosC, Projection );	// Позиция относительно экрана
    output.Norm = mul( input.Norm, World );
    return output;
}

//--------------------------------------------------------------------------------------
// Пиксельный шейдер для объектов
//--------------------------------------------------------------------------------------

float4 PS( PS_INPUT input) : SV_Target
{

    float4 finalColor = 0;   				// чёрный цвет без освещения
											// складываем освещенность пикселя от всех источников света

    for(int i=0; i<2; i++){

        finalColor += saturate( dot( (float3)vLightDir[i], input.Norm) * vLightColor[i] );

    }

    finalColor *= txDiffuse.Sample( samLinear, input.Tex );
    finalColor.a = 1.0f;
    return finalColor;
}

//--------------------------------------------------------------------------------------
// Пиксельный шейдер для фона
//--------------------------------------------------------------------------------------

float4 PSBkg( PS_INPUT input) : SV_Target
{

    float4 finalColor = txDiffuse.Sample( samLinear, input.Tex );
    finalColor.a = 1.0f;
    return finalColor;
}

 
//--------------------------------------------------------------------------------------
// Пиксельный шейдер для источников света
//--------------------------------------------------------------------------------------

float4 PSSolid( PS_INPUT input) : SV_Target
{
    return vOutputColor;

}

float4 PSSpecular(PS_INPUT input) : SV_TARGET
{
    float4 diffuse = float4(1.0, 0.0, 0.0, 1.0);
    float4 finalColor = diffuse *0.1;
    float3 Eye = float3(0.0f, 1.0f, 10.0f);
    float4 intensity = 0.9;
    float power = 1;
	
	float3 V; 
    float3 R;
	V = normalize( Eye - input.WorldPos );
	
    for (int i = 0; i < 2; i++)
    {
		
		R = reflect( normalize( vLightDir[i] ), normalize( input.Norm ) );
        finalColor += intensity * vLightColor[i] * pow( saturate( dot( R, V ) ), power );
    }   
    return finalColor;
}

//--------------------------------------------------------------------------------------
// Pixel Shader №4 (для кубической текстуры c зеркальным отражением)
//--------------------------------------------------------------------------------------
float4 PSMirror( PS_INPUT4 input) : SV_Target
{
float3 reflection = reflect(normalize((float3)(input.PosW - CameraPos)), normalize(input.Norm));
float4 Color = txCube.Sample( samLinear, reflection );

// Эффект тумана
float4 R = input.PosC;
float r = sqrt( R.x * R.x + R.y * R.y + R.z * R.z );
float a = exp ( - Pale * r );
Color = a * Color + ( 1 - a ) * CommonColor;
return Color;
}

//--------------------------------------------------------------------------------------
// Pixel Shader №5 (для кубической текстуры c металлическим отражением)
//--------------------------------------------------------------------------------------
//float4 PSMetal( PS_INPUT4 input) : SV_Target
//{
//float3 b = normalize((float3)(input.PosW-CameraPos));
//float c;

	//float3 H  =  normalize(-b) + normalize(LightDir);
	//float HN = dot ( normalize(H), normalize(input.Norm) ); 
	//c = HN * HN;	


//c *= dot( input.Norm, LightDir ) >= 0 ;
//c *= saturate ( dot( normalize(input.Norm), normalize(LightDir) ) / SpotSize + 1 );

//float4 Color = MetalAmbientColor + c * LightColor;


// Эффект тумана
//float4 R = input.R0;
//float r = sqrt( R.x * R.x + R.y * R.y + R.z * R.z );
//float a = exp ( - Pale * r );
//Color = a * Color + ( 1 - a ) * CommonColor;

//return Color;
//}



