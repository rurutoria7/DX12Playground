#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]

float4 main(float3 aPos : aPos) : SV_Position
{
    return float4(aPos, 1.0f);
}