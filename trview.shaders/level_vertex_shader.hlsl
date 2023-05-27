cbuffer cb : register (b0)
{
    matrix scale;
    float4 colour;
    float4 light_dir;
    float light_intensity;
    int light_enable;
    int use_colour_override;
    float4 colour_override;
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
    output.colour = colour;

    if (use_colour_override)
    {
         output.colour *= colour_override;
    }
    else
    {
        output.colour *= input.colour;
    }

    if (light_enable != 0)
    {
        output.colour *= max(0.2f, dot(light_dir, normalize(float4(input.normal, 1)))) * light_intensity;
        output.colour.a = 1.0f;
    }

    return output;
}