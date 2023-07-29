; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

;修改文件名称
#define MyAppName "期末冲刺"
;修改版本号
#define MyAppVersion "v2.1.3"
#define MyAppPublisher "周霖"
#define MyAppExeName "PracticeQuestions.exe"

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (若要生成新的 GUID，可在菜单中点击 "工具|生成 GUID"。)
AppId={{426F8E95-2CF1-461D-8383-54C8BF74786A}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
; 移除以下行，以在管理安装模式下运行（为所有用户安装）。
PrivilegesRequired=admin

;创建安装包后的存放位置
OutputDir=D:\SharedFolders

;创建安装包的名字
OutputBaseFilename=mysetup

;图标为止
SetupIconFile=D:\SharedFolders\app.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone;

[Files]
;需要打包的应用程序的路径
Source: "D:\SharedFolders\1\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

;需要打包的文件夹
Source: "D:\SharedFolders\1\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall 