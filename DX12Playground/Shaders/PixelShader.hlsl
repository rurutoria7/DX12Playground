#include "RootSignature.hlsl"

[RootSignature(ROOTSIG)]

float4 main(): SV_Target
{
    return float4(0.5f, 1.0f, 1.0f, 1.0f);
}