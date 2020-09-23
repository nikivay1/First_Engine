cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// 
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	// »змените вектор положени€ на 4 единицы дл€ правильных вычислений матрицы.
	input.position.w = 1.0f;

	// ¬ычислить положение вершины относительно матриц мира, вида и проекции.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	return output;
}