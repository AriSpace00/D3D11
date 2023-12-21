#include "Shared.hlsli"

static const float PI = 3.141592;
static const float Epsilon = 0.00001;
static const float3 Fdielectric = 0.04;

float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 main(PS_INPUT input) : SV_Target
{
    // Normal
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

    // Albedo
    float3 Albedo;
    if (UseDiffuseMap)
    {
        Albedo = txDiffuse.Sample(samLinear, input.Texcoord);
    }
    else
    {
        Albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    Albedo.rgb = pow(Albedo, 2.2);

    // Ambient
    float3 Ambient = LightAmbient * MaterialAmbient * Albedo;

    float3 LightDirVector = normalize(LightDirection.xyz);
    float3 ViewVector = normalize(EyePosition - input.PosWorld);
    float3 HalfVector = normalize(-LightDirVector + ViewVector);

    float CosNH = max(0.0f, dot(HalfVector, Normal));
    float CosLH = max(0.0f, dot(ViewVector, HalfVector));
    float CosNL = max(0.0f, dot(Normal, -LightDirVector));
    float CosNV = max(0.0f, dot(Normal, ViewVector));

    // PBR
    float Metalic = txMetalic.Sample(samLinear, input.Texcoord).r;
    float Roughness = txRoughness.Sample(samLinear, input.Texcoord).r;

    float LightReflection = 2.0 * CosNH * Normal - ViewVector;

    float3 FresenalFactor = lerp(Fdielectric, Albedo, Metalic);

    float3 F = fresnelSchlick(FresenalFactor, CosLH);
    float D = ndfGGX(CosNH, max(0.01, Roughness));
    float G = gaSchlickGGX(CosNL, CosNH, Roughness);

    float3 Kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), Metalic);
    float3 DiffuseBRDF = Kd * Albedo / PI;
    float3 SpecularBRDF = (F * D * G) / max(Epsilon, 4.0 * CosNL * CosNV);

    float3 PBR = (DiffuseBRDF + SpecularBRDF) * CosNL;

    // Final
    float3 finalColor = PBR + Ambient;
    finalColor = pow(finalColor, 1 / 2.2f);
    return float4(finalColor, 1.0f);
}