///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// This is a generic InnoSetup file and needs to be preprocessed:
//
// "..\..\customize.py generic.iss ..\..\..\interface\tcat_dice_defaults.h custom.iss"
//
// This file will not compile unprocessed!
//
///////////////////////////////////////////////////////////////////////////////

[Setup]
AppName=kTCAT_DICE_USR
AppVerName=kTCAT_DICE_USR kTCAT_DICE_VERSION
AppVersion=kTCAT_DICE_VERSION
AppPublisher=kTCAT_DICE_VENDOR
;AppPublisherURL={#MyAppPublisherURL}
;AppSupportURL={#MyAppPublisherURL}
DefaultDirName={pf}\kTCAT_DICE_INSTALL_DEFAULT_DIR_NAME
DefaultGroupName=kTCAT_DICE_INSTALL_DEFAULT_GROUP_NAME
DisableProgramGroupPage=yes
VersionInfoVersion=kTCAT_DICE_VERSION
VersionInfoCopyright=kTCAT_DICE_COPYRIGHT
OutputDir=..\..\..\binary\install\win\[BUILDTYPE]
OutputBaseFilename=kTCAT_DICE_DRVInstaller
Compression=lzma
SolidCompression=yes
AlwaysRestart=yes
ArchitecturesInstallIn64BitMode=x64
SetupIconFile=..\..\..\resource\icon.ico
WizardImageFile=..\..\..\resource\installer_logo.bmp
WizardImageStretch=no
;WizardSmallImageFile=..\..\..\resource\icon.bmp
// this prevents installation on anything before Windows XP
MinVersion=0,5.01

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
Source: ..\..\..\docs\public\drv\tcat_dice_drv_win_release_notes.html; DestDir: {app};
Source: ..\..\..\resource\cpl.defs; DestDir: {app}; flags: skipifsourcedoesntexist;
Source: ..\..\..\resource\eap_mixer.xml; DestDir: {app}; flags: skipifsourcedoesntexist;
Source: ..\..\..\resource\logo.png; DestDir: {app}; flags: skipifsourcedoesntexist;
; 32-bit
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_INF; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_CAT; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_SYS; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_AUDIO_SYS; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_MIDI_SYS; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_ASIO_DLL; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\tools\cpl\win32\release\kTCAT_DICE_DRV_CPL_EXE; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\tools\cpl\win32\release\kTCAT_DICE_DRV_CPL; DestDir: {sys}; flags: ignoreversion; Check: not x64
; 64-bit
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_INF; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_CAT; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_SYS; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_AUDIO_SYS; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_MIDI_SYS; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_ASIO_DLL; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\kTCAT_DICE_DRV_ASIO_DLL; DestDir: {sys}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\win32\release\kTCAT_DICE_DRV_ASIO_DLL; DestDir: {syswow64}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\tools\cpl\x64\release\kTCAT_DICE_DRV_CPL_EXE; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\tools\cpl\x64\release\kTCAT_DICE_DRV_CPL; DestDir: {sys}; flags: ignoreversion; Check: x64
; Installer
Source: ..\..\..\binary\drv\win32\release\Uninstall.dll; DestDir: {app}; flags: ignoreversion;
Source: ..\..\..\binary\drv\win32\release\DIFxAPI.dll; DestDir: {sys}; flags: onlyifdoesntexist; Check: vista and not x64
Source: ..\..\..\binary\drv\x64\release\DIFxAPI.dll; DestDir: {sys}; flags: onlyifdoesntexist; Check: vista and x64
Source: ..\..\..\binary\drv\x64\release\Vista64Install.exe; DestDir: {app}; flags: ignoreversion; Check: vista and x64

[Run]
Filename: {app}\tcat_dice_drv_win_release_notes.html; Flags: shellexec

[Icons]
Name: {group}\kTCAT_DICE_USR_FS_SAFE; Filename: {app}\kTCAT_DICE_DRV_CPL_EXE
Name: {group}\{cm:UninstallProgram, kTCAT_DICE_USR_FS_SAFE}; Filename: {uninstallexe}
Name: {userdesktop}\kTCAT_DICE_USR_FS_SAFE; Filename: {app}\kTCAT_DICE_DRV_CPL_EXE; Tasks: desktopicon

[Code]
type
  _OSVERSIONINFOA = record
    dwOSVersionInfoSize: DWORD;
    dwMajorVersion: DWORD;
    dwMinorVersion: DWORD;
    dwBuildNumber: DWORD;
    dwPlatformId: DWORD;
    szCSDVersion: array[0..127] of Char; { Maintenance string for PSS usage }
  end;
TOSVERSIONINFO = _OSVERSIONINFOA;

function GetVersionExA (var lpVersionInformation: TOSVersionInfo): bool; external 'GetVersionExA@Kernel32.dll stdcall delayload';
function Wow64EnableWow64FsRedirection (Wow64FsEnableRedirection: bool):bool; external 'Wow64EnableWow64FsRedirection@Kernel32.dll stdcall delayload';
function DriverPackageInstallA (DriverPackageInfPath: string; flags: DWORD; pInstallerInfo: DWORD; var pNeedReboot: bool): DWORD; external 'DriverPackageInstallA@Difxapi.dll stdcall delayload';
function DriverPackageUninstallA (DriverPackageInfPath: string; flags: DWORD; pInstallerInfo: DWORD; var pNeedReboot: bool): DWORD; external 'DriverPackageUninstallA@Difxapi.dll stdcall delayload';
function SetupCopyOEMInf (SourceInfFileName, OEMSourceMediaLocation: string; OEMSourceMediaType, CopyStyle: DWORD; DestinationInfFileName: string; DestinationInfFileNameSize: DWORD; var RequiredSize: DWORD; DestinationInfFileNameComponent: string): bool; external 'SetupCopyOEMInfA@setupapi.dll stdcall';
function FreeLibrary(hLibModule: LongWord): bool; external 'FreeLibrary@Kernel32.dll stdcall delayload';

function DeleteReadOnlyFile (fileName: string): bool; external 'DeleteReadOnlyFile@Uninstall.dll stdcall delayload';
function DeleteThisService (serviceName: string): bool; external 'DeleteThisService@Uninstall.dll stdcall delayload';
function FindAndRemoveExistingDevice(hardwareID: string): bool; external 'FindAndRemoveExistingDevice@Uninstall.dll stdcall delayload';
function SearchAndRemoveInf(searchString, keyName, sectionName: string): bool; external 'SearchAndRemoveInf@Uninstall.dll stdcall delayload';



///////////////////////////////////////////////////////////////////////////////
//
// vista
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: This routine determines if the OS is Vista or not.
//
///////////////////////////////////////////////////////////////////////////////
//
function vista: boolean;
var
  osVerInfo: TOSVERSIONINFO;
begin
  result := false;
  osVerInfo.dwOSVersionInfoSize := sizeof (osVerInfo);
  GetVersionExA (osVerInfo);
  if osVerInfo.dwMajorVersion = 6 then
    result := true;
end;



///////////////////////////////////////////////////////////////////////////////
//
// x64
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: This routine finds out if installation is running in 64 bit mode.
//
///////////////////////////////////////////////////////////////////////////////
//
function x64: boolean;
begin
  result := Is64BitInstallMode and (ProcessorArchitecture = paX64);
end;



///////////////////////////////////////////////////////////////////////////////
//
// remove
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: This routine takes care of removing services, files and registry entries for the driver.
//
///////////////////////////////////////////////////////////////////////////////
//
procedure remove();
var
  sys: string;
  app: string;
  reboot: bool;
  dll_name: string;
  dll_name_prev: string;
  dll_handle: longword;
  error: integer;
begin
  app := ExpandConstant('{app}') + '\';
  sys := ExpandConstant('{sys}') + '\';
  dll_name := app + 'Uninstall.dll';
  dll_name_prev := sys + 'kTCAT_DICE_DRVUninstall.dll';
  FileCopy (dll_name_prev, dll_name, true);
  DeleteFile (dll_name_prev);
  dll_handle := LoadDLL (dll_name, error);
  if (dll_handle <> 0)then begin
    if (x64) then begin
      sys := ExpandConstant('{syswow64}') + '\';
      UnregisterServer (true, app + 'kTCAT_DICE_DRV_ASIO_DLL', false);
      UnregisterServer (false, ExpandConstant('{sys}') + '\kTCAT_DICE_DRV_ASIO_DLL', false);
      Wow64EnableWow64FsRedirection (false);
    end else begin
      UnregisterServer (false, 'kTCAT_DICE_DRV_ASIO_DLL', false);
    end;

    DeleteReadOnlyFile (sys + 'drivers\kTCAT_DICE_DRV_SYS');
    DeleteReadOnlyFile (sys + 'drivers\kTCAT_DICE_DRV_AUDIO_SYS');
    DeleteReadOnlyFile (sys + 'drivers\kTCAT_DICE_DRV_MIDI_SYS');
    DeleteReadOnlyFile (sys + 'kTCAT_DICE_DRV_ASIO_DLL');

    DeleteThisService ('kTCAT_DICE_DRV');
    DeleteThisService ('kTCAT_DICE_DRV_AUDIO');
    DeleteThisService ('kTCAT_DICE_DRV_MIDI');

    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\Audio&Adapter01');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\Audio&Adapter02');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\Audio&Adapter03');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\Audio&Adapter04');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\Audio&Adapter05');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\MidiIn');
    FindAndRemoveExistingDevice ('kTCAT_DICE_DRV\MidiOut');

    #ifdef kTCAT_DICE_DEVICE_1_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_1_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_2_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_2_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_3_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_3_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_4_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_4_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_5_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_5_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_6_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_6_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_7_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_7_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_8_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_8_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_9_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_9_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_10_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_10_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_11_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_11_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_12_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_12_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_13_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_13_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_14_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_14_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_15_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_15_MODEL_ID');
    #endif
    #ifdef kTCAT_DICE_DEVICE_16_MODEL_ID
    FindAndRemoveExistingDevice ('1394\kTCAT_DICE_VENDOR_SHORT&kTCAT_DICE_DEVICE_16_MODEL_ID');
    #endif

    if (x64) then
      Wow64EnableWow64FsRedirection(true);

    if (vista) then begin
      if (x64) then begin
        Exec( app + 'Vista64Install.exe', 'U kTCAT_DICE_DRV_INF', app, SW_HIDE, ewWaitUntilTerminated, error );
      end else begin
        reboot := false;
        // passing flags 0x24 (DRIVER_PACKAGE_DELETE_FILES | DRIVER_PACKAGE_FORCE)
        DriverPackageUninstallA (app + 'kTCAT_DICE_DRV_INF', 36, 0, reboot);
      end;
    end;

    SearchAndRemoveInf ('kTCAT_DICE_USR_FS_SAFE', 'Strings', 'DESCRIPTION_PRODUCT_LINE');

    FreeLibrary (dll_handle);
    FreeLibrary (dll_handle);
    DeleteFile (dll_name);
  end;
end;



///////////////////////////////////////////////////////////////////////////////
//
// CurStepChanged
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: This routine occurs on the current step change event.
//
///////////////////////////////////////////////////////////////////////////////
//
procedure CurStepChanged(CurStep: TSetupStep);
var
  app: string;
  inf: string;
  reboot: bool;
  dummy: string;
  size: DWORD;
  error: integer;
begin
  if CurStep = ssInstall then
    remove();

  if CurStep = ssPostInstall then begin
    app := ExpandConstant('{app}') + '\';
    inf := app + 'kTCAT_DICE_DRV_INF';
    if (vista) then begin
      if (x64) then begin
         Exec( app + 'Vista64Install.exe', 'I kTCAT_DICE_DRV_INF', app, SW_HIDE, ewWaitUntilTerminated, error );
      end else begin
        reboot := false;
        // passing flags 0x14 (DRIVER_PACKAGE_LEGACY_MODE | DRIVER_PACKAGE_FORCE)
        DriverPackageInstallA (inf, 20, 0, reboot);
      end;
    end else begin
      SetupCopyOEMInf (inf, ExpandConstant('{app}'), 1, 1, dummy, 0, size, dummy);
    end;

    RegDeleteKeyIncludingSubkeys (HKEY_LOCAL_MACHINE, 'Software\ASIO\kTCAT_DICE_DRV');
    if (x64) then begin
      RegDeleteKeyIncludingSubkeys (HKEY_LOCAL_MACHINE_32, 'Software\ASIO\kTCAT_DICE_DRV');
      RegisterServer(true,  ExpandConstant('{app}') + '\kTCAT_DICE_DRV_ASIO_DLL', false);
      RegisterServer(false, ExpandConstant('{sys}') + '\kTCAT_DICE_DRV_ASIO_DLL', false);
      RegWriteStringValue (HKEY_LOCAL_MACHINE_32,
        'Software\ASIO\kTCAT_DICE_DRV',
        'kTCAT_DICE_CPL_PATH',
        ExpandConstant('{app}') + '\kTCAT_DICE_DRV_CPL_EXE');
    end else begin
      RegisterServer(false, ExpandConstant('{app}') + '\kTCAT_DICE_DRV_ASIO_DLL', false);
    end;

    RegWriteStringValue (HKEY_LOCAL_MACHINE,
      'Software\ASIO\kTCAT_DICE_DRV',
      'kTCAT_DICE_CPL_PATH',
      ExpandConstant('{app}') + '\kTCAT_DICE_DRV_CPL_EXE');
  end;
end;



///////////////////////////////////////////////////////////////////////////////
//
// CurUninstallStepChanged
//
///////////////////////////////////////////////////////////////////////////////
//
// Description: This routine occurs on the current step change event.
//
///////////////////////////////////////////////////////////////////////////////
//
procedure CurUninstallStepChanged (CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
    remove();
end;


