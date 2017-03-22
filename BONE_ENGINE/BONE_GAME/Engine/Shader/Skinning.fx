//////////////////////////////////////////////////////////////////////////
//					Character Animation with Direct3D					//
//						   Author: C. Granberg							//
//							   2008 - 2009								//
//////////////////////////////////////////////////////////////////////////

//World and View*Proj Matrices
matrix matWorld;
matrix matVP;
//Light Position
float3 lightPos;
//Texture
texture texDiffuse;

//Skinning variables
extern float4x4 BoneMatrices[40]; 
extern int MaxNumAffectingBones = 2;

//Sampler
sampler DiffuseSampler = sampler_state
{
   Texture = (texDiffuse);
   MinFilter = Linear;   MagFilter = Linear;   MipFilter = Linear;
   AddressU  = Wrap;     AddressV  = Wrap;     AddressW  = Wrap;
   MaxAnisotropy = 16;
};

//Vertex Output / Pixel Shader Input
struct VS_OUTPUT
{
     float4 position : POSITION0;
     float2 tex0     : TEXCOORD0;
     float  shade	 : TEXCOORD1;
};

//Vertex Input
struct VS_INPUT_SKIN
{
     float4 position : POSITION0;
     float3 normal   : NORMAL;
     float2 tex0     : TEXCOORD0;
	 float4 weights  : BLENDWEIGHT0;
     int4   boneIndices : BLENDINDICES0;
};

VS_OUTPUT vs_Skinning(VS_INPUT_SKIN IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

    float4 v = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 norm = float3(0.0f, 0.0f, 0.0f);
    float lastWeight = 0.0f;
    
    IN.normal = normalize(IN.normal);
    
    for(int i = 0; i < MaxNumAffectingBones-1; i++)
    {
	    v += IN.weights[i] * mul(IN.position, BoneMatrices[IN.boneIndices[i]]);
	    norm += IN.weights[i] * mul(IN.normal, BoneMatrices[IN.boneIndices[i]]);
	    
	    //Sum up the weights
        lastWeight += IN.weights[i];
    }
    //Make sure weights add up to 1
    lastWeight = 1.0f - lastWeight;
    
    //Apply last bone
    v += lastWeight * mul(IN.position, BoneMatrices[IN.boneIndices[MaxNumAffectingBones-1]]);
    norm += lastWeight * mul(IN.normal, BoneMatrices[IN.boneIndices[MaxNumAffectingBones-1]]);
    
    //Get the world position of the vertex
    v.w = 1.0f;
	float4 posWorld = mul(v, matWorld);
    OUT.position = mul(posWorld, matVP);
    //Output texture coordinate is same as input
    OUT.tex0 = IN.tex0;
    
	//Calculate Lighting
    norm = normalize(norm);
    norm = mul(norm, matWorld);
	OUT.shade = max(dot(norm, normalize(lightPos - posWorld)), 0.5f);
     
    return OUT;
}

//Pixel Shader
float4 ps_Lighting(VS_OUTPUT IN) : COLOR0
{
	float4 color = tex2D(DiffuseSampler, IN.tex0);
	return color * IN.shade;
}

technique Skinning
{
    pass P0
    {	
        VertexShader = compile vs_2_0 vs_Skinning();
        PixelShader  = compile ps_2_0 ps_Lighting();        
    }
}

//Vertex Input
struct VS_INPUT
{
     float4 position : POSITION0;
     float3 normal   : NORMAL;
     float2 tex0     : TEXCOORD0;
};

VS_OUTPUT vs_Lighting(VS_INPUT IN)
{
    VS_OUTPUT OUT = (VS_OUTPUT)0;

	float4 posWorld = mul(IN.position, matWorld);
    float4 normal = normalize(mul(IN.normal, matWorld));
    
    OUT.position = mul(posWorld, matVP);
    
	//Calculate Lighting
	OUT.shade = max(dot(normal, normalize(lightPos - posWorld)), 0.5f);
	
	 //Output texture coordinate is same as input
    OUT.tex0=IN.tex0;
     
    return OUT;
}

technique Lighting
{
    pass P0
    {	
        VertexShader = compile vs_2_0 vs_Lighting();
        PixelShader  = compile ps_2_0 ps_Lighting();        
    }
}