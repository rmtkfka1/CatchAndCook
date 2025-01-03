#pragma once
#include "Vertex.h"

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern DXGI_FORMAT SWAP_CHAIN_FORMAT;
extern DXGI_FORMAT DEPTH_STENCIL_FORMAT;

extern size_t CBV_ROOT_INDEX_COUNT;
extern size_t SRV_TABLE_INDEX;
extern size_t UAV_TABLE_INDEX;

extern size_t SRV_TABLE_REGISTER_COUNT;
extern size_t UAV_TABLE_REGISTER_COUNT;

extern std::vector<VertexProp> SkinProp;
extern std::vector<VertexProp> StaticProp;

const uint32 SWAP_CHAIN_FRAME_COUNT = 4; // 1 Front + (N-1) Back Buffer
const uint32 MAX_FRAME_COUNT = SWAP_CHAIN_FRAME_COUNT - 1;