#pragma once
#include <GameObject.h>
#include <Transform3D.h>
#include <StaticMesh.h>
using namespace BONE_GRAPHICS;

class TestMesh : public GameObject {
private:
    Transform3D* transform;

public:
    virtual void Init();
};