//
//  phong.cpp
//  
//
//  Created by Chocomel on 2015/11/28.
//
//

cbuffer VSMatrix:register(b0){
    float4x4 World : packoffset(c0);
    float4x4 WorldView : packoffset(c4);
    float4x4 WorldViewProj : packoffset(c8);
};

cbuffer PSMaterial: register(b1){
    float4 Diffuse: packoffset(c0);
    float3 Specular: packoffset(c1);
    float Power: packoffset(c1.w);
    float3 Emissive: packoffset(c2);
};

cbuffer PSVector: register(b2){
    float3 LightDir:    packoffset(c0);
    float3 ViewDir:    packoffset(c1);
};

cbuffer PSLightColor: register(b3){
    float4 LightAmbient:    packoffset(c0);
    float4 LightDiffuse:    packoffset(c1);
    float4 LightSpecular:    packoffset(c2);
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct VSInput{
    float4 Position:    SV_POSITION;
    float3 Normal:  NORMAL;
    float2 Texture: TEXCOORD0;
};

VSOutput VS(VSInput input)
{
    VSOutput output=(VSOutput)0;
    
    output.Position=mul(float4(input.Position,1.0f),WorldViewProj);
    output.Normal=normalize(mul(input.Normal,(float3x3)World));
    output.Texture=input.Texture;
    
    return output;
}

float4 PS(VSOutput input):SV_TARGET
{
    float3 L=normalize(LightDir);
    float3 N=normalize(input.Normal);
    
    float lambertian=saturate(dot(L,N));
    
    float3 V=normalize(-ViewDir);
    
    float3 half_dir=normalize(L+V);
    float spec_angle=saturate(dot(half_dir,N));
    float specular=pow(spec_angle,Power);
    
    float4 Kt=txDiffuse.Sample(samLinear,input.Texture);
    float4 Kd=LightDiffuse*Diffuse*Kt;
    float4 Ks=LightSpecular*float4(Specular,1.0f);
    
    float4 color_linear=LightAmbient*Kd+lambertian*Kd+specular*Ks;
}