struct PixelInput
{
    float2 uv : TEXCOORD0;
};

Texture2D tex;
SamplerState samplerState;

float4 main(PixelInput input) : SV_TARGET
{
    return tex.Sample(samplerState, input.uv);
}