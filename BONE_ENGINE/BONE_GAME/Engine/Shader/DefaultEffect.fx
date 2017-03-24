/*-----------------------------------------------------------------------------
	Name	: Soft Shadows.fx
	Desc	: Soft shadows effect file.
	Author	: Anirudh S Shastry. Copyright (c) 2004.
	Date	: 22nd June, 2004.
-----------------------------------------------------------------------------*/

//--------------------------------------------
// Global variables
//--------------------------------------------
matrix	g_matWorldViewProj	: WorldViewProjection;
matrix	g_matLightViewProj	: LightViewProjection;
matrix	g_matWorld			: World;
matrix	g_matWorldIT		: WorldInverseTranspose;
matrix	g_matTexture		: Texture;
matrix  g_matTexture2		: Texture;
matrix  g_matTexture3		: Texture;
matrix  g_matTexture4		: Texture;
matrix  g_matTexture5		: Texture;
matrix  g_matTexture6		: Texture;
matrix  g_matTexture7		: Texture;
matrix  g_matTexture8		: Texture;

vector	g_vLightPos			: LightPosition;
vector	g_vLightColor		: LightColor;

vector	g_vEyePos			: EyePosition;

float2  g_vSampleOffsets[15];
float   g_fSampleWeights[15];
bool	g_bOnLight[8];

texture tShadowMap1;
texture tShadowMap2;
texture tShadowMap3;
texture tShadowMap4;
texture tShadowMap5;
texture tShadowMap6;
texture tShadowMap7;
texture tShadowMap8;
texture tScreenMap;
texture tBlurHMap;
texture tBlurVMap;
texture	tColorMap;

sampler ShadowSampler1 = sampler_state
{
	texture = (tShadowMap1);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler2 = sampler_state
{
	texture = (tShadowMap2);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler3 = sampler_state
{
	texture = (tShadowMap3);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler4 = sampler_state
{
	texture = (tShadowMap4);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler5 = sampler_state
{
	texture = (tShadowMap5);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler6 = sampler_state
{
	texture = (tShadowMap6);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler7 = sampler_state
{
	texture = (tShadowMap7);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ShadowSampler8 = sampler_state
{
	texture = (tShadowMap8);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ScreenSampler = sampler_state
{
	texture = (tScreenMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler BlurHSampler = sampler_state
{
	texture = (tBlurHMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler BlurVSampler = sampler_state
{
	texture = (tBlurVMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Clamp;
	AddressV = Clamp;
};

sampler ColorSampler = sampler_state
{
	texture = (tColorMap);

	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
	
	AddressU = Wrap;
	AddressV = Wrap;
};

////////////////////////////////////////////////////////////////////////////////////
//// Shadow Tech
////////////////////////////////////////////////////////////////////////////////////

struct VSOUTPUT_SHADOW
{
	float4 vPosition	: POSITION;
	float  fDepth		: TEXCOORD0;
};

VSOUTPUT_SHADOW VS_Shadow( float4 inPosition : POSITION )
{
	// Output struct
	VSOUTPUT_SHADOW OUT = (VSOUTPUT_SHADOW)0;

	// Output the transformed position
	inPosition = mul (inPosition, g_matWorld);
	OUT.vPosition = mul( inPosition, g_matLightViewProj );

	// Output the scene depth
	OUT.fDepth = OUT.vPosition.z;

	return OUT;
}

float4  PS_Shadow( VSOUTPUT_SHADOW IN ) : COLOR0
{
	return float4( IN.fDepth, IN.fDepth, IN.fDepth, 1.0f );
}
	

////////////////////////////////////////////////////////////////////////////////////
//// Unlit Tech
////////////////////////////////////////////////////////////////////////////////////

struct VSOUTPUT_UNLIT
{
	float4 vPosition	: POSITION;
	float4 vTexCoord	: TEXCOORD0;
	float4 vTexCoord2	: TEXCOORD1;
	float4 vTexCoord3	: TEXCOORD2;
	float4 vTexCoord4	: TEXCOORD3;
	float4 vTexCoord5	: TEXCOORD4;
	float4 vTexCoord6	: TEXCOORD5;
	float4 vTexCoord7	: TEXCOORD6;
    float4 vTexCoord8	: TEXCOORD7;
};

VSOUTPUT_UNLIT VS_Unlit( float4 inPosition : POSITION )
{
	// Output struct
	VSOUTPUT_UNLIT OUT = (VSOUTPUT_UNLIT)0;

	// Output the transformed position
    inPosition = mul(inPosition, g_matWorld);
	OUT.vPosition = mul( inPosition, g_matWorldViewProj );
		
	// Output the projective texture coordinates
	OUT.vTexCoord = mul( inPosition, g_matTexture );
	OUT.vTexCoord2 = mul( inPosition, g_matTexture2);
	OUT.vTexCoord3 = mul( inPosition, g_matTexture3);
	OUT.vTexCoord4 = mul( inPosition, g_matTexture4);
	OUT.vTexCoord5 = mul( inPosition, g_matTexture5);
	OUT.vTexCoord6 = mul( inPosition, g_matTexture6);
	OUT.vTexCoord7 = mul( inPosition, g_matTexture7);
	return OUT;
}

// Shadow mapping pixel shader
float4  PS_Unlit( VSOUTPUT_UNLIT IN ) : COLOR0
{
	// Generate the 9 texture co-ordinates for a 3x3 PCF kernel
	float4 vTexCoords[9];

	// Texel size
	float fTexelSize = 1.0f / 512.0f;

	// Generate the tecture co-ordinates for the specified depth-map size
	// 4 3 5
	// 1 0 2
	// 7 6 8

	vTexCoords[0] = 0;

	vTexCoords[1] = float4( -fTexelSize, 0.0f, 0.0f, 0.0f );
	vTexCoords[2] = float4(  fTexelSize, 0.0f, 0.0f, 0.0f );
	vTexCoords[3] = float4( 0.0f, -fTexelSize, 0.0f, 0.0f );
	vTexCoords[6] = float4( 0.0f,  fTexelSize, 0.0f, 0.0f );

	vTexCoords[4] = float4( -fTexelSize, -fTexelSize, 0.0f, 0.0f );
	vTexCoords[5] = float4(  fTexelSize, -fTexelSize, 0.0f, 0.0f );
	vTexCoords[7] = float4( -fTexelSize,  fTexelSize, 0.0f, 0.0f );
	vTexCoords[8] = float4(  fTexelSize,  fTexelSize, 0.0f, 0.0f );
	

	// Sample each of them checking whether the pixel under test is shadowed or not
	float fShadowTerm = 0.0f;
    float4 temp;
	
    for( int i = 0; i < 9; i++ )
	{
		float A1 = 1, A2 = 0;
		float B1 = 1, B2= 0;
		float C1 = 1, C2= 0;
		float D1 = 1, D2= 0;
		float E1 = 1, E2= 0;
		float F1 = 1, F2= 0;
		float G1 = 1, G2= 0;
		float H1 = 1, H2= 0;

		if (g_bOnLight[0])
		{
            temp = IN.vTexCoord + vTexCoords[i];

			A1 = tex2Dproj( ShadowSampler1, temp ).r;
			A2 = (IN.vTexCoord.z - 0.001f);
		}

		if (g_bOnLight[1])
		{
            temp = IN.vTexCoord2 + vTexCoords[i];
	
			B1 = tex2Dproj( ShadowSampler2, temp ).r;
			B2 = (IN.vTexCoord2.z - 0.001f);
		}

		if (g_bOnLight[2])
		{
            temp = IN.vTexCoord3 + vTexCoords[i];
	
			C1 = tex2Dproj( ShadowSampler3, temp ).r;
			C2 = (IN.vTexCoord3.z - 0.001f);
		}

		if (g_bOnLight[3])
		{
            temp = IN.vTexCoord4 + vTexCoords[i];
	

			D1 = tex2Dproj( ShadowSampler4, temp ).r;
			D2 = (IN.vTexCoord4.z - 0.001f);
		}

		if (g_bOnLight[4])
		{
            temp = IN.vTexCoord5 + vTexCoords[i];
	
			E1 = tex2Dproj( ShadowSampler5, temp ).r;
			E2 = (IN.vTexCoord5.z - 0.001f);
		}

		if (g_bOnLight[5])
		{
            temp = IN.vTexCoord6 + vTexCoords[i];
	
			F1 = tex2Dproj( ShadowSampler6, temp ).r;
			F2 = (IN.vTexCoord6.z - 0.001f);
		}

		if (g_bOnLight[6])
		{
			temp = IN.vTexCoord7 + vTexCoords[i];
	

			G1 = tex2Dproj( ShadowSampler7, temp ).r;
			G2 = (IN.vTexCoord7.z - 0.001f);
		}

		if (g_bOnLight[7])
		{
			temp = IN.vTexCoord8 + vTexCoords[i];
	

			H1 = tex2Dproj( ShadowSampler8, temp ).r;
			H2 = (IN.vTexCoord8.z - 0.001f);
		}

		// Texel is shadowed
		if ( A1 < A2 && g_bOnLight[0] == true)
			fShadowTerm += 0.1f;
		else if ( B1 < B2 && g_bOnLight[1] == true)
			fShadowTerm += 0.1f;
		else if ( C1 < C2 && g_bOnLight[2] == true)
			fShadowTerm += 0.1f;
		else if ( D1 < D2 && g_bOnLight[3] == true)
			fShadowTerm += 0.1f;
		else if ( E1 < E2 && g_bOnLight[4] == true)
			fShadowTerm += 0.1f;
		else if ( F1 < F2 && g_bOnLight[5] == true)
			fShadowTerm += 0.1f;
		else if ( G1 < G2 && g_bOnLight[6] == true)
			fShadowTerm += 0.1f;
		else if ( H1 < H2 && g_bOnLight[7] == true)
			fShadowTerm += 0.1f;
		else
			fShadowTerm += 1.0f;
	}
	
	// Get the average
	fShadowTerm /= 9.0f;
	
	return fShadowTerm;
}



////////////////////////////////////////////////////////////////////////////////////
//// BLUR Tech
////////////////////////////////////////////////////////////////////////////////////

struct VSOUTPUT_BLUR
{
	float4 vPosition	: POSITION;
	float2 vTexCoord	: TEXCOORD0;
};

// Gaussian filter vertex shader
VSOUTPUT_BLUR VS_Blur( float4 inPosition : POSITION, float2 inTexCoord : TEXCOORD0 )
{
	// Output struct
	VSOUTPUT_BLUR OUT = (VSOUTPUT_BLUR)0;

	// Output the position
	OUT.vPosition = inPosition;

	// Output the texture coordinates
	OUT.vTexCoord = inTexCoord;

	return OUT;
}

// Horizontal blur pixel shader
float4 PS_BlurH( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		vAccum += tex2D( ScreenSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}

// Vertical blur pixel shader
float4 PS_BlurV( VSOUTPUT_BLUR IN ) : COLOR0
{
	// Accumulated color
	float4 vAccum = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
	for( int i = 0; i < 15; i++ )
	{
		vAccum += tex2D( BlurHSampler, IN.vTexCoord + g_vSampleOffsets[i] ) * g_fSampleWeights[i];
	}

	return vAccum;
}
		
struct VSOUTPUT_SCENE
{
	float4 vPosition		: POSITION;
	float2 vTexCoord		: TEXCOORD0;
	float4 vProjCoord		: TEXCOORD1;
	float4 vScreenCoord		: TEXCOORD2;
	float3 vNormal			: TEXCOORD3;
	float3 vLightVec		: TEXCOORD4;
	float3 vEyeVec			: TEXCOORD5;
};

// Scene pixel shader
VSOUTPUT_SCENE VS_Scene( float4 inPosition : POSITION, float3 inNormal : NORMAL, float2 inTexCoord : TEXCOORD0 )
{
	VSOUTPUT_SCENE OUT = (VSOUTPUT_SCENE)0;

	// Output the transformed position
	inPosition = mul (inPosition, g_matWorld);
	OUT.vPosition = mul( inPosition, g_matWorldViewProj );

	// Output the texture coordinates
	OUT.vTexCoord = inTexCoord;

	// Output the projective texture coordinates
	OUT.vProjCoord = mul( inPosition, g_matTexture );

	// Output the screen-space texture coordinates
	OUT.vScreenCoord.x = ( OUT.vPosition.x * 0.5 + OUT.vPosition.w * 0.5 );
	OUT.vScreenCoord.y = ( OUT.vPosition.w * 0.5 - OUT.vPosition.y * 0.5 );
	OUT.vScreenCoord.z = OUT.vPosition.w;
	OUT.vScreenCoord.w = OUT.vPosition.w;

	// Get the world space vertex position
	float4 vWorldPos = mul( inPosition, g_matWorld );

	// Output the world space normal
	OUT.vNormal = mul( inNormal, g_matWorldIT );

	// Move the light vector into tangent space
	OUT.vLightVec = g_vLightPos.xyz - vWorldPos.xyz;

	// Move the eye vector into tangent space
	OUT.vEyeVec = g_vEyePos.xyz - vWorldPos.xyz;
	
	return OUT;
}

float4 PS_Scene( VSOUTPUT_SCENE IN ) : COLOR0
{
	// Normalize the normal, light and eye vectors
	IN.vNormal	 = normalize( IN.vNormal );
	IN.vLightVec = normalize( IN.vLightVec );
	IN.vEyeVec   = normalize( IN.vEyeVec );

	// Sample the color and normal maps
	float4 vColor  = tex2D( ColorSampler, IN.vTexCoord );

	// Compute the diffuse and specular lighting terms
	float diffuse  = max( dot( IN.vNormal, IN.vLightVec ), 0 );
	float specular = pow( max( dot( 2 * dot( IN.vNormal, IN.vLightVec ) * IN.vNormal - IN.vLightVec, IN.vEyeVec ), 0 ), 8 );

	if( diffuse == 0 ) specular = 0;

	// Grab the shadow term
	float fShadowTerm = tex2Dproj( BlurVSampler, IN.vScreenCoord );

	// Compute the final color
	return (diffuse * vColor * g_vLightColor * fShadowTerm) +
		   (specular * vColor * g_vLightColor.a * fShadowTerm);
}

//--------------------------------------------
// Techniques
//--------------------------------------------
technique techShadow
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Shadow();
		PixelShader  = compile ps_3_0 PS_Shadow();
	}
}

technique techUnlit
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Unlit();
		PixelShader  = compile ps_3_0 PS_Unlit();
	}
}

technique techBlurH
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_3_0 VS_Blur();
		PixelShader  = compile ps_3_0 PS_BlurH();
	}
}

technique techBlurV
{
	pass p0
	{
		Lighting	= False;
		CullMode	= None;
		
		VertexShader = compile vs_3_0 VS_Blur();
		PixelShader  = compile ps_3_0 PS_BlurV();
	}
}

technique techScene
{
	pass p0
	{
		Lighting	= False;
		CullMode	= CCW;
		
		VertexShader = compile vs_3_0 VS_Scene();
		PixelShader  = compile ps_3_0 PS_Scene();
	}
}