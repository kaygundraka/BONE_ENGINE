#pragma once
#include "Common.h"
#include "GameObject.h"
#include "SceneManager.h"
#include <thread>

namespace BONE_GRAPHICS
{
	typedef struct _IEnumulator {
		enum TYPE { TIME, FRAME, NONE, FINISH };

		TYPE type;
		double time;

	} IEnumulator;

	template <class T>
	class Coroutine {
	public:
		static IEnumulator WaitForSecond(double _time)
		{
			IEnumulator result;

			result.type = IEnumulator::TIME;

			if (_time <= 0)
				_time = 0;
			result.time = _time;

			return result;
		}

		static IEnumulator WaitForFrame()
		{
			IEnumulator result;

			result.type = IEnumulator::FRAME;

			return result;
		}

		static IEnumulator Continue()
		{
			IEnumulator result;

			result.type = IEnumulator::NONE;

			return result;
		}

		static IEnumulator FinishCoroutine()
		{
			IEnumulator result;

			result.type = IEnumulator::FINISH;

			return result;
		}

		VOID StartCoroutine(LPVOID _instance, IEnumulator(T::*coroutine)(VOID))
		{
			thread* coroutineThread = new thread(&Coroutine::CoroutineThread, this, _instance, coroutine);
		}

	private:
		VOID CoroutineThread(LPVOID _instance, IEnumulator(T::*coroutine)(VOID))
		{
			T* instance = (T*)_instance;

			IEnumulator returnValue;
			double lastTime = (double)timeGetTime();

			bool loop = true;

			while (loop)
			{
				IEnumulator returnValue = (instance->*coroutine)();

				if (returnValue.type == IEnumulator::TIME)
					Sleep(returnValue.time * 1000);
					
				if (returnValue.type == IEnumulator::FRAME)
					Sleep((1 / SceneMgr->GetFrame()) * 1000);
					
				if (returnValue.type == IEnumulator::FINISH)
					loop = false;
			}
		}
	};

	//typedef struct _IEnumulator { 
	//	enum TYPE { TIME, FRAME, NONE, FINISH };
	//	
	//	TYPE type;
	//	double time;

	//} IEnumulator;

	//IEnumulator WaitForSecond(double _time);

	//IEnumulator WaitForFrame();

	//IEnumulator Continue();

	//IEnumulator FinishCoroutine();

	//template <class T>
	//VOID CoroutineThread(LPVOID _instance, IEnumulator (T::*coroutine)(VOID) const)
	//{
	//	T* instance = (T*)_instance;

	//	IEnumulator returnValue;
	//	double lastTime = (double)timeGetTime();

	//	bool loop = true;

	//	while (loop)
	//	{
	//		IEnumulator returnValue = (instance->*coroutine)();

	//		switch (returnValue.type) {
	//		case IEnumulator::NONE:
	//			break;

	//		case IEnumulator::TIME:
	//			Sleep(returnValue.time);
	//			break;

	//		case IEnumulator::FRAME:
	//			Sleep(1 / SceneMgr->GetFrame());
	//			break;

	//		case IEnumulator::FINISH:
	//			loop = false;
	//			break;
	//		}
	//	}
	//}

	//template <class T>
	//VOID StartCoroutine(LPVOID _instance, IEnumulator(T::*coroutine)(VOID) const)
	//{
	//	thread coroutineThread(CoroutineThread<T>, _instance, coroutine);
	//}
}