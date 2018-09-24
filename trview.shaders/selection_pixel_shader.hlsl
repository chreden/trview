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
    float4 t = tex.Sample(samplerState, input.uv);

    // Should give a left side outline??
    if (t.r > 0 && 
        (tex.Sample(samplerState, input.uv + float2(width_pixel, 0)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(width_pixel, height_pixel)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(0, height_pixel)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(-width_pixel, height_pixel)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(-width_pixel, 0)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(-width_pixel, -height_pixel)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(0, -height_pixel)).r < 1 ||
        tex.Sample(samplerState, input.uv + float2(width_pixel, -height_pixel)).r < 1))
    {
        t = float4(1, 1, 1, 1);
    }
    else
    {
        clip(-1);
    }

    return t;
}