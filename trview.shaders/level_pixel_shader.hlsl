cbuffer cb : register(b0)
{
    bool disable_transparency;
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
    float4 output = saturate(tex.Sample(samplerState, input.uv) * input.colour);
    if (disable_transparency)
    {
        output.a = 1.0f;
    }
    return output;
}