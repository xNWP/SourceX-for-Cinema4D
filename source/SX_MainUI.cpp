#include "SX_MainUI.h"

/* REGISTER THE PLUGIN */
Bool SX_Register::Execute(BaseDocument *doc)
{
	SX_MainUI *dlg = new SX_MainUI();
	Int32 w, h;
	dlg->GetMinSize(w, h);
	return dlg->Open(DLG_TYPE_ASYNC, ID_PLUGIN_ID, -1, -1, w, h);
}

/* DIALOG IMPLEMENTATION */
Bool SX_MainUI::CreateLayout()
{
	// Call Parent
	GeDialog::CreateLayout();

	return LoadDialogResource(DLG_MAINUI, nullptr, 0);
}

Bool SX_MainUI::InitValues()
{
	// first call the parent instance
	if (!GeDialog::InitValues())
		return false;


	this->SetTitle(GeLoadString(IDS_PLUGIN_NAME) + " -- version " + GeLoadString(IDS_PLUGIN_VERSION));
	this->SetInt32(IDC_PLANE_SEARCH_EPSILON, -3, -8, 8, 1);

	return true;
}

Bool SX_MainUI::Command(Int32 id, const BaseContainer &msg)
{
	switch (id)
	{
		/* Browse Button */
		case IDC_BROWSE_BIG:
		case IDC_BROWSE_SMALL:
			m_Map = BROWSE_MAP();

			if (m_Map == nullptr)
			{ return false; }
			
			// else
			GePrint("-- Loaded Map File " + m_Map.GetString());
			this->SetString(IDC_BROWSE_BIG, m_Map.GetFile().GetString());

			return true;

		/* Import Button */
		case IDC_IMPORT:
			if (m_Map == nullptr)
			{ MessageDialog(GeLoadString(IDS_BAD_MAP_FILE)); return false; }
			
			// else
			GePrint("-- Beginning import with file " + m_Map.GetString());
			return ImportVMF(m_Map);
	}

	return false;
}

Filename SX_MainUI::BROWSE_MAP()
{
	Filename RVAL;
	Bool bOk = RVAL.FileSelect(FILESELECTTYPE_ANYTHING, FILESELECT_LOAD, GeLoadString(IDS_LOAD_FILE_TITLE), "Load");

	if (bOk)
	if (!RVAL.CheckSuffix("vmf"))
	{
		GePrint("-- Bad Map File");
		MessageDialog(GeLoadString(IDS_BAD_MAP_FILE));
	}
	else
		return RVAL;

	return nullptr;
}

// Begins import process
Bool SX_MainUI::ImportVMF(Filename fn)
{
	StatusSetSpin(); StatusSetText(GeLoadString(IDS_PROCESSING_FILE) + fn.GetFile().GetString());

	AutoAlloc<BaseFile> VMF_File;
	VMF_File->Open(fn, FILEOPEN_READ, FILEDIALOG_ANY, BYTEORDER_MOTOROLA);
	
	Int64 ByteSize = VMF_File->GetLength();

	Char *VMF_Data = new Char[ByteSize];
	VMF_File->ReadBytes(VMF_Data, ByteSize);
	
	std::vector<String> VMF_Line_Data = ParseLines(VMF_Data);
	if(VMF_Data != nullptr)	delete VMF_Data;

	StatusClear();
	SX_Map *Map = new SX_Map(VMF_Line_Data);

	BaseDocument *aDoc = GetActiveDocument();
	BaseObject *root = BaseObject::Alloc(Onull);
	root->SetName(fn.GetFile().GetString());

	aDoc->InsertObject(root, nullptr, nullptr);

	Int32 *SearchEpsilon = new Int32;
	this->GetInt32(IDC_PLANE_SEARCH_EPSILON, *SearchEpsilon);
	Map->CreateWorld(aDoc, root, *SearchEpsilon);
	MessageDialog("HeckedBySmiffEdits");
	
	// After Import, dealloc members
	if (SearchEpsilon != nullptr)
		delete SearchEpsilon;
	if (Map != nullptr)
		delete Map;

	return true;
}

Bool SX_MainUI::GetMinSize(Int32 &w, Int32 &h)
{
	w = 350;
	h = 500;
	return true;
}