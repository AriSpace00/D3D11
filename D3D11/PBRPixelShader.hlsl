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
		// ǥ���� �޴� �ݱ��� ���� ���⿡�� ���� �������� ���ø��Ѵ�. Lambertian BRDF�� �����Ͽ� ���ԵǾ� �ִ�.
        float3 irradiance = txEnvDiffuse.Sample(samLinear, Normal).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions 
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		// �񽺵��� ��������, cos���� �� ������ Ư���Ҽ� ���� �ݱ��� ���� ���⿡�� ���� �������̹Ƿ� 
		// dot(Half,View)�� ��������ʰ�  dot(Normal,View)�� ����Ѵ�.
        float3 F = fresnelSchlick(FresenalFactor, CosNV);

		// Get diffuse contribution factor (as with direct lighting).
		// ǥ���� ������ ���Ѵ�.
        float3 kd = lerp(1.0 - F, 0.0, Metalic);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		// txIBL_Diffuse �ʿ��� Lambertian BRDF�� �����Ͽ� ���ԵǾ� �����Ƿ� PI�� �����ʿ䰡 ����.
        float3 diffuseIBL = kd * Albedo * irradiance; // IBL�� diffuse ��

		// Sample pre-filtered specular reflection environment at correct mipmap level.
        uint specularTextureLevels = querySpecularTextureLevels(); // ��ü LOD �Ӹ� ������ 
		// View,Normal �ݻ纤�͸� ����Ͽ� ���ø��Ѵ�. ��ĥ�⿡ ���� ������ �ݻ���� ǥ���ϱ�����  LOD ������ ����ȴ�. 
        float3 Lr = 2.0 * CosNV * Normal - ViewVector;
        float3 specularIrradiance = txEnvSpecular.SampleLevel(samLinear, Lr, Roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		// dot(Normal,View) , roughness�� �ؼ���ǥ�� ���ø��Ͽ� �̸����� x,y���� �����´�.
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