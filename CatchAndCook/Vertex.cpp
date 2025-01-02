#include "pch.h"
#include "Vertex.h"

SelectorInfo SelectorInfo::GetInfo(const std::vector<VertexProp>& props)
{
    SelectorInfo info;
    VertexPropInfo propInfo;
    info.props = props;


    int offset = 0;
    int byteOffset = 0;
    for (int i = 0; i < props.size(); i++)
    {
        auto& select = props[i];
        int elementTypeSize = sizeof(float);
        int byteSize = 0;
        int index = 0;
        switch (select)
        {
        case VertexProp::pos:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::normal:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::tangent:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::bitangent:
            byteSize = sizeof(Vector3);
            break;
        case VertexProp::color:
            byteSize = sizeof(Vector4);
            break;
        case VertexProp::uv:
        case VertexProp::uv0:
            byteSize = sizeof(Vector2);
            index = 0;
            break;
        case VertexProp::uv1:
            byteSize = sizeof(Vector2);
            index = 1;
            break;
        case VertexProp::uv2:
            byteSize = sizeof(Vector2);
            index = 2;
            break;
        case VertexProp::uv3:
            byteSize = sizeof(Vector2);
            index = 3;
            break;
        case VertexProp::uv4:
            byteSize = sizeof(Vector2);
            index = 4;
            break;
        case VertexProp::uv5:
            byteSize = sizeof(Vector2);
            index = 5;
            break;
        case VertexProp::uv6:
            byteSize = sizeof(Vector2);
            index = 6;
            break;
        case VertexProp::uv7:
            byteSize = sizeof(Vector2);
            index = 7;
            break;
        case VertexProp::bone_id0:
            byteSize = sizeof(float);
            index = 0;
            break;
        case VertexProp::bone_id1:
            byteSize = sizeof(float);
            index = 1;
            break;
        case VertexProp::bone_id2:
            byteSize = sizeof(float);
            index = 2;
            break;
        case VertexProp::bone_id3:
            byteSize = sizeof(float);
            index = 3;
            break;
        case VertexProp::bone_ids:
            byteSize = sizeof(Vector4);
            index = 0;
            break;
        case VertexProp::bone_w0:
            byteSize = sizeof(float);
            index = 0;
            break;
        case VertexProp::bone_w1:
            byteSize = sizeof(float);
            index = 1;
            break;
        case VertexProp::bone_w2:
            byteSize = sizeof(float);
            index = 2;
            break;
        case VertexProp::bone_w3:
            byteSize = sizeof(float);
            index = 3;
            break;
        case VertexProp::bone_weights:
            byteSize = sizeof(Vector4);
            index = 0;
            break;
        }
        int size = byteSize / elementTypeSize;

        propInfo.offset = offset;
        propInfo.byteOffset = byteOffset;
        propInfo.size = size;
        propInfo.byteSize = byteSize;
        propInfo.prop = select;
        propInfo.index = index;

        info.propInfos.push_back(propInfo);

        offset += size;
        byteOffset += byteSize;
    }
    info.totalByteSize = byteOffset;
    info.totalSize = offset;
    info.propCount = static_cast<int>(props.size());
    return info;
}