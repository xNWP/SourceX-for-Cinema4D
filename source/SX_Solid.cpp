// Copyright (c) 2018 Brett Anthony. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "SX_Solid.h"

// Default ctor
SX_Solid::SX_Solid() : id(-1) {}

// CTOR w/ params
SX_Solid::SX_Solid(Int32 iid, std::vector<SX_Plane> &planes) : id(iid), Planes(planes), HasDisp(false) {}