struct Input
{
    [[vk::location(0)]] float3 position : POSITION; // POSITION;  SV_Position = clip space
    [[vk::location(1)]] float3 color : COLOR;
};

struct Output
{
    float4 position : SV_Position;
    float3 color : COLOR;
};

Output main(uint vid : SV_VertexID, Input input)
{
    Output output;
    output.position = float4(input.position, 1.0);
    output.color = float4(input.color, 1.0);
    return output;
}
