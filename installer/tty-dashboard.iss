[Setup]
AppName=TTY Dashboard
AppVersion=0.1.0
AppPublisher=Bon

DefaultDirName={autopf}\TTY Dashboard
DefaultGroupName=TTY Dashboard
DisableDirPage=auto
DisableProgramGroupPage=auto

UninstallDisplayName=TTY Dashboard
UninstallDisplayIcon={app}\tty-dashboard.exe

ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin

SetupIconFile=..\resources\terbon\bon.ico

OutputDir=..\release
OutputBaseFilename=TTY-Setup

Compression=lzma2
SolidCompression=yes
WizardStyle=modern

CloseApplications=yes
RestartApplications=no

VersionInfoCompany=Bon
VersionInfoDescription=TTY Dashboard Installer
VersionInfoProductName=TTY Dashboard
VersionInfoProductVersion=0.1.0.0
VersionInfoVersion=0.1.0.0

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; Flags: unchecked

[Files]
; Qt runtime, plugins, and application DLLs.
; Config files are packaged separately from the project's root configs folder.
Source: "..\dist\tty-dashboard\*"; Excludes: "\configs\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; Default application configuration.
; Existing config files are preserved during upgrades so user changes are not lost.
Source: "..\configs\*"; DestDir: "{app}\configs"; Flags: ignoreversion recursesubdirs createallsubdirs onlyifdoesntexist

; Application fonts and other assets.
Source: "..\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs

; Runtime resources such as themes, icons, and TerBon tools.
Source: "..\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\TTY Dashboard"; Filename: "{app}\tty-dashboard.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\TTY Dashboard"; Filename: "{app}\tty-dashboard.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\tty-dashboard.exe"; Description: "Launch TTY Dashboard"; WorkingDir: "{app}"; Flags: postinstall skipifsilent nowait runasoriginaluser
