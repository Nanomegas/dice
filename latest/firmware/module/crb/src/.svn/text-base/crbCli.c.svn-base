//===========================================================================
//
// This confidential and proprietary software may be used only as authorized
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
 * DESCRIPTION: Config ROM Builder (CRB) Command Line Interface

	crb CLI Tools and Variables

	These are used by the CLI to view crb info.

	Revisions:
		created 05/10/2008 brian
****************************************************************************/

#ifdef _CLI

#include "TCTypes.h"
#include "crb.h"
#include "csr.h"
#include "TCDebug.h"
#include "cli.h"
#include "crbCli.h"

#ifdef _CLI_TOOL_CRB

//static char 		moduleName[] = __FILE__ ;

#define CRB_MAX_DIR_HANDLES		16
#define CRB_HANDLE_DESCR_LEN	16

typedef struct 
{
	CRBHANDLE handle;
	char description[16];
} DIRHANDLE;

static DIRHANDLE gDirHandles[CRB_MAX_DIR_HANDLES] = { 0 };
static uint32 gNumDirHandles = 0;
static BOOL gInitialized = FALSE;

#define MAX_ROM_SIZE 100
static uint32 testRomSize;
static uint32 testRom[MAX_ROM_SIZE];


// initialize the cli structures and the crb
static HRESULT crb_init_cli(uint32 bMinimal)
{
	if (gNumDirHandles > CRB_MAX_DIR_HANDLES)
		gNumDirHandles = CRB_MAX_DIR_HANDLES;

	while (gNumDirHandles > 0)
	{
		memset(&gDirHandles[gNumDirHandles-1], 0, sizeof(DIRHANDLE));
		gNumDirHandles--;
	}
	uint32 WWUIDHigh, WWUIDLow;

	lalGetWWUID(&WWUIDHigh, &WWUIDLow);
	crbNewConfiguration(bMinimal, WWUIDHigh, WWUIDLow);

	gDirHandles[0].handle = crbGetRoot();
	strncpy (gDirHandles[0].description, "root", CRB_HANDLE_DESCR_LEN-1);

	gNumDirHandles = 1;
	gInitialized = TRUE;

	return (NO_ERROR);
}
static  char	crb_init_UsageString[]=\
				"crb.init: \n\r"\
				"use: init <minimal>\n\r"\
				"------------------------------------------------------------\n\r"\
				"clear current crb entries\n\r"\
				"<minimal> {yes|no}\n\r";

static	CLIDescriptor	crb_init_Descriptor[]=
{
	{kCLI_Name,				(uint32)"init"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_init_cli},
	{kCLI_UsageString, 		(uint32)crb_init_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"yes=1,no=0"},
	{kCLI_ModuleName,		(uint32)"init crb entries"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// display the directory handles
static HRESULT crb_dh_cli(void)
{
	uint32 i = 0;

	if (!gInitialized)
	{
		cliPrintf("*** Initialize first using crb.init\n\r");
		return (0);
	}

	if (gNumDirHandles > CRB_MAX_DIR_HANDLES)
		gNumDirHandles = CRB_MAX_DIR_HANDLES;

	cliPrintf("directory handles\nindex  description\n-----------------------\n");
	for (i=0; i<gNumDirHandles; i++)
	{
		cliPrintf("%d      %s\n", i, gDirHandles[i].description);
	}
	cliPrintf("\n");

	return (NO_ERROR);
}
static  char	crb_dh_UsageString[]=\
				"crb.dh: \n\r"\
				"use: dh \n\r"\
				"------------------------------------------------------------\n\r"\
				"list available directory handles\n\r";
static	CLIDescriptor	crb_dh_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dh"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_dh_cli},
	{kCLI_UsageString, 		(uint32)crb_dh_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dump crb dir handles"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// add an immediate element
static HRESULT crb_ai_cli(uint32 index, uint32 keyId, uint32 val)
{
	if (!gInitialized)
	{
		cliPrintf("*** Initialize first using crb.init\n\r");
		return (0);
	}

	if (index >= gNumDirHandles)
	{
		return(E_CRB_BAD_INPUT_PARAM);
	}
	else
	{
		CRBHANDLE dir = gDirHandles[index].handle;
		if (dir)
		{
			return crbAddImmediateEntry(dir, keyId, val);
		}
		else
		{
			return(E_CRB_INVALID_HANDLE);
		}
	}
}
static  char	crb_ai_UsageString[]=\
				"crb.ai: \n\r"\
				"use: ai dir id val \n\r"\
				"------------------------------------------------------------\n\r"\
				"add immediate entry to a directory\n\r"\
				"<dir> index\n\r"\
				"<id> keyId of entry\n\r"\
				"<val> 24-bit value (interpreted hex)\n\r";

static	CLIDescriptor	crb_ai_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ai"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_ai_cli},
	{kCLI_UsageString, 		(uint32)crb_ai_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iih"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"add immediate crb entry"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// add a directory element
//   the description is for user reference, and isn't part of the element
static HRESULT crb_ad_cli(uint32 index, uint32 keyId, char * desc)
{
	if (!gInitialized)
	{
		cliPrintf("*** Initialize first using crb.init\n\r");
		return (0);
	}

	if (index >= gNumDirHandles)
	{
		return(E_CRB_BAD_INPUT_PARAM);
	}
	else if (gNumDirHandles >= CRB_MAX_DIR_HANDLES)
	{
		return(E_CRB_TOO_MANY_ITEMS);
	}
	else
	{
		CRBHANDLE dir = gDirHandles[index].handle;
		if (dir)
		{
			CRBHANDLE newdir = crbAddSubDirectoryEntry(dir, keyId);
			gDirHandles[gNumDirHandles].handle = newdir;
			strncpy(gDirHandles[gNumDirHandles].description, desc, CRB_HANDLE_DESCR_LEN-1);
			gNumDirHandles++;

			return (NO_ERROR);
		}
		else
		{
			return(E_CRB_INVALID_HANDLE);
		}
	}
}

static  char	crb_ad_UsageString[]=\
				"crb.ad: \n\r"\
				"use: ad dir id desc \n\r"\
				"------------------------------------------------------------\n\r"\
				"add sub directory entry to a directory\n\r"\
				"<dir> index\n\r"\
				"<id> keyId of entry\n\r"\
				"<desc> a description string (15 chars max)\n\r";

static	CLIDescriptor	crb_ad_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ad"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_ad_cli},
	{kCLI_UsageString, 		(uint32)crb_ad_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iis"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"add subdirectory crb entry"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// add a leaf text element
static HRESULT crb_al_cli(uint32 index, char * text)
{
	if (!gInitialized)
	{
		cliPrintf("*** Initialize first using crb.init\n\r");
		return (0);
	}

	if (index >= gNumDirHandles)
	{
		return(E_CRB_BAD_INPUT_PARAM);
	}
	else
	{
		CRBHANDLE dir = gDirHandles[index].handle;
		if (dir)
		{
			return crbAddTextLeafEntry(dir, text);
		}
		else
		{
			return(E_CRB_INVALID_HANDLE);
		}
	}
}

static  char	crb_al_UsageString[]=\
				"crb.al: \n\r"\
				"use: al dir text \n\r"\
				"------------------------------------------------------------\n\r"\
				"add text leaf descriptor entry to a directory\n\r"\
				"<dir> index, as found in handle table\n\r"\
				"<text> descriptor text string\n\r";

static	CLIDescriptor	crb_al_Descriptor[]=
{
	{kCLI_Name,				(uint32)"al"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_al_cli},
	{kCLI_UsageString, 		(uint32)crb_al_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"is"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"add text leaf crb entry"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// build the ROM quadlets from the tree
static HRESULT crb_build_cli(uint32 bNoCommit)
{
	HRESULT hResult;
	if (!gInitialized)
	{
		cliPrintf("*** Initialize first using crb.init\n\r");
		return (0);
	}
	
	testRomSize = MAX_ROM_SIZE;
	hResult = crbCreateRom(testRom, &testRomSize);
	if ((!bNoCommit) && (hResult == NO_ERROR))
	{
		hResult = csrUpdateConfigROM(testRom, testRomSize);
	}
	return hResult;
}
static  char	crb_build_UsageString[]=\
				"crb.build: \n\r"\
				"use: build <nocommit>\n\r"\
				"------------------------------------------------------------\n\r"\
				"build ROM from current crb entries\n\r"\
				"<nocommit> optional, set to 1 to avoid committing to live ROM\n\r";
static	CLIDescriptor	crb_build_Descriptor[]=
{
	{kCLI_Name,				(uint32)"build"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_build_cli},
	{kCLI_UsageString, 		(uint32)crb_build_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"I"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"build ROM"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


// dump the ROM quadlets created by the build command
HRESULT crb_dump_cli(void)
{
	uint32 i = 0;
	uint32 iRom = 0xf0000400;
	uint32 * pRomPtr = testRom;
	uint32 nSize = testRomSize;

	cliPrintf("ROM size %d quadlets.\n", nSize);
	if (0 != nSize)
	{
		cliPrintf("pRom:\naddr        val         rom\n");
		for (i=0; i<nSize; i++)
		{
			cliPrintf("0x%08X  0x%08X  0x%08X\n", pRomPtr, *pRomPtr, iRom);
			iRom+=4;
			pRomPtr++;
		}
	}
	return (0);
}
static  char	crb_dump_UsageString[]=\
				"crb.dump: \n\r"\
				"use: dump \n\r"\
				"------------------------------------------------------------\n\r"\
				"dump ROM quadlets\n\r";
static	CLIDescriptor	crb_dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_dump_cli},
	{kCLI_UsageString, 		(uint32)crb_dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dump ROM quadlets"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static char * keyTypes[4] =
{"IMMEDIATE",
 "CSR_OFFSET",
 "LEAF",
 "DIRECTORY"
};

typedef struct
{
	char * name;
	uint32 id;
} KEY_ID_NAME;

static KEY_ID_NAME keyIDs[] = {
 {"DESCRIPTOR",							RDK_ID_DESCRIPTOR	},
 {"BUS_DEPENDENT_INFO",			RDK_ID_BUS_DEPENDENT_INFO	},
 {"VENDOR",									RDK_ID_VENDOR	},
 {"HARDWARE_VERSIOn",				RDK_ID_HARDWARE_VERSIOn	},
 {"MODULE",									RDK_ID_MODULE	},
 {"NODE_CAPS",							RDK_ID_NODE_CAPS	},
 {"EUI64",									RDK_ID_EUI64	},
 {"UNIT",										RDK_ID_UNIT	},
 {"SPECIFIER_ID",						RDK_ID_SPECIFIER_ID	},
 {"VERSION",								RDK_ID_VERSION	},
 {"DEPENDENT_INFO",					RDK_ID_DEPENDENT_INFO	},
 {"UNIT_LOCATION",					RDK_ID_UNIT_LOCATION	},
 {"MODEL",									RDK_ID_MODEL	},
 {"INSTANCE",								RDK_ID_INSTANCE	},
 {"KEYWORD",								RDK_ID_KEYWORD	},
 {"FEATURE",								RDK_ID_FEATURE	},
 {"EXTENDED_ROM",						RDK_ID_EXTENDED_ROM	},
 {"EXTENDED_KEY_SPEC_ID",		RDK_ID_EXTENDED_KEY_SPEC_ID	},
 {"EXTENDED_KEY",						RDK_ID_EXTENDED_KEY	},
 {"EXTENDED_DATA",					RDK_ID_EXTENDED_DATA	},
 {"MODIFIABLE_DESCRIPTOR",	RDK_ID_MODIFIABLE_DESCRIPTOR	},
 {"DIRECTORY_ID",						RDK_ID_DIRECTORY_ID	},
 {"HAL_VENDOR",							RDK_ID_HAL_VENDOR	},
 {"HAL_MODEL",							RDK_ID_HAL_MODEL	},
 {"NC_VENDOR",							RDK_ID_NC_VENDOR	},
 {"NC_MODEL",								RDK_ID_NC_MODEL	}
};


static char * getKeyIdName (uint32 keyid)
{
	int i;
	for (i=0; i<sizeof(keyIDs)/sizeof(KEY_ID_NAME); i++)
	{
		if (keyIDs[i].id==keyid) return keyIDs[i].name;
	}
	return "Unknown";
}


HRESULT crb_printCB(CRBHANDLE node, uint32 depth, void * pData)
{
	uint32 * pLeaf;
	uint32  i;

	if (node)
	{
		cliPrintf("%*s-Node  %02x %s %s\r\n", 2*depth,"",node->nodeNum,getKeyIdName(node->keyId),keyTypes[node->keyType>>30]);
		switch (node->keyType)
		{
			case RDK_TYPE_IMMEDIATE:
				cliPrintf("%*s  Val: 0x%08x\r\n",2*depth,"",node->data.immVal);
				break;
			case RDK_TYPE_DIRECTORY:
				cliPrintf("%*s |\r\n",2*depth,"");
				break;
			case RDK_TYPE_LEAF:
				pLeaf = (uint32 *)node->data.pLeafData;
				for (i=0; i<node->nElements; i++)
				{
					cliPrintf("%*s  0x%08X %c%c%c%c\n",2*depth,"",*pLeaf,(*pLeaf>>24)&0xff,(*pLeaf>>16)&0xff,(*pLeaf>>8)&0xff,(*pLeaf>>0)&0xff);
					pLeaf++;
				}
				break;
		}
	}
	return NO_ERROR;
}

// dump the nodes in order of creation
static HRESULT crb_dt_cli(void)
{
	return crbWalkTreeDFirst(crb_printCB, 0);
}


static  char	crb_dt_UsageString[]=\
				"crb.dt: \n\r"\
				"use: dt \n\r"\
				"------------------------------------------------------------\n\r"\
				"dump crb tree\n\r";
static	CLIDescriptor	crb_dt_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dt"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_dt_cli},
	{kCLI_UsageString, 		(uint32)crb_dt_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dt ROM quadlets"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT crb_sz_cli(void)
{
	uint32 size;
	HRESULT hRes = crbCalculateRomSize(&size);
	if (hRes==NO_ERROR)
	{
		cliPrintf("Total ROM size = %i\r\n",size);
	}
	return hRes;
}


static  char	crb_sz_UsageString[]=\
				"crb.sz: \n\r"\
				"use: sz \n\r"\
				"------------------------------------------------------------\n\r"\
				"get ROM size\n\r";
static	CLIDescriptor	crb_sz_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sz"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crb_sz_cli},
	{kCLI_UsageString, 		(uint32)crb_sz_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"sz size of ROM in quads"},
	{kCLI_GroupName,		(uint32)"crb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT crbCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(crb_init_Descriptor);	// init
	hResult += cliInstallCLIDescriptor(crb_dh_Descriptor);		// dump dir handles
	hResult += cliInstallCLIDescriptor(crb_ai_Descriptor);		// add immediate entry to dir
	hResult += cliInstallCLIDescriptor(crb_ad_Descriptor);		// add directory entry to dir
	hResult += cliInstallCLIDescriptor(crb_al_Descriptor);		// add leaf text to dir
	hResult += cliInstallCLIDescriptor(crb_build_Descriptor);	// build ROM quadlets from tree
	hResult += cliInstallCLIDescriptor(crb_dump_Descriptor);	// list ROM quadlets
	hResult += cliInstallCLIDescriptor(crb_dt_Descriptor);		// list tree entries
	hResult += cliInstallCLIDescriptor(crb_sz_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing crb tools");
	}
	return hResult;
}

#endif //_CLI_TOOL_CRB
#endif //_CLI
