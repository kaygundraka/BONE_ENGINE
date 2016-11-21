#include "Common.h"
#include "Vertex.h"
const DWORD VERTEX::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

void SetVtx3DTexture(VERTEX &ct, D3DXVECTOR3 p, D3DXVECTOR3 n, float tu, float tv)
{
	ct.p.x = p.x; ct.p.y = p.y; ct.p.z = p.z;
	ct.n.x = n.x; ct.n.y = n.y; ct.n.z = n.z;
	ct.uv.x = tu;  ct.uv.y = tv;
}

void SetIdx(VERTEX_INDEX &ci, WORD _0, WORD _1, WORD _2)
{
	ci._0 = _0; ci._1 = _1;	ci._2 = _2;
}

float CalculateDistance(D3DXVECTOR3 a, D3DXVECTOR3 b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) *  (a.z - b.z));
}