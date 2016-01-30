//プロジェクション用のシェーダー
cbuffer VSMatrix:register(b0){
    float4x4 World: packoffset(c0);
    float4x4 WorldView: packoffset(c4);
    float4x4 WorldViewProj: packoffset(c8);
    
};

cbuffer VSProjection:register(b4){
    float4x4 PrWorldViewProj:packoffset(c0);
};

Texture2D txProjection:register(t1);
SamplerState samLinear:register(s1);

struct VSInput{
    float3 Position:SV_Position;
    float3 Normal:NORMAL;
    float2 Texture:TEXCOORD0;
};

struct VSOutput{
    float4 Position:SV_POSITION;
    float4 Texture:TEXCOORD0;
};



//頂点シェーダ
VSOutput VS(VSInput input){
    VSOutput output=(VSOutput)0;
    
    output.Position=mul(float4(input.Position,1.0f),WorldViewProj);
    output.Texture=mul(float4(input.Position,1.0f),PrWorldViewProj);
    
    return output;
}

//ピクセルシェーダ
float4 PS(VSOutput input):SV_TARGET
{
    float2 tex=input.Texture.xy/input.Texture.w;
    float4 color=txProjection.Sample(samLinear,tex);
    return color;
}