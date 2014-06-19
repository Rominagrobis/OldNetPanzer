; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "NetPanzer"
#define MyAppVersion "0.8.5-test-1"
#define MyAppPublisher "NetPanzer Team"
#define MyAppURL "http://www.netpanzer.org/"
#define MyAppExeName "netpanzer.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{2A5E66D9-4E48-43A1-B269-010CD64E95EE}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=C:\users\krom\My Documents\program\netpanzer-svn\releases\win\netpanzer\COPYING
OutputDir=C:\users\krom\My Documents\program\netpanzer-svn\releases
OutputBaseFilename=netpanzer-{#MyAppVersion}
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\users\krom\My Documents\program\netpanzer-svn\releases\win\netpanzer\netpanzer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\users\krom\My Documents\program\netpanzer-svn\releases\win\netpanzer\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[INI]
Filename: "{app}\netpanzer.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://www.netpanzer.org"
Filename: "{app}\webbrowser.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://www.netpanzer.org/sb"

[Registry]
Root: HKCR; Subkey: "netpanzer"; ValueType: "string"; ValueData: "URL:netpanzer (NetPanzer, an online multiplayer tactical warfare game)"; Flags: uninsdeletekey
Root: HKCR; SubKey: "netpanzer"; ValueType: "string"; ValueName: "URL Protocol"; ValueData: ""; Flags: uninsdeletekey
Root: HKCR; SubKey: "netpanzer\DefaultIcon"; ValueType: "string"; ValueData: "{app}\netpanzer.ico"; Flags: uninsdeletekey
Root: HKCR; SubKey: "netpanzer\shell\open\command"; ValueType: "string"; ValueData: "{app}\netpanzer.exe -c %1"; Flags: uninsdeletekey

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\{#MyAppName} Debug"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Parameters: "-g"
Name: "{group}\{#MyAppName} Dedicated Server"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Parameters: "-d"
Name: "{group}\{#MyAppName} Dedicated Server Debug"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"; Parameters: "-d -g"
Name: "{group}\NetPanzer Homepage"; Filename: "{app}\netpanzer.url"
Name: "{group}\NetPanzer Web Server Browser"; Filename: "{app}\webbrowser.url"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\netpanzer.url"
Type: files; Name: "{app}\webbrowser.url"
Type: files; Name: "{app}\stdout.txt"
Type: files; Name: "{app}\stderr.txt"
