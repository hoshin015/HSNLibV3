#include "ShadowMapCaster.hlsli"


float4 main(VS_IN vin) : SV_POSITION
{   
    float4 blendedPosition = { 0, 0, 0, 1 };
    for (int boneIndex = 0; boneIndex < 4; ++boneIndex)
    {
        blendedPosition += vin.boneWeights[boneIndex] * mul(vin.position, boneTransforms[vin.boneIndices[boneIndex]]);
    }
    vin.position = float4(blendedPosition.xyz, 1.0f);
    
    return mul(vin.position, mul(world, lightViewProjection));
}