@echo Please wait..
.\bin\cl65 -t none -c --create-dep obj\nes\smb.d -v -g --forget-inc-paths --include-dir src -o obj\nes\smbdishl.o src\smbdishl.s
.\bin\cl65 -t none --mapfile smb.map -Wl --dbgfile,smb.nes.dbg -o smb.nes -C src\nes.cfg obj\nes\smbdishl.o