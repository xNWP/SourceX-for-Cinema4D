#ifndef SX_MAINUI_H
#define SX_MAINUI_H

#include "c4d.h"
#include "c4d_symbols.h"
#include "SX_Map.h"
#include "SX_Globals.h"
#include "lib_modeling.h"

class SX_Register : public CommandData
{
public:
	Bool Execute(BaseDocument*);
};

class SX_MainUI : public GeDialog
{
public:
	Bool CreateLayout();
	Bool InitValues();
	Bool Command(Int32, const BaseContainer&);

	Bool GetMinSize(Int32 &w, Int32 &h);

private:
	Filename BROWSE_MAP();
	Bool ImportVMF(Filename);

	Filename m_Map;
};

#endif