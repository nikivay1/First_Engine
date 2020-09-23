Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct PixelInputType
{
    float4 position: SV_POSITION;
    float2 tex: TEXCOORD0;
    float3 normal: NORMAL;
};

float4 LightPS(PixelInputType input): SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    // Выборка цвета пикселя из текстуры с помощью сэмплера в этой координате текстуры. 
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    // Инвертируем направление света для расчетов. 
    lightDir = -lightDirection;

    // Рассчитываем количество света на этот пиксель. 
    lightIntensity = saturate(dot(input.normal, lightDir));

    // Определяем окончательное количество диффузного цвета на основе диффузного цвета в сочетании с интенсивностью света. 
    color = saturate(diffuseColor * lightIntensity);

    // Умножаем пиксель текстуры и окончательный диффузный цвет, чтобы получить окончательный результат цвета пикселя. 
    color = color * textureColor;

    return color;
}