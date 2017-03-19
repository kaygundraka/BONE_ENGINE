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

	////파티클의 위치와 컬러를 표현하는 데의 구조체 포맷.
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
		// 파티클과 파티클의 속성들. 

		// 파티클을 렌더링하는데 필요한 데이터와 파티클 속성을 별도의 구조체 내에 보관하게 되는데,
		// 파티클을 만들고 파괴하고 갱신할 때는 바로 이러한 속성을 이용해 작업하는 것이며,
		// 이를 렌더링할 준비가 완료되면 Particle 구조체로 위치와 속성을 복사해 이용.
		Vec3 position;     // 월드 스페이스 내의 파티클 위치.
		Vec3 velocity;     // 파티클의 속도, 보통은 초당 이동 단위로 기록.
		Vec3 acceleration; // 파티클의 가속. 보통은 초당 이동 단위로 기록.
		float       lifeTime;     // 파티클이 소멸할 때까지 유지되는 시간.
								   // 예, 레이저 빔 파티클은 특정한 시간동안만 유지.
		float       age;          // 파티클의 현재 나이
		D3DXCOLOR   color;        // 파티클의 컬러
		Vec3   colorFade;    // 파티클의 컬러가 시간의 흐름에 따라 퇴색하는 방법
		bool        isAlive;    // 파티클이 생존한 경우 true, 소멸한 경우 false
	};

	// 파티클 시스템의 요소들.

	// 파티클 시스템은 파티클들의 모임이며 파티클을 보여주고 관리하는 역할을 담당한다.
	// 파티클의 크기나 파티클 원천의 위치, 파티클에 적용할 텍스처 등 
	// 시스템 내의 모든 파티클에 영향을 주는 전역 특성들을 관리.
	// 기능적 관점에서 보면
	// 파티클 갱신과 디스플레이, 소멸, 생성 등을 관장하는 역할.
	class ParticleSystem : public Component
	{
	public:
		// 디폴트 값을 초기화하는 생성자와 
		// 장치 인터페이스 (버텍스 버퍼, 텍스처)를 해제하는 디스트럭터
		ParticleSystem();
		virtual ~ParticleSystem();

		// init - 포인트 스프라이트를 저장하기 위한 버텍스 버퍼를 만들고
		// 텍스처를 만드는 등의 Direct3D의 장치 의존적인 초기화 작업을 처리.
		virtual void LoadContent();

		// 시스템 내의 모든 파티클 속성을 리셋.
		virtual void Reset();

		virtual void ResetParticle(Attribute* attribute) = 0;

		void SetTextreAddress(std::string _address);

		// 시스템에 파티클을 추가.
		virtual void AddParticle();

		// 시스템 내의 모든 파티클들을 갱신.
		virtual void Update(double _TimeDelta) = 0;

		// 렌더링에 앞서 지정해야 할 초기 렌더 상태를 지정.
		// 이 메서드는 시스템에 따라 달라질 수 있으므로 가상 함수로 선언.
		virtual void PreRender();

		// 시스템 내의 모든 파티클들을 렌더링
		virtual void Render();

		// 특정 파티클 시스템이 지정했을 수 있는 렌더 상태를 복구하는 데 이용.
		// 이 메서드는 시스템에 따라 달라질 수 있으므로 가상 메서드로 선언.
		virtual void PostRender();

		// 현재 시스템에 파티클이 없는 경우 true 리턴.
		bool IsEmpty();

		// 시스템 내의 파티클이 모두 죽은 경우 true 리턴.
		bool IsDead();

	protected:
		// 속성 리스트 _particle을 검색하여 죽은 파티클을 리스트에서 제거.
		virtual void RemoveDeadParticles();

	protected:
		std::string textureAddress;
		Transform3D*       transform3D;  // 시스템의 원천, 시스템 내에서 파티클이 시작되는 곳.
		BoundingBox        boundingBox;  // 파티클이 이동할 수 있는 부피를 제한하는 데 이용. 
											   // 예, 산 정상을 둘러싼 지역에만 눈이 오는 시스템.
											   // 원하는 영역을 경계상자로 정의하면 이 영역을 벗어난 파티클들을 곧바로 소멸.
		float                   emitRate;   // 시스템에 새로운 파티클이 추가되는 비율. 보통은 초당 파티클 수로 기록.
		float                   size;       // 시스템 내 모든 파티클의 크기
		IDirect3DVertexBuffer9* vb;
		std::list<Attribute>    particles;  // 시스템 내 파티클 속성의 리스트.
											 // 우리는 파티클을 만들고 제거하고 갱신하는 데 이 리스트를 이용.
											 // 파티클을 그릴 준비가 완료되면 
											 // 리스트 노드의 일부를 버텍스 버퍼로 복사하고
											 // 파티클 드로잉 과정을 거친다.
											 // 이어 다음 단계의 데이터를 복사하고 다시 파티클을 그리며,
											 // 모든 파티클을 그릴 때까지 이 과정을 반복.

		int                     maxParticles; // 주어진 시간 동안 시스템이 가질 수 있는 최대 파티클의 수.
											   // 예, 파티클이 파괴되는 속도보다 만들어지는 속도가 빠르다면 
											   // 엄청나게 많은 수의 파티클이 동시에 존재할 수 있다.
											   // 이 멤버는 이와 같은 상황을 막는다.

		int vbSize;      // 버텍스 버퍼가 보관할 수 있는 파티클의 수. 
							// 이 값은 실제 파티클 시스템 내의 파티클 수와는 독립적.
		int vbOffset;    // 파티클 시스템의 렌더링에 이용.
		int vbBatchSize; // 파티클 시스템의 렌더링에 이용.
	};

	// 눈
	class SnowParticle : public ParticleSystem
	{
	public:
		// 생성자
		// 경계 상자 구조체를 가리키는 포인터와 시스템 내 파티클의 수
		// 경계 상자는 눈송이가 떨어질 부피를 정의하며, 
		// 만약 눈송이가 이 범위 밖으로 벗어 나면 즉시 소멸하고 다시 만들어짐.
		// 항상 같은 수의 파티클을 유지.
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