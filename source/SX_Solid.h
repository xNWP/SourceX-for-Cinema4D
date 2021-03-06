// Copyright (c) 2018 Brett Anthony. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

/*
	world
	{
		solid
		{ YOU ARE HERE }
	}
*/

#ifndef SX_SOLID_H
#define SX_SOLID_H

#include "c4d.h"
#include <vector>
#include "SX_Plane.h"

class SX_Solid
{
public:
	SX_Solid();
	SX_Solid(Int32, std::vector<SX_Plane>&);

	std::vector<SX_Plane> Planes;
	Int32 id;
	Bool HasDisp;
};

#endif