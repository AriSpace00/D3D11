#include "Shared.hlsli"

PS_INPUT_ENVIRONMENT main(VS_INPUT input)
{
    PS_INPUT_ENVIRONMENT output = (PS_INPUT_ENVIRONMENT) 0;

    // 정점 좌표계 변환
    float4 pos = mul(input.PosModel, World);
    output.Texcoord = pos.xyz;
    float3 pos3 = mul(pos.xyz, (float3x3) View);
    pos = mul(float4(pos3, 1.0f), Projection);
    output.PosProjection = pos;

    return output;
}