matrix matLightViewProj;

struct VSOUTPUT_SHADOW
{
   float4 vPosition	: POSITION;
   float  fDepth	   : TEXCOORD0;
};

VSOUTPUT_SHADOW VS_Shadow( float4 inPosition : POSITION )
{
   VSOUTPUT_SHADOW OUT = (VSOUTPUT_SHADOW)0;
 
   OUT.vPosition = mul( inPosition, matLightViewProj );
   OUT.fDepth = OUT.vPosition.z;
   return OUT;
}

float4  PS_Shadow( VSOUTPUT_SHADOW IN ) : COLOR0
{
   return float4( IN.fDepth, IN.fDepth, IN.fDepth, 1.0f );
}

technique Shadow
{
    pass P0
    {	
        VertexShader = compile vs_3_0 VS_Shadow();
        PixelShader  = compile ps_3_0 PS_Shadow();        
    }
}