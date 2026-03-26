@echo off
setlocal
set OLD_EXE=stoneshard-helper.exe
set NEW_EXE=%OLD_EXE%.tmp

:WAIT
timeout /t 1 >nul
tasklist | find /i "stoneshard-helper.exe" >nul
if not errorlevel 1 goto WAIT

if exist "%NEW_EXE%" (
    if exist "%OLD_EXE%" del /F /Q "%OLD_EXE%" >nul
    ren "%NEW_EXE%" "%OLD_EXE%" >nul
#    if exist "%OLD_EXE%" start "" "%OLD_EXE%"
)

del "%~f0" >nul