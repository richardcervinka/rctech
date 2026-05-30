static const float2 positions[3] = {
    float2(-1.0, -3.0),
    float2(3.0, 1.0),
    float2(-1.0, 1.0)
};

static const float3 colors[3] = {
    float3(1, 0, 0),
    float3(0, 1, 0),
    float3(0, 0, 1)
};

struct VsOutput
{
    float3 color : COLOR;
    float4 sv_position : SV_Position;
};

VsOutput main(uint vid : SV_VertexID)
{
    VsOutput output;
    output.sv_position = float4(positions[vid], 0.0, 1.0);
    output.color = colors[vid];
    return output;
}
