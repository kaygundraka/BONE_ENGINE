#pragma once
#include "MultiThreadSync.h"

namespace BONE_SYSTEM
{
	template <class T, int ALLOC_BLOCK_SIZE = 50>
	class MemoryPool : public MultiThreadSync<T>
	{
	public:
		static VOID* operator new(std::size_t allocLength)
		{
			ThreadSync Sync;

			assert(sizeof(T) == allocLength);
			assert(sizeof(T) >= sizeof(UCHAR*));

			if (!freePointer)
				allocBlock();

			UCHAR *ReturnPointer = freePointer;
			freePointer = *reinterpret_cast<UCHAR**>(ReturnPointer);

			return ReturnPointer;
		}

		static VOID operator delete(VOID* deletePointer)
		{
            ThreadSync Sync;

			*reinterpret_cast<UCHAR**>(deletePointer) = freePointer;

			freePointer = static_cast<UCHAR*>(deletePointer);
		}

	private:
		static VOID allocBlock()
		{
			freePointer = new UCHAR[sizeof(T) * ALLOC_BLOCK_SIZE];

			UCHAR **Current = reinterpret_cast<UCHAR**>(freePointer);

			UCHAR *Next = freePointer;

			for (INT i = 0; i < ALLOC_BLOCK_SIZE - 1; i++)
			{
				Next += sizeof(T);
				*Current = Next;
				Current = reinterpret_cast<UCHAR**>(Next);
			}

			*Current = 0;
		}

	private:
		static UCHAR *freePointer;

	protected:
		~MemoryPool()
		{
		}
	};

	template <class T, int ALLOC_BLOCK_SIZE>
	UCHAR* MemoryPool<T, ALLOC_BLOCK_SIZE>::freePointer;
}