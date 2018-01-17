#include "SX_World.h"

// Default CTOR
SX_World::SX_World()
{
	Construct();
}

// CTOR w/ params, pass world class
SX_World::SX_World(std::vector<String> *data, int start, int end)
{
	Construct();

	// Iterate through each line
	for (int i = start + 1; i < end; i++)
	{
		// Status
		float fProgress = (i * 1.0) / end;
		Int32 sProgress = Int32(fProgress * 100);
		StatusSetBar(sProgress);
		String sI = String::IntToString(i);
		String sSize = String::IntToString(end);
		StatusSetText(GeLoadString(IDS_PROCESSING_WORLD) + sI + GeLoadString(IDS_OF) + sSize);

		String line = (*data)[i];
		std::vector<String> vLine = ParseProperties(line);

		for (int j = 0; j < vLine.size(); j++)
		{
			if (vLine[j] == "id" && id == -1) { this->id = (vLine[j + 1]).ParseToInt32(); j++; continue; }
			if (vLine[j] == "solid") { m_Solids.push_back(ParseSolid(data, i)); }
			if (vLine[j] == "skyname") { this->SkyName = vLine[j + 1]; j++; continue; }
		}
	}

	StatusClear();
}

// Creates solid and returns it, increases iterator it.
SX_Solid& SX_World::ParseSolid(std::vector<String> *data, int &it)
{
	it += 2; // Jump to start of data.
	Int32 ClassDepth = 1;
	Int32 S_ID = -1;
	std::vector<SX_Plane> Planes;
	Bool disp = false;

	for (it = it; ClassDepth > 0; it++)
	{
		std::vector<String> vLine = ParseProperties((*data)[it]);
		
		for (int j = 0; j < vLine.size(); j++)
		{
			if (vLine[j] == "{") { ClassDepth++; continue; }
			if (vLine[j] == "}") { ClassDepth--; continue; }
			if (vLine[j] == "id") { S_ID = (vLine[j + 1]).ParseToInt32(); j++; continue; }
			if (vLine[j] == "side")
			{
				SX_Plane newPlane = ParseSide(data, it);
				if (newPlane.normal.GetLength() != 0) // bad plane otherwise
				{
					Planes.push_back(newPlane);
					if (newPlane.IsDisp == true)
						disp = true;
				}
			}
		}
	} it--;

	SX_Solid *rval = new SX_Solid(S_ID, Planes);
	if (disp == true)
		rval->HasDisp = true;

	return *rval;
}

// Creates Polygon and returns it, increases iterator it.
SX_Plane& SX_World::ParseSide(std::vector<String>* data, int& it)
{
	it += 2; // Jump to start of data
	Int32 P_ID = -1;
	SX_Plane *Plane;
	Bool disp = false;

	Int32 ClassDepth = 1;

	for (it = it; ClassDepth > 0; it++)
	{
		std::vector<String> vLine = ParseProperties((*data)[it]);
		for (int j = 0; j < vLine.size(); j++)
		{
			if (vLine[j] == "{") { ClassDepth++; continue; }
			if (vLine[j] == "}") { ClassDepth--; continue; }
			if (vLine[j] == "id") { P_ID = (vLine[j + 1]).ParseToInt32(); j++; continue; }
			if (vLine[j] == "plane")
			{
				std::vector<Vector> Vec = ParseVector3(vLine[j + 1]);
				Plane = new SX_Plane(P_ID, Vec);
				j++; continue;
			}
			if (vLine[j] == "dispinfo") { disp = true; }
			if (vLine[j] == "power" && disp) { Plane->power = (vLine[j + 1]).ParseToInt32(); j++; continue; }
			if (vLine[j] == "startposition" && disp) { Plane->startposition = ParseVector(vLine[j + 1], 1, vLine[j + 1].GetLength() - 2); continue; }
			if (vLine[j] == "elevation" && disp) { Plane->elevation = (vLine[j + 1]).ParseToFloat(); j++; continue; }
			if (vLine[j] == "normals" && disp)
			{
				int k = 0;
				for (int h = it + 2; k < Pow(2.0, Plane->power) + 1; h++, k++)
				{
					std::vector<String> hLine = ParseProperties((*data)[h]);
					Plane->normals.push_back(ParseVectorArray(hLine[1]));
				}
				continue;
			}
			if (vLine[j] == "distances" && disp)
			{
				int k = 0;
				for (int h = it + 2; k < Pow(2.0, Plane->power) + 1; h++, k++)
				{
					std::vector<String> hLine = ParseProperties((*data)[h]);
					Plane->distances.push_back(ParseFloatArray(hLine[1]));
				}
				continue;
			}
			if (vLine[j] == "offsets" && disp)
			{
				int k = 0;
				for (int h = it + 2; k < Pow(2.0, Plane->power) + 1; h++, k++)
				{
					std::vector<String> hLine = ParseProperties((*data)[h]);
					Plane->offsets.push_back(ParseVectorArray(hLine[1]));
				}
				continue;
			}
			if (vLine[j] == "offset_normals" && disp)
			{
				int k = 0;
				for (int h = it + 2; k < Pow(2.0, Plane->power) + 1; h++, k++)
				{
					std::vector<String> hLine = ParseProperties((*data)[h]);
					Plane->offset_normals.push_back(ParseVectorArray(hLine[1]));
				}
				continue;
			}
			if (vLine[j] == "alphas" && disp)
			{
				int k = 0;
				for (int h = it + 2; k < Pow(2.0, Plane->power) + 1; h++, k++)
				{
					std::vector<String> hLine = ParseProperties((*data)[h]);
					Plane->alphas.push_back(ParseFloatArray(hLine[1]));
				}
				continue;
			}
		}
	} it--;

	if (disp == true)
		Plane->IsDisp = true;

	return *Plane;

}

void SX_World::Construct()
{
	id = -1;
}