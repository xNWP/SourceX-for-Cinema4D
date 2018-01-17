#ifndef SX_MAP_H
#define SX_MAP_H

#include <vector>
#include "c4d.h"
#include "SX_World.h"
#include "SX_VMF.h"
#include "lib_modeling.h"

class SX_Map
{
public:
	SX_Map();
	SX_Map(std::vector<String>&);
	~SX_Map();

	Bool CreateWorld(BaseDocument*, BaseObject*, const Int32 &);

	Int32 MapVersion;
private:
	SX_World *World;

};

#endif