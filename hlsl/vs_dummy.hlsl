float4 main([[vk::location(0)]] float3 pos : POSITION) : SV_Position
{
    return float4(pos, 1.0);
}
