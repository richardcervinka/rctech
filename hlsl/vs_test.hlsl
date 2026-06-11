struct Input
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float3 color : COLOR;
};

struct Output
{
    float4 position : SV_Position; // clip space
    float3 color : COLOR;
};

static const float4x4 test_projection =
{
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.010101, 0.101010,
    0.0, 0.0, -1.0, 0.0
};

Output main(uint vid : SV_VertexID, Input input)
{
    Output output;
    output.position = mul(test_projection, float4(input.position, 1.0));
    output.color = float4(input.color, 1.0);
    return output;
}
