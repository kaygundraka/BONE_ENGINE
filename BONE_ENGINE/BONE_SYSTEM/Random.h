#pragma once
#include "Common.h"
#include <time.h>

namespace BONE_SYSTEM
{
	class Random {
	private:
		static bool setup;

	public:
		static int GetRange(int min, int max)
		{
			float result = 0;
			
			if (!setup)
			{
				srand((unsigned)time(NULL));
				setup = true;
			}

			do
			{
				result = (rand() % max) + 1;
			} while (result < min);

			return result;
		}
	};

	bool Random::setup = false;
}