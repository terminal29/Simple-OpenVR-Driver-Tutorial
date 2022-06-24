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
    ECHO Usage: install_driver.bat ^[^<driver path^>^] ^[^<path to vrpathreg.exe^>^]
    ECHO ^<driver path^> defaults to "%DRIVER_PATH%"
    ECHO ^<path to vrpathreg.exe^> defaults to "%VRPATHREG_EXE%"
    GOTO end
)

IF NOT "%1"=="" (
    SET "DRIVER_PATH=%1"
)

IF NOT "%2"=="" (
    SET "VRPATHREG_EXE=%2"
)

IF NOT EXIST "%DRIVER_PATH%" (
    ECHO Driver install not found: "%DRIVER_PATH%"
    GOTO end
)
IF NOT EXIST "%VRPATHREG_EXE%" (
    ECHO vrpathreg.exe not found: "%VRPATHREG_EXE%"
    GOTO end
)

REM clean up our driver installs
CALL "%VRPATHREG_EXE%" removedriver "%DRIVER_PATH%"
IF NOT "%errorlevel%"=="0" GOTO end

CALL "%VRPATHREG_EXE%" removedriverswithname "%DRIVER_NAME%"
IF NOT "%errorlevel%"=="0" GOTO end

REM add the new driver
CALL "%VRPATHREG_EXE%" adddriver "%DRIVER_PATH%"
IF NOT "%errorlevel%"=="0" GOTO end

REM display the current configuration, with the newly added driver
CALL "%VRPATHREG_EXE%" show

:end
ENDLOCAL
PAUSE
REM END OF FILE
