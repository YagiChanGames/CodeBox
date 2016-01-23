//
//  farshedar.cpp
//  
//
//  Created by Chocomel on 2015/12/12.
//
//

#include "farshedar.hpp"

pd3dImmediateContext->PSSetSamplers(0,1,g_pSamplerLinear,GetAddressOf());
pd3dImmediateContext->PSSetShaderResources(0,1,g_pTextureRV.GetAddressOf());
pd3dImmediateContext->PSSetShaderResources(1,1,g_pNormalMapRV.GetAddressOf());
pd3dImmediateContext->PSSetShaderResources(2,1,g_FurRV.GetAddressOf());
pd3dImmediateContext->DrawIndexed(g_nIndexCount,0,0);

pd3dImmediateContext->OMSetDepthStencilState(g_StateObjects->DepthRead(),0);
FLOAT BlendFactor[]={0,0,0,0};
pd3dImmediateContext->OMSetBlendState(g_StateObjects->NonPremultiplied(),BlendFactor,0xffffffff);

static const int N = 42;
static const float MAX=(float)(N-1);
for(int i=1;i<N;i++){
    float offset=(float)(1*length)/MAX*0.002f;
    float alpha=(MAX-(float)i)/MAX;
    g_pPSFur.Set(pd3dImmediateContext,offset,alpha);
    g_pPSFur.VSSet(pd3dImmediateContext);
    g_pPSFur.PSSet(pd3dImmediateContext);
    
    pd3dImmediateContext->DrawIndexed(g_nIndexCount,0,0);
}

pd3dImmediateContext->OMSetBlendState(g_StateObjects->Opaque(),BlendFactor,0xffffffff);
pd3dImmediateContext->OMSetDepthStencilState(g_StateObjects->DepthDefault(),0);


cbuffer VSMatrix: register(b0){
    float4x4 World: packoffst(c0);
    float4x4 WorldView: packoffst(c4);
    float4x4 WorldViewProj  packoffst(c8);
};

cbuffer PSMaterial: register(b1){
    float4 Diffuse: packoffst(c0);
    float3 Specular:packoffst(c1);
    float Power:packoffst(c1.w);
    float3 Emissive:packoffst(c2);
}

cbuffer PSVector:register(b2){
    float3 LightDir:packoffst(c0);
    float3 ViewDir:packoffst(c1);
}

cbuffer PSLightColor:register(b3){
    float4 LightAmbient:packoffst(c0);
    float4 LightDiffuse:packoffst(c1);
    float4 LightSpecular:packoffst(c2);
};

cbuffer PSFur:register(b4){
    float4 FurParam:packoffst(c0);
};

Texture2D txDiffuse:register(t0);
Texture2D txLightMap:register(t1);
Texture2D txFur:register(t2);
SampleState samLinear:register(s0);

struct VSInput{
    float3 Position:SV_Position;
    float3 Normal:NORMAL;
    float3 Tangent:TANGENT;
    float2 Texture:TEXCOORD0;
};;

struct VSOutput{
    float4 Position:SV_POSITION;
    float2 Texture:TEXCOORD0;
    float3 LightDir:TEXCOORD1;
    tloat3 ViewDir:TEXCOORD2;
};

//////
//頂点シェーダ
//////
VSOutput VS(VSInput input)
{
    VSOutput output=(VSOutput)0;
    
    float3 position=input.Position+input.Normal*FurParam.x;
    output.Position=mul(float4(position,1.0f),WorldViewProj);
    
    float3 normal=mul(input.Normal,(float3x3)World);
    float3 tangent=mul(input.Tangent,(float3x3)World);
    float3 binormal=normalize(cross(normal,tangent));
    
    float3 view_dir=-ViewDir;
    output.ViewDir.x=dot(view_dir,tangent);
    output.ViewDir.y=dot(view_dir,binormal);
    output.ViewDir.z=dot(view_dir,normal);
    output.ViewDir=normalize(output.ViewDir);
    
    output.LightDir.x=dot(LightDir,tangent);
    output.LightDir.y=dot(LightDir,binormal);
    output.LightDir.z=dot(LightDir,normal);
    output.LightDir=normalize(output,LightDir);
    
    output.Texture=input.Texture;
    
    return output;
}

//////
//ピクシェルシェーダー
//////
float4 PS(VSOutput input):SV_TARGET
{
    float4 map=txFur.Sample(samLinear,input.Texture*2);
    
    if(map.a<=0.0||map.b<FurParam.x)discard;
    float3 normal=txLightMap.Sample(samLinear,input.Texture).xyz;
    normal=normalize((normal*2.0f)-1.0f);
    float lambertian=saturate(dot(input.LightDir,normal));
    
    float3 half_dir=normalize(input.LightDir+input.ViewDir);
    float spec_angle=saturate(dot(half_dir,normal));
    float specular=pow(spec_angle,Power);
    
    float4 Kt=txDiffuse.Sample(samLinear,input.Texture);
    float4 Kd=LightDiffuse*Diffuse*Kt;
    float4 Ks=LightSpecular*float4(Specular,1.0f);
    
    float4 color_linear=LightAmbient*Kd+lambertian*Kd+specular*Ks;
    
    color_linear.a=FurParam.a;
    
    return saturate(color_linear);
}