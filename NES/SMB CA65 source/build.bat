@echo Please wait..
.\bin\cl65 -t none -c --forget-inc-paths --include-dir src -o obj\nes\smbdishl.o src\smbdishl.s
.\bin\cl65 -t none --mapfile smb.map -o smb.nes -C src\nes.cfg obj\nes\smbdishl.o
pause
