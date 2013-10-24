//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
#if defined (_AVC) && defined (_EDS) && defined (_DRD)

#include "TCTypes.h"

uint32 AudioSubunitData[]= {

//Audio_subunit_dependent_info
0x00A20001, // [2]audio dependent info fields length, = 162, [1] audio subunit version = 1.0, [1]# of configs = 1

// Configuration[1]_dependent_info
0x009E0001, // [2]length = 158, [2]config id = 1

// Master_cluster_info 
0x00080202, // [2]len = 8, [1]# of channels = 2, [1]ch config type = GENERIC_MULTI-SPEAKER
0xC0000000, // [2]predefined ch config, Lfront, Rfront, [2]ch names 1 = Lfront
0x00010481, // [2]ch names 2 = Rfront, [1]# of SU src plug link info's = 4, [1]feature fb type

// Subunit_source_plug_info[]_link_info 
0x01810281, // [1]feature fb type cont'd, ID = 1, [2]feature fb type, ID = 2, [1]feature fb type
0x03810404, // [1]feature fb type cont'd, ID = 3, [2]feature fb type, ID = 4, [1]# of fb dependent info = 4

// Function_block_dependent_info

// feature fb 1
0x00208101, // [2]len = 32, [1]fb feature type, [1]fb id = 1
0x000201F0, // [2]fb name db entry = 2, [1]# fb input plugs = 1, [1]source_ID type: F0 = su dest plug
0x00000A02, // [1]source_ID fb id: 00 = su dest plug 0, [2]cluster info len = 10, [1]# ch's = 2
0x02800140, // [1]ch config type = 02 GENERIC_MULTI-SPEAKER, [2]ch config = L, non-conventional, [1]ch config
0x01000300, // [1]ch config cont'd = R, non-conventional, [2]name db entry  = 3, [1]name db entry
0x04000B00, // [1]name db entry cont'd = 4, [2]fb type dep info len = 11, [1]size
0x09000200, // [1]cont'd, size of controls and general_tag = 9, [2]size of controls element, [1]general_tag = mute
0x00000000, // [2]master controls set for these, [2]set for logical ch 0
0x00000020, // [2]set for logical ch 1, [2]next block len = 32

// feature fb 2
0x81020005, // [1]fb feature type, [1]fb id = 2, [2]fb name db entry = 5
0x01F00100, // [1]# fb input plugs = 1, [1]source_ID type: F0 = su dest plug, [1]source_ID fb id: 01 = su dest plug 1, [1]len 
0x0A020280, // [1]cont'd, cluster info len = 10,  [1]# ch's = 2, [1]ch config type = 02 GENERIC_MULTI-SPEAKER, ch config
0x01400100, // [1]cont'd, ch config = L, non-conventional, [2]ch config = R, non-conventional, [1]entry
0x06000700, // [1]cont'd name db entry  = 6, [2]name db entry  = 7, [1]len
0x0B000900, // [1]cont'd, fb type dep info len = 11, [2]size of controls and general_tag = 9, [1]size
0x02000000, // [1]cont'd, size of controls element, [1]general_tag = mute, [2]master controls set for these
0x00000000, // [2]set for logical ch 0, [2]set for logical ch 1

// feature fb 3
0x00208103, // [2]len = 32, [1]fb feature type, [1]fb id = 3
0x000801F0, // [2]fb name db entry = 8, [1]# fb input plugs = 1, [1]source_ID type: F0 = su dest plug
0x02000A02, // [1]source_ID fb id: 02 = su dest plug 2, [2]cluster info len = 10, [1]# ch's = 2
0x02800140, // [1]ch config type = 02 GENERIC_MULTI-SPEAKER, [2]ch config = L, non-conventional, [1]ch config
0x01000900, // [1]ch config cont'd = R, non-conventional, [2]name db entry  = 9, [1]name db entry
0x0A000B00, // [1]name db entry cont'd = 10, [2]fb type dep info len = 11, [1]size
0x09000200, // [1]cont'd, size of controls and general_tag = 9, [2]size of controls element, [1]general_tag = mute
0x00000000, // [2]master controls set for these, [2]set for logical ch 0
0x00000020, // [2]set for logical ch 1, [2]next block len = 32

// feature fb 4
0x8104000B, // [1]fb feature type, [1]fb id = 4, [2]fb name db entry = 11
0x01F00300, // [1]# fb input plugs = 1, [1]source_ID type: F0 = su dest plug, [1]source_ID fb id: 03 = su dest plug 3, [1]len 
0x0A020280, // [1]cont'd, cluster info len = 10,  [1]# ch's = 2, [1]ch config type = 02 GENERIC_MULTI-SPEAKER, ch config
0x01400100, // [1]cont'd, ch config = L, non-conventional, [2]ch config = R, non-conventional, [1]entry
0x0C000D00, // [1]cont'd name db entry  = 12, [2]name db entry  = 13, [1]len
0x0B000900, // [1]cont'd, fb type dep info len = 11, [2]size of controls and general_tag = 9, [1]size
0x02000000, // [1]cont'd, size of controls element, [1]general_tag = mute, [2]master controls set for these
0x00000000, // [2]set for logical ch 0, [2]set for logical ch 1

};

#endif //_AVC &&  _EDS && _DRD
