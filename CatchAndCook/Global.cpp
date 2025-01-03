#include "pch.h"
#include "Global.h"
#include "Core.h"
#include "Vertex.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

DXGI_FORMAT SWAP_CHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

size_t CBV_ROOT_INDEX_COUNT = 16;
size_t SRV_TABLE_INDEX = CBV_ROOT_INDEX_COUNT + 0;
size_t UAV_TABLE_INDEX = CBV_ROOT_INDEX_COUNT + 1;

size_t SRV_TABLE_REGISTER_COUNT = 8;
size_t UAV_TABLE_REGISTER_COUNT = 4;


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