#include "Common.h"
#include "ParticleSystem.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
	FireworkParticle::FireworkParticle()
	{
		SetTypeName("FireworkParticle");
	}

	void FireworkParticle::Init(GameObject* owner, int numParticles, float size)
	{
		transform3D = (Transform3D*)(owner->GetComponent("Transform3D"));
		this->size = size;
		vbSize = 2048;
		vbOffset = 0;
		vbBatchSize = 512;
        resetTimer = 0;
        this->numParticles = numParticles;

		for (int i = 0; i < numParticles; i++)
			AddParticle();
	}

    int FireworkParticle::GetNumOfParticles()
    {
        return numParticles;
    }
    
    int FireworkParticle::GetSize()
    {
        return this->size;
    }

	void FireworkParticle::ResetParticle(Attribute* attribute)
	{
        attribute->isAlive = true;
        attribute->position = Vec3(0, 0, 0);

		Vec3 min = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 max = Vec3(1.0f, 1.0f, 1.0f);

		GetRandomVector(&attribute->velocity, &min, &max);

		// normalize to make spherical
		D3DXVec3Normalize(
			&attribute->velocity,
			&attribute->velocity);

        attribute->velocity *= 9.0f;

        attribute->color = D3DXCOLOR(
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f);

        attribute->age = 0.0f;
        attribute->lifeTime = 2.0f; // lives for 2 seconds
	}

	// 점과 점 사이의 거리 3D
	double GetDistance3D(const D3DXVECTOR3& _v1, const D3DXVECTOR3& _v2)	// p1, p2 간의 거리 계산
	{
		return (float)sqrt(pow(_v2.x - _v1.x, 2) + pow(_v2.y - _v1.y, 2) + pow(_v2.z - _v1.z, 2));
	}

	void FireworkParticle::Update()
	{
        if (resetTimer > 2.0f)
        {
            resetTimer = 0;
            this->Reset();
        }

		std::list<Attribute>::iterator i, j;
		static bool bDiffusion = true;
		float distance = 0;
		D3DXVECTOR3 r;

		//srand(time(nullptr));
		for (i = particles.begin(); i != particles.end(); i++)
		{
			i->position = i->position + (i->velocity) * SceneMgr->GetTimeDelta();	// p = p0 + 시간변화량*v'(속도변화량)		
			i->acceleration *= 0;

			for (j = particles.begin(); j != particles.end(); j++)
			{
				// 가속도 계산
				r = j->position - i->position;	// r벡터 계산
													//distance = D3DXVec3Length(&r);	// 
				distance = GetDistance3D(j->position, i->position);	// r의 거리값 계산
				if (distance > 1.0f)
				{
					i->acceleration += r * (120 / particles.size()) / pow(distance, 3);	// A 가속도 계산			
				}
			}
			//i->_position = i->_position + (i->_velocity) * timeDelta;	// p = p0 + 시간변화량*v'(속도변화량)		
			i->velocity = i->velocity + i->acceleration * SceneMgr->GetTimeDelta();// v' = v0 + 시간변화량*가속도
		}

        resetTimer += SceneMgr->GetTimeDelta();
	}

	void FireworkParticle::PreRender()
	{
		ParticleSystem::PreRender();

		RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		// read, but don't write particles to z-buffer
		RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, false);
	}

	void FireworkParticle::PostRender()
	{
		ParticleSystem::PostRender();

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}
}