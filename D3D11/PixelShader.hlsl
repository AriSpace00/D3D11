#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    float Opacity = 1.0f;
    
    // 물체로부터 뻗어나가는 노멀 방향만 남기기 위해 노멀라이즈
    float3 Normal = normalize(input.NorWorld);

    // 노말맵 적용을 위한 탄젠트벡터, 바이탄젠트벡터 방향만 남기기 위해 노멀라이즈
    float3 Tangent = normalize(input.TanWorld);
    float3 BiTangent = cross(Normal, Tangent);

    if (UseNormalMap)
    {
        float3 NormalTangentSpace = txNormal.Sample(samLinear, input.Texcoord).rgb * 2.0 - 1.0f;
        float3x3 WorldTransform = float3x3(Tangent, BiTangent, Normal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        Normal = normalize(Normal);
    }

    // 빛의 방향만 남기기 위해 노멀라이즈
    float3 vLightDirection = normalize(LightDirection.xyz);

    // View : 시선벡터
    // 내 시선 끝의 물체 픽셀로부터 카메라를 바라보는 방향만 남기기 위해 노멀라이즈
    float3 View = normalize(EyePosition - input.PosWorld);

    // Ambient Light
    // 모든 물체에 적용하는 바탕 색 * 물체가 Ambient를 조절하는 색
    float4 AmbientLight = LightAmbient * MaterialAmbient;

    // Diffuse
    float4 Diffuse = txDiffuse.Sample(samLinear, input.Texcoord);

    // Diffuse Light(Lambertian Lighting)
    float4 DiffuseColor = LightDiffuse * MaterialDiffuse * Diffuse;
    float4 DiffuseLight = dot(Normal, -vLightDirection);
    if (UseDiffuseMap)
    {
        // 방향이 다른 두 벡터(Normal, vLightDirection)을 내적하면 음수가 도출된다.
        // 따라서 부호를 맞춰주기 위해 vLightDirection에 -를 곱해준다.
        DiffuseLight *= DiffuseColor;
    }
    else
    {
        DiffuseColor.rgb = float4(0.8f, 0.8f, 0.8f, 1.0f);
        DiffuseLight *= DiffuseColor;
    }

    // Specular Map
    // Specular Map용 이미지에서 물체에 비출 빛들의 강도를 가져옴
    float SpecularIntensity = 1;
    if (UseSpecularMap)
    {
        SpecularIntensity = txSpecular.Sample(samLinear, input.Texcoord).r;
    }

    // Specular Light
    // Blinn Phong
    float4 SpecularLight;
    float3 HalfVector = normalize(-vLightDirection + View);
    float fHDotN = max(0.0f, dot(HalfVector, Normal));
    float4 BlinnPhong = pow(fHDotN, MaterialSpecularPower) * MaterialSpecular * LightSpecular * SpecularIntensity;
    SpecularLight = BlinnPhong;

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
    finalColor = saturate(AmbientLight + DiffuseLight + SpecularLight + Emissive);

    return float4(finalColor.rgb, Opacity);
}