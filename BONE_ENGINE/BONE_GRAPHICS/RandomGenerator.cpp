#include "Common.h"
#include "randomGenerator.h"

namespace BONE_GRAPHICS
{
    std::random_device randDeivce;
    std::default_random_engine RandomGenerator::engine(randDeivce());

    RandomGenerator::RandomGenerator()
    {
    }

    RandomGenerator::~RandomGenerator()
    {
    }

    int RandomGenerator::GetRandInt(int min, int max)
    {
        std::uniform_int_distribution<int> random(min, max);

        return random(engine);
    }
}