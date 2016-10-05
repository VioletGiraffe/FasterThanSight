REM this script must set QTDIR32 path to the root of the Qt folder. Example:
REM set QTDIR32=k:\Qt\5\5.4\msvc2013_opengl\

call set_qt_paths.bat
set VS_TOOLS_DIR=%VS140COMNTOOLS%

SETLOCAL

RMDIR /S /Q binaries\

pushd ..\
%QTDIR32%\bin\qmake.exe -tp vc -r
popd

call "%VS_TOOLS_DIR%VsDevCmd.bat" x86
msbuild ..\FasterThanSight.sln /t:Build /p:Configuration=Release;PlatformToolset=v140

xcopy /R /Y ..\bin\release\FasterThanSight.exe binaries\
xcopy /R /Y "3rdparty binaries"\Win\32\* binaries\

SETLOCAL
SET PATH=%QTDIR32%\bin\
%QTDIR32%\bin\windeployqt.exe --dir binaries\Qt --force --no-translations --release --no-compiler-runtime --no-angle binaries\FasterThanSight.exe
ENDLOCAL

xcopy /R /Y %SystemRoot%\SysWOW64\msvcp140.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\SysWOW64\vcruntime140.dll binaries\msvcr\
xcopy /R /Y "%programfiles(x86)%\Windows Kits\10\Redist\ucrt\DLLs\x86\*" binaries\msvcr\

del binaries\Qt\opengl*.*

ENDLOCAL

"c:\Program Files (x86)\Inno Setup 5\iscc" setup.iss