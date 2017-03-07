#include "Common.h"
#include "RenderManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "TrailRenderer.h"
#include "Vertex.h"

namespace BONE_GRAPHICS
{
	TrailRenderer::TrailRenderer()
	{
		ThreadSync sync;

		SetTypeName("TrailRenderer");

		vertexBuffer = nullptr;
		indexBuffer = nullptr;

		liveCycle = 1;
		createCycle = 2;
		curCreateTime = 0;

		pivot = Vector3(0, 0, 0);
		upDir = Vector3(0, 1, 0);

		startSize = 1;
		endSize = 1;

		renderMode = RENDER_ALPHA;
		IsActive = true;
		
		IsInit = false;
	}

	TrailRenderer::~TrailRenderer()
	{
		ThreadSync sync;

		if (vertexBuffer != nullptr)
			vertexBuffer->Release();

		if (indexBuffer != nullptr)
			 indexBuffer->Release();
	}

	void TrailRenderer::LoadContent()
	{
		ThreadSync sync;

		IsInit = true;
	}

	void TrailRenderer::SetTargetObject(GameObject* _targetObject, Vector3 _pivot)
	{
		ThreadSync sync;

		target = _targetObject;
		pivot = _pivot;
	}

	void TrailRenderer::SetSize(float _start, float _end)
	{
		ThreadSync sync;

		startSize = _start;
		endSize = _end;
	}

	void TrailRenderer::SetCycle(float _liveCycle, float _createCycle)
	{
		ThreadSync sync;

		if (_liveCycle < 0)
			liveCycle = 0;
		else
			liveCycle = _liveCycle;

		if (_createCycle < 0)
			createCycle = 0;
		else
			createCycle = _createCycle;
	}
	
	void TrailRenderer::UpdateStatus(double _timeDelta)
	{
		ThreadSync sync;

		for (auto Iter = trailList.begin(); Iter != trailList.end(); Iter++)
		{
			if (liveCycle < (*Iter).curLiveTime)
				Iter = trailList.erase(Iter);
			else
			{
				float time = (*Iter).curLiveTime / liveCycle;

				(*Iter).curSize = (-(startSize + endSize)) * time + (startSize + endSize) + endSize;
				(*Iter).curLiveTime += _timeDelta;
			}
		}

		if (IsActive)
		{
			if (createCycle < curCreateTime)
			{
				TrailMesh temp;
				temp.curLiveTime = 0;
				temp.curSize = startSize;
				
				Vector3 Posit = pivot;
				D3DXVec3TransformCoord(&Posit, &Posit, &((Transform3D*)target->GetComponent("Transform3D"))->GetTransform());
				temp.pivot = Posit;

				Vector3 UpDir = upDir;
				D3DXVec3TransformNormal(&UpDir, &UpDir, &((Transform3D*)target->GetComponent("Transform3D"))->GetTransform());
				D3DXVec3Normalize(&UpDir, &UpDir);
				temp.upDir = UpDir;

				trailList.push_back(temp);

				curCreateTime = 0;
			}

			curCreateTime += _timeDelta;

			if (trailList.size() >= 2)
				SetMeshBuffer();

		}
	}

	void TrailRenderer::SetMeshBuffer()
	{
		ThreadSync sync;

		VERTEX* vertex;
		vertex = new VERTEX[trailList.size() * 2];

		int index = 0;

		for (int i = trailList.size() - 1; i >= 0 ; i--)
		{
			vertex[index].p = trailList[i].pivot + (trailList[i].upDir * trailList[i].curSize / 2);
			vertex[index].n = vertex[index].p;
			D3DXVec3Normalize(&vertex[index].n, &vertex[index].n);
			vertex[index].uv.x = trailList.size() - 1 - i;
			vertex[index++].uv.y = 0;

			vertex[index].p = trailList[i].pivot - (trailList[i].upDir * trailList[i].curSize / 2);
			vertex[index].n = vertex[index].p;
			D3DXVec3Normalize(&vertex[index].n, &vertex[index].n);
			vertex[index].uv.x = trailList.size() - 1 - i;
			vertex[index++].uv.y = 1;
		}

		if (vertexBuffer != nullptr)
		{
			vertexBuffer->Release();
			vertexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateVertexBuffer(
			trailList.size() * 2 * sizeof(VERTEX), D3DUSAGE_WRITEONLY,
			VERTEX::FVF, D3DPOOL_DEFAULT, &vertexBuffer, nullptr)))
		{
			return;
		}

		VOID* pVertices;

		if (FAILED(vertexBuffer->Lock(0, trailList.size() * 2 * sizeof(VERTEX), (void**)&pVertices, 0)))
			return;

		VERTEX* pV = (VERTEX*)pVertices;

		for (int i = 0; i < trailList.size() * 2; i++)
			*(pV++) = vertex[i];

		vertexBuffer->Unlock();

		delete[] vertex;

		if (indexBuffer != nullptr)
		{
			indexBuffer->Release();
			indexBuffer = nullptr;
		}

		if (FAILED(RenderMgr->GetDevice()->CreateIndexBuffer(
			(trailList.size() * 2 - 2) * sizeof(VERTEX_INDEX),
			0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr)))
			return;

		VERTEX_INDEX IB;
		int IB_Index = 0;
		VOID* pVertices2;

		if (FAILED(indexBuffer->Lock(0, (trailList.size() * 2 - 2) * sizeof(VERTEX_INDEX), (void**)&pVertices2, 0)))
			return;

		VERTEX_INDEX* pI = (VERTEX_INDEX*)pVertices2;

		for (int i = 0; i < (trailList.size() * 2 - 2) / 2; i++)
		{
			IB._0 = IB_Index;
			IB._1 = IB_Index + 1;
			IB._2 = IB_Index + 2;
			*(pI++) = IB;

			IB._0 = IB_Index + 2;
			IB._1 = IB_Index + 1;
			IB._2 = IB_Index + 3;
			*(pI++) = IB;

			IB_Index += 2;
		}

		indexBuffer->Unlock();
	}

	void TrailRenderer::SetUpVector(Vector3 _upDir)
	{
		ThreadSync sync;

		upDir = _upDir;
		D3DXVec3Normalize(&upDir, &upDir);
	}
	
	void TrailRenderer::SetActive(bool _active)
	{
		ThreadSync sync;

		IsActive = _active;
	}

	void TrailRenderer::SetRenderMode(int _mode)
	{
		ThreadSync sync;

		renderMode = _mode;
	}

	void TrailRenderer::Render(IShader* _shaderOption)
	{
		ThreadSync sync;

		if (IsInit && trailList.size() >= 2 && IsActive)
		{
			if (renderMode == RENDER_ALPHA)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);

				RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_TEXTURE);
			}
			else if (renderMode == RENDER_STENCIL)
			{
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0x00);
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

			}

			Matrix World;
			D3DXMatrixIdentity(&World);
			RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &World);
			RenderMgr->GetDevice()->SetStreamSource(0, vertexBuffer, 0, sizeof(VERTEX));
			RenderMgr->GetDevice()->SetIndices(indexBuffer);
			RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureAddress));
			RenderMgr->GetDevice()->SetFVF(VERTEX::FVF);
			RenderMgr->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
				trailList.size() * 2, 0, (trailList.size() - 1) * 2);

			if (renderMode == RENDER_ALPHA)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			else if (renderMode == RENDER_STENCIL)
				RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		}
	}

	void TrailRenderer::SetTexturesAddress(string _address)
	{
		ThreadSync sync;

		textureAddress = _address;
	}

	string TrailRenderer::GetTexturesAddress()
	{
		ThreadSync sync;

		return textureAddress;
	}
}