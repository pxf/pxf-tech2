#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <Pxf/Math/Vector.h>

struct material_t
{
	Pxf::Math::Vec3f ambient;
	Pxf::Math::Vec3f diffuse;
	Pxf::Math::Vec3f specular;
	float shininess;
	float reflectiveness;
	Pxf::Math::Vec3f refraction; // refraction for each channel (r,g,b)
};

#endif /* _MATERIAL_H_ */