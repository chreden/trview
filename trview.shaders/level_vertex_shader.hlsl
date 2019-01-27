cbuffer cb : register (b0)
{
    matrix scale : packoffset (c0);
    float4 colour : packoffset(c4);
    float3 light_dir : packoffset(c5);
    bool light_enable : packoffset(c5.w);
}

struct VertexInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
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