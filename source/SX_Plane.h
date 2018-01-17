#ifndef SX_PLANE_H
#define SX_PLANE_H

#include "c4d.h"
#include <vector>

class SX_Plane
{
public:
	SX_Plane();
	SX_Plane(Int32, std::vector<Vector>&);

	bool operator==(const SX_Plane&) const;

	Int32 id;
	Float64 d;
	Vector x, y, z, normal;

	Bool IsDisp;

	// Disp Section
	Int32 power;
	Vector startposition;
	Float64 elevation;
	std::vector<std::vector<Vector>> normals;
	std::vector<std::vector<Float32>> distances;
	std::vector<std::vector<Vector>> offsets;
	std::vector<std::vector<Vector>> offset_normals;
	std::vector<std::vector<Float32>> alphas;

};

#endif