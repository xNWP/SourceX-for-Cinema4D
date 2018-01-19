// Copyright (c) 2018 Brett Anthony. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

/*
	world { YOU ARE HERE }
*/

#ifndef SX_WORLD_H
#define SX_WORLD_H

#include "c4d.h"
#include "c4d_symbols.h"
#include "SX_Solid.h"
#include "SX_VMF.h"
#include "SX_Plane.h"
#include <vector>
#include <string>

class SX_World
{
public:
	SX_World();
	SX_World(std::vector<String>*, int, int);
	std::vector<SX_Solid> m_Solids;

	Int32 id;
	String SkyName;

private:
	SX_Solid& ParseSolid(std::vector<String>*, int&);
	SX_Plane& ParseSide(std::vector<String>*, int&);
	
	void Construct();
};

#endif