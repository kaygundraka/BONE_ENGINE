#include "Common.h"
#include "ParticleSystem.h"
#include "Component.h"
#include "SceneManager.h"

namespace BONE_GRAPHICS
{
	SnowParticle::SnowParticle()
	{
		SetTypeName("SnowParticle");
	}

	void SnowParticle::Init(GameObject* _owner, BoundingBox* _boundingBox, int _numParticles)
	{
		transform3D = (Transform3D*)(_owner->GetComponent("Transform3D"));;
		boundingBox = *_boundingBox;
		
		size = 0.25f;
		vbSize = 2048;
		vbOffset = 0;
		vbBatchSize = 512;

		for (int i = 0; i < _numParticles; i++)
			AddParticle();
	}

	void SnowParticle::SetParticleSize(int _particleSize)
	{
		size = _particleSize;
	}

	void SnowParticle::ResetParticle(Attribute* _attribute)
	{
		BoundingBox Box;

		Box = boundingBox;
		Box.max = boundingBox.max + transform3D->GetPosition();
		Box.min = boundingBox.min + transform3D->GetPosition();

		_attribute->isAlive = true;

		// get random x, z coordinate for the position of the snow flake.
		GetRandomVector(
			&_attribute->position,
			&Box.min,
			&Box.max);

		// no randomness for height (y-coordinate).  Snow flake
		// always starts at the top of bounding box.
		_attribute->position.y = Box.max.y;

		// snow flakes fall downwards and slightly to the left
		_attribute->velocity.x = GetRandomFloat(0.0f, 1.0f) * -3.0f;
		_attribute->velocity.y = GetRandomFloat(0.0f, 1.0f) * -10.0f;
		_attribute->velocity.z = 0.0f;

		// white snow flake
		_attribute->color = COLOR::WHITE;
	}

	void SnowParticle::Update()
	{
		BoundingBox Box;

		Box = boundingBox;
		Box.max = boundingBox.max + transform3D->GetPosition();
		Box.min = boundingBox.min + transform3D->GetPosition();
		
		std::list<Attribute>::iterator i;
		for (i = particles.begin(); i != particles.end(); i++)
		{
			i->position += i->velocity * SceneMgr->GetTimeDelta();

			// is the point outside bounds?
			if (Box.isPointInside(i->position) == false)
			{
				// nope so kill it, but we want to recycle dead 
				// particles, so respawn it instead.
				ResetParticle(&(*i));
			}
		}
	}
}
