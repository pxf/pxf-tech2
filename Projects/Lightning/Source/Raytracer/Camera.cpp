#include "Camera.h"

using namespace Pxf;
using namespace Math;

void SimpleCamera::SetPerspective(float _fovx, float _aspect, float _znear, float _zfar)
{
	float e = 1.0f / tanf(Deg2Rad(_fovx) / 2.0f);
	float aspect_inv = 1.0f / _aspect;
	float fovy = 2.0f * atanf(aspect_inv / e);
	float scale_x = 1.0f / tanf(0.5f * fovy);
	float scale_y = scale_x / aspect_inv;

	m_Projection.m[0] = scale_x;
	m_Projection.m[5] = scale_y;
	m_Projection.m[10] = (_zfar + _znear) / (_znear - _zfar);
	m_Projection.m[11] = -1.0f;
	m_Projection.m[14] = (2.0f * _zfar * _znear) / (_znear - _zfar);
	m_Projection.m[15] = 0.0f;

	m_FovX = _fovx;
	m_AspectRatio = _aspect;
	m_ZNear = _znear;
	m_ZFar = _zfar;
}

void SimpleCamera::UpdateModelViewMtx()
{
	m_Orientation.CreateMatrix(&m_Model);
	m_XAxis.Set(m_Model.m[0], m_Model.m[4], m_Model.m[ 8]);
	m_YAxis.Set(m_Model.m[1], m_Model.m[5], m_Model.m[ 9]);
	m_ZAxis.Set(m_Model.m[2], m_Model.m[6], m_Model.m[10]);
	m_ViewDir = -m_ZAxis;

	m_Model.m[12] = -Dot(m_XAxis, m_Eye);
	m_Model.m[13] = -Dot(m_YAxis, m_Eye);
	m_Model.m[14] = -Dot(m_ZAxis, m_Eye);
}


Pxf::Math::Vec3f SimpleCamera::GetPosition() { 
	return m_Eye; 
}

void SimpleCamera::SetPosition(float x,float y,float z)
{
	m_Eye.x = x;
	m_Eye.y = y;
	m_Eye.z = z;
	UpdateModelViewMtx();
}

void SimpleCamera::SetLookAt(float x,float y,float z)
{

	Vec3f _Target(x,y,z);
	m_ZAxis = Normalized(m_Eye - _Target);
	m_XAxis = Normalized(Cross(m_YAxis, m_ZAxis));
	m_YAxis = Normalized(Cross(m_ZAxis, m_XAxis));
	m_ViewDir = -m_ZAxis;

	m_Model.m[ 0] = m_XAxis.x;
	m_Model.m[ 4] = m_XAxis.y;
	m_Model.m[ 8] = m_XAxis.z;
	m_Model.m[12] = -Dot(m_XAxis, m_Eye);

	m_Model.m[ 1] = m_YAxis.x;
	m_Model.m[ 5] = m_YAxis.y;
	m_Model.m[ 9] = m_YAxis.z;
	m_Model.m[13] = -Dot(m_YAxis, m_Eye);

	m_Model.m[ 2] = m_ZAxis.x;
	m_Model.m[ 6] = m_ZAxis.y;
	m_Model.m[10] = m_ZAxis.z;	
	m_Model.m[14] = -Dot(m_ZAxis, m_Eye);

	m_Orientation.CreateFromMatrix(&m_Model);
	m_PitchDegrees = Rad2Deg(asinf(m_Model.m[7]));
}

void SimpleCamera::SetPosition(const Vec3f &_Pos)
{
	SetPosition(_Pos.x,_Pos.y,_Pos.z);
}

void SimpleCamera::Translate(float x,float y,float z)
{
	Vec3f p(x,y,z);
	SetPosition(p + m_Eye);
}

void SimpleCamera::Translate(Pxf::Math::Vec3f v)
{
	SetPosition(v + m_Eye);
}

void SimpleCamera::SetRotation(float yaw, float pitch, float roll)
{
	Quaternion q;
	//q.CreateFromEulerAngles(yaw,pitch,roll);
	//m_Orientation *= q;

	Quaternion rot;

	// Rotate camera about the world y axis.
	if (yaw != 0.0f)
	{
		rot.CreateFromAxisAngleDeg(Vec3f(0.0f,1.0f,0.0f), -yaw);
		m_Orientation = rot * m_Orientation;
	}

	// Rotate camera about its local x axis.
	if (pitch != 0.0f)
	{
		rot.CreateFromAxisAngleDeg(Vec3f(1.0f,0.0f,0.0f), -pitch);
		m_Orientation = m_Orientation * rot;
	}

	UpdateModelViewMtx();
}

struct mouse_state 
{
	enum MS {
		NO_STATE,
		LEFT,
		RIGHT,
		LEFT_RIGHT
	};

	Pxf::Math::Vec2i last_pos;
	MS state;
} ms;

void MoveCamera(Camera* cam, Input::InputDevice* inp)
{
	SimpleCamera* c = (SimpleCamera*) cam;
	// Mouse movement:
	Math::Vec2i dpos;
	inp->GetMousePos(&dpos.x,&dpos.y);
	dpos = dpos - ms.last_pos;

	if(inp->IsButtonDown(Pxf::Input::MOUSE_LEFT) && !inp->IsButtonDown(Pxf::Input::MOUSE_RIGHT))
	{
		Vec3f pos = c->GetDir();
		Vec3f pp_vec;
		pp_vec.x = -pos.z;
		pp_vec.z = pos.x;
		pp_vec.y = pos.y;

		pp_vec = (pp_vec * -dpos.x) * 0.2f;

		c->Translate(pp_vec.x,dpos.y*0.2f,pp_vec.z); 

		ms.state = mouse_state::LEFT;
	}
	else if(inp->IsButtonDown(Pxf::Input::MOUSE_RIGHT) && !inp->IsButtonDown(Pxf::Input::MOUSE_LEFT))
	{		
		c->SetRotation(dpos.x * 0.5f,dpos.y * 0.5f,0.0f);
		ms.state = mouse_state::RIGHT;
	}
	else if(inp->IsButtonDown(Pxf::Input::MOUSE_LEFT) && inp->IsButtonDown(Pxf::Input::MOUSE_RIGHT))
	{
		Math::Vec3f ed = c->GetDir();

		c->Translate((ed)*dpos.y);
		ms.state = mouse_state::LEFT_RIGHT;
	}
	else
		ms.state = mouse_state::NO_STATE;

	// update mouse state
	inp->GetMousePos(&ms.last_pos.x,&ms.last_pos.y);
}