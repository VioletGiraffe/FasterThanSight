[Setup]
AppName=Faster Than Sight
AppId=FasterThanSight
AppVerName=Faster Than Sight
DefaultDirName={pf}\Faster Than Sight
DefaultGroupName=Faster Than Sight
AllowNoIcons=true
OutputDir=.
OutputBaseFilename=FasterThanSight
UsePreviousAppDir=yes

SetupIconFile=..\app\resources\FTS_icon.ico
UninstallDisplayIcon={app}\FasterThanSight.exe

WizardImageBackColor=clWhite
ShowTasksTreeLines=yes

SolidCompression=true
LZMANumBlockThreads=4
Compression=lzma2/ultra64
LZMAUseSeparateProcess=yes
LZMABlockSize=8192

[Files]

;App binaries
Source: binaries/*; DestDir: {app}; Flags: ignoreversion

;Qt binaries
Source: binaries/Qt/*; DestDir: {app}; Flags: ignoreversion recursesubdirs

;MSVC binaries
Source: binaries/msvcr/*; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {group}\Faster Than Sight; Filename: {app}\FasterThanSight.exe;
Name: {group}\{cm:UninstallProgram,Faster Than Sight}; Filename: {uninstallexe}

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons};

[Run]
Filename: {app}\FasterThanSight.exe; Description: {cm:LaunchProgram,Faster Than Sight}; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: dirifempty; Name: "{app}"