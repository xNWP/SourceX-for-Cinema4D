#include "SX_Map.h"

// Default CTOR
SX_Map::SX_Map()
{
	/* Nothing for now. */
}

// CTOR w/ param for data seperated by line.
SX_Map::SX_Map(std::vector<String> &data)
{
	// Fill Map File Data
	int v_start, v_end;
	ClassBounds(data, "versioninfo", v_start, v_end);

	for (int i = v_start + 1; i < v_end; i++)
	{
		std::vector<String> vLine = ParseProperties(data[i]);
		for (int j = 0; j < vLine.size(); j++)
		{
			if (vLine[j] == "mapversion") { this->MapVersion = (vLine[j + 1]).ParseToInt32(); j++; continue; }
		}
	}

	
	int w_start, w_end;
	ClassBounds(data, "world", w_start, w_end);

	World = new SX_World(&data, w_start, w_end);
}

// DTOR
SX_Map::~SX_Map()
{
	if (World != nullptr) delete World; // free world
}

Bool SX_Map::CreateWorld(BaseDocument *doc, BaseObject *parent, const Int32 &SearchEpsilon)
{
	StopAllThreads();

	// Create Null
	BaseObject *sx_null = BaseObject::Alloc(Onull);
	sx_null->SetName("static");

	doc->InsertObject(sx_null, parent, nullptr);
	
	for (int i = 0; i < World->m_Solids.size(); i++) // for each solid
	{
		std::vector<Int32> index;
		std::vector<Vector> points;

		float fProgress = (i * 1.0) / World->m_Solids.size();
		Int32 sProgress = Int32(fProgress * 100);
		StatusSetBar(sProgress);
		String sI = String::IntToString(i);
		String sSize = String::IntToString(static_cast<int>(World->m_Solids.size()));
		StatusSetText(GeLoadString(IDS_BUILDING_POLY) + sI + GeLoadString(IDS_OF) + sSize);

		PolygonObject *newPoly = PolygonObject::Alloc(0, 0);
		String sName = "static_" + String::IntToString(World->m_Solids[i].id);
		newPoly->SetName(sName);
		doc->InsertObject(newPoly, sx_null, nullptr);

		AutoAlloc<Modeling> mod;
		mod->InitObject(newPoly);

		if (World->m_Solids[i].HasDisp) // Displacement, seperate procedure.
		{
			for (Int32 j = 0; j < World->m_Solids[i].Planes.size(); j++) // For each face
			{
				if (World->m_Solids[i].Planes[j].IsDisp == false) // skip non-disp faces
					continue;

				Int32 RowColCount = Pow(2.0, World->m_Solids[i].Planes[j].power);
				Int32 RowColPointCount = RowColCount + 1;

				Vector SP = World->m_Solids[i].Planes[j].startposition;
				Vector X = World->m_Solids[i].Planes[j].x;
				Vector Y = World->m_Solids[i].Planes[j].y;
				Vector Z = World->m_Solids[i].Planes[j].z;

				for (Int32 R = 0; R < RowColPointCount; R++) // For Each Row
				{
					Vector REndPoint = Z + ((1.0 * R / RowColCount) * (Y - Z));
					Vector RStartPoint = SP + ((1.0 * R / RowColCount) * (X - SP));
					Vector RowTravelVector = REndPoint - RStartPoint;

					Vector R2EndPoint = Z + ((R + 1.0 / RowColCount) * (Y - Z));
					Vector R2StartPoint = SP + ((R + 1.0 / RowColCount) * (X - SP));
					Vector Row2TravelVector = R2EndPoint - R2StartPoint;

					for (Int32 C = 0; C < RowColPointCount; C++) // For Each Col
					{
						Vector CEndPoint = X + ((1.0 * C / RowColCount) * (Y - X));
						Vector CStartPoint = SP + ((1.0 * C / RowColCount) * (Z - SP));
						Vector ColTravelVector = CEndPoint - CStartPoint;

						Vector a = SP + ((1.0 * C / RowColPointCount) * RowTravelVector) + (((R * 1.0) / RowColPointCount) * ColTravelVector);
						Vector b = a + ((1.0 / RowColPointCount) * RowTravelVector);
						Vector c = b + ((1.0 / RowColPointCount) * ColTravelVector);
						Vector d = a + ((1.0 / RowColPointCount) * ColTravelVector);

						if ((R + C) % 2 == 0)
						{
							// Create Poly's a, b, c -- and a, c, d.
							index.push_back(mod->AddPoint(newPoly, a));
							index.push_back(mod->AddPoint(newPoly, b));
							index.push_back(mod->AddPoint(newPoly, c));
							points.push_back(a);
							points.push_back(b);
							points.push_back(c);

							index.push_back(mod->AddPoint(newPoly, a));
							index.push_back(mod->AddPoint(newPoly, c));
							index.push_back(mod->AddPoint(newPoly, d));
							points.push_back(a);
							points.push_back(c);
							points.push_back(d);
						}
						else
						{
							// Create Poly's a, b, d -- and b, c, d.
							index.push_back(mod->AddPoint(newPoly, a));
							index.push_back(mod->AddPoint(newPoly, b));
							index.push_back(mod->AddPoint(newPoly, d));
							points.push_back(a);
							points.push_back(b);
							points.push_back(d);

							index.push_back(mod->AddPoint(newPoly, b));
							index.push_back(mod->AddPoint(newPoly, c));
							index.push_back(mod->AddPoint(newPoly, d));
							points.push_back(b);
							points.push_back(c);
							points.push_back(d);
						}

						Int32 *padr = &index[index.size() - 6];
						mod->CreateNgon(newPoly, padr, 3);
						padr = &index[index.size() - 3];
						mod->CreateNgon(newPoly, padr, 3);
					}
				}
			}
		}
		else
		{
			std::vector<std::vector<Vector>> Polygons = CreatePolyList(World->m_Solids[i], SearchEpsilon);
			for (int j = 0; j < Polygons.size(); j++) // for each face
			{
				int it = 0;
				for (it = 0; it < Polygons[j].size(); it++) // for each point
				{
					index.push_back(mod->AddPoint(newPoly, (Polygons[j])[it]));
					points.push_back((Polygons[j])[it]);
				}
				Int32 *padr = &index[index.size() - it];
				mod->CreateNgon(newPoly, padr, it);
			}
		}

		// weld like points
		std::vector<std::vector<Int32>> LikePoints = FindLikePoints(index, points);
		for (int i = 0; i < LikePoints.size(); i++)
		{
			for (int j = 0; j < (LikePoints[i]).size() - 1; j++)
			{
				mod->WeldPoints(newPoly, (LikePoints[i])[j], (LikePoints[i])[j + 1]);
			}
		}

		mod->Commit(newPoly);
	}	

	StatusClear();
	DrawViews(DRAWFLAGS_FORCEFULLREDRAW);

	return true;
}