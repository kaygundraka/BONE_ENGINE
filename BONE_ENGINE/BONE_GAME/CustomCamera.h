#pragma once
#include <Common.h>
using namespace BONE_GRAPHICS;

class CustomCamera
{
public:
    CustomCamera();
    ~CustomCamera();
    void RotateCamera(double Angle, double x, double y, double z);
    void updateOrientation(float elapsedTimeSec);
    void updateViewMatrix();

    bool m_enableSpringSystem;
    float m_springConstant;
    float m_dampingConstant;
    float m_offsetDistance;
    float m_headingDegrees;
    float m_pitchDegrees;
    float m_fovx;
    float m_znear;
    float m_zfar;
    D3DXVECTOR3 m_eye;
    D3DXVECTOR3 m_target;
    D3DXVECTOR3 m_targetYAxis;
    D3DXVECTOR3 m_xAxis;
    D3DXVECTOR3 m_yAxis;
    D3DXVECTOR3 m_zAxis;
    D3DXVECTOR3 m_velocity;
    D3DXMATRIX m_projMatrix;
    D3DXVECTOR3 View;
    D3DXMATRIX viewMatrix;
    D3DXQUATERNION orientation;
};
