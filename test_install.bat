
@echo off

setlocal enableextensions enabledelayedexpansion

set /a count = 1
set /a line = 0

for /F %%A in (%localappdata%\openvr\openvrpaths.vrpath) do (
 if %%A=="runtime" set /a line = count+2
 set /a count += 1
)

set /a count = 1

for /F "tokens=*" %%A in (%localappdata%\openvr\openvrpaths.vrpath) do (
 if !count!==!line! set VRPATH=%%A
 set /a count += 1
)

set VRPATH=%VRPATH:"=%

set VRPATH=!VRPATH!\\bin\\win64\\vrpathreg.exe

echo !VRPATH!

CALL "%VRPATH%"