#pragma once
#include "Common.h"

namespace BONE_GRAPHICS
{
    class RandomGenerator {
    private:
        static std::default_random_engine engine;

    public:
        RandomGenerator();
        ~RandomGenerator();

        static int GetRandInt(int min, int max);
    };
}