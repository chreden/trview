struct VertexInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}