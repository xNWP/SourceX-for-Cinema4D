#include "SX_Plane.h"

// default CTOR
SX_Plane::SX_Plane(): id(-1) {}

// CTOR from 3 points in plane
SX_Plane::SX_Plane(Int32 iid, std::vector<Vector> &points) : id(iid), IsDisp(false), power(4), elevation(0.0)
{
	// Create Normal Vector for plane
	Vector XY = points[0] - points[1];
	Vector XZ = points[0] - points[2];

	this->normal = Cross(XY, XZ);
	normal = normal.GetNormalized();

	this->x = points[0];
	this->y = points[1];
	this->z = points[2];

	this->d = -1 * (0 - (normal.x * x.x) - (normal.y * x.y) - (normal.z * x.z));
}

bool SX_Plane::operator==(const SX_Plane &rval) const
{
	return (this->id == rval.id);
}