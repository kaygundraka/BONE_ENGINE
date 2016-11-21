#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "SpriteBillBoard.h"
#include <shlwapi.h>
#include "Camera.h"
#include "GameObject.h"
#include "Vertex.h"
#pragma comment(lib, "shlwapi")
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	SpriteBillBoard::SpriteBillBoard()
	{
		CThreadSync sync;

		SetTypeName("SpriteBillBoard");

		vertexBuffer = NULL;
		indexBuffer = NULL;

		rectWidth = 1;
		rectHeight = 1;

		renderMode = RENDER_DEFAULT;

		rightPlay = true;
	}

	SpriteBillBoard::~SpriteBillBoard()
	{
		CThreadSync sync;

		if(vertexBuffer == NULL)
			vertexBuffer->Release();
		
		if(indexBuffer == NULL)
			indexBuffer->Release();

		ResourceMgr->ReleaseTexture(textureAddress);
	}

	void SpriteBillBoard::LoadContent()
	{
		CThreadSync sync;

		VERTEX vertex[4];

#pragma region Vertex Setup

		vertex[0].p.x = rectWidth/2;
		vertex[0].p.y = rectHeight/2;
		vertex[0].p.z = 0;

		vertex[0].n = vertex[0].p;
		D3DXVec3Normalize(&vertex[0].n, &vertex[0].n);

		vertex[0].uv.x = (float)curRect.left / (float)width;
		vertex[0].uv.y = (float)curRect.top / (float)height;

		vertex[1].p.x = -rectWidth / 2;
		vertex[1].p.y = rectHeight / 2;
		vertex[1].p.z = 0;

		vertex[1].n = vertex[1].p;
		D3DXVec3Normalize(&vertex[1].n, &vertex[1].n);

		vertex[1].uv.x = (float)curRect.right / (float)width;
		vertex[1].uv.y = (float)curRect.top / (float)height;

		vertex[2].p.x = -rectWidth / 2;
		vertex[2].p.y = -rectHeight / 2;
		vertex[2].p.z = 0;

		vertex[2].n = vertex[2].p;
		D3DXVec3Normalize(&vertex[2].n, &vertex[2].n);

		vertex[2].uv.x = (float)curRect.right / (float)width;
		vertex[2].uv.y = (float)curRect.bottom / (float)height;

		vertex[3].p.x = rectWidth / 2;
		vertex[3].p.y = -rectHeight / 2;
		vertex[3].p.z = 0;

		vertex[3].n = vertex[3].p;
		D3DXVec3Normalize(&vertex[3].n, &vertex[3].n);

		vertex[3].uv.x = (float)curRect.left / (float)width;
		vertex[3].uv.y = (float)curRect.bottom / (float)height;

#pragma endregion

#pragma region Vertex/Index Buffer Setup
		if (vertexBuffer != NULL)
		{
			vertexBuffer->Release();
			vertexBuffer = NULL;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_WRITEONLY,
			VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, NULL)))
		{
			return;
		}

		VOID* pVertices;

		if (FAILED(vertexBuffer->Lock(0, 4 * sizeof(VERTEX), (void**)&pVertices, 0)))
			return;

		VERTEX* pV = (VERTEX*)pVertices;

		for (int i = 0; i < 4; i++)
			*(pV++) = vertex[i];

		vertexBuffer->Unlock();

		if (indexBuffer != NULL)
		{
			indexBuffer->Release();
			indexBuffer = NULL;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer( 2 * sizeof(VERTEX_INDEX),
			0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, NULL)))
			return;

		VERTEX_INDEX IB;
		int IB_Index = 0;
		VOID* pVertices2;

		if (FAILED(indexBuffer->Lock(0, 2 * sizeof(VERTEX_INDEX), (void**)&pVertices2, 0)))
			return;

		VERTEX_INDEX* pI = (VERTEX_INDEX*)pVertices2;

		IB._0 = IB_Index;
		IB._1 = IB_Index + 1;
		IB._2 = IB_Index + 3;
		*(pI++) = IB;

		IB._0 = IB_Index + 3;
		IB._1 = IB_Index + 1;
		IB._2 = IB_Index + 2;
		*(pI++) = IB;

		indexBuffer->Unlock();

#pragma endregion

		IsInit = true;
	}

	void SpriteBillBoard::SetPlayDirection(bool _isRight)
	{
		rightPlay = _isRight;
	}

	void SpriteBillBoard::SetRectSize(float _width, float _height)
	{
		CThreadSync sync;

		rectWidth = _width;
		rectHeight = _height;
	}

	Vector2 SpriteBillBoard::GetRectSize()
	{
		CThreadSync sync;

		return Vector2(rectWidth, rectHeight);
	}

	void SpriteBillBoard::SetAnimation(int _width, int _height, int _animationCut, int _animationScene, float _alpha)
	{
		CThreadSync sync;

		width = _width;
		height = _height;
		animeCut = _animationCut;
		animeScene = _animationScene;

		RECT rect = {
			0, 0, width / animeCut, height / animeScene,
		};

		RECT Margin = { 0, 0, 0, 0 };

		curAnimeCut = 0;
		curAnimeScene = 0;
		alpha = 255;

		RECT rect2 = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect2;
	}

	void SpriteBillBoard::PlayCutAnimation(float _timeDelta, float _cutTimer)
	{
		CThreadSync sync;

		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;

		static float CutTimer = 0;

		if (CutTimer > _cutTimer)
		{
			if (rightPlay)
			{
				if (curAnimeCut >= animeCut - 1)
					curAnimeCut = 0;
				else
					curAnimeCut++;
			}
			else
			{
				if (curAnimeCut <= 0)
					curAnimeCut = animeCut - 1;
				else
					curAnimeCut--;
			}

			CutTimer = 0;
		}
		else
			CutTimer += _timeDelta;

		LoadContent();
	}

	void SpriteBillBoard::PlayFullAnimation(float _timeDelta, float _cutTimer)
	{
		CThreadSync sync;

		RECT rect = {
			width / animeCut * curAnimeCut, height / animeScene * curAnimeScene,
			width / animeCut * (curAnimeCut + 1), height / animeScene * (curAnimeScene + 1)
		};

		curRect = rect;

		static float CutTimer = 0;

		if (CutTimer > _cutTimer)
		{
			if (curAnimeCut >= animeCut - 1)
			{
				curAnimeCut = 0;

				if (curAnimeScene >= animeScene - 1)
					curAnimeScene = 0;
				else
					curAnimeScene++;

			}
			else
				curAnimeCut++;

			CutTimer = 0;
		}
		else
			CutTimer += _timeDelta;

		LoadContent();
	}
	
	void SpriteBillBoard::SelectAnimation(int _sceneIndex)
	{
		CThreadSync sync;

		if (curAnimeScene != _sceneIndex)
		{
			curAnimeScene = _sceneIndex;
			curAnimeCut = 0;
		}
	}
	
	bool SpriteBillBoard::CheckMouseRayInMesh(Transform3D* _tr)
	{
		CThreadSync sync;

		Ray ray = RenderMgr->GetPickingRayToView(false);

		//bool Result = RenderMgr->CheckRayInMesh(&ray, _tr->GetTransform(), , NULL);

		return true;// Result;
	}

	void SpriteBillBoard::SetTexturesAddress(string _address)
	{
		CThreadSync sync;

		textureAddress = _address;
	}

	string	SpriteBillBoard::GetTexturesAddress()
	{
		CThreadSync sync;

		return textureAddress;
	}

	void SpriteBillBoard::SetTargetCamera(GameObject* _target)
	{
		CThreadSync sync;

		target = _target;
	}

	void SpriteBillBoard::SetRenderMode(RENDER_MODE _mode)
	{
		CThreadSync sync;

		renderMode = _mode;
	}

	void SpriteBillBoard::Render(IShader* _shaderOption, GameObject* _object)
	{
		CThreadSync sync;

		if (IsInit)
		{
			if (target != NULL)
			{
				Vector3 Dir = ((Camera*)target->GetComponent("Camera"))->GetTargetPosition() - ((Transform3D*)target->GetComponent("Transform3D"))->GetPosition();
				D3DXVec3Normalize(&Dir, &Dir);

				float x = Dir.x;
				float y = Dir.y;
				float z = Dir.z;

				float YAngle = asin(z / (sqrt(pow(x, 2) + pow(z, 2))));
				float ZAngle = asin(y / (sqrt(pow(x, 2) + pow(y, 2))));

				ZAngle = 0;

				if (Dir.x >= 0 && Dir.z >= 0)
				{
					YAngle = asin(z / (sqrt(pow(x, 2) + pow(z, 2))));
				}
				else if (Dir.x < 0 && Dir.z >= 0)
				{
					YAngle = asin(-x / (sqrt(pow(x, 2) + pow(z, 2))));

					YAngle += 3.14f / 2;
				}
				else if (Dir.z < 0 && Dir.x < 0)
				{
					YAngle = asin(-z / (sqrt(pow(x, 2) + pow(z, 2))));

					YAngle += 3.14f;
				}
				else if (Dir.z < 0 && Dir.x >= 0)
				{
					YAngle = asin(x / (sqrt(pow(x, 2) + pow(z, 2))));

					YAngle += 3.14f * (3.0f / 2.0f);
				}

				YAngle += 3.14f + (3.0f / 2.0f);

				((Transform3D*)_object->GetComponent("Transform3D"))->SetRotate(0, -YAngle, -ZAngle);
			}

			Matrix Transform = ((Transform3D*)_object->GetComponent("Transform3D"))->GetTransform();
			RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &Transform);

			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);

				RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);

				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
			}
			else if (renderMode == RENDER_STENCIL)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x00);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			}

			RenderMgr->GetDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(VERTEX));
			RenderMgr->GetDevice()->SetIndices(indexBuffer);

			if (_shaderOption == NULL)
			{
				//RenderMgr->GetDevice()->SetMaterial(&meshMaterial);
				RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureAddress));
			}
			else
				_shaderOption->Render(0, _object);
			
			RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
			RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			RenderMgr->GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
			}
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);

		}
	}
}