//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

// This is the subunit_type_dependent_info for the Music Subunit Identifier Descriptor
uint32 MusicSubunitIDData[]= {

0x001D0011, // [2]music_subunit_type_dependent_info_length = 29, [1]attributes, [1]Music SU version 1.1
0x00192706, // [2]music_subunit_specific_information_length = 25, [1]capability_attributes mask, [1]general caps len
0x0201ffff, // [1]tx caps = blocking, [1]rx caps = non-blocking, [2]latency_caps 
0xffff0701, // [2]cont'd latency_caps, [1]audio caps len = 7, [1]available_audio_format_info_number = 1
0x00080008, // [2]number_of_maximum_audio_input_channels = 8, [2]number_of_maximum_audio_output_channels = 8
0x00400600,	// [2]available_audio_format = FDF:0, AM824Label:0x40, [1]MIDI cap len = 6, [1]MIDI version = 0, revision = 0
0x00000100, // [1]MIDI_adaptation_layer_version = 0, [2]# MIDI in ports = 1, [1]# MIDI out ports
0x01010300  // [1]cont'd # MIDI out ports = 1, [1]audio sync caps len = 1, [1]audio sync caps = Ex+Bus, [1]quadlet alignment

};

// This is the Music Subunit Status Descriptor
//  This is created as an Opaque descriptor type, so the host can bulk-read
uint32 MusicSubunitStatusData[]= {

// descriptor total length
//          12	general su status area
//         418	music plug output status area
//		  1430  routing status info
//        ----
//        1860 = 0x0744

// General_music_subunit_status_area_info_block
0x000A8100,	// [2]info block len = 10, [2]info block type = subunit status area
0x00060201, // [2]primary fields len, [1]tx caps = blocking, [1]rx caps = non-blocking
0xFFFFFFFF, // [4]current latency capability

// Music_plug_output_status_area_info_block
//  length =
//           2	length bytes
//		     5  8101 block
//		   143	(141+2)		= output plug 0: spdif/aes/midi
//		    55	(53+2)		= output plug 1: spdif 
//		   159	(51+2)*3	= output plugs 2-4: aes02-04 
//		    40	(38+2)		= output plug 5: midi 
//		    14	(12+2)		= output plug 6: sync 
//        ----
//         418 = 0x01A2

0x01A08101, // [2]next info block len = 416, [2]info block type Music Output status Area
0x00010700, // [2]len, [1]# src plugs = 7, [1]len
0x8D810200, // [2]next info block len, cont'd = 141, [1]info block type = 8102 Src Plug Status, [1]len

// output src plug 0
0x01000068, // [1]primary fields len, cont'd = 1, [1]src plug number = 0, [2]next info block len = 104
0x81030001, // [2]info block type Audio, [2]primary fields len
0x08006100, // [1]# audio streams = 8, [2]next info block len = 97, [2]type
0x0B005D00, // [1]info block type, cont'd = Name, [2]primary fields len = 93, [1]name ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]max # chars = not-defined, [1]len
0x57000A00, // [1]next info block len, cont'd = 87, [2]info block type = Raw Text, [1]# chars
0x53532F50, // [1]# of char's, cont'd = 83 (incl terminator), [3]name data start
0x44494620, 
0x4C0D0A53, // = [3]"S/PDIF L" + crlf, [1]next name 
0x2F504449,
0x4620520D, 
0x0A414553, // = [1]"S/PDIF R" + crlf, [3]next name
0x20303220, 
0x4C0D0A41, // = [3]"AES 02 L" + crlf, [1]next name 
0x45532030, 
0x3220520D, 
0x0A414553, // = [1]"AES 02 R" + crlf, [3]next name
0x20303320, 
0x4C0D0A41, // = [3]"AES 03 L" + crlf, [1]next name 
0x45532030,
0x3320520D, 
0x0A414553, // = [1]"AES 03 R" + crlf, [3]next name
0x20303420, 
0x4C0D0A41, // = [3]"AES 04 L" + crlf, [1]next name 
0x45532030,
0x3420520D, 
0x0A000000, // = [1]"AES 04 R" + crlf,  [3]text terminator

0x001C8104, // [2]next info block len = 28, info block type MIDI
0x00010100, // [2]primary fields len, [1]# MIDI streams, [1]next info block len
0x15000B00, // [1]next info block len cont'd = 21, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd = 17, [1]name ref type = [1]immediate, name data attributes = read only stored here in the SU, [1]max # of char
0xFF000B00, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A00074D, // [1]info block type cont'd = Raw Text, [2]primary fields len, [1] name data start
0x49444920, 
0x31000035, // [1] = "MIDI 1", [1] text terminator, [2]next info block len = 53

// output src plug 1
0x81020001, // [2]info block type Src Plug Status, [2]primary fields len
0x01002E81, // [1]source plug number, [2]next info block len = 46, [1]info block type
0x03000102, // [1]info block type cont'd = Audio, [2]pri flds len, [1]# audio streams = 2
0x0027000B, // [2]next info block len = 39, [2]info block type  = Name
0x00230000, // [2]pri flds len = 35, [1]ref type = immediate, [1]name data attributes = read only stored here in the SU
0xFFFF001D, // [2]max # of char = not-defined, [2]next info block len = 29
0x000A0019, // [2]Raw Text type, [2]# of char's = 25
0x53504449, // name data start
0x464F7574, 
0x204C0D0A, // [4]= "SPDIFOut L" crlf
0x53504449, 
0x464F7574, 
0x20520D0A, // [4]= "SPDIFOut R" crlf

// output src plug 2
0x00003381, // [1]text terminator, [2]next info block len = 51, [1]info block type
0x02000102, // [1]info block type cont'd = Src Plug Status, [2]pri flds len, [1]src plug number
0x002C8103, // [2]next info block len = 44, [2]info block type Audio
0x00010200, // [2]pri flds len, [1]# audio streams = 2, [1]next info block len
0x25000B00, // [1]next info block len cont'd = 37, [2]info block type = Name, [1]pri flds len
0x210000FF, // [1]pri flds len cont'd = 33, [1]ref type = immediate, [1]name data attributes = read only stored here in the SU, [1]max # of char
0xFF001B00, // [1]max # of char cont'd = not-defined, [2]next info block len = 27, [1]info block type
0x0A001741, // [1]info block type cont'd = Raw Text, [2]# of char's = 23, [1]name data start
0x4553324F, 
0x7574204C,
0x0D0A4145, // [2]= "AES2Out L" crlf, [2] next name
0x53324F75, 
0x7420520D, 
0x0A000033, // [1]= "AES2Out R" crlf, [1]text terminator, [2]next info block len = 51

// output src plug 3
0x81020001, // [2]info block type Src Plug Status, [2]pri flds len
0x03002C81, // [1]src plug number, [2]next info block len = 44, [1]info block type
0x03000102, // info block type cont's = Audio, [2]pri flds len, [1]# audio streams = 2
0x0025000B, // [2]next info block len, [2]info block type Name
0x00210000, // [2]pri flds len = 33, [1]ref type = immediate, [1]name data attributes = read only stored here in the SU
0xFFFF001B, // [2]max # of char = not-defined, [2]next info block len
0x000A0017, // [2]info block type Raw Text, [2]# of char's = 23
0x41455333, // name data start
0x4F757420, 
0x4C0D0A41, // [3]= "AES3Out L" crlf, [1]next name
0x4553334F, 
0x75742052, 
0x0D0A0000, // [2]= "AES3Out R" crlf, [1]text terminator, [1]next info block len

// output src plug 4
0x33810200, // [1]next info block len cont'd = 51, [2]info block type Src Plug Status, [1]primary fields len
0x0104002C, // [1]primary fields len cont'd, [1]src plug number, [2]next info block len = 44
0x81030001, // [2]info block type Audio, [2]primary fields len
0x02002500, // [1]# audio streams, [2]next info block len = 37, [1]info block type
0x0B002100, // [1]info block type = Name, [2]primary fields len = 33, [1]ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]max # of char = not-defined, [1]next info block len
0x1B000A00, // [1]next info block len cont'd = 27, [2]info block type Raw Text, [1]# of char's
0x17414553, // [1]# of char's cont'd = 23, [3]name data start
0x344F7574, 
0x204C0D0A, // = "AES4Out L" crlf
0x41455334, 
0x4F757420,
0x520D0A00, // [3]= "AES4Out R", [1]text terminator

// output src plug 5
0x00268102, // [2]next info block len = 38, [2]info block type Src Plug Status
0x00010500, // [2]primary fields len, [1]src plug number, [1]next info block len
0x1F810400, // [1]next info block len cont'd = 31, [2]info block type MIDI, [1]pri flds len
0x01010018, // [1]pri flds len cont'd, [1]# MIDI streams, [2]next info block len = 24
0x000B0014, // [2] info block type Name, [2]primary fields len = 20, 
0x0000FFFF, // [1]ref type = immediate, [1]name data attributes = read only stored here in the SU, [2]max # of char = not-defined
0x000E000A, // [2] next info block len = 14, [2]info block type Raw Text 
0x000A4D69, // [2]# of char's, [2] name data start
0x64694F75, 
0x74203100, // [3]= "MidiOut 1", [1]text terminator

// output src plug 6
0x000C8102, // [2]next info block len = 12, [2]info block type Src Plug Status
0x00010600, // [2]primary fields len, [1]src plug number 6, [1]next info block len
0x05810700, // [1]next info block len cont'd, [2]info block type Audio SYNC, [1]primary fields len
0x01030594, // [1]primary fields len cont'd, [1]audio SYNC activity = Bus + Ex (sync source), next info block len = 0x594

// Routing_status_info_block len (this is the length from here to the end of the descriptor)
//			 2	length bytes (above)
//		     8  routing status info
//		   520	su dst plug info
//		   520	su src plug info
//		   380	music plug info
//        ----
//        1430 = 0x0596

// Routing_status_info_block 
0x81080004, // [2]info block type Routing Status, [2]primary fields len
0x07070013, // [1]# SU dst plugs = 7, [1]# SU src plugs = 7, [2]# Music plugs = 19

//	Subunit_dst_plug_info_blocks (su input plugs)
//  length =
//           2	length bytes
//		   206	12 + (38+2)*4 + (32+2)	= dst plug 0: spdif/aes/midi
//		   216	(52+2)*4				= dst plugs 1-4: spdif/aes02-04 
//		    48	(46+2)					= dst plug 5: midi 
//		    48	(46+2)					= dst plug 6: sync 
//        ----
//         520

// su dst plug 0
0x00CE8109, // [2]next info block len = 206, [2]info block type SU Plug info
0x00080090, // [2]primary fields len, [1]SU dst plug ID, [1]signal format
0x02000005, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = IsoStream, [2]# clusters = 5
0x00090026, // [2]# channels = 9, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]pri flds len = 11
0x06040200, // [1]stream format = MBLA, [1]port type = SPDIF, [1]# signals, [1]id
0x09000100, // [1]id cont'd = signal 0, music plug ID 9, [1]iso stream position, [1]stream location, Lfront, [1]id
0x0A010200, // [1]id cont'd = signal 1, music plug ID 10, [1]iso stream position, [1]stream location, Rfront, next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of char
0xFF000B00, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000753, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data start
0x2F504449, 
0x46000026, // [1]= "S/PDIF", [1] text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]pri flds len = 11
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# signals, [1]id
0x0B020100, // [d] id cont'd = signal 0, music plug ID 11, [1]iso stream position, [1]Lfront, [1]id
0x0C030200, // [1]id cont'd = signal 1, music plug ID 12, [1]iso stream position, [1]Rfront, [1]next info block len 
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max #of char
0xFF000B00, // [1]max # of char cont's = not-defined, [2]next info block len, [1]info block type
0x0A000717, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data start
0x45532030, 
0x32000026, // [1]= "AES 02", [1]text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals, [1]id
0x0D040100, // [1] id cont'd = signal 0, music plug ID 13, [1]iso stream position, [1]Lfront, [1]id
0x0E050200, // [1]id cont'd = signal 1, music plug ID 14, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of char
0xFF000B00, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data start
0x45532030, 
0x33000026, // [1]= "AES 03", [1]text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals, [1]id
0x0F060100, // [1]id cont'd = signal 0, music plug ID 15, [1]iso stream position, [1]Lfront, [1]id
0x10070200, // [1]id cont'd = signal 1, music plug ID 16, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of char
0xFF000B00, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont'd = Raw Text, [2]# of char's, [1] name data start
0x45532030, 
0x34000020, // [1]= "AES 04", [1] text terminator, text terminator, [2]next info block len = 32

0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x0D0A0100, // [1]stream format = MIDI, [1]port type = MIDI, [1]# signals, [1]id
0x11080B00, // [1]id cont'd = signal 0, music plug ID 17, [1]iso stream position, [1]Side Right (??), [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name, [1]primary fields len 
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1] max # of char
0xFF000900, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A00054D, // [1]info block type cont's = Raw Text, [2]# of char's, [1]name data start
0x69646900, // [3]= "Midi", [1]text terminator

// su dst plug 1
0x00348109, // [2]next info block len = 52, [2]info block type SU Plug info
0x00080190, // [2]primary fields len, [1]SU dst plug ID, [1] signal fmt
0x02000001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type IsoStream, [2]# clusters
0x00020026, // [2]# channels, [2]next info block len = 38
0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x06040200, // [1]stream format = MBLA, [1]port type = SPDIF, [1]# signals, [1]id
0x00000100, // [1]id cont'd = signal 0, music plug ID 0, [1]iso stream position, [1]Lfront, [1]id
0x01010200, // [1]id cont'd = signal 1, music plug ID 1, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len, [2]info block type Name, [1]primary fields len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of char
0xFF000B00, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000753, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data start
0x2F504449, 
0x46000034, // [1]= "S/PDIF", [1] text terminator, [2]next info block len = 52

// su dst plug 2
0x81090008, // [2]info block type SU Plug info, [2]primary fields len
0x02900200, // [1]SU dst plug ID, [2]signal format: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type IsoStream
0x00010002, // [2]# clusters, [2]# channels
0x0026810A, // [2]next info block len = 38, [2]info block type Cluster info
0x000B060B, // [2]primary fields len, [1]stream format MBLA, [1]port type = AES/EBU
0x02000200, // [1]# signals, [2]id = signal 0, music plug ID 2, [1]iso stream position
0x01000301, // [1]Lfront, [2]id = signal 1, music plug ID 3, [1]iso stream position
0x02001500, // [1]Rfront, [2]next info block len, [1]info block type
0x0B001100, // [1]info block type cont'd = Name, [2]primary fields len, [1]ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]max # of char= not-defined, [1]next info block len
0x0B000A00, // [1]next info block len cont'd, [2]info block type Raw Text, [1]# of char's
0x07414553, // [1]# of char's cont'd, [3]name data start
0x20303200, // [3]= "AES 02", [1]text terminator

// su dst plug 3
0x00348109, // [2]next info block len = 52, [2]info block type SU Plug info
0x00080390, // [2]pri flds len, [1]SU dst plug ID, [1]fmt
0x02000001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type IsoStream, [2]# clusters
0x00020026, // [2]# channels, [2]next info block len = 38
0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format MBLA, [1]port type AES/EBU, [1]# signals, [1]id
0x04000100, // [1]id cont'd = signal 0, music plug ID 4, [1]iso stream position, [1]Lfront, [1]id
0x05010200, // [1]id cont'd = signal 1, music plug ID 5, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont'd = Raw Text , [2]# of char's, [1] name data start
0x45532030, 
0x33000034, // [1]= "AES 03", [1]text terminator, [2]next info block len = 52

// su dst plug 4
0x81090008, // [2]info block type SU Plug info, [2]primary fields len
0x04900200, // [1]SU dst plug ID, [2]signal format: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type IsoStream
0x00010002, // [2]# clusters, [2]# channels
0x0026810A, // [2]next info block len = 38, [2]info block type Cluster info 
0x000B060B, // [2]primary fields len, [1]stream format = MBLA, [1]port type = AES/EBU
0x02000600, // [1]# signals, [2]signal 0, music plug ID 6, [1]iso stream position
0x01000701, // [1]Lfront, [2]signal 1, music plug ID 7, [1]iso stream position
0x02001500, // [1]RFront, [2]next info block len, [1]info block type
0x0B001100, // [1]info block type Name, [2]primary fields len, [1]ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]max # of char = not-defined, [1]
0x0B000A00, // [1]next info block len cont'd, [2]info block type Raw Text, [1] # of chars
0x07414553, // [1]# of char's cont'd, [3]name data starts
0x20303400, // [3]= "AES 04", [1]text terminator

// su dst plug 5
0x002E8109, // [2]next info block len = 46, [2]info block type SU Plug info
0x00080590, // [2]primary fields len, [1]SU dst plug ID, [1]fmt
0x02020001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = Midi, [2]# clusters
0x00010020, // [2]# channels, [2]next info block len = 32
0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x0D0A0100, // [1]stream format = MIDI, [1]port type = MIDI, [1]# of signals, [1]id
0x08000100, // [1]id cont'd = music plug ID 8, [1]iso stream  position, [1]Lfront, [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000900, // [1]max # of char cont'd = not-defined, [2]next info block len, [1]info block type
0x0A00054D, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data starts
0x69646900, // [3]= "Midi", [1]text terminator

// su dst plug 6
0x002E8109, // [2]next info block len = 46, [2]info block type SU Plug info
0x00080690, // [2]primary fields len, [1]SU dst plug ID, [1]fmt
0x02030001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = Sync, [2]# clusters
0x00020020, // [2]# channels, [2]next info block len = 32
0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x40090100, // [1]stream format = SYNC stream, [1]port type = Digital, [1]# of signals, [1]id
0x12000100, // [1]id cont'd = music plug ID 18, [1]iso stream position, [1]Lfront, [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name , [1]primary fields len
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000900, // [1]max # of chars cont's = not-defined, [2]next info block len, [1]info block type
0x0A000553, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data starts
0x796E6300, // [3] = "Sync", [1]text terminator

//  Subunit_src_plug_info_blocks  (su output plugs)
//  length =
//           2	length bytes
//		   206	12 + (38+2)*4 + (32+2)	= src plug 0: spdif/aes/midi
//		   216	(52+2)*4				= src plugs 1-4: spdif/aes02-04 
//		    48	(46+2)					= src plug 5: midi 
//		    48	(46+2)					= src plug 6: sync 
//        ----
//         520

// su src plug 0
0x00CE8109, // [2]next info block len = 206, [2]info block type SU Plug info
0x00080090, // [2]primary fields len, [1]SU src plug ID 0, [1]fmt
0x02000005, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = IsoStream, [2]# clusters
0x00090026, // [2]# channels = 9, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x06040200, // [1]stream format = MBLA, [1]port type = SPDIF, [1]# of signals, [1]id
0x00000100, // [1]id cont'd = music plug ID 0, [1]iso stream position, [1]Lfront, [1]id
0x01010200, // [1]id cont'd = music plug ID 1, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name , [1]len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1] max # of chars
0xFF000B00, // [1]max # of chars cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000753, // [1]info block type cont'd = Raw Text, [2]# of char's, [1] name data starts
0x2F504449, 
0x46000026, // [1]= "S/PDIF", [1]text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals, [1]id
0x02020100, // [1]id cont'd = music plug ID 2, [1]iso stream position, [1]Lfront, [1]id
0x03030200, // [1]id cont'd = music plug ID 3, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len 
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars cont'd = not-defined, [2]next info block len, [1]
0x0A000741, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data starts
0x45532030, 
0x32000026, // [1]= "AES 02", [1]text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals,[1]id
0x04040100, // [1]id cont'd = music plug ID 4, [1]iso stream position, [1]Lfront, [1]id
0x05050200, // [1]id cont's = music plug ID 5, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name , [1]len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data starts
0x45532030,
0x33000026, // [1]= "AES 03", [1]text terminator, [2]next info block len = 38

0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals, [1]id
0x06060100, // [1]id cont'd = music plug ID 6, [1]iso stream position, [1]Lfront, [1]id
0x07070200, // [1]id cont'd = music plug ID 7, [1]iso stream position, [1]Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len
0x110000FF, // [1]primary fields len, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars cont's = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont's = Raw Text, [2]# of char's, [1]name data starts
0x45532030, 
0x34000020, // [1]= "AES 04", [1]text terminator, [2]next info block len = 32

0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x0D0A0100, // [1]stream format = MIDI, [1]port type = MIDI, [1]# of signals, [1]id
0x08080B00, // [1]id cont'd = music plug ID 8, [1]iso stream position, [1](Side Right?), [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name [1]len
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000900, // [1]max # of chars cont's = not-defined, [2]next info block len, [1]info block type
0x0A00054D, // [1]info block type cont's = Raw Text , [2]# of char's, [1]name data starts
0x69646900, // [3]= "Midi", [1]text terminator

// su src plug 1
0x00348109, // [2]next info block len = 52, [2]info block type SU Plug info
0x00080190, // [2]primary fields len, [1]SU src plug ID, [1]fmt
0x02000001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type= IsoStream, [2]# clusters
0x00020026, // [2]# channels, [2]next info block len = 38
0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x06040200, // [1]stream format = MBLA, [1]port type = SPDIF, [1]# of signals, [1]id
0x09000100, // [1]id cont'd = music plug ID 9, [1]iso stream position, [1]Lfront, [1]id
0x0A010200, // [1]id cont'd = music plug ID 10, [1]iso stream position, [1]Rfront, next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len 
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars cont's = not-defined, [2]next info block len, [1]info block type
0x0A000753, // [1]info block type Raw Text , [2]# of char's, [1]name data starts
0x2F504449, 
0x46000034, // [1]= "S/PDIF", [1]text terminator, [2]next info block len = 52

// su src plug 2
0x81090008, // [2]info block type SU Plug info, [2]primary fields len
0x02900200, // [1]SU src plug ID, [2]signal format: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type= IsoStream
0x00010002, // [2]# clusters, [2]# channels
0x0026810A, // [2]next info block len, [2]info block type Cluster info 
0x000B060B, // [2]primary fields len, [1]stream format = MBLA, [1]port type = AES/EBU
0x02000B00, // [1]# of signals, [2]music plug ID 11, [1]iso steam position
0x01000C01, // [1]Lfront, [2]music plug ID 12, [1]iso stream position
0x02001500, // [1]Rfront, [2]next info block len, [1]info block type
0x0B001100, // [1]info block type cont'd = Name, [2]primary fields len, [1]ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]max # of chars = not-defined, [1] next info block len
0x0B000A00, // [1]next info block len cont'd, [2]info block type Raw Text , [1]# of chars
0x07414553, // [1]# of char's cont'd, [3]name data starts
0x20303200, // [3]= "AES 02", [1]text terminator

// su src plug 3
0x00348109, // [2]next info block len = 52, [2]info block type SU Plug info
0x00080390, // [2]primary fields len, [1]SU src plug ID, [1]fmt
0x02000001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = IsoStream, [2]# clusters
0x00020026, // [2]# channels, [2]next info block len = 38
0x810A000B, // [2]info block type Cluster info, [2]primary fields len
0x060B0200, // [1]stream format = MBLA, [1]port type = AES/EBU, [1]# of signals, [1]id
0x0D000100, // [1]id cont'd = music plug ID 13, [1]iso stream position, [1]Lfront, [1]id
0x0E010200, // [1]id cont's = music plug ID 14, [1]iso stream position, [1] Rfront, [1]next info block len
0x15000B00, // [1]next info block len cont'd, [2]info block type Name , [1]len
0x110000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000B00, // [1]max # of chars cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000741, // [1]info block type cont's = Raw Text, [2]# of char's, [1]name data starts
0x45532030, 
0x33000034, // [1]= "AES 03", [1]text terminator, [2]next info block len = 52

// su src plug 4
0x81090008, // [2]info block type SU Plug info, [2]primary fields len
0x04900200, // [1]SU src plug ID, [2]signal format: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = IsoStream
0x00010002, // [2]# clusters, [2]# channels
0x0026810A, // [2]next info block len = 38, [2]info block type Cluster info
0x000B060B, // [2]primary fields len, [1]stream format = MBLA, [1]port type = AES/EBU
0x02000F00, // [1]# of signals, [2]music plug ID 15, [1]iso stream position
0x01001001, // [1]Lfront, [2]music plug ID 16, [1]iso stream position
0x02001500, // [1]Rfront, [2]next info block len, [1]info block type
0x0B001100, // [1]info block type Name cont'd, [2]primary fields len, [1]ref type = immediate
0x00FFFF00, // [1]name data attributes = read only, stored here in the SU, [2]FFFF max # of chars = not-defined, [1]info block len
0x0B000A00, // [1]next info block len cont'd, [2]info block type Raw Text , [1]# of chars
0x07414553, // [1]# of char's cont'd, [3]name date starts
0x20303400, // [3]= "AES 04", text terminator

// su src plug 5
0x002E8109, // [2]next info block len = 46, [2]info block type SU Plug info
0x00080590, // [2]primary fields len, [1]SU src plug ID, [1]fmt
0x02020001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = Midi, [2]# clusters
0x00010020, // [2]# channels, [2]next info block len = 32
0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x0D0A0100, // [1]stream format = MIDI, [1]port type = MIDI, [1]# of signals, [1]id
0x11000100, // [1]id cont'd = music plug ID 17, [1]iso stream position, [1]Lfront, [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000900, // [1]max # of chars cont's = not-defined, [2]next info block len, info block type
0x0A00054D, // [1]info block type cont'd = Raw Text, [2]# of char's, [1]name data starts
0x69646900, // [3]= "Midi", [1]text terminator

// su src plug 6
0x002E8109, // [2]next info block len = 46, [2]info block type SU Plug info
0x00080690, // [2]primary fields len, [1]SU src plug ID, [1]fmt
0x02030001, // [1]signal format cont'd: FMT = 0x90 AM824, FDF = 0x02 (48k), [1]plug type = Sync, [2]# clusters
0x00020020, // [2]# channels, [2]next info block len = 32
0x810A0007, // [2]info block type Cluster info, [2]primary fields len
0x40090100, // [1]stream format = SYNC Stream, [1]port type = Digital, [1]# of signals, [1]id
0x12000100, // [1]id cont'd = music plug ID = 18, [1]iso stream position, [1]Lfront, [1]next info block len
0x13000B00, // [1]next info block len cont'd, [2]info block type Name, [1]len
0x0F0000FF, // [1]primary fields len cont'd, [1]ref type = immediate, [1]name data attributes = read only, stored here in the SU, [1]max # of chars
0xFF000900, // [1]max # of chars cont'd = not-defined, [2]next info block len, [1]info block type
0x0A000553, // [1]info block type cont's = Raw Text, [2]# char's, [1]name data starts
0x796E6300, // [3]= "Sync", [1]text terminator


//	Music_plug_info_blocks 
//	length = (19 blocks)*(18+2) = 380

// music plug 0
0x0012810B, // [2]next info block len = 18 (20 total), [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0000F001, // [1]id cont'd = music plug ID 0, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 1
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0002, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location 

// music plug 1
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0100F001, // [1]id cont'd = music plug ID 1, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 1
0xFF0102F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0101, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 1, [1]dst stream location

// music plug 2
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0200F002, // [1]id cont'd = music plug ID 2, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 2
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0201, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 2, [1]dst stream location

// music plug 3
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0300F002, // [1]id cont'd = music plug ID 3, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 2
0xFF0102F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0302, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 3, [1]dst stream location

// music plug 4
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0400F003, // [1]id cont'd = music plug ID 4, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 3
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0402, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 4, [1]dst stream location

// music plug 5
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0500F003, // [1]id cont'd = music plug ID 5, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 3
0xFF0102F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0501, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 5, [1]dst stream location

// music plug 6
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2] pri_len = 14, [1]music plug type = Audio, [1]id
0x0600F004, // [1]id cont'd = music plug ID 6, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 4
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0601, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 6, [1]dst stream location

// music plug 7
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0700F004, // [1]id cont'd = music plug ID 7, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 4
0xFF0102F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0702, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 7, [1]dst stream location

// music plug 8
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0100, // [1]pri_len = 14, [1]music plug type = MIDI, [1]id
0x0800F005, // [1]id cont'd = music plug ID 8, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 5
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x00FF0801, // [1]dst plug ID = 0, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 8, [1]dst stream location

// music plug 9
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0900F000, // [1]id cont'd = music plug ID 9, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0002F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x01FF0001, // [1]dst plug ID = 1, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location

// music plug 10
0x0012810B, // [2]next info block len = 18, [2]Info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0A00F000, // [1]id cont'd = music plug ID 10, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0101F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x01FF0102, // [1]dst plug ID = 1, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 1, [1]dst stream location

// music plug 11
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0B00F000, // [1]id cont'd = music plug ID 11, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0201F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x02FF0001, // [1]dst plug ID = 2, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location

// music plug 12
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0C00F000, // [1]id cont'd = music plug ID 12, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0302F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x02FF0102, // [1]dst plug ID = 2, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 1, [1]dst stream location

// music plug 13
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0D00F000, // [1]id cont'd = music plug ID 13, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0402F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x03FF0001, // [1]dst plug ID = 3, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location

// music plug 14
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0E00F000, // [1]id cont'd = music plug ID 14, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0501F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x03FF0102, // [1]dst plug ID = 3, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 1, [1]dst stream location

// music plug 15
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x0F00F000, // [1]id cont'd = music plug ID 15, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0601F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x04FF0001, // [1]dst plug ID = 4, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location

// music plug 16
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0000, // [2]pri_len = 14, [1]music plug type = Audio, [1]id
0x1000F000, // [1]id cont'd = music plug ID 16, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0802F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x04FF0102, // [1]dst plug ID = 4, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 1, [1]dst stream location

// music plug 17
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E0100, // [2]pri_len = 14, [1]music plug type = MIDI, [1]id
0x1100F000, // [1]id cont'd = music plug ID 17, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 0
0xFF0901F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x05FF0001, // [1]dst plug ID = 5, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location

// music plug 18
0x0012810B, // [2]next info block len = 18, [2]info block type = Music plug info
0x000E8000, // [2]pri_len = 14, [1]music plug type = Audio SYNC, [1]id
0x1200F006, // [1]id cont'd = music plug ID 18, [1]routing support, [1]src plg fn type = su_dst_plug, [1]src plug ID = 6
0xFF0001F1, // [1]src plug fn block ID, 0xff resvd, [1]src stream position, [1]src stream location, [1]dst plug fn type = su_src_plug
0x06FF0001, // [1]dst plug ID = 6, [1]dst plug fn block ID, 0xff resvd, [1]dst stream position = 0, [1]dst stream location
};
#endif //_AVC &&  _EDS && _DRD
