@ECHO OFF
SETLOCAL EnableExtensions EnableDelayedExpansion

SET "DRIVER_NAME=apriltagtrackers"

SET "DRIVER_PATH=%cd%\%DRIVER_NAME%"

REM first, find the path to the vrpathreg.exe

set /a count = 1
set /a line = 0

REM the path can be found in openvrpaths.vrpath, which should always be in %localappdata%

REM the file is in json, and the path is under the "runtime" field. We first find the line number of the "runtime" field

for /F %%A in (%localappdata%\openvr\openvrpaths.vrpath) do (
 if %%A=="runtime" set /a line = count+2
 set /a count += 1
)

set /a count = 1

REM then, we parse whole lines and save the path. It should be 2 fields under "runtime"

for /F "tokens=*" %%A in (%localappdata%\openvr\openvrpaths.vrpath) do (
 if !count!==!line! set VRPATH=%%A
 set /a count += 1
)

set VRPATH=%VRPATH:"=%

set VRPATHREG_EXE=!VRPATH!\\bin\\win64\\vrpathreg.exe

IF "%1"=="help" (
    ECHO Usage: uninstall_driver.bat ^[^<path to vrpathreg.exe^>^]
    ECHO ^<path to vrpathreg.exe^> defaults to "%VRPATHREG_EXE%"
    GOTO end
)

IF NOT "%1"=="" (
    SET "VRPATHREG_EXE=%1"
)

REM remove driver from older versions

IF NOT EXIST "%VRPATHREG_EXE%" (
    ECHO vrpathreg.exe not found: "%VRPATHREG_EXE%"
    GOTO end
)

IF EXIST "!VRPATH!\\drivers\\apriltagtrackers" (
    ECHO Found old driver install at "!VRPATH!\\drivers\\apriltagtrackers, removing

    RMDIR /S /Q "!VRPATH!\\drivers\\apriltagtrackers"	
)

REM clean up our driver installs
CALL "%VRPATHREG_EXE%" removedriver "%DRIVER_PATH%"
IF NOT "%errorlevel%"=="0" GOTO end

CALL "%VRPATHREG_EXE%" removedriverswithname "%DRIVER_NAME%"
IF NOT "%errorlevel%"=="0" GOTO end

REM display the current configuration, with the newly added driver
CALL "%VRPATHREG_EXE%" show

:end
ENDLOCAL
PAUSE
REM END OF FILE
