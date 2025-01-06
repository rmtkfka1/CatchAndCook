#pragma once

class Vertex;
class RenderTarget;
class Texture;

enum class CompOper
{
    Less,
    LEqual,
    Equal,
    GEqual,
    Greater,
    NotEqual,
    Always,
    Never
};

enum class CullingType
{
    NONE,
    FRONT,
    BACK,
    WIREFRAME
};

enum class RenderQueueType
{
    Sky = 1000,
    Opaque = 2000,
    AlphaTest = 2500,
    Transparent = 3000,
    Geometry = 1500,
    UI = 4000
};

enum class FrontWise
{
    CW,
    CCW
};

enum class BlendType
{
    AlphaBlend = 0, //FinalColor = SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
    Add, //FinalColor = SrcColor + DestColor
    Multiple, //FinalColor = SrcColor * DestColor
    ColorDodge, //FinalColor = DestColor / (1 - SrcColor)
    Subtract, //FinalColor = DestColor - SrcColor
    Screen, // FinalColor = 1 - (1 - SrcColor) * (1 - DestColor)
};

struct ShaderStructPropertyInfo
{
    std::string elementType;
    int elementTypeRange = 0;
    std::string semantic;
    int semanticIndex = 0;
    int registerIndex = 0;
};

struct ShaderStructInfo
{
    std::string name;
    int count = 0;
    std::vector<ShaderStructPropertyInfo> propertys;
};

struct ShaderRegisterInfo
{
    std::string elementType;
    std::string name;
    int registerIndex = 0;
    char registerType = 0;
    std::string registerTypeString = "b0";
    int registerCount = 0;
    std::string bufferType;
    int space = 0;
    int numSample = 0;
};

struct ShaderCBufferPropertyInfo
{
    std::string _name;
    std::string elementType;
    std::string _className;

    int elementCount = 0;
    int byteOffset = 0;
    int byteSize = 0;
    int index = 0;
    int rowCount = 0;
    int colCount = 0;
};

struct ShaderCBufferInfo
{
    std::string name;
    int index;
    int bufferByteSize = 0;
    std::vector<ShaderCBufferPropertyInfo> propertys;
};


class ShaderProfileInfo
{
public:
    std::vector<ShaderRegisterInfo> registers;
    std::vector<ShaderCBufferInfo> cbuffers;

    //shader shaderType names ¥‹¿ß∑Œ
    std::unordered_map<std::string, ShaderStructInfo> _typeToStructTable;
    std::unordered_map<std::string, ShaderCBufferInfo> _nameToCBufferTable;
    std::unordered_map<std::string, ShaderRegisterInfo> _nameToRegisterTable;

    ShaderRegisterInfo GetRegisterByName(const std::string& name);
    ShaderCBufferInfo GetCBufferByName(const std::string& name);
};

// --------------

struct ShaderCode
{
    std::string shaderType;
    std::string endPointName;

    ComPtr<ID3DBlob> _blob;
    ComPtr<ID3DBlob> _errorBlob;
    std::vector<char> _shaderPrecompiledBuffer;
    D3D12_SHADER_BYTECODE _shaderByteCode = {};
};


//------------


class ShaderInfo
{
public:
    ShaderInfo();
    ~ShaderInfo();
    bool _zTest = true;
    bool _zWrite = true;
    bool _depthOnly = false;
    CompOper _zComp = CompOper::LEqual;
    bool _stencilTest = false;
    int _stencilIndex = 0;
    CompOper _stencilComp = CompOper::Always;
    D3D12_STENCIL_OP _stencilFailOp = D3D12_STENCIL_OP_KEEP;
    D3D12_STENCIL_OP _stencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    D3D12_STENCIL_OP _stencilPassOp = D3D12_STENCIL_OP_KEEP;

    CullingType cullingType = CullingType::BACK;
    FrontWise _wise = FrontWise::CW;

    D3D12_PRIMITIVE_TOPOLOGY_TYPE _primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //D3D12_PRIMITIVE_TOPOLOGY _topologyType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //D3D_PRIMITIVE_TOPOLOGY_POINTLIST
    //D3D_PRIMITIVE_TOPOLOGY_LINELIST
    //D3D_PRIMITIVE_TOPOLOGY_LINESTRIP
    //D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    //D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
    //D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST

    BlendType _blendType[8] = {
        BlendType::AlphaBlend,BlendType::AlphaBlend ,BlendType::AlphaBlend,BlendType::AlphaBlend,
        BlendType::AlphaBlend,BlendType::AlphaBlend ,BlendType::AlphaBlend,BlendType::AlphaBlend
    };

    int renderTargetCount = 1;
    DXGI_FORMAT RTVForamts[8]{
        SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT,
        SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT,SWAP_CHAIN_FORMAT
    };
    DXGI_FORMAT DSVFormat = DEPTH_STENCIL_FORMAT;

    int _renderQueue = 2000;
    RenderQueueType _renderQueueType = RenderQueueType::Opaque;

public:
    void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& renderTargets);
    void SetDSTexture(const std::shared_ptr<Texture>& DSTexture);
};

class Shader
{
public:
    std::unordered_map<std::string, std::shared_ptr<ShaderCode>> _shaderCodeTable;
    ShaderInfo _info;
    ShaderProfileInfo _profileInfo;

public:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC _pipelineDesc = {};
    ComPtr<ID3D12PipelineState> _pipelineState;

    void Init(const std::vector<VertexProp>& prop);
    void SetInfo(const ShaderInfo& info);
    void Profile();

    int GetRegisterIndex(const std::string& name);

    static std::shared_ptr<Shader> Load(const std::wstring& path, const std::vector<std::pair<std::string, std::string>>& shaderParams);
protected:
    static std::shared_ptr<ShaderCode> LoadBlob(std::wstring path, std::string endPointName, std::string shaderType);
};