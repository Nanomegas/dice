///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 - 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#include <Security/Authorization.h>
#include <Security/AuthorizationTags.h>
#include "../../../../interface/tcat_dice_myproduct.h"


#define PROMPT_FOR_REBOOT 0

#define AUTHFLAGS kAuthorizationFlagDefaults | kAuthorizationFlagPreAuthorize | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagExtendRights

#define FINDCOMMAND "/usr/bin/find /Library/Receipts -name \"" kTCAT_DICE_DRV_STR "*.pkg\" -print"
#define LSBOMCOMMAND "/usr/bin/lsbom -s \"%s/Contents/Archive.bom\" | /usr/bin/grep \"" kTCAT_DICE_DRV_STR "\""

#define FINDPACKAGE "/usr/sbin/pkgutil --packages 2>/dev/null | /usr/bin/grep \"" kTCAT_DICE_DRV_STR "\""
#define GETPACKAGEFILES "/usr/sbin/pkgutil --files \"%s\" 2>/dev/null | /usr/bin/grep \"" kTCAT_DICE_DRV_STR "\""

#define CAPTION "\p" kTCAT_DICE_USR_STR " Uninstaller"

#define NOTFOUNDTEXT "\pNo " kTCAT_DICE_USR_STR " installation was found!\n\nThis uninstaller recognizes driver versions 3.0.0 and later. If you are uninstalling versions earlier than that please consult the driver installation documents."


bool Authenticate( AuthorizationRef *pAuthorizationRef );


int main( int argc, char *argv[] )
{
	char	command[1024];
	char	returnBuf[1024];
	char	*rmArgs[] = { "-rf", "", NULL };
	SInt32	systemVersion = 0;
	SInt16	outItemHint = 0;
	FILE	*pFindPackageOutput = NULL;
	AlertStdAlertParamRec dialogParam;
	AuthorizationRef authorizationRef = NULL;
	bool	anythingFound = false;
	bool	usePkgUtil = false;
	OSErr	err;

	// retrieve the OS version
	if ((err = Gestalt( gestaltSystemVersion, &systemVersion )) == noErr)
	{
		fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: system version is 0x%lx\n", systemVersion );
		if (systemVersion >= 0x1060)
		{
			// starting with OS X 10.6 we need to use the pkgutil tool
			usePkgUtil = true;
		}
	}
	else
	{
		fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: unable to retrieve system version (error %d)\n", err );
	}

	memset( &dialogParam, 0, sizeof (dialogParam) );
	dialogParam.position = kWindowDefaultPosition;
	dialogParam.movable = true;
	dialogParam.cancelText = "\pNo";
	dialogParam.cancelButton = kAlertStdAlertCancelButton;
	dialogParam.defaultButton = kAlertStdAlertOKButton;
	dialogParam.defaultText = "\pYes";
	StandardAlert( kAlertCautionAlert, CAPTION,"\pThis will remove the " kTCAT_DICE_USR_STR " driver and software installation!\n\nDo you wish to proceed?", &dialogParam, &outItemHint );

	if (outItemHint != kAlertStdAlertOKButton)
		goto Exit;

	// First look for any receipts in "/Library/Receipts/", which is where receipts were recorded on OS X versions before 10.6.x.
	// This should still be checked on 10.6.x because old receipts could be present if OS X was upgraded from an older version.
	{
		snprintf( command, sizeof(command), FINDCOMMAND );
		fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: command: \"%s\"\n", command );

		pFindPackageOutput = popen( command, "r" );
		if (pFindPackageOutput)
		{
			while (fgets( returnBuf, sizeof(returnBuf), pFindPackageOutput ))
			{
				FILE	*fileListCommandOutput = NULL;
				size_t	returnBufLength = strlen(returnBuf);

				if ((returnBufLength > 0) && (returnBuf[returnBufLength-1] == '\n'))
				{
					returnBuf[returnBufLength-1] = '\0';
				}

				if (!authorizationRef)
				{
					// Let's pre-authenticate now!
					if (!Authenticate( &authorizationRef ))
					{
						fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: Could not Authenticate\n" );
						goto Exit;
					}
				}

				anythingFound = true;

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: Now Processing: %s\n", returnBuf );

				snprintf( command, sizeof(command), LSBOMCOMMAND, returnBuf );

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: command: \"%s\"\n", command );

				fileListCommandOutput = popen( command, "r" );
				if (fileListCommandOutput)
				{
					char fileListReturnBuf[1024];

					while (fgets( fileListReturnBuf, sizeof(fileListReturnBuf), fileListCommandOutput ))
					{
						size_t	fileListLength = strlen(fileListReturnBuf);

						if ((fileListLength > 0) && (fileListReturnBuf[fileListLength-1] == '\n'))
						{
							fileListReturnBuf[fileListLength-1] = 0;
						}

						if (fileListLength > 2)
						{
							if (strncmp ("./", fileListReturnBuf, 2) == 0)
							{
								snprintf( command, sizeof(command), "%s", &fileListReturnBuf[1] );
								rmArgs[1] = command;

								fprintf (stderr, kTCAT_DICE_USR_STR " Uninstaller:\tRemoving %s\n", command);

								AuthorizationExecuteWithPrivileges( authorizationRef, "/bin/rm", kAuthorizationFlagDefaults, rmArgs, NULL );
							}
							else
							{
								fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller:\tNot removing: %s\n", fileListReturnBuf );
							}
						}
					}

					pclose( fileListCommandOutput );
				}

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller:\tRemoving package receipt for %s\n", returnBuf );

				snprintf( command, sizeof(command), "%s", returnBuf );
				rmArgs[1] = command;

				AuthorizationExecuteWithPrivileges( authorizationRef, "/bin/rm", kAuthorizationFlagDefaults, rmArgs, NULL );
			}

			pclose( pFindPackageOutput );
		}
	}

	if (usePkgUtil)
	{
		// On OS X 10.6.x and later, use pkgutil

		snprintf( command, sizeof(command), FINDPACKAGE );
		fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: command: \"%s\"\n", command );

		pFindPackageOutput = popen( command, "r" );
		if (pFindPackageOutput)
		{
			while (fgets( returnBuf, sizeof(returnBuf), pFindPackageOutput ))
			{
				FILE	*fileListCommandOutput = NULL;
				size_t	returnBufLength = strlen(returnBuf);

				if ((returnBufLength > 0) && (returnBuf[returnBufLength-1] == '\n'))
				{
					returnBuf[returnBufLength-1] = '\0';
				}

				if (!authorizationRef)
				{
					// Let's pre-authenticate now!
					if (!Authenticate( &authorizationRef ))
					{
						fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: Could not Authenticate\n" );
						goto Exit;
					}
				}

				anythingFound = true;

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: Now Processing: %s\n", returnBuf );

				snprintf( command, sizeof(command), GETPACKAGEFILES, returnBuf );

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: command: \"%s\"\n", command );

				fileListCommandOutput = popen( command, "r" );
				if (fileListCommandOutput)
				{
					char fileListReturnBuf[1024];

					while (fgets( fileListReturnBuf, sizeof(fileListReturnBuf), fileListCommandOutput ))
					{
						size_t	fileListLength = strlen(fileListReturnBuf);

						if ((fileListLength > 0) && (fileListReturnBuf[fileListLength-1] == '\n'))
						{
							fileListReturnBuf[fileListLength-1] = 0;
						}

						if (fileListLength > 1)
						{
							snprintf (command, sizeof(command), "/%s", fileListReturnBuf);
							rmArgs[1] = command;

							fprintf (stderr, kTCAT_DICE_USR_STR " Uninstaller:\tRemoving %s\n", command);

							AuthorizationExecuteWithPrivileges( authorizationRef, "/bin/rm", kAuthorizationFlagDefaults, rmArgs, NULL );
						}
					}
					pclose( fileListCommandOutput );
				}

				fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller:\tRemoving package receipt for %s\n", returnBuf );

				char *forgetArgs[] = { "--forget", returnBuf, NULL };

				AuthorizationExecuteWithPrivileges( authorizationRef, "/usr/sbin/pkgutil", kAuthorizationFlagDefaults, forgetArgs, NULL );
			}

			pclose( pFindPackageOutput );
		}
	}

	if (anythingFound)
	{
#if PROMPT_FOR_REBOOT
		memset (&dialogParam, 0, sizeof (dialogParam));
		dialogParam.position = kWindowDefaultPosition;
		dialogParam.movable = TRUE;
		dialogParam.cancelText = "\pNo";
		dialogParam.cancelButton = kAlertStdAlertCancelButton;
		dialogParam.defaultButton = kAlertStdAlertOKButton;
		dialogParam.defaultText = "\pYes";
		StandardAlert( kAlertCautionAlert, CAPTION,"\pThe system must be restarted to complete this operation.\nDo you wish to restart now?", &dialogParam, &outItemHint );

		if (outItemHint == kAlertStdAlertOKButton)
		{
			char	*shutdownArgs[] = { "-r", "now", NULL };

			// restart
			AuthorizationExecuteWithPrivileges( authorizationRef, "/sbin/shutdown", kAuthorizationFlagDefaults, shutdownArgs, NULL );
		}
#else
		StandardAlert( kAlertCautionAlert, CAPTION,"\p" kTCAT_DICE_USR_STR " drivers have been removed.", NULL, &outItemHint );
#endif
	}
	else
	{
		StandardAlert( kAlertCautionAlert, CAPTION, NOTFOUNDTEXT, NULL, &outItemHint );
		fprintf( stderr, kTCAT_DICE_USR_STR " Uninstaller: No " kTCAT_DICE_USR_STR " installation was found\n" );
	}

Exit:		
	if (authorizationRef)
	{
		AuthorizationFree( authorizationRef, kAuthorizationFlagDefaults );
		authorizationRef = NULL;
	}

	return 0;
}



/*------------------------------------------------------------
*	Authenticate
*
*	Parameters:  
*		pAuthorizationRef - pointer to receive Authorization ref.
*
*	Globals Used:
*		None
*
*	Description:
*		This routine calls on the Authorization APIs to pre-authenticate.
*		This allows the caller to use the Authorization ref to execute
*		commands as root.
*
*	Returns:
*		true on success, false on failure
*
*------------------------------------------------------------*/	
bool Authenticate( AuthorizationRef *pAuthorizationRef )
{
	AuthorizationItem myItems = { kAuthorizationRightExecute, 0, NULL, 0 };
	AuthorizationRights myRights = { 1, &myItems };
	AuthorizationFlags flags = AUTHFLAGS;
	OSStatus status;

	// Create a new authorization reference which will later be passed to the tool
	*pAuthorizationRef = NULL;
	status = AuthorizationCreate( NULL, kAuthorizationEmptyEnvironment, flags, pAuthorizationRef );
	if (status != errAuthorizationSuccess)
	{
		return false;
	}
    
	// This shows how AuthorizationCopyRights() can be used in order to pre-authorize the user before
	// attempting to perform the privileged operation.  Pre-authorization is optional but can be useful in
	// certain situations.  For example, in the Installer application, the user is asked to pre-authorize before
	// configuring the installation because it would be a waste of time to let the user proceed through the
	// entire installation setup, only to be denied at the final stage because they weren't the administrator. */

	status = AuthorizationCopyRights( *pAuthorizationRef, &myRights, kAuthorizationEmptyEnvironment, flags, NULL );

	return (status == errAuthorizationSuccess);
}

