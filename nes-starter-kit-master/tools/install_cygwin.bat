@echo off
echo This script will download the installer for cygwin, and start it with a list of packages that is needed for NES development.
echo It is released under the GPL - please be sure to read the terms before installing!
echo We download it manually, as cygwin is actively maintained, and you don't want older software with security vulnerabilities or bugs, right? :)

REM Quick-n-dirty script since Windows doesn't have a good standard way to download files w/o using powershell/etc, which you
REM might not have...
echo Downloading installer...
@echo on
mkdir cygwin_installer
cscript /nologo misc\wget_script.js https://www.cygwin.com/setup-x86_64.exe cygwin_installer\setup-x86_64.exe
@echo off
echo Done downloading; launching!
@echo on
cygwin_installer\setup-x86_64.exe --packages curl,make,wget,chere
