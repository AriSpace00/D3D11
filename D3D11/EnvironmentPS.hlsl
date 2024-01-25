#include "Shared.hlsli"

float4 main(PS_INPUT_ENVIRONMENT input) : SV_Target
{
    return txEnvTexture.Sample(samLinear, input.Texcoord);
}