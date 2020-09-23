cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};
struct VertexInputType
{
    float4 position: POSITION;
    float2 tex: TEXCOORD0;
    float3 normal: NORMAL;
};

struct PixelInputType
{
    float4 position: SV_POSITION;
    float2 tex: TEXCOORD0;
    float3 normal: NORMAL;
};

PixelInputType LightVS(VertexInputType input)
{
    PixelInputType output;

    // »змените вектор положени€ на 4 единицы дл€ правильных вычислений матрицы.
    input.position.w = 1.0f;

    // ¬ычислить положение вершины относительно матриц мира, вида и проекции.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // —охран€ем координаты текстуры дл€ пиксельного шейдера.
    output.tex = input.tex;

    // ¬ычислить вектор нормали только против мировой матрицы.
    output.normal = mul(input.normal, (float3x3) worldMatrix);

    // Ќормализовать вектор нормали.
    output.normal = normalize(output.normal);

    return output;
}