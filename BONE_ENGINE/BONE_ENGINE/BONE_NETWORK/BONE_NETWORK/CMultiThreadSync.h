#pragma once
#include "Common.h"

namespace BONE_NETWORK
{
	class CCriticalSection
	{
	private:
		CRITICAL_SECTION sync;

	public:
		CCriticalSection(VOID)
		{
			InitializeCriticalSection(&sync);
		}

		~CCriticalSection(VOID)
		{
			DeleteCriticalSection(&sync);
		}

		inline VOID Enter(VOID)
		{
			EnterCriticalSection(&sync);
		}

		inline VOID Leave(VOID)
		{
			LeaveCriticalSection(&sync);
		}
	};

	template <class T>
	class CMultiThreadSync {
		friend class CThreadSync;

	public:
		
		class CThreadSync
		{
		public:
			CThreadSync(VOID)
			{
				T::sync.Enter();
			}

			~CThreadSync(VOID)
			{
				T::sync.Leave();
			}
		};

	private:
		static CCriticalSection sync;
	};

	template<class T>
	CCriticalSection CMultiThreadSync<T>::sync;
}

