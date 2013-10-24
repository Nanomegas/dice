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
AppName=TCAT Dice EVM Platform
AppVerName=TCAT Dice EVM Platform 4.1.3.17357
AppVersion=4.1.3.17357
AppPublisher=TC Applied Technologies
;AppPublisherURL={#MyAppPublisherURL}
;AppSupportURL={#MyAppPublisherURL}
DefaultDirName={pf}\TcatDice
DefaultGroupName=TC Applied Technologies
DisableProgramGroupPage=yes
VersionInfoVersion=4.1.3.17357
VersionInfoCopyright=(c) TC Applied Technologies 2004-2013
OutputDir=..\..\..\binary\install\win\Release
OutputBaseFilename=TcatDiceInstaller
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
Source: ..\..\..\binary\drv\win32\release\TcatDice.inf; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\TcatDice.cat; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\TcatDice.sys; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\TcatDiceAudio.sys; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\TcatDiceMidi.sys; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\drv\win32\release\TcatDiceAsio.dll; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\tools\cpl\win32\release\TcatDice.exe; DestDir: {app}; flags: ignoreversion; Check: not x64
Source: ..\..\..\binary\tools\cpl\win32\release\TcatDice.cpl; DestDir: {sys}; flags: ignoreversion; Check: not x64
; 64-bit
Source: ..\..\..\binary\drv\x64\release\TcatDice.inf; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDice.cat; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDice.sys; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDiceAudio.sys; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDiceMidi.sys; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDiceAsio.dll; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\x64\release\TcatDiceAsio.dll; DestDir: {sys}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\drv\win32\release\TcatDiceAsio.dll; DestDir: {syswow64}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\tools\cpl\x64\release\TcatDice.exe; DestDir: {app}; flags: ignoreversion; Check: x64
Source: ..\..\..\binary\tools\cpl\x64\release\TcatDice.cpl; DestDir: {sys}; flags: ignoreversion; Check: x64
; Installer
Source: ..\..\..\binary\drv\win32\release\Uninstall.dll; DestDir: {app}; flags: ignoreversion;
Source: ..\..\..\binary\drv\win32\release\DIFxAPI.dll; DestDir: {sys}; flags: onlyifdoesntexist; Check: vista and not x64
Source: ..\..\..\binary\drv\x64\release\DIFxAPI.dll; DestDir: {sys}; flags: onlyifdoesntexist; Check: vista and x64
Source: ..\..\..\binary\drv\x64\release\Vista64Install.exe; DestDir: {app}; flags: ignoreversion; Check: vista and x64

[Run]
Filename: {app}\tcat_dice_drv_win_release_notes.html; Flags: shellexec

[Icons]
Name: {group}\TCAT Dice EVM Platform; Filename: {app}\TcatDice.exe
Name: {group}\{cm:UninstallProgram, TCAT Dice EVM Platform}; Filename: {uninstallexe}
Name: {userdesktop}\TCAT Dice EVM Platform; Filename: {app}\TcatDice.exe; Tasks: desktopicon

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
  dll_name_prev := sys + 'TcatDiceUninstall.dll';
  FileCopy (dll_name_prev, dll_name, true);
  DeleteFile (dll_name_prev);
  dll_handle := LoadDLL (dll_name, error);
  if (dll_handle <> 0)then begin
    if (x64) then begin
      sys := ExpandConstant('{syswow64}') + '\';
      UnregisterServer (true, app + 'TcatDiceAsio.dll', false);
      UnregisterServer (false, ExpandConstant('{sys}') + '\TcatDiceAsio.dll', false);
      Wow64EnableWow64FsRedirection (false);
    end else begin
      UnregisterServer (false, 'TcatDiceAsio.dll', false);
    end;

    DeleteReadOnlyFile (sys + 'drivers\TcatDice.sys');
    DeleteReadOnlyFile (sys + 'drivers\TcatDiceAudio.sys');
    DeleteReadOnlyFile (sys + 'drivers\TcatDiceMidi.sys');
    DeleteReadOnlyFile (sys + 'TcatDiceAsio.dll');

    DeleteThisService ('TcatDice');
    DeleteThisService ('TcatDiceAudio');
    DeleteThisService ('TcatDiceMidi');

    FindAndRemoveExistingDevice ('TcatDice\Audio&Adapter01');
    FindAndRemoveExistingDevice ('TcatDice\Audio&Adapter02');
    FindAndRemoveExistingDevice ('TcatDice\Audio&Adapter03');
    FindAndRemoveExistingDevice ('TcatDice\Audio&Adapter04');
    FindAndRemoveExistingDevice ('TcatDice\Audio&Adapter05');
    FindAndRemoveExistingDevice ('TcatDice\MidiIn');
    FindAndRemoveExistingDevice ('TcatDice\MidiOut');

    FindAndRemoveExistingDevice ('1394\TCAT&DICE_II_Evaluation_Board');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_EVM002_Evaluation_Board');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_II_Evaluation_Board_EAP');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_EVM002_Evaluation_Board_EAP');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_EVM003_Evaluation_Board');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_EVM003_Evaluation_Board_EAP');
    FindAndRemoveExistingDevice ('1394\TCAT&DICE_EVM004_FW843_Evaluation_Board');

    if (x64) then
      Wow64EnableWow64FsRedirection(true);

    if (vista) then begin
      if (x64) then begin
        Exec( app + 'Vista64Install.exe', 'U TcatDice.inf', app, SW_HIDE, ewWaitUntilTerminated, error );
      end else begin
        reboot := false;
        // passing flags 0x24 (DRIVER_PACKAGE_DELETE_FILES | DRIVER_PACKAGE_FORCE)
        DriverPackageUninstallA (app + 'TcatDice.inf', 36, 0, reboot);
      end;
    end;

    SearchAndRemoveInf ('TCAT Dice EVM Platform', 'Strings', 'DESCRIPTION_PRODUCT_LINE');

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
    inf := app + 'TcatDice.inf';
    if (vista) then begin
      if (x64) then begin
         Exec( app + 'Vista64Install.exe', 'I TcatDice.inf', app, SW_HIDE, ewWaitUntilTerminated, error );
      end else begin
        reboot := false;
        // passing flags 0x14 (DRIVER_PACKAGE_LEGACY_MODE | DRIVER_PACKAGE_FORCE)
        DriverPackageInstallA (inf, 20, 0, reboot);
      end;
    end else begin
      SetupCopyOEMInf (inf, ExpandConstant('{app}'), 1, 1, dummy, 0, size, dummy);
    end;

    RegDeleteKeyIncludingSubkeys (HKEY_LOCAL_MACHINE, 'Software\ASIO\TcatDice');
    if (x64) then begin
      RegDeleteKeyIncludingSubkeys (HKEY_LOCAL_MACHINE_32, 'Software\ASIO\TcatDice');
      RegisterServer(true,  ExpandConstant('{app}') + '\TcatDiceAsio.dll', false);
      RegisterServer(false, ExpandConstant('{sys}') + '\TcatDiceAsio.dll', false);
      RegWriteStringValue (HKEY_LOCAL_MACHINE_32,
        'Software\ASIO\TcatDice',
        'Control Panel Binary Path',
        ExpandConstant('{app}') + '\TcatDice.exe');
    end else begin
      RegisterServer(false, ExpandConstant('{app}') + '\TcatDiceAsio.dll', false);
    end;

    RegWriteStringValue (HKEY_LOCAL_MACHINE,
      'Software\ASIO\TcatDice',
      'Control Panel Binary Path',
      ExpandConstant('{app}') + '\TcatDice.exe');
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


