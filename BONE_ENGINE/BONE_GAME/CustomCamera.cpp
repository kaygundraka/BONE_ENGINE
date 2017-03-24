#include "CustomCamera.h"

CustomCamera::CustomCamera()
{
}

CustomCamera::~CustomCamera()
{
}

D3DXQUATERNION conjugate(D3DXQUATERNION quat)
{
    quat.x = -quat.x;
    quat.y = -quat.y;
    quat.z = -quat.z;
    return quat;
}

D3DXQUATERNION mult(D3DXQUATERNION A, D3DXQUATERNION B)
{
    D3DXQUATERNION C;

    C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
    C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;
    C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;
    C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;
    return C;
}

void CustomCamera::RotateCamera(double Angle, double x, double y, double z)
{
    D3DXQUATERNION temp, quat_view, result;

    temp.x = x * sin(Angle / 2);
    temp.y = y * sin(Angle / 2);
    temp.z = z * sin(Angle / 2);
    temp.w = cos(Angle / 2);

    quat_view.x = View.x;
    quat_view.y = View.y;
    quat_view.z = View.z;
    quat_view.w = 0;

    //result = mult(mult(temp, quat_view), conjugate(temp));
    result = (temp*quat_view * conjugate(temp));

    View.x = result.x;
    View.y = result.y;
    View.z = result.z;
}

void CustomCamera::updateOrientation(float elapsedTimeSec)
{
    m_pitchDegrees *= elapsedTimeSec;
    m_headingDegrees *= elapsedTimeSec;

    float heading = D3DXToRadian(m_headingDegrees);
    float pitch = D3DXToRadian(m_pitchDegrees);

    D3DXQUATERNION rot;

    if (heading != 0.0f)
    {
        D3DXQuaternionRotationAxis(&rot, &m_targetYAxis, heading);
        D3DXQuaternionMultiply(&orientation, &rot, &orientation);
    }

    if (pitch != 0.0f)
    {
        D3DXQuaternionRotationAxis(&rot, &m_targetYAxis, pitch);
        D3DXQuaternionMultiply(&orientation, &orientation, &rot);
    }
}

void CustomCamera::updateViewMatrix()
{
    D3DXQuaternionNormalize(&orientation, &orientation);
    D3DXMatrixRotationQuaternion(&viewMatrix, &orientation);

    m_xAxis = D3DXVECTOR3(viewMatrix(0, 0), viewMatrix(1, 0), viewMatrix(2, 0));
    m_yAxis = D3DXVECTOR3(viewMatrix(0, 1), viewMatrix(1, 1), viewMatrix(2, 1));
    m_zAxis = D3DXVECTOR3(viewMatrix(0, 2), viewMatrix(1, 2), viewMatrix(2, 2));

    m_eye = m_target + m_zAxis * -m_offsetDistance;

    viewMatrix(3, 0) = -D3DXVec3Dot(&m_xAxis, &m_eye);
    viewMatrix(3, 1) = -D3DXVec3Dot(&m_yAxis, &m_eye);
    viewMatrix(3, 2) = -D3DXVec3Dot(&m_zAxis, &m_eye);
}