cbuffer cb : register (b0)
{
    matrix scale : packoffset (c0);
    float4 colour : packoffset(c4);
}

struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 colour : TEXCOORD1;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 colour : TEXCOORD1;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    output.position = mul(scale, input.position);
    output.uv = input.uv;
    output.colour = colour * input.colour;
    return output;
}