cbuffer cbVertex : register(b0)
{
    float2 ViewPortSize;
    float2 InvViewportSize;
};

struct VSInput
{
    float2 position : POSITION0;
    float2 uv : TEXCOORD0;
};

float2 ScreenToNDC(float2 screenPos, float2 invViewportSize)
{
    float2 uv = screenPos * invViewportSize;
    return (uv - 0.5f) * float2(2, -2);
}

void VSMain(in VSInput input, out float4 out_position : SV_Position, out float2 out_uv : TEXCOORD0)
{
     
    out_position = float4(ScreenToNDC(input.position, InvViewportSize), 0, 1);
    out_uv = input.uv;
}




sampler sampler0;
Texture2D texture0;
            
float4 PSMain(in float4 pos : SV_Position, in float2 in_uv : TEXCOORD0) : SV_Target
{
    //return float4(1,1,1,1);
    
    return texture0.Sample(sampler0, in_uv);
}