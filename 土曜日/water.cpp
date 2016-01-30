//
//  water.cpp
//  
//
//  Created by Chocomel on 2016/01/16.
//
//

const int N = 128;
const float c = 2.0f;
const float dt = 0.1f;
const float dd = 2.0f;

float z[3][N][N];

void init()
{
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            z[0][i][j]=0.0f;
        }
    }
    z[0][N/2][N/2] = 120.0f;
    
    for(int i=1;i<N-1;i++){
        for(int j=1;j<N-1;j++){
            z[1][i][j]=z[0][i][j]+c*c/2.0f*dt*dt/(dd*dd)*(z[0][i+1][j]+z[0][i-1][j]+z[0][i][j+1]-4.0f*z[0][i][j]);
        }
    }
    for(int i=0;i<N;i++){
        z[1][i][0]=z[1][i][N-1]=z[1][0][i]=z[1][N-1][i]=0.0f;
    }
}

void next()
{
    for(int i=1;i<N-1;i++){
        for(int j=1;j<N-1;j++){
            z[2][i][j]=2.0f*z[1][i][j]-z[0][i][j]+c*c*dt*dt/(dd*dd)
            *(z[1][i+1][j]+z[1][i-1][j]+z[1][i][j+1]+z[1][i][j-1]-4.0f*z[1][i][j]);
        }
    }
    
    for(int i=0;i<N;i++){
        z[2][i][0]=z[2][i][N-1]=z[2][0][i]=z[2][N-1][i]=0.0f
    }
    
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            z[0][i][j]=z[1][i][j];
            z[1][i][j]=z[2][i][j];
        }
    }
}


///////////

HRESULT CreateTexture(ID3D11Device* pd3dDevice)
{
    HRESULT hr=S_OK;
    
    DXGI_FORMAT texturefmt=DXGI_FORMAT_R8G8_TYPELESS;
    DXGI_FORMAT SRVfmt=DXGI_FORMAT_R8G8_SNORM;
    
    D3D11_TEXTURE2D_DEST dtd={
        N,N,1.1.texturefmt,1,0,D3D11_USAGE_DYNAMIC,D3D11_BIND_SHADER_RESOURCE,D3D11_CPU_ACCESS_WRITE,0};
    
    hr=pd3dDevice->CreateTexture2D(&dtd,NULL,g_pWaterTexture.GetAddressOf());
    if(FAILED(hr))return hr;
    
    auto pd3dImmediateContext=DXUTGetD3D11DeviceContext();
    
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if(SUCCEEDED(pd3dImmediateContext->Map(g_pWaterTexture.Get(),0,
                                           D3D11_MAP_WRITE_DISCARD,0,&MappedResource))){
        char *p=reinterpret_cast<char*>(MappedResource.pData);
        
        for(int y=0;y<N;y++){
            for(int x=0;x<N;x++){
                *p++=0;
                *p++=0;
            }
        }
        pd3dImmediateContext->Unmap(g_pWaterTexture.Get(),0);
    }
    return hr;
}


//////////

void CALLBACK OnFrameMove(double fTime,float fElapsedTime,void* pUserContext)
{
    g_Camera.FrameMove(fElapsedTime);
    
    lest_time-=fElapsedTime;
    if(left_time<0){
        left_time+=4.0f;
        int x=rand()%N;
        int y=rand()%N;
        z[0][y][x]+=100.0f;
        z[1][y][x]+=100.0f;
    }
    next();
    
    auto pd3dImmediateContext=DXUTGetD3D11DeviceContext();
    
    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if(SUCCEEDED(pd3dImmediateContext->Map(g_pWaterTexture.Get(),0,
                                           D3D11_MAP_WRITE_DISCARD,0,&MappedResource))){
        char *pDst=reinterpret_cast<char*>(MappedResource.pData);
        
        for(int y=1;y<N;y++){
            for(int x=1;x<N;x++){
                pDst[2*x+0]=(char)(z[2][y][x]-z[2][y][x-1]);
                pDst[2*x+1]=(char)(z[2][y][x]-z[2][y-1][x]);
            }
            pDst+=N*2;
        }
        pd3dImmediateContext->Unmap(g_pWaterTexture.Get(),0);
    }
}


///////////

float4 PS(VSOutput input):SV_TARGET
{
    float3 L = normalize(LightDir);
    float3 N = normalize(input.Normal);
    
    float lambertian=saturate(dot(L,N));
    
    float3 V = normalize(-ViewDir);
    
    float3 half_dir=normalize(L+V);
    float spec_angle=saturate(dot(half_dir,N));
    float specular = pow(spec_angle,Power);
    
    float2 texel=txBumpMap.Sample(samLinear,inpupt.Texture).xy;
    float2 tex=input.Texture+taxel*0.2f;
    
    float4 Kt=txDiffuse.Sample(samLinear,tex);
    float Kd=LightDiffuse*Diffuse*Kt;
    float4 Ks=LightSpecular*float4(Specular,1.0f);
    
    float4 color_linear=LightAmbient*Kd+lambertian*Kd+specular*Ks;
    
    return saturate(color_linear);
}