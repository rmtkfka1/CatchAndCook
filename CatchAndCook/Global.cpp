#include "pch.h"
#include "Global.h"
#include "Core.h"
#include "Vertex.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

DXGI_FORMAT SWAP_CHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

uint8_t CBV_ROOT_INDEX_COUNT = 16;
uint8_t SRV_TABLE_INDEX = 16;
uint8_t UAV_TABLE_INDEX = 17;


std::vector<VertexProp> SkinProp
{
    VertexProp::pos,
    VertexProp::normal,
    VertexProp::tangent,
    VertexProp::uv0,
    VertexProp::uv1,
    VertexProp::bone_ids,
    VertexProp::bone_weights,
};

std::vector<VertexProp> StaticProp
{
    VertexProp::pos,
    //VertexProp::normal,
    VertexProp::uv0,
    VertexProp::uv1
};