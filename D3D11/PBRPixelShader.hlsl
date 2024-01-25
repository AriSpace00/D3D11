#include "Shared.hlsli"

static const float PI = 3.141592;
static const float Epsilon = 0.00001;
static const float3 Fdielectric = 0.04;

float ndfGGX(float cosNH, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosNH * cosNH) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosNL, float cosNV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return gaSchlickG1(cosNL, k) * gaSchlickG1(cosNV, k);
}

float3 fresnelSchlick(float3 fresenalFactor, float cosLH)
{
    return fresenalFactor + (1.0 - fresenalFactor) * pow(1.0 - cosLH, 5.0);
}

uint querySpecularTextureLevels()
{
    uint width, height, levels;
    txEnvSpecular.GetDimensions(0, width, height, levels);
    return levels;
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
    float3 Albedo = 0.0f;
    if (UseDiffuseMap)
    {
        Albedo = txDiffuse.Sample(samLinear, input.Texcoord).rgb;
    }
    else
    {
        Albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    Albedo.rgb = pow(Albedo, 2.2);

    float3 LightDirVector = normalize(LightDirection.xyz);
    float3 ViewVector = normalize(EyePosition - input.PosWorld);
    float3 HalfVector = normalize(-LightDirVector + ViewVector);

    float CosNH = max(0.0f, dot(Normal, HalfVector));
    float CosVH = max(0.0f, dot(ViewVector, HalfVector));
    float CosNL = max(0.0f, dot(Normal, -LightDirVector));
    float CosNV = max(0.0f, dot(Normal, ViewVector));

    // PBR
    float Metalic = 0.f;
    if (UseMetalicMap)
    {
        Metalic = txMetalic.Sample(samLinear, input.Texcoord).r;
    }

    float Roughness = 0.f;
    if (UseRoughnessMap)
    {
        Roughness = txRoughness.Sample(samLinear, input.Texcoord).r;
    }

    float3 FresenalFactor = lerp(Fdielectric, Albedo, Metalic);

    float3 F = fresnelSchlick(FresenalFactor, CosVH);
    float D = ndfGGX(CosNH, max(0.1, Roughness));
    float G = gaSchlickGGX(CosNL, CosNV, Roughness);

    float3 Kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), Metalic);
    float3 DiffuseBRDF = Kd * Albedo / PI;
    float3 SpecularBRDF = (F * D * G) / max(Epsilon, 4.0 * CosNL * CosNV);

    float3 PBR = (DiffuseBRDF + SpecularBRDF) * CosNL;

    // Ambient
    float3 Ambient = 0;
    if (UseIBL)
    {
	    // Sample diffuse irradiance at normal direction.
		// 표면이 받는 반구의 여러 방향에서 오는 간접광을 샘플링한다. Lambertian BRDF를 가정하여 포함되어 있다.
        float3 irradiance = txEnvDiffuse.Sample(samLinear, Normal).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions 
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		// 비스듬이 보는정도, cos값은 빛 방향을 특정할수 없는 반구의 여러 방향에서 오는 간접광이므로 
		// dot(Half,View)를 사용하지않고  dot(Normal,View)을 사용한다.
        float3 F = fresnelSchlick(FresenalFactor, CosNV);

		// Get diffuse contribution factor (as with direct lighting).
		// 표면산란 비율을 구한다.
        float3 kd = lerp(1.0 - F, 0.0, Metalic);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		// txIBL_Diffuse 맵에는 Lambertian BRDF를 가정하여 포함되어 있으므로 PI로 나눌필요가 없다.
        float3 diffuseIBL = kd * Albedo * irradiance; // IBL의 diffuse 항

		// Sample pre-filtered specular reflection environment at correct mipmap level.
        uint specularTextureLevels = querySpecularTextureLevels(); // 전체 LOD 밉맵 레벨수 
		// View,Normal 반사벡터를 사용하여 샘플링한다. 거칠기에 따라 뭉게진 반사빛을 표현하기위해  LOD 보간이 적용된다. 
        float3 Lr = 2.0 * CosNV * Normal - ViewVector;
        float3 specularIrradiance = txEnvSpecular.SampleLevel(samLinear, Lr, Roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		// dot(Normal,View) , roughness를 텍셀좌표로 샘플링하여 미리계산된 x,y값을 가저온다.
        float2 specularBRDF = txEnvSpecular.Sample(samplerClamp, float2(CosNV, Roughness)).rg;

		// Total specular IBL contribution.
        float3 specularIBL = (FresenalFactor * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
        Ambient = (diffuseIBL + specularIBL) * AmbientOcclusion;
    }

    // Final
    float3 finalColor = PBR + Ambient;
    finalColor = pow(finalColor, 1 / 2.2);

    return float4(finalColor, 1.0f);
}