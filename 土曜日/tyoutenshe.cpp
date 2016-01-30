struct Vertex{
    SimpleMath::Vector3 position;
    SimpleMath::Vector3 offset;
    SimpleMath::Vector3 normal;
    SimpleMath::Vector2 textureCoordinate;
    SimpleMath::Vector3 initial;
    unsigned indices;
    
    Vertex(SimpleMath::Vector3 const& p,SimpleMath::Vector3 const& 0,SimpleMath::Vector3 const&n,SimpleMath::Vector2 const& t,SimpleMath::Vector3 const& i,unsigned idx):position(p),offset(o),normal(n),textureCoordinate(t),initial(i),indices(idx)
    {
    }
    
    static const int InutElementCount=6;
    static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

const D3D11_INPUT_ELEMENT_DESC Vertex::InputElements[]=
{
    {"SV_Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"OFFSET",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"TEXCOORD",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"INITTAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"BLENDINDICES",0,DXGI_FORMAT_R8G8B8A8_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
};