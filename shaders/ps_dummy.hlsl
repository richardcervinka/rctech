struct VsOutput
{
    float3 color : COLOR;
    float4 sv_position : SV_Position;
};

float4 main(VsOutput vs_output) : SV_Target0
{
    return float4(vs_output.color, 1.0);
}