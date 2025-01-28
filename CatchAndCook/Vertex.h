#pragma once

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using vec2 = DirectX::SimpleMath::Vector2;
using vec3 = DirectX::SimpleMath::Vector3;
using vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Ray = DirectX::SimpleMath::Ray;
using Quaternion = DirectX::SimpleMath::Quaternion;


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

enum class VertexType
{
    Vertex_Skinned,
    Vertex_Static
};

struct Vertex_Skinned
{
    vec3 position = vec3(0, 0, 0);
    vec3 normal = vec3(0, 0, 1);
    vec3 tangent = vec3(0, 1, 0);
    vec4 color = vec4(1,1,1,1);
    std::array<vec2, 3> uvs;
    vec4 boneId = vec4(-1, -1, -1, -1);
    vec4 boneWeight = vec4(0, 0, 0, 0);
};

struct Vertex_Static
{
    vec3 position = vec3(0, 0, 0);
    vec2 uv;
    vec3 normal = vec3(0, 0, 1);
};

struct Vertex_Color
{
    vec3 position = vec3(0, 0, 0);
    vec2 uv;
    vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};


//struct Vertex_Full
//{
//    Vector3 position = Vector3(0, 0, 0);
//    Vector3 normal = Vector3(0, 0, 1);
//    Vector3 tangent = Vector3(0, 1, 0);
//    Vector3 bitangent = Vector3(0, 0, 0);
//    Vector4 color = Vector4(1, 1, 0, 1);
//    std::array<Vector3, 8> uvs;
//    Vector4 boneId = Vector4(-1, -1, -1, -1);
//    Vector4 boneWeight = Vector4(0, 0, 0, 0);
//};
