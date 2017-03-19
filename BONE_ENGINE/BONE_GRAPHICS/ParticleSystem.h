#pragma once
#include "Common.h"
#include "Component.h"
#include "Transform3D.h"
#include <list>

namespace BONE_GRAPHICS
{
	struct BoundingBox
	{
		BoundingBox();

		bool isPointInside(Vec3& p);

		Vec3 min;
		Vec3 max;
	};

	struct BoundingSphere
	{
		BoundingSphere();

		Vec3 center;
		float   radius;
	};

	////��ƼŬ�� ��ġ�� �÷��� ǥ���ϴ� ���� ����ü ����.
	struct Particle
	{
		Vec3 position;
		D3DCOLOR    color;
		static const int FVF;
	};

	struct Attribute
	{
		Attribute()
		{
			lifeTime = 0.0f;
			age = 0.0f;
			isAlive = true;
		}
		// ��ƼŬ�� ��ƼŬ�� �Ӽ���. 

		// ��ƼŬ�� �������ϴµ� �ʿ��� �����Ϳ� ��ƼŬ �Ӽ��� ������ ����ü ���� �����ϰ� �Ǵµ�,
		// ��ƼŬ�� ����� �ı��ϰ� ������ ���� �ٷ� �̷��� �Ӽ��� �̿��� �۾��ϴ� ���̸�,
		// �̸� �������� �غ� �Ϸ�Ǹ� Particle ����ü�� ��ġ�� �Ӽ��� ������ �̿�.
		Vec3 position;     // ���� �����̽� ���� ��ƼŬ ��ġ.
		Vec3 velocity;     // ��ƼŬ�� �ӵ�, ������ �ʴ� �̵� ������ ���.
		Vec3 acceleration; // ��ƼŬ�� ����. ������ �ʴ� �̵� ������ ���.
		float       lifeTime;     // ��ƼŬ�� �Ҹ��� ������ �����Ǵ� �ð�.
								   // ��, ������ �� ��ƼŬ�� Ư���� �ð����ȸ� ����.
		float       age;          // ��ƼŬ�� ���� ����
		D3DXCOLOR   color;        // ��ƼŬ�� �÷�
		Vec3   colorFade;    // ��ƼŬ�� �÷��� �ð��� �帧�� ���� ����ϴ� ���
		bool        isAlive;    // ��ƼŬ�� ������ ��� true, �Ҹ��� ��� false
	};

	// ��ƼŬ �ý����� ��ҵ�.

	// ��ƼŬ �ý����� ��ƼŬ���� �����̸� ��ƼŬ�� �����ְ� �����ϴ� ������ ����Ѵ�.
	// ��ƼŬ�� ũ�⳪ ��ƼŬ ��õ�� ��ġ, ��ƼŬ�� ������ �ؽ�ó �� 
	// �ý��� ���� ��� ��ƼŬ�� ������ �ִ� ���� Ư������ ����.
	// ����� �������� ����
	// ��ƼŬ ���Ű� ���÷���, �Ҹ�, ���� ���� �����ϴ� ����.
	class ParticleSystem : public Component
	{
	public:
		// ����Ʈ ���� �ʱ�ȭ�ϴ� �����ڿ� 
		// ��ġ �������̽� (���ؽ� ����, �ؽ�ó)�� �����ϴ� ��Ʈ����
		ParticleSystem();
		virtual ~ParticleSystem();

		// init - ����Ʈ ��������Ʈ�� �����ϱ� ���� ���ؽ� ���۸� �����
		// �ؽ�ó�� ����� ���� Direct3D�� ��ġ �������� �ʱ�ȭ �۾��� ó��.
		virtual void LoadContent();

		// �ý��� ���� ��� ��ƼŬ �Ӽ��� ����.
		virtual void Reset();

		virtual void ResetParticle(Attribute* attribute) = 0;

		void SetTextreAddress(std::string _address);

		// �ý��ۿ� ��ƼŬ�� �߰�.
		virtual void AddParticle();

		// �ý��� ���� ��� ��ƼŬ���� ����.
		virtual void Update(double _TimeDelta) = 0;

		// �������� �ռ� �����ؾ� �� �ʱ� ���� ���¸� ����.
		// �� �޼���� �ý��ۿ� ���� �޶��� �� �����Ƿ� ���� �Լ��� ����.
		virtual void PreRender();

		// �ý��� ���� ��� ��ƼŬ���� ������
		virtual void Render();

		// Ư�� ��ƼŬ �ý����� �������� �� �ִ� ���� ���¸� �����ϴ� �� �̿�.
		// �� �޼���� �ý��ۿ� ���� �޶��� �� �����Ƿ� ���� �޼���� ����.
		virtual void PostRender();

		// ���� �ý��ۿ� ��ƼŬ�� ���� ��� true ����.
		bool IsEmpty();

		// �ý��� ���� ��ƼŬ�� ��� ���� ��� true ����.
		bool IsDead();

	protected:
		// �Ӽ� ����Ʈ _particle�� �˻��Ͽ� ���� ��ƼŬ�� ����Ʈ���� ����.
		virtual void RemoveDeadParticles();

	protected:
		std::string textureAddress;
		Transform3D*       transform3D;  // �ý����� ��õ, �ý��� ������ ��ƼŬ�� ���۵Ǵ� ��.
		BoundingBox        boundingBox;  // ��ƼŬ�� �̵��� �� �ִ� ���Ǹ� �����ϴ� �� �̿�. 
											   // ��, �� ������ �ѷ��� �������� ���� ���� �ý���.
											   // ���ϴ� ������ �����ڷ� �����ϸ� �� ������ ��� ��ƼŬ���� ��ٷ� �Ҹ�.
		float                   emitRate;   // �ý��ۿ� ���ο� ��ƼŬ�� �߰��Ǵ� ����. ������ �ʴ� ��ƼŬ ���� ���.
		float                   size;       // �ý��� �� ��� ��ƼŬ�� ũ��
		IDirect3DVertexBuffer9* vb;
		std::list<Attribute>    particles;  // �ý��� �� ��ƼŬ �Ӽ��� ����Ʈ.
											 // �츮�� ��ƼŬ�� ����� �����ϰ� �����ϴ� �� �� ����Ʈ�� �̿�.
											 // ��ƼŬ�� �׸� �غ� �Ϸ�Ǹ� 
											 // ����Ʈ ����� �Ϻθ� ���ؽ� ���۷� �����ϰ�
											 // ��ƼŬ ����� ������ ��ģ��.
											 // �̾� ���� �ܰ��� �����͸� �����ϰ� �ٽ� ��ƼŬ�� �׸���,
											 // ��� ��ƼŬ�� �׸� ������ �� ������ �ݺ�.

		int                     maxParticles; // �־��� �ð� ���� �ý����� ���� �� �ִ� �ִ� ��ƼŬ�� ��.
											   // ��, ��ƼŬ�� �ı��Ǵ� �ӵ����� ��������� �ӵ��� �����ٸ� 
											   // ��û���� ���� ���� ��ƼŬ�� ���ÿ� ������ �� �ִ�.
											   // �� ����� �̿� ���� ��Ȳ�� ���´�.

		int vbSize;      // ���ؽ� ���۰� ������ �� �ִ� ��ƼŬ�� ��. 
							// �� ���� ���� ��ƼŬ �ý��� ���� ��ƼŬ ���ʹ� ������.
		int vbOffset;    // ��ƼŬ �ý����� �������� �̿�.
		int vbBatchSize; // ��ƼŬ �ý����� �������� �̿�.
	};

	// ��
	class SnowParticle : public ParticleSystem
	{
	public:
		// ������
		// ��� ���� ����ü�� ����Ű�� �����Ϳ� �ý��� �� ��ƼŬ�� ��
		// ��� ���ڴ� �����̰� ������ ���Ǹ� �����ϸ�, 
		// ���� �����̰� �� ���� ������ ���� ���� ��� �Ҹ��ϰ� �ٽ� �������.
		// �׻� ���� ���� ��ƼŬ�� ����.
		SnowParticle();
		void Init(GameObject* _owner, BoundingBox* _boundingBox, int _numParticles);
		void SetParticleSize(int _particleSize);
		void ResetParticle(Attribute* _attribute);
		void Update(double _timeDelta);
	};

	class FireworkParticle : public ParticleSystem
	{
	public:
		FireworkParticle();
		void Init(GameObject* _owner, int _numParticles, float _size);
		void ResetParticle(Attribute* attribute);
		void Update(double _timeDelta);
		void PreRender();
		void PostRender();
	};

	float GetRandomFloat(float lowBound, float highBound);

	void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);
}