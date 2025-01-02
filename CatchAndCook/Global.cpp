#include "pch.h"
#include "Global.h"
#include "Core.h"
#include "Vertex.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 800;


uint8_t SRV_ROOT_INDEX = 10;
uint8_t UAV_ROOT_INDEX = 11;


std::vector<VertexProp> SkinData
{
    VertexProp::pos,
    VertexProp::normal,
    VertexProp::tangent,
    VertexProp::uv0,
    VertexProp::uv1,
    VertexProp::bone_ids,
    VertexProp::bone_weights,
};

std::vector<VertexProp> StaticData
{
    VertexProp::pos,
    //VertexProp::normal,
    VertexProp::uv0,
    VertexProp::uv1
};