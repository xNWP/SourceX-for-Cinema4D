// Copyright (c) 2018 Brett Anthony. All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "c4d.h"
#include "c4d_symbols.h"
#include "SX_Globals.h"

#include "SX_MainUI.h"


/* Plugin State Commands */
Bool PluginStart()
{
	// Register the plugin
	try
	{
		SX_Register *dlg = new SX_Register();
		RegisterCommandPlugin(ID_PLUGIN_ID, GeLoadString(IDS_PLUGIN_NAME), 0, nullptr, GeLoadString(IDS_PLUGIN_DESCRIPTION), dlg);
		String VersionTag = xstr(VERSION_TAG);
		GePrint("Loaded " + GeLoadString(IDS_PLUGIN_NAME) + " v" + xstr(VERSION_MAJOR) + "." + xstr(VERSION_MINOR) + (VersionTag == "0" ? "" : " " + VersionTag));
	}
	catch (std::exception &ex)
	{
		GePrint("Could not launch " + GeLoadString(IDS_PLUGIN_NAME) + ":");
		GePrint(ex.what());
	}

	return true;
}

void PluginEnd(void)
{
}

Bool PluginMessage(Int32 id, void *data)
{
	switch (id)
	{
		case C4DPL_INIT_SYS: // Do not init w/o resources.
			if (!resource.Init())
				return false;
	}

	return true;
}