#include "pch.h"
#include "Global.h"
#include "Core.h"
#include "Vertex.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

bool Initalize=false;

DXGI_FORMAT SWAP_CHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;

size_t CBV_ROOT_INDEX_COUNT = 8;

size_t SRV_TABLE_INDEX = CBV_ROOT_INDEX_COUNT + 0;
size_t UAV_TABLE_INDEX = CBV_ROOT_INDEX_COUNT + 1;
size_t SPRITE_TABLE_INDEX = CBV_ROOT_INDEX_COUNT + 2;

size_t GLOBAL_SRV_SHADOW_INDEX    = CBV_ROOT_INDEX_COUNT + 3;
size_t GLOBAL_SRV_POSITION_INDEX  = CBV_ROOT_INDEX_COUNT + 4;
size_t GLOBAL_SRV_NORAML_INDEX    = CBV_ROOT_INDEX_COUNT + 5;
size_t GLOBAL_SRV_COLOR_INDEX     = CBV_ROOT_INDEX_COUNT + 6;
size_t GLOBAL_SRV_DEPTH_INDEX     = CBV_ROOT_INDEX_COUNT + 7;

//size_t SRV_TABLE_REGISTER_COUNT = 8;
size_t UAV_TABLE_REGISTER_COUNT = 4;


std::vector<VertexProp> SkinProp
{
    VertexProp::pos,
	VertexProp::normal,
    VertexProp::tangent,
    VertexProp::color,
    VertexProp::uv0,
    VertexProp::uv1,
    VertexProp::uv2,
    VertexProp::bone_ids,
    VertexProp::bone_weights,
};

std::vector<VertexProp> StaticProp
{
    VertexProp::pos,
    VertexProp::uv0,
    VertexProp::normal,
    //VertexProp::uv1
};

std::vector<VertexProp> SpriteProp
{ 
    VertexProp::pos,
    //VertexProp::normal,
    VertexProp::uv0,
    VertexProp::color
};