#include "Common.h"
#include "ParticleSystem.h"
#include "RenderManager.h"
#include "ResourceManager.h"

namespace BONE_GRAPHICS
{
	BoundingBox::BoundingBox()
	{
		// infinite small 
		min.x = FLT_MAX;
		min.y = FLT_MAX;
		min.z = FLT_MAX;

		max.x = -FLT_MAX;
		max.y = -FLT_MAX;
		max.z = -FLT_MAX;
	}

	bool BoundingBox::isPointInside(Vector3& p)
	{
		if (p.x >= min.x && p.y >= min.y && p.z >= min.z &&
			p.x <= max.x && p.y <= max.y && p.z <= max.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	BoundingSphere::BoundingSphere()
	{
		radius = 0.0f;
	}

	const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	ParticleSystem::ParticleSystem()
	{
		vb = 0;
	}

	ParticleSystem::~ParticleSystem()
	{
		vb->Release();
	}

	void ParticleSystem::SetTextreAddress(string _address)
	{
		textureAddress = _address;
	}

	void ParticleSystem::LoadContent()
	{
		// vertex buffer's size does not equal the number of particles in our system.  We
		// use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
		// size we choose for the vertex buffer is specified by the _vbSize variable.

		HRESULT hr = 0;

		hr = RenderMgr->GetDevice()->CreateVertexBuffer(
			vbSize * sizeof(Particle),
			D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
			Particle::FVF,
			D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
			&vb,
			0);

		ResourceMgr->LoadTexture(textureAddress);

		if (FAILED(hr))	{
			return;
		}
	}

	void ParticleSystem::Reset()
	{
		std::list<Attribute>::iterator i;

		for (i = particles.begin(); i != particles.end(); i++)
		{
			ResetParticle(&(*i));
		}
	}

	void ParticleSystem::AddParticle()
	{
		Attribute attribute;

		ResetParticle(&attribute);

		particles.push_back(attribute);
	}

	DWORD FtoDw(float f)
	{
		return *((DWORD*)&f);
	}

	void ParticleSystem::PreRender()
	{
		RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, true);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSIZE, FtoDw(size));
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDw(0.0f));

		// control the size of the particle relative to distance
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSCALE_A, FtoDw(0.0f));
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSCALE_B, FtoDw(0.0f));
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSCALE_C, FtoDw(1.0f));

		// use alpha from texture
		RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		RenderMgr->GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	void ParticleSystem::PostRender()
	{
		RenderMgr->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_POINTSCALEENABLE, false);
		RenderMgr->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}

	void ParticleSystem::Render()
	{
		//
		// Remarks:  The render method works by filling a section of the vertex buffer with data,
		//           then we render that section.  While that section is rendering we lock a new
		//           section and begin to fill that section.  Once that sections filled we render it.
		//           This process continues until all the particles have been drawn.  The benifit
		//           of this method is that we keep the video card and the CPU busy.  
		if (!particles.empty())
		{
			//
			// set render states
			//

			PreRender();

			RenderMgr->GetDevice()->SetTransform(D3DTS_WORLD, &transform3D->GetTransform());
			RenderMgr->GetDevice()->SetTexture(0, ResourceMgr->LoadTexture(textureAddress));
			RenderMgr->GetDevice()->SetFVF(Particle::FVF);
			RenderMgr->GetDevice()->SetStreamSource(0, vb, 0, sizeof(Particle));

			//
			// render batches one by one
			//

			// start at beginning if we're at the end of the vb
			if (vbOffset >= vbSize)
				vbOffset = 0;

			Particle* v = 0;

			vb->Lock(
				vbOffset    * sizeof(Particle),
				vbBatchSize * sizeof(Particle),
				(void**)&v,
				vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			DWORD numParticlesInBatch = 0;

			//
			// Until all particles have been rendered.
			//
			std::list<Attribute>::iterator i;
			for (i = particles.begin(); i != particles.end(); i++)
			{
				if (i->isAlive)
				{
					//
					// Copy a batch of the living particles to the
					// next vertex buffer segment
					//
					v->position = i->position;
					v->color = (D3DCOLOR)i->color;
					v++; // next element;

					numParticlesInBatch++; //increase batch counter

										   // if this batch full?
					if (numParticlesInBatch == vbBatchSize)
					{
						//
						// Draw the last batch of particles that was
						// copied to the vertex buffer. 
						//
						vb->Unlock();

						RenderMgr->GetDevice()->DrawPrimitive(
							D3DPT_POINTLIST,
							vbOffset,
							vbBatchSize);

						//
						// While that batch is drawing, start filling the
						// next batch with particles.
						//

						// move the offset to the start of the next batch
						vbOffset += vbBatchSize;

						// don't offset into memory thats outside the vb's range.
						// If we're at the end, start at the beginning.
						if (vbOffset >= vbSize)
							vbOffset = 0;

						vb->Lock(
							vbOffset    * sizeof(Particle),
							vbBatchSize * sizeof(Particle),
							(void**)&v,
							vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

						numParticlesInBatch = 0; // reset for new batch
					}
				}
			}

			vb->Unlock();

			// its possible that the LAST batch being filled never 
			// got rendered because the condition 
			// (numParticlesInBatch == _vbBatchSize) would not have
			// been satisfied.  We draw the last partially filled batch now.

			if (numParticlesInBatch)
			{
				RenderMgr->GetDevice()->DrawPrimitive(
					D3DPT_POINTLIST,
					vbOffset,
					numParticlesInBatch);
			}

			// next block
			vbOffset += vbBatchSize;

			PostRender();
		}
	}

	bool ParticleSystem::IsEmpty()
	{
		return particles.empty();
	}

	bool ParticleSystem::IsDead()
	{
		std::list<Attribute>::iterator i;
		for (i = particles.begin(); i != particles.end(); i++)
		{
			// is there at least one living particle?  If yes,
			// the system is not dead.
			if (i->isAlive)
				return false;
		}
		// no living particles found, the system must be dead.
		return true;
	}

	void ParticleSystem::RemoveDeadParticles()
	{
		std::list<Attribute>::iterator i;

		i = particles.begin();

		while (i != particles.end())
		{
			if (i->isAlive == false)
			{
				// erase returns the next iterator, so no need to
				// incrememnt to the next one ourselves.
				i = particles.erase(i);
			}
			else
			{
				i++; // next in list
			}
		}
	}

	float GetRandomFloat(float lowBound, float highBound)
	{
		if (lowBound >= highBound) // bad input
			return lowBound;

		// get random float in [0, 1] interval
		float f = (rand() % 10000) * 0.0001f;

		// return float in [lowBound, highBound] interval. 
		return (f * (highBound - lowBound)) + lowBound;
	}

	void GetRandomVector(
		D3DXVECTOR3* out,
		D3DXVECTOR3* min,
		D3DXVECTOR3* max)
	{
		out->x = GetRandomFloat(min->x, max->x);
		out->y = GetRandomFloat(min->y, max->y);
		out->z = GetRandomFloat(min->z, max->z);
	}
}