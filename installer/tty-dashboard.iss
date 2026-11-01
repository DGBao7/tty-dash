[Setup]
AppName=TTY Dashboard
AppVersion=0.1.0
AppPublisher=Bon
DefaultDirName={autopf}\TTY Dashboard
DefaultGroupName=TTY Dashboard
UninstallDisplayIcon={app}\tty-dashboard.exe

ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

PrivilegesRequired=admin

OutputDir=..\release
OutputBaseFilename=TTY-Setup

Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Tasks]
Name: "desktopicon"; \
    Description: "Create a desktop shortcut"; \
    Flags: unchecked

[Files]
Source: "..\dist\tty-dashboard\*"; \
    DestDir: "{app}"; \
    Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\TTY Dashboard"; \
    Filename: "{app}\tty-dashboard.exe"

Name: "{autodesktop}\TTY Dashboard"; \
    Filename: "{app}\tty-dashboard.exe"; \
    Tasks: desktopicon

[Run]
Filename: "{app}\tty-dashboard.exe"; \
    Description: "Launch TTY Dashboard"; \
    Flags: nowait postinstall skipifsilent