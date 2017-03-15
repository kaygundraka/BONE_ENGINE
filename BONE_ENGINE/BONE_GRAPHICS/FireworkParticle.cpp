#include "Common.h"
#include "ParticleSystem.h"

namespace BONE_GRAPHICS
{
	FireworkParticle::FireworkParticle()
	{
		SetTypeName("FireworkParticle");
	}

	void FireworkParticle::Init(GameObject* _owner, int _numParticles, float _size)
	{
		transform3D = (Transform3D*)(_owner->GetComponent("Transform3D"));
		size = _size;
		vbSize = 2048;
		vbOffset = 0;
		vbBatchSize = 512;

		for (int i = 0; i < _numParticles; i++)
			AddParticle();
	}

	void FireworkParticle::ResetParticle(Attribute* _attribute)
	{
		_attribute->isAlive = true;
		_attribute->position = Vector3(0, 0, 0);

		Vector3 min = Vector3(-1.0f, -1.0f, -1.0f);
		Vector3 max = Vector3(1.0f, 1.0f, 1.0f);


		GetRandomVector(&_attribute->velocity, &min, &max);

		// normalize to make spherical
		D3DXVec3Normalize(
			&_attribute->velocity,
			&_attribute->velocity);

		_attribute->velocity *= 9.0f;

		_attribute->color = D3DXCOLOR(
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f,//GetRandomFloat(0.0f, 1.0f),
			1.0f);

		_attribute->age = 0.0f;
		_attribute->lifeTime = 2.0f; // lives for 2 seconds
	}

	// 점과 점 사이의 거리 3D
	double GetDistance3D(const D3DXVECTOR3& _v1, const D3DXVECTOR3& _v2)	// p1, p2 간의 거리 계산
	{
		return (float)sqrt(pow(_v2.x - _v1.x, 2) + pow(_v2.y - _v1.y, 2) + pow(_v2.z - _v1.z, 2));
	}

	void FireworkParticle::Update(double _timeDelta)
	{
		std::list<Attribute>::iterator i, j;
		static bool bDiffusion = true;
		float distance = 0;
		D3DXVECTOR3 r;

		//srand(time(nullptr));
		for (i = particles.begin(); i != particles.end(); i++)
		{
			i->position = i->position + (i->velocity) * _timeDelta;	// p = p0 + 시간변화량*v'(속도변화량)		
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
			i->velocity = i->velocity + i->acceleration * _timeDelta;// v' = v0 + 시간변화량*가속도
		}
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