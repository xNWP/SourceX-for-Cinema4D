// Defines various functions to aid in parsing vmf files.

#ifndef SX_VMF_H
#define SX_VMF_H

#include "c4d.h"
#include "SX_Solid.h"
#include "SX_Plane.h"
#include "c4d_symbols.h"
#include <vector>
#include <map>

std::vector<String>& ParseProperties(const String&);
std::vector<Vector>& ParseVector3(const String&);
Vector& ParseVector(const String&, Int32, Int32);
std::vector<Vector>& ParseVectorArray(const String&);
std::vector<Float32>& ParseFloatArray(const String&);
std::vector<String>& ParseLines(const String&);
void ClassBounds(std::vector<String>&, String, int&, int&);
void GetVertices(SX_Solid&, const Int32&);
Bool VectorInList(const Vector&, const std::vector<Vector>&);
void OrderByAngle(SX_Plane&);
Float32 Distance(const Vector&, const Vector&);
std::vector<std::vector<Int32>>& FindLikePoints(const std::vector<Int32>&, const std::vector<Vector>&);

#endif