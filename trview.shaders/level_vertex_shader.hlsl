cbuffer cb : register (b0)
{
    matrix scale      : packoffset(c0);
    float4 colour     : packoffset(c4);
    float4 light_dir  : packoffset(c5);
    bool light_enable : packoffset(c6);
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

    // float d = light_enable ? dot(light_dir, input.normal) : 1.0f;
    // float d = dot(light_dir, input.normal);
    // output.colour = colour * input.colour * d;
    // output.colour.a = 1.0f;
    // output.colour = float4(normalize(input.normal), 1);

    // float d = light_enable ? dot(light_dir, normalize(input.normal)) : 1.0f;
    // float d = dot(light_dir, normalize(input.normal));
    // output.colour = colour * input.colour * d;
    // output.colour = float4(light_dir, 1);
//     output.colour = input.colour * colour;
    // output.colour = float4(normalize(input.normal), 1);
    // output.colour = float4(normalize(light_dir), 1);
    // output.colour = colour2 * input.colour;
    // output.colour = colour * input.colour;

    //float d = light_enable ? dot(light_dir, normalize(input.normal)) : 1.0f;
    float d = 1.0f;
    output.colour = input.colour * colour * d;
    return output;
}