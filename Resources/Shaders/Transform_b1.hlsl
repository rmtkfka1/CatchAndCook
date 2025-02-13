
#ifndef INCLUDE_TRANSFORM
#define INCLUDE_TRANSFORM

cbuffer Transform : register(b1)
{
    row_major matrix LocalToWorldMatrix;
    float3 worldPosition;
}

#endif
