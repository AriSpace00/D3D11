#include "Shared.hlsli"

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // ���� ��ǥ�� ��ȯ
    float4 pos = float4(input.PosModel, 1);

    float4x4 matWorld;
    matWorld = World;

    pos = mul(pos, matWorld);
    output.PosWorld = pos.xyz;

    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.PosProjection = pos;

    // �ؽ�ó �ε�
    output.Texcoord = input.Texcoord;

    // ��ü�� ��ֺ��Ϳ� ź��Ʈ���͸� ������ǥ���� ��ķ� ��ȯ�� �� ��ֶ�����
    // ������ ��ȯ�� ������ ���� �ʴ� ��� ���͸� ���ϱ� ����
    output.NorWorld = normalize(mul(input.NorModel, (float3x3) matWorld));
    output.TanWorld = normalize(mul(input.TanModel, (float3x3) matWorld));

    output.PosShadow = mul(output.PosWorld, ShadowView);
    output.PosShadow = mul(output.PosShadow, ShadowProjection);

    return output;
}