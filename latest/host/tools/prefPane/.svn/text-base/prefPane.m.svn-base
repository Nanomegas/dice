///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
//
#include "tcat_dice_myproduct.h"
#import <PreferencePanes/PreferencePanes.h>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@interface PreferencePanePref: NSPreferencePane 
{}
- (void) didSelect;
@end

@implementation PreferencePanePref
- (void) didSelect;
{
	[[NSWorkspace sharedWorkspace] launchApplication:@"/Applications/" kTCAT_DICE_DRV_STR " Control Panel.app"];
}
@end
