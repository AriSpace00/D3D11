Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);

SamplerState samLinear : register(s0);

cbuffer Transform : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer DirectionLight : register(b1)
{
    float3 LightDirection;
    float DirectionLight_Pad0;
    float4 LightAmbient;
    float4 LightDiffuse;
    float4 LightSpecular;
    float3 EyePosition;
    float DirectionLight_Pad1;
}

cbuffer Material : register(b2)
{
    float4 MaterialAmbient;
    float4 MaterialDiffuse;
    float4 MaterialSpecular;
    float4 MaterialEmissive;
    float MaterialSpecularPower;
    bool UseDiffuseMap;
    bool UseNormalMap;
    bool UseSpecularMap;
    bool UseEmissiveMap;
    bool UseOpacityMap;
    float2 MaterialPad0;
}

cbuffer MatrixPalette : register(b3)
{
    matrix MatrixPaletteArray[128];
}

struct VS_INPUT
{
    float3 PosModel : POSITION;
    float2 Texcoord : TEXCOORD0;
    float3 NorModel : NORMAL;
    float3 TanModel : TANGENT;
    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHTS;
};

struct PS_INPUT
{
    float4 PosProjection : SV_POSITION;
    float3 PosWorld : POSITION;
    float2 Texcoord : TEXCOORD0;
    float3 NorWorld : NORMAL;
    float3 TanWorld : TANGENT;
};



