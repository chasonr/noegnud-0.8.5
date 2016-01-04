PATH=\MinGW\bin\;%PATH%
REM ---------- SLASH'EM ----------
del /S /Q slashem\win\noegnud
mkdir slashem\win\noegnud
xcopy /E /-Y noegnud\* slashem\win\noegnud\
cd slashem\sys\winnt
call nhsetup
cd ..\..\src
mingw32-make -f Makefile.gcc spotless
mingw32-make -f Makefile.gcc all
cd ..\..
REM ---------- Build ----------
del /S /Q build\slashem
mkdir build\slashem
xcopy /E /Y slashem\binary\* build\slashem\
xcopy /E /Y noegnud\dlls\* build\slashem\
