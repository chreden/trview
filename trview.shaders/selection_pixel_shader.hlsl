cbuffer cb : register (b0)
{
    float width_pixel;
    float height_pixel;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 colour : TEXCOORD1;
};

Texture2D tex;
SamplerState samplerState;

float4 main(PixelInput input) : SV_TARGET
{
    // If this is a red pixel and there are surrounding pixels that are black, make it a white pixel.
    float2x4 samples = 
    {
        1 - tex.Sample(samplerState, input.uv + float2(width_pixel, 0)).r,
        1 - tex.Sample(samplerState, input.uv + float2(width_pixel, height_pixel)).r,
        1 - tex.Sample(samplerState, input.uv + float2(0, height_pixel)).r,
        1 - tex.Sample(samplerState, input.uv + float2(-width_pixel, height_pixel)).r,
        1 - tex.Sample(samplerState, input.uv + float2(-width_pixel, 0)).r,
        1 - tex.Sample(samplerState, input.uv + float2(-width_pixel, -height_pixel)).r,
        1 - tex.Sample(samplerState, input.uv + float2(0, -height_pixel)).r,
        1 - tex.Sample(samplerState, input.uv + float2(width_pixel, -height_pixel)).r
    };

    clip(-1.5 + tex.Sample(samplerState, input.uv).r + any(samples));
    return float4(1, 0, 1, 1);
}