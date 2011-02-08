#ifndef _PXF_RENDERER_CAMERA_H_
#define _PXF_RENDERER_CAMERA_H_

#include <Pxf/Math/Quaternion.h>

class Camera
{
protected:
	Pxf::Math::Mat4 m_Model;
	Pxf::Math::Mat4 m_Projection;

	/*
	static const Math::Vec3f WORLD_XAXIS;
	static const Math::Vec3f WORLD_YAXIS;
	static const Math::Vec3f WORLD_ZAXIS; */
public:
	Camera()
	{
		m_Model = Pxf::Math::Mat4::Identity;
		m_Projection = Pxf::Math::Mat4::Identity;
	}
	virtual ~Camera() {}

	Pxf::Math::Mat4* GetModelView() { return &m_Model; }
	Pxf::Math::Mat4* GetProjectionView() { return &m_Projection; }
};

class SimpleCamera : public Camera
{
private:
	Pxf::Math::Quaternion m_Orientation;
	Pxf::Math::Vec3f m_Eye;
	Pxf::Math::Vec3f m_ViewDir;

	Pxf::Math::Vec3f m_XAxis;
	Pxf::Math::Vec3f m_YAxis;
	Pxf::Math::Vec3f m_ZAxis;

	float m_PitchDegrees;
	float m_FovX;
	float m_AspectRatio;
	float m_ZNear;
	float m_ZFar;

public:
	SimpleCamera()
		: Camera()
		, m_Eye(0.0f,0.0f,0.0f)
		, m_ViewDir(0.f, 0.f, -1.f)
		, m_XAxis(1.0f,0.0f,0.0f)
		, m_YAxis(0.0f,1.0f,0.0f)
		, m_ZAxis(0.0f,0.0f,1.0f)
		, m_PitchDegrees(0.0f)
	{ }

	void UpdateModelViewMtx();

	const Pxf::Math::Quaternion* GetOrientation() const { return &m_Orientation; }
	const Pxf::Math::Vec3f* GetPos() const { return &m_Eye; }
	const Pxf::Math::Vec3f* GetDir() const { return &m_ViewDir; }

	void SetPerspective(float _fovx, float _aspect, float _znear, float _zfar);
	void SetRotation(float yaw, float pitch, float roll);
	void SetLookAt(float x,float y,float z);
	void SetPosition(float x,float y,float z);
	void SetPosition(const Pxf::Math::Vec3f &_Pos);
};

#endif // _PXF_GRAPHICS_CAMERA_H_
