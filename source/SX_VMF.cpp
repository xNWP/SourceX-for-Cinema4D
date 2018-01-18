#include "SX_VMF.h"

std::vector<String>& ParseProperties(const String &line)
{
	/*
	Splits line into component parts,
	some stuff -> [some, stuff]
	"some stuff" "and more" -> [some stuff, and more]
	*/
	std::vector<String>* rval = new std::vector<String>();
	Int32 *pSt = new Int32();
	Int32 *pEn = new Int32();
	Int32 *cSt = new Int32();
	Int32 *pTs = new Int32();
	Int32 pos = 0;

	// Remove Tab Spaces
	String nline = line;
	while (nline.FindFirst('\t', pTs, 0))
		nline = nline.SubStr(*pTs + 1, line.GetLength() - *pTs - 1);

	while (pos < nline.GetLength())
	{
		nline.FindFirst('\"', pSt, pos);
		nline.FindFirst(' ', cSt, pos);

		if (*pSt == -1 && *cSt == -1) { rval->push_back(nline.SubStr(pos, nline.GetLength() - pos)); break; }

		if (*pSt == -1 || (*pSt > *cSt) && *cSt != -1)
		{
			if ((*cSt - pos - 1) > 0)
				rval->push_back(nline.SubStr(pos, *cSt - pos - 1));

			pos += *cSt - pos + 1;
			
			if (pSt != nullptr) *pSt = -1;
			if (pEn != nullptr) *pEn = -1;
			if (cSt != nullptr) *cSt = -1;

			continue;
		}

		if (*cSt == -1 || (*cSt > *pSt) && *pSt != -1)
		{
			nline.FindFirst('\"', pEn, *pSt + 1);
			if (*pEn == -1){ rval->push_back(nline.SubStr(*pSt + 1, nline.GetLength() - *pSt)); break; }
			rval->push_back(nline.SubStr(*pSt + 1, *pEn - *pSt - 1));
			pos = *pEn + 1;
			
			if (pSt != nullptr) *pSt = -1;
			if (pEn != nullptr) *pEn = -1;
			if (cSt != nullptr) *cSt = -1;

			continue;
		}
	}
	// Cleanup any ptrs
	if (pSt != nullptr) delete pSt;
	if (pEn != nullptr) delete pEn;
	if (cSt != nullptr) delete cSt;

	return *rval;
}

std::vector<Vector>& ParseVector3(const String &vector)
{
	std::vector<Vector> *rval = new std::vector<Vector>();

	Int32 *vSt = new Int32();
	Int32 *vEn = new Int32();
	Int32 *space = new Int32();
	Int32 pos = 0;

	while (pos < vector.GetLength() - 2)
	{
		vector.FindFirst('(', vSt, pos);
		vector.FindFirst(')', vEn, *vSt);

		String sVec = vector.SubStr(*vSt + 1, *vEn - *vSt - 1);
		std::vector<Float> fVec;

		for (int i = 0; i < sVec.GetLength(); i++)
		{
			sVec.FindFirst(' ', space, i);
			if (*space == -1)
			{
				fVec.push_back((sVec.SubStr(i, sVec.GetLength() - i)).ParseToFloat());
				break;
			}

			fVec.push_back((sVec.SubStr(i, *space - i)).ParseToFloat());
			i = *space;
		}

		rval->push_back(Vector(fVec[0], fVec[1], fVec[2]));
		pos += sVec.GetLength() + 2; // +2 for the brackets we cut off
	}

	// free up memory
	if (vSt != nullptr) delete vSt;
	if (vEn != nullptr) delete vEn;
	if (space != nullptr) delete space;

	return *rval;
}

// Take's 3 values in a string seperated by spaces, returns them as one vector.
Vector& ParseVector(const String &string, Int32 start, Int32 end)
{
	Vector *rval = new Vector;
	Int32 it = 0;
	Int32 size = 0;
	Int32 subStrStart = start;
	for (Int32 i = start; i <= end; i++)
	{
		if (string[i] == ' ' || i == end) // hit space/end of string, do parse now.
		{
			if (i == end)
				size++;

			String substr = string.SubStr(subStrStart, size);
			(*rval)[it] = substr.ParseToFloat();
			it++;
			subStrStart = subStrStart + size + 1;
			size = 0;
			continue;
		}
		size++;
	}

	return *rval;
}

// Take's string of vectors seperated by spaces, returns vector of each 3 items.
std::vector<Vector>& ParseVectorArray(const String &vectorArray)
{
	std::vector<Vector> *rval = new std::vector<Vector>;
	
	// Each vector will have 3 spaces excepts last vector.
	Int32 Spaces = 0;
	Int32 start = 0;
	for (Int32 i = start; i < vectorArray.GetLength(); i++)
	{
		if (vectorArray[i] == ' ') // hit space
		{
			Spaces++;
		}
		if (Spaces == 3 || (Spaces == 2 && i == (vectorArray.GetLength() - 1))) // got a full vector
		{
			rval->push_back(ParseVector(vectorArray, start, i));
			Spaces = 0; start = i + 1;
		}
	}

	return *rval;
}

// Take's string of floats seperated by spaces, returns vector of each float.
std::vector<Float32>& ParseFloatArray(const String &FloatArray)
{
	std::vector<Float32> *rval = new std::vector<Float32>;

	Int32 Start = 0;
	for (Int32 i = 0; i < FloatArray.GetLength(); i++)
	{
		if (FloatArray[i] == ' ' || (i == FloatArray.GetLength() - 1)) // hit space/end
		{
			String subStr = FloatArray.SubStr(Start, i - Start);
			rval->push_back(subStr.ParseToFloat());
			Start = i + 1;
		}
	}

	return *rval;
}

// splits data to an array of lines
std::vector<String>& ParseLines(const String & data)
{
	std::vector<String> *rval = new std::vector<String>();
	Int32 *it = new Int32();
	Int32 pos = 0;

	while (pos < data.GetLength())
	{
		if (data.FindFirst("\r\n", it, pos))
		{
			rval->push_back(data.SubStr(pos, *it - pos));
			pos = *it + 2;
		}
		else
		{
			rval->push_back(data.SubStr(pos, data.GetLength() - pos));
			break;
		}
	}
	
	// Free resources
	if (it != nullptr) delete it;

	return *rval;
}

// Returns the start and end values for a class in data.
// i.e. myClass	<-- start
//		{		
//			data;
//			moreData;
//		}		<-- end
void ClassBounds(std::vector<String> &data, String searchclass, int &start, int &end)
{
	start = end = -1;
	int classdepth = 0;

	for (int i = 0; i < data.size(); i++)
	{
		if (data[i] == searchclass && classdepth == 0) { start = i + 1; }
		if (data[i] == "}" && classdepth == 1 && start != -1) { end = i; return; }
		if (data[i] == "{") { classdepth++; }
		if (data[i] == "}") { classdepth--; }
	}
}

// Takes a solid, fills each plane with points needed to make that polygon.
void GetVertices(SX_Solid &solid, const Int32 &Iepsilon)
{
	std::vector<Vector> ValidVertices;
	
	Float64 epsilon = Pow(10.0, Iepsilon);

	GePrint("[SX] Received Search Epsilon: " + String::IntToString(Iepsilon));

	// Create List of all valid vertices
	for (int i = 0; i < solid.Planes.size(); i++)
	{
		// check that the vertex isn't already in the list, then add
		if (!VectorInList(solid.Planes[i].x, ValidVertices)) ValidVertices.push_back(solid.Planes[i].x);
		if (!VectorInList(solid.Planes[i].y, ValidVertices)) ValidVertices.push_back(solid.Planes[i].y);
		if (!VectorInList(solid.Planes[i].z, ValidVertices)) ValidVertices.push_back(solid.Planes[i].z);
	}

	// Check each plane to see if they contain valid vertices
	for (int i = 0; i < solid.Planes.size(); i++)
	{
		if (solid.HasDisp && !(solid.Planes[i].IsDisp))
			continue; // no need to do calculations on non-drawn poly's

		for (int j = 0; j < ValidVertices.size(); j++)
		{
			Vector64 vec = ValidVertices[j] - solid.Planes[i].x;
			Float64 evald = Dot(vec, solid.Planes[i].normal);
			if ((evald < epsilon) && (evald > -epsilon)) // I'm using an epsilon here to help mitigate rounding errors.
			{
				solid.Planes[i].vertices.push_back(ValidVertices[j]); // Found a point on the plane
			}
		}

		// Order list by angle to avoid lines intersecting
		if (solid.Planes[i].vertices.size() > 2) // bad poly otherwise!
		{
			OrderByAngle(solid.Planes[i]);
		}
	}
}

Bool VectorInList(const Vector &vec, const std::vector<Vector> &list)
{
	// Checks if a given vector is in the list
	for (int i = 0; i < list.size(); i++)
	{
		if (list[i] == vec)
			return true;
	}
	return false;
}

// Returns the distance between two vectors
Float32 Distance(const Vector &v1, const Vector &v2)
{
	return sqrt(Pow(v1.x - v2.x, 2) + Pow(v1.y - v2.y, 2) + Pow(v1.z - v2.z, 2));
}

// Organizes a polygons vertices by angle about center point
void OrderByAngle(SX_Plane &plane)
{
	// TO-DO: Order by distance secondarily :ssss
	Vector C(0, 0, 0);

	// get the center point
	for (int i = 0; i < plane.vertices.size(); i++)
	{
		C.x += plane.vertices[i].x;
		C.y += plane.vertices[i].y;
		C.z += plane.vertices[i].z;
	}
	C = C / plane.vertices.size();

	std::vector<Vector> nPoints; // move the plane to the origin
	for (int i = 0; i < plane.vertices.size(); i++)
	{
		Vector nVec(plane.vertices[i].x - C.x, plane.vertices[i].y - C.y, plane.vertices[i].z - C.z);
		nPoints.push_back(nVec);
	}

	// calculate rotation matrix to sit plane in xy plane
	Vector u = Vector(plane.normal.y, -plane.normal.x, 0) / plane.normal.GetLength(); // axis to rotate along
	Float64 theta = ACos(plane.normal.z / plane.normal.GetLength()); // Angle to rotate

	// create the matrix
	Vector off(0, 0, 0);
	Vector v1(Cos(theta) + Pow(u.x, 2)*(1 - Cos(theta)),
		u.x*u.y*(1 - Cos(theta)), -u.y*Sin(theta));
	Vector v2(u.x*u.y*(1 - Cos(theta)),
		Cos(theta) + Pow(u.y, 2)*(1 - Cos(theta)), u.x*Sin(theta));
	Vector v3(u.y*Sin(theta), -u.x*Sin(theta), Cos(theta));
	Matrix rMat(off, v1, v2, v3);

	// rotate all the points
	for (int i = 0; i < nPoints.size(); i++)
		nPoints[i] = rMat * nPoints[i];

	std::vector<Float64> angles;

	// get the angle for each point
	for (int i = 0; i < nPoints.size(); i++)
		angles.push_back(ATan2(nPoints[i].y, nPoints[i].x));

	// load up map with angles and points
	std::multimap<Float64, Vector> AnglePoints;
	for (int i = 0; i < plane.vertices.size(); i++)
		AnglePoints.emplace(angles[i], plane.vertices[i]); // this will auto-sort

	// push ordered values into new vector
	std::vector<Vector> newVerts;
	for (std::map<Float64, Vector>::iterator i = AnglePoints.begin(); i != AnglePoints.end(); i++)
		newVerts.push_back(i->second);

	plane.vertices = newVerts;
}

// Lists points with identical positions in 3d space
// indices		indices of points
// points		list of points
// returns		list of list of equal points
std::vector<std::vector<Int32>>& FindLikePoints(const std::vector<Int32> &indices, const std::vector<Vector> &points)
{
	std::vector<std::vector<Int32>> *rval = new std::vector<std::vector<Int32>>;
	std::vector<Bool> tagged(points.size(), false);

	// for each point
	for (int i = 0; i < points.size(); i++)
	{
		std::vector<Int32> matches;
		if (tagged[i] == true)
			continue; // skip
		// for every other point
		for (int j = i + 1; j < points.size(); j++)
		{
			if (tagged[j] == true)
				continue; // skip

			if (points[i] == points[j])
			{
				if (tagged[i] == false)
				{
					tagged[i] = true;
					matches.push_back(-i - 1);
				}
				tagged[j] = true;
				matches.push_back(-j - 1);
			}
		}
		if(matches.size() > 0)
			rval->push_back(matches);
		matches.clear();
	}

	return *rval;
}
