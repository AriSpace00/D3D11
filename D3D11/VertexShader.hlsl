#include "Shared.hlsli"
// 정점 셰이더.

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    // 정점 좌표계 변환
    float4 pos = float4(input.PosModel, 1);

    float4x4 matWorld;
    matWorld = mul(input.BlendWeights.x, MatrixPaletteArray[input.BlendIndices.x]);
    matWorld += mul(input.BlendWeights.y, MatrixPaletteArray[input.BlendIndices.y]);
    matWorld += mul(input.BlendWeights.z, MatrixPaletteArray[input.BlendIndices.z]);
    matWorld += mul(input.BlendWeights.w, MatrixPaletteArray[input.BlendIndices.w]);
    //matWorld = World;

    pos = mul(pos, matWorld);
    output.PosWorld = pos.xyz;

    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.PosProjection = pos;

    // 텍스처 로드
    output.Texcoord = input.Texcoord;

    // 물체의 노멀벡터와 탄젠트벡터를 월드좌표계의 행렬로 변환한 후 노멀라이즈
    // 스케일 변환에 영향을 받지 않는 노멀 벡터를 구하기 위함
    output.NorWorld = normalize(mul(input.NorModel, (float3x3) matWorld));
    output.TanWorld = normalize(mul(input.TanModel, (float3x3) matWorld));

    return output;
}