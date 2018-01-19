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
			GetVertices(World->m_Solids[i], SearchEpsilon);
			for (Int32 j = 0; j < World->m_Solids[i].Planes.size(); j++) // For each face
			{
				if (World->m_Solids[i].Planes[j].IsDisp == false) // skip non-disp faces
					continue;

				Int32 RowColCount = Pow(2.0, World->m_Solids[i].Planes[j].power);
				Int32 RowColPointCount = RowColCount + 1;

				// Find start position (bottom left corner)
				Int32 iSP = 0;
				while (iSP < World->m_Solids[i].Planes[j].vertices.size())
				{
					if (World->m_Solids[i].Planes[j].vertices[iSP] == World->m_Solids[i].Planes[j].startposition)
						break;
					else
						iSP++;
				}

				// Modulate around list size negatively (since angles will be ccw)
				Vector SP = World->m_Solids[i].Planes[j].vertices[iSP];
				Vector X = World->m_Solids[i].Planes[j].vertices[(iSP - 1) % World->m_Solids[i].Planes[j].vertices.size()];
				Vector Y = World->m_Solids[i].Planes[j].vertices[(iSP - 2) % World->m_Solids[i].Planes[j].vertices.size()];
				Vector Z = World->m_Solids[i].Planes[j].vertices[(iSP - 3) % World->m_Solids[i].Planes[j].vertices.size()];

				for (Int32 R = 0; R < RowColCount; R++) // For Each Row
				{
					Vector REndPoint = Z + ((1.0 * R / RowColCount) * (Y - Z));
					Vector RStartPoint = SP + ((1.0 * R / RowColCount) * (X - SP));
					Vector RowTravelVector = REndPoint - RStartPoint;

					Vector R2EndPoint = Z + (((R + 1.0) / RowColCount) * (Y - Z));
					Vector R2StartPoint = SP + (((R + 1.0) / RowColCount) * (X - SP));
					Vector Row2TravelVector = R2EndPoint - R2StartPoint;

					for (Int32 C = 0; C < RowColCount; C++) // For Each Col
					{
						// Calculate initial point locations.
						Vector a = RStartPoint + ((1.0 * C / RowColCount) * RowTravelVector);
						Vector d = R2StartPoint + ((1.0 * C / RowColCount) * Row2TravelVector);
						Vector b = a + ((1.0 / RowColCount) * RowTravelVector);
						Vector c = d + ((1.0 / RowColCount) * Row2TravelVector);

						// Apply displacement
						a += World->m_Solids[i].Planes[j].elevation * World->m_Solids[i].Planes[j].normal // elevation
							+ World->m_Solids[i].Planes[j].offsets[R][C] // offset
							+ (World->m_Solids[i].Planes[j].distances[R][C] //
							* (World->m_Solids[i].Planes[j].offset_normals[R][C] // final normal * dist
							+ World->m_Solids[i].Planes[j].normals[R][C]).GetNormalized()); //

						b += World->m_Solids[i].Planes[j].elevation * World->m_Solids[i].Planes[j].normal // elevation
							+ World->m_Solids[i].Planes[j].offsets[R][C + 1] // offset
							+ (World->m_Solids[i].Planes[j].distances[R][C + 1] //
							* (World->m_Solids[i].Planes[j].offset_normals[R][C + 1] // final normal * dist
							+ World->m_Solids[i].Planes[j].normals[R][C + 1]).GetNormalized()); //

						c += World->m_Solids[i].Planes[j].elevation * World->m_Solids[i].Planes[j].normal // elevation
							+ World->m_Solids[i].Planes[j].offsets[R + 1][C + 1] // offset
							+ (World->m_Solids[i].Planes[j].distances[R + 1][C + 1] //
							* (World->m_Solids[i].Planes[j].offset_normals[R + 1][C + 1] // final normal * dist
							+ World->m_Solids[i].Planes[j].normals[R + 1][C + 1]).GetNormalized()); //

						d += World->m_Solids[i].Planes[j].elevation * World->m_Solids[i].Planes[j].normal // elevation
							+ World->m_Solids[i].Planes[j].offsets[R + 1][C] // offset
							+ (World->m_Solids[i].Planes[j].distances[R + 1][C] //
							* (World->m_Solids[i].Planes[j].offset_normals[R + 1][C] // final normal * dist
							+ World->m_Solids[i].Planes[j].normals[R + 1][C]).GetNormalized()); //

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
			GetVertices(World->m_Solids[i], SearchEpsilon);
			for (int j = 0; j < World->m_Solids[i].Planes.size(); j++) // for each face
			{
				int it = 0;
				for (it = 0; it < World->m_Solids[i].Planes[j].vertices.size(); it++) // for each point
				{
					index.push_back(mod->AddPoint(newPoly, (World->m_Solids[i].Planes[j].vertices[it])));
					points.push_back(World->m_Solids[i].Planes[j].vertices[it]);
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