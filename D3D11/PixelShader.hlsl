#include "Shared.hlsli"

float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    float Opacity = 1.0f;
    
    // ��ü�κ��� ������� ��� ���⸸ ����� ���� ��ֶ�����
    float3 Normal = normalize(input.NorWorld);

    // �븻�� ������ ���� ź��Ʈ����, ����ź��Ʈ���� ���⸸ ����� ���� ��ֶ�����
    float3 Tangent = normalize(input.TanWorld);
    float3 BiTangent = cross(Normal, Tangent);

    if (UseNormalMap)
    {
        float3 NormalTangentSpace = txNormal.Sample(samLinear, input.Texcoord).rgb * 2.0 - 1.0f;
        float3x3 WorldTransform = float3x3(Tangent, BiTangent, Normal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        Normal = normalize(Normal);
    }

    // ���� ���⸸ ����� ���� ��ֶ�����
    float3 vLightDirection = normalize(LightDirection.xyz);

    // View : �ü�����
    // �� �ü� ���� ��ü �ȼ��κ��� ī�޶� �ٶ󺸴� ���⸸ ����� ���� ��ֶ�����
    float3 View = normalize(EyePosition - input.PosWorld);

    // Ambient Light
    // ��� ��ü�� �����ϴ� ���� �� * ��ü�� Ambient�� �����ϴ� ��
    float4 AmbientLight = LightAmbient * MaterialAmbient;

    // Diffuse
    float4 Diffuse = txDiffuse.Sample(samLinear, input.Texcoord);

    // Diffuse Light(Lambertian Lighting)
    float4 DiffuseColor = LightDiffuse * MaterialDiffuse * Diffuse;
    float4 DiffuseLight = dot(Normal, -vLightDirection);
    if (UseDiffuseMap)
    {
        // ������ �ٸ� �� ����(Normal, vLightDirection)�� �����ϸ� ������ ����ȴ�.
        // ���� ��ȣ�� �����ֱ� ���� vLightDirection�� -�� �����ش�.
        DiffuseLight *= DiffuseColor;
    }
    else
    {
        DiffuseColor.rgb = float4(0.8f, 0.8f, 0.8f, 1.0f);
        DiffuseLight *= DiffuseColor;
    }

    // Specular Map
    // Specular Map�� �̹������� ��ü�� ���� ������ ������ ������
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

    // ������Ҹ� ���ϸ� ���� �����Ǵ� ������ �������� ����
    // ���� ������ �ƴ� �������� �� ������Ҹ� ������
    finalColor = saturate(AmbientLight + DiffuseLight + SpecularLight + Emissive);

    return float4(finalColor.rgb, Opacity);
}