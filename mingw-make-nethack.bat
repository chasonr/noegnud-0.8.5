PATH=\MinGW\bin\;%PATH%
REM ---------- NetHack ----------
del /S /Q nethack\win\noegnud
mkdir nethack\win\noegnud
xcopy /E /Y noegnud\* nethack\win\noegnud\
cd nethack\sys\winnt
call nhsetup
cd ..\..\src
mingw32-make -f Makefile.gcc spotless
mingw32-make -f Makefile.gcc all
cd ..\..
REM ---------- Build ----------
del /S /Q build\nethack
mkdir build\nethack
xcopy /E /Y nethack\binary\* build\nethack\
xcopy /E /Y noegnud\dlls\* build\nethack\
