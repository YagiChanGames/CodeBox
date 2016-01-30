//
//  shadow.cpp
//  
//
//  Created by Chocomel on 2016/01/23.
//
//

//Phong.fx


cbuffer VSMatrix: register(b0){
    float4x4 World: packoffset(c0);
    float4x4 WorldView: packoffset(c4);
    float4x4 WorldViewProj: packoffset(c8);
};

cbuffer PSMaterial: register(b1){
    float4 Diffuse: packoffset(c0);
    float3 Specular:    packoffset(c1);
    float Power:    packoffset(c1.w);
    float3 Emissive:    packoffset(c2);
};

cbuffer PSVector:   register(b2){
    float3 LightDir: packoffset(c0);
    float3 ViewDir: packoffset(c1);
};

cbuffer PSLightColor:   register(b3){
    float4 LightAmbient:    packoffset(c0);
    float4 LightDiffuse:    packoffset(c1);
    float4 LightSpecular:   packoffset(c2);
};

cbuffer VSShadow:   register(b4){
    float4x4 LightViewProj: packoffset(c0);
};

Texture2D txDiffuse:    register(t0);
Texture2D txShadow: register(t1);
SamplerState samLinear: register(s0);

struct VSInput{
    float3 Position:    SV_Position;
    float3 Normal:  NORMAL;
    float2 Texture: TEXCOORD0;
};

struct VSOutput{
    float4 Position:    SV_POSITION;
    float3 Normal:  NORMAL;
    float2 Texture: TEXCOORD0;
    float4 Shadow:  TEXCOORD1;
};


//頂点シェーダ
VSOutput VS(VSInout input)
{
    VSOutput output=(VSOutput)0;
    
    output.Position=mul(float4(input.Position,1.0f),WorldViewProj);
    output.Normal=normalize(mul(input.Normal,(float3x3)World));
    output.Texture=inout.Texture;
    output.Shadow=mul(float4(input.Position,1.0f),LightViewProj);
    return output;
}


//ピクセルシェーダ
float4 PS(VSOutput input):SV_TARGET
{
    float3 L=normalize(LightDir);
    float3 N=normalize(input.Normal);
    
    float lambertian=saturate(dot(L,N));
    
    float3 V=normalize(-ViewDir);
    
    float3 half_dir=normalize(L+V);
    float spec_angle=saturate(dot(half_dir,N));
    float specular=pow(spec_angle,Power);
    
    float4 Kt=txDiffuse.Sample(input.Texture);
    float4 Kd=LightDiffuse*Diffuse*Kt;
    float4 Ks=LightSpecular*float4(Specular,1.0f);
    
    float4 color_linear=LightAmbient*Kd;
    
    float2 texel=float2(
    input.Shadow.x/input.Shadow.w*0.5f+0.5f,
                        input.Shadow.y/input.Shadow.w*-0.5f+0.5f);
    if(texel.x<0||texel.x>1||texel.y<0||texel.y>1){
        color_linear+=lambertian*Kd+specular*Ks;
    }
    else{
        float shadow=txShadow.Sample(samLinear,texel).r;
        if(shadow>=input.Shadow.z/input.Shadow.w-0.0001f){
            color_linear+=lambertian*Kd+specular*Ks;
        }
    }
    return saturate(color_linear);
}



//////page5

HRESULT CreateShadowTexture(ID3D11Device* pd3dDevice)
{
    HRESULT hr=S_OK;
    
    DXGI_FORMAT texturefmt=DXGI_FORMAT_R32_TYPELESS;
    DXGI_FORMAT SRVfmt=DXGI_FORMAT_FORMAT_R32_FLOAT;
    DXGI_FORMAT DSVfmt=DXGI_FORMAT_FORMAT_R32_FLOAT;
    
    D3D11_TEXTURE2D_DESC dtd={
        texture_size,
        texture_size,
        1,
        1,
        texturefmt,
        1,
        0,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE,
        0,
        0
    };
    
    hr=pd3dDevice->CreateTexture2D(&dtd,NULL,g_pShadowMapTexture.GetAddressOf());
    if(FAILED(hr))return hr;
    
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvs={
        DSVfmt,
        D3D11_DSV_DIMENSION_TEXTURE2D,
        0
    };
    
    hr=pd3dDevice->CreateDepthStencilView(g_pShadowMapTexture.Get(),&dsvd,
                                          g_pShadowMapDSV.GetAddressOf());
    if(FAILED(hr))return hr;
    
    D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd={
        SRVfmt,
        D3D11_SRV_DIMENSION_TEXTURE2D,
        0,
        0
    };
    dsrvd.Texture2D.MipLevels=1;
    
    hr=pd3dDevice->CreateShaderResourceView(g_pShadowMapTexture.Get(),&dsrvd,
                                            g_pShadowMapDSV.GetAddressOf());
    if(FAILED(hr))return hr;
    
    return hr;
}


/////////////

static void RenderScene(ID3D11DeviceContext* pd3dImmediateContext,bool shadow)
{
    XMFLOAT4 ViewDir;
    XMStoreFloat4(&ViewDir,XMVectorSubtract(g_Camera.GetLookAtPt(),g_Camera.GetEyePt()));
    
    pd3dImmediateContext->OMSetBlendState(g_StateObjects->Opaque(),nullptr,0xFFFFFFFF);
    pd3dImmediateContext->OMSetDepthStencilState(g_StateObjects->DepthDefault(),0);
    
    ID3D11SamplerState* samplerState=g_StateObjects->LinearClamp();
    pd3dImmediateContext->PSSetSamplers(0,1,&samplerState);
    
    XMMATRIX mView=g_Camera.GetViewMatrix();
    XMMATRIX mProj=g_Camera.GetProjMatrix();
    
    if(shadow){
        pd3dImmediateContext->IASetInputLayout(g_pShadowVertexLayout.Get());
        pd3dImmediateContext->VSSetShader(g_pShadowVertexShader.Get(),nullptr,0);
        pd3dImmediateContext->PSSetShader(nullptr,nullptr,0);
    }
    else{
        g_PSVector.Set(pd3dImmediateContext,s_LightDir,ViewDir);
        g_PSLightColor.Set(pd3dImmediateContext,XMFLOAT4(0.075f,0.075f,0.075f,1.f),
                           XMFLOAT4(0.85f,0.85f,0.85f,1.f),XMFLOAT4(0.8f,0.8f,0.8f,1.f));
        g_PSMaterial.Set(pd3dImmediateContext,XMFLOAT4(1.0f,1.0f,1.0f,1.0f),
                         XMFLOAT3(1.0f,1.0f,1.0f),500.0f,XMFLOAT3(0.0f,0.0f,0.0f));
        
        g_PSVector.VSSet(pd3dImmediateContext);
        g_PSVector.PSSet(pd3dImmediateContext);
        g_PSLightColor.PSSet(pd3dImmediateContext);
        g_PSMaterial.PSSet(pd3dImmediateContext);
        
        pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());
        pd3dImmediateContext->VSSetShader(g_pVertexShader.Get(),nullptr,0);
        pd3dImmediateContext->PSSetShader(g_pPixelShader.Get(),nullptr,0);
        
        pd3dImmediateContext->PSSetShaderResources(1,1,g_pShadowMapSRV.GetAddressOf());
    }
    
    pd3dImmediateContext->PSSetSamplers(0,1,g_pSamplerLinear.GetAddressOf());
    

    XMMATRIX world_view=world*view;
    XMMATRIX world_view_projection=world_view*projection;
    
    g_VSShadow.Set(pd3dImmediateContext,world*g_ShadowView*g_ShadowProjection);
    g_VSShadow.VSSet(pd3dImmediateContext);
    
    g_PSMaterial.Set(pd3dImmediateContext,color,
                     XFLOAT3(0.0f,0.0f,0.0f),500.0f,XMFLOAT3(0.0f,0.0f,0.0f));
    g_PSMaterial.PSSet(pd3dImmediateContext);
}

//page7の途中まで書いた