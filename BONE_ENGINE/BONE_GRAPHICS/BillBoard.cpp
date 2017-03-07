#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "BillBoard.h"
#include <shlwapi.h>
#include "Camera.h"
#include "Vertex.h"
#include "GameObject.h"
#pragma comment(lib, "shlwapi")
#pragma warning(disable:4996)

namespace BONE_GRAPHICS
{
	BillBoard::BillBoard()
	{
		ThreadSync sync;

		SetTypeName("BillBoard");

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		target = nullptr;

		rectWidth = 1;
		rectHeight = 1;

		renderMode = RENDER_DEFAULT;
	}

	BillBoard::~BillBoard()
	{
		ThreadSync sync;

		if (vertexBuffer == nullptr)
			vertexBuffer->Release();

		if (indexBuffer == nullptr)
			indexBuffer->Release();

		ResourceMgr->ReleaseTexture(textureAddress);
	}

	void BillBoard::LoadContent()
	{
		ThreadSync sync;

		VERTEX vertex[4];

#pragma region Vertex Setup

		vertex[0].p.x = rectWidth / 2;
		vertex[0].p.y = rectHeight / 2;
		vertex[0].p.z = 0;

		vertex[0].n = vertex[0].p;
		D3DXVec3Normalize(&vertex[0].n, &vertex[0].n);

		vertex[0].uv.x = (double)originRect.LeftTop.x / (double)width;
		vertex[0].uv.y = (double)originRect.LeftTop.y / (double)height;

		vertex[1].p.x = -rectWidth / 2;
		vertex[1].p.y = rectHeight / 2;
		vertex[1].p.z = 0;

		vertex[1].n = vertex[1].p;
		D3DXVec3Normalize(&vertex[1].n, &vertex[1].n);

		vertex[1].uv.x = (double)originRect.RightBottom.x / (double)width;
		vertex[1].uv.y = (double)originRect.LeftTop.y / (double)height;

		vertex[2].p.x = -rectWidth / 2;
		vertex[2].p.y = -rectHeight / 2;
		vertex[2].p.z = 0;

		vertex[2].n = vertex[2].p;
		D3DXVec3Normalize(&vertex[2].n, &vertex[2].n);

		vertex[2].uv.x = (double)originRect.RightBottom.x / (double)width;
		vertex[2].uv.y = (double)originRect.RightBottom.y / (double)height;

		vertex[3].p.x = rectWidth / 2;
		vertex[3].p.y = -rectHeight / 2;
		vertex[3].p.z = 0;

		vertex[3].n = vertex[3].p;
		D3DXVec3Normalize(&vertex[3].n, &vertex[3].n);

		vertex[3].uv.x = (double)originRect.LeftTop.x / (double)width;
		vertex[3].uv.y = (double)originRect.RightBottom.y / (double)height;

#pragma endregion

#pragma region Vertex/Index Buffer Setup
		if (vertexBuffer != nullptr)
		{
			vertexBuffer->Release();
			vertexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_WRITEONLY,
			VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
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

		if (indexBuffer != nullptr)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer(2 * sizeof(VERTEX_INDEX),
			0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr)))
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

	bool BillBoard::CheckMouseRayInMesh(Transform3D* _tr)
	{
		ThreadSync sync;

		RAY ray = RenderMgr->GetPickingRayToView(false);

		//bool Result = RenderMgr->CheckRayInMesh(&ray, _tr->GetTransform(), , nullptr);
		//bool Result = RenderMgr->CheckRayInMesh(&ray, _tr->GetTransform(), ResourceMgr->FindMesh("BillBoard")->_mesh, nullptr);

		return true;//Result;
	}

	void BillBoard::SetTexturesAddress(string _address, int _width, int _height)
	{
		ThreadSync sync;

		textureAddress = _address;
		width = _width;
		height = _height;
	}

	string	BillBoard::GetTexturesAddress()
	{
		ThreadSync sync;

		return textureAddress;
	}

	void BillBoard::SetTarget(GameObject* _target)
	{
		ThreadSync sync;

		target = _target;
	}

	void BillBoard::SetRenderMode(RENDER_MODE _mode)
	{
		ThreadSync sync;

		renderMode = _mode;
	}

	void BillBoard::SetRectSize(float _width, float _height)
	{
		ThreadSync sync;

		rectWidth = _width;
		rectHeight = _height;

		if (IsInit)
			LoadContent();
	}

	Vector2 BillBoard::GetRectSize()
	{
		ThreadSync sync;

		return Vector2(rectWidth, rectHeight);
	}

	void BillBoard::SetOriginRect(Rect _rect)
	{
		ThreadSync sync;

		originRect = _rect;

		if (IsInit)
			LoadContent();
	}

	void BillBoard::SetOriginRect(Vector2 _leftUp, Vector2 _rightBottom)
	{
		ThreadSync sync;

		originRect.LeftTop = _leftUp;
		originRect.RightBottom = _rightBottom;

		if (IsInit)
			LoadContent();
	}

	Rect BillBoard::GetOriginRect()
	{
		ThreadSync sync;

		return originRect;
	}

	void BillBoard::Render(IShader* _shaderOption, GameObject* _object)
	{
		ThreadSync sync;

		if (IsInit)
		{
			if (target != nullptr)
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

			if (_shaderOption == nullptr)
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