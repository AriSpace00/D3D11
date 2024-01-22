#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    float Opacity = 1.0f;

    float3 Normal = normalize(input.NorWorld);
    float3 Tangent = normalize(input.TanWorld);
    float3 BiTangent = cross(Normal, Tangent);

    if (UseNormalMap)
    {
        float3 NormalTangentSpace = txNormal.Sample(samLinear, input.Texcoord).rgb * 2.0 - 1.0f;
        float3x3 WorldTransform = float3x3(Tangent, BiTangent, Normal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        Normal = normalize(Normal);
    }

    float3 vLightDirection = normalize(LightDirection.xyz);
    float3 View = normalize(EyePosition - input.PosWorld);

    // Diffuse
    float NDotL = max(dot(Normal, -vLightDirection), 0);
    float4 Diffuse = LightDiffuse * NDotL;
    float4 DiffuseColor = txDiffuse.Sample(samLinear, input.Texcoord);
    if (UseDiffuseMap)
    {
        // 방향이 다른 두 벡터(Normal, vLightDirection)을 내적하면 음수가 도출된다.
        // 따라서 부호를 맞춰주기 위해 vLightDirection에 -를 곱해준다.
        Diffuse *= DiffuseColor;
    }
    else
    {
        DiffuseColor.rgb = float4(0.8f, 0.8f, 0.8f, 1.0f);
        Diffuse *= DiffuseColor;
    }

    // Specular Map
    float3 HalfVector = normalize(-vLightDirection + View);
    float fHDotN = max(dot(HalfVector, Normal), 0);
    float4 Specular = pow(fHDotN, MaterialSpecularPower) * LightSpecular;
    if (UseSpecularMap)
    {
        Specular *= txSpecular.Sample(samLinear, input.Texcoord).r;
    }

    float4 Emissive = 0;
    if (UseEmissiveMap)
    {
        Emissive = txEmissive.Sample(samLinear, input.Texcoord) * MaterialEmissive;
    }

    if (UseOpacityMap)
    {
        Opacity = txOpacity.Sample(samLinear, input.Texcoord).a;
    }

    // 구성요소를 곱하면 값이 조절되는 것이지 합쳐지지 않음
    // 따라서 곱셈이 아닌 덧셈으로 각 구성요소를 결합함
    finalColor = Diffuse + Specular + Emissive;

    return float4(finalColor.rgb, Opacity);
}