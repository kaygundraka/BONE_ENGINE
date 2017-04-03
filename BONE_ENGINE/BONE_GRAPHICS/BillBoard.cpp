#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "BillBoard.h"
#include "Camera.h"
#include "Vertex.h"
#include "GameObject.h"

namespace BONE_GRAPHICS
{
	BillBoard::BillBoard()
	{
		SetTypeName("BillBoard");

		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		target = nullptr;
        isShow = true;

		rectWidth = 1;
		rectHeight = 1;

		renderMode = RENDER_DEFAULT;
	}

	BillBoard::~BillBoard()
	{
		if (vertexBuffer == nullptr)
			vertexBuffer->Release();

		if (indexBuffer == nullptr)
			indexBuffer->Release();

		ResourceMgr->ReleaseTexture(fileName);
	}

	void BillBoard::LoadContent()
	{
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

		if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
		{
			return;
		}

		VOID* pVertices;

		if (FAILED(vertexBuffer->Lock(0, 4 * sizeof(VERTEX), (void**)&pVertices, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
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

		if (FAILED(indexBuffer->Lock(0, 2 * sizeof(VERTEX_INDEX), (void**)&pVertices2, D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
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

	bool BillBoard::CheckMouseRayInMesh(Transform3D* tr)
	{
		RAY ray = RenderMgr->GetPickingRayToView(false);

		return true;
	}

	void BillBoard::SetTexture(std::string fileName, int width, int height)
	{
		this->fileName = fileName;
		this->width = width;
        this->height = height;
	}

	std::string	BillBoard::GetTexture()
	{
		return fileName;
	}

	void BillBoard::SetTarget(GameObject* target)
	{
		this->target = target;
	}

	void BillBoard::SetRenderMode(RENDER_MODE mode)
	{
		renderMode = mode;
	}

	void BillBoard::SetRectSize(float width, float height)
	{
		rectWidth = width;
		rectHeight = height;

		if (IsInit)
			LoadContent();
	}

	Vec2 BillBoard::GetRectSize()
	{
		return Vec2(rectWidth, rectHeight);
	}

	void BillBoard::SetOriginRect(Rect rect)
	{
		originRect = rect;

		if (IsInit)
			LoadContent();
	}

	void BillBoard::SetOriginRect(Vec2 leftUp, Vec2 rightBottom)
	{
		originRect.LeftTop = leftUp;
		originRect.RightBottom = rightBottom;

		if (IsInit)
			LoadContent();
	}

	Rect BillBoard::GetOriginRect()
	{
		return originRect;
	}

    void BillBoard::SetShow(bool show)
    {
        isShow = show;
    }
    
    bool BillBoard::IsShow()
    {
        return isShow;
    }

	void BillBoard::Render(IShader* shaderOpt, GameObject* object)
	{
		if (IsInit && isShow)
		{
            if (target != nullptr)
            {
                auto mat = ((Camera*)target->GetComponent("Camera"))->GetViewMatrix(target);
                mat._41 = 0;
                mat._42 = 0;
                mat._43 = 0;
                D3DXMatrixInverse(&mat, 0, &mat);

                mat._41 = ((Transform3D*)object->GetComponent("Transform3D"))->GetPosition().x;
                mat._42 = ((Transform3D*)object->GetComponent("Transform3D"))->GetPosition().y;
                mat._43 = ((Transform3D*)object->GetComponent("Transform3D"))->GetPosition().z;

                RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &mat);
            }
			
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

			if (shaderOpt == nullptr)
				RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(fileName));
			else
				shaderOpt->Render(0, object);

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