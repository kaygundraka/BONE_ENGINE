#include "TestMesh.h"
#include <PhongShader.h>
#include <Material.h>
#include <PointLight.h>

void TestMesh::Init()
{
    SetPriority(1);
    SetTag("TestMesh");

    Transform3D* tr = new Transform3D();
    tr->SetPosition(Vector3(0, 0, 0));
    this->AddComponent(tr);
    transform = ((Transform3D*)GetComponent("Transform3D"));

    StaticMesh* sm = new StaticMesh();
    sm->SetFileAddress("TestMesh.X");
    sm->SetRenderMode(StaticMesh::RENDER_DEFAULT);
    this->AddComponent(sm);

    Material* material = new Material();
    material->SetAmbient(1, 1, 1, 1);
    material->SetDiffuse(1, 1, 1, 1);
    material->SetEmissive(0.2f, 0.2f, 0.2f, 0.2f);
    material->SetSpecular(0.2f, 0.2f, 0.2f, 0.2f);
    material->SetShininess(0.3f);
    this->AddComponent(material);

    PointLight* Light1 = new PointLight();
    Light1->SetAmbient(1, 1, 1, 1);
    Light1->SetDiffuse(1, 1, 1, 1);
    Light1->SetSpecular(0.2f, 0.2f, 0.2f, 0.2f);
    Light1->SetLight(true);
    Light1->SetRadius(50);
    Light1->SetPosition(Vector3(0, 30, 0), this);
    this->AddComponent(Light1);
}