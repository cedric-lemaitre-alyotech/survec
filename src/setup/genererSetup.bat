@ECHO OFF

REM =====================================================
rem appel de l'environnement
if not exist ./../environnement.bat (
 	echo environnement.bat n'a pas ete trouve!!
 	GOTO :EOF
)
call ./../environnement.bat


REM =====================================================
rem nom de l'appli
set PRODUCTNAME=SURVEC

rem 32 ou 64 bit
if "%SOIXANTE_QUATRE%"=="1" (
	set PRODUCTBIT=64
	set MSVCP_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x64\Microsoft.VC120.CRT\msvcp120.dll"
	set MSVCR_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x64\Microsoft.VC120.CRT\msvcr120.dll"
) else (
	set PRODUCTBIT=32
	set MSVCP_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcp120.dll"
	set MSVCR_BIN_PATH="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\redist\x86\Microsoft.VC120.CRT\msvcr120.dll"
)

rem Version complète
set PRODUCTVERSION=1.0


REM =====================================================
rem lancement de l'exe
set PATH=%NSISDIR%;%PATH%
makensis.exe setup.nsi > setup_log.txt 2>&1 
