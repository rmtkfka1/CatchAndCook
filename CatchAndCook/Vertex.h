#pragma once

enum class VertexProp
{
    pos = 0,
    normal,
    tangent,
    bitangent,
    color,
    uv,
    uv0,
    uv1,
    uv2,
    uv3,
    uv4,
    uv5,
    uv6,
    uv7,
    bone_ids,
    bone_id0,
    bone_id1,
    bone_id2,
    bone_id3,
    bone_weights,
    bone_w0,
    bone_w1,
    bone_w2,
    bone_w3,
    COUNT
};

const std::array<std::string, static_cast<size_t>(VertexProp::COUNT)> PropNameStrings = {
    "POSITION",
    "NORMAL",
    "TANGENT",
    "BITANGENT",
    "COLOR",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "TEXCOORD",
    "BONEIDs",
    "BONEID",
    "BONEID",
    "BONEID",
    "BONEID",
    "BONEWs",
    "BONEW",
    "BONEW",
    "BONEW",
    "BONEW"
};

struct VertexPropInfo
{
    int size = 0;
    int byteSize = 0;
    int offset = 0;
    int byteOffset = 0;
    int index = 0;
    VertexProp prop;
};

class SelectorInfo
{
public:
    int totalSize = 0;
    int totalByteSize = 0;
    int propCount = 0;
    std::vector<VertexPropInfo> propInfos;
    std::vector<VertexProp> props;

    static SelectorInfo GetInfo(const std::vector<VertexProp>& props);
};

