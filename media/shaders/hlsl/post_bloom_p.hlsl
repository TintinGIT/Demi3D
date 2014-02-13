
#include <common.hlsl>

uniform const sampler2D image;

static const int KERNEL_SIZE = 13;

static const float BLUR_WEIGHTS[KERNEL_SIZE] = 
{
	0.003450f,
	0.012483f,
	0.035351f,
	0.078353f,
	0.135933f,
	0.184604f,
	0.099654f,
	0.184604f,
	0.135933f,
	0.078353f,
	0.035351f,
	0.012483f,
	0.003450f
};

struct VS_OUTPUT
{
	float4 Position	 : POSITION;
	float2 Texcoord0 : TEXCOORD0;
	float4 Texcoord1 : TEXCOORD1;
	float4 Texcoord2 : TEXCOORD2;
	float4 Texcoord3 : TEXCOORD3;
	float4 Texcoord4 : TEXCOORD4;
	float4 Texcoord5 : TEXCOORD5;
	float4 Texcoord6 : TEXCOORD6;
};

PS_OUTPUT ps_main( VS_OUTPUT In )
{
	PS_OUTPUT Out;
	Out.Color = 0.0f;
	Out.Color += tex2D(image, In.Texcoord0) * BLUR_WEIGHTS[0];
 	Out.Color += tex2D(image, In.Texcoord1.xy) * BLUR_WEIGHTS[1];
 	Out.Color += tex2D(image, In.Texcoord1.zw) * BLUR_WEIGHTS[2];
 	Out.Color += tex2D(image, In.Texcoord2.xy) * BLUR_WEIGHTS[3];
 	Out.Color += tex2D(image, In.Texcoord2.zw) * BLUR_WEIGHTS[4];
 	Out.Color += tex2D(image, In.Texcoord3.xy) * BLUR_WEIGHTS[5];
 	Out.Color += tex2D(image, In.Texcoord3.zw) * BLUR_WEIGHTS[6];
 	Out.Color += tex2D(image, In.Texcoord4.xy) * BLUR_WEIGHTS[7];
 	Out.Color += tex2D(image, In.Texcoord4.zw) * BLUR_WEIGHTS[8];
 	Out.Color += tex2D(image, In.Texcoord5.xy) * BLUR_WEIGHTS[9];
 	Out.Color += tex2D(image, In.Texcoord5.zw) * BLUR_WEIGHTS[10];
 	Out.Color += tex2D(image, In.Texcoord6.xy) * BLUR_WEIGHTS[11];
 	Out.Color += tex2D(image, In.Texcoord6.zw) * BLUR_WEIGHTS[12];

	return Out;
}