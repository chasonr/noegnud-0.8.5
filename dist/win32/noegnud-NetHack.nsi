Name "noegnud NetHack"
OutFile "noegnud-a.b.c-NetHack_win32-d.exe"
InstallDir "$PROGRAMFILES\noegnud\NetHack"

;--------------------------------

Page directory
Page instfiles

;--------------------------------

Section ""
  SetOutPath $INSTDIR
  File ..\..\build\nethack\*

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-NetHack" "DisplayName" "noegnud NetHack"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-NetHack" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-NetHack" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-NetHack" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  SetOutPath $INSTDIR
  CreateShortcut "$DESKTOP\noegnud NetHack.lnk" "$INSTDIR\noegnud-NetHack.exe" "" "$INSTDIR\noegnud-NetHack.exe" 0
SectionEnd

Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\noegnud\NetHack"
  CreateShortCut "$SMPROGRAMS\noegnud\NetHack\Play noegnud NetHack.lnk" "$INSTDIR\noegnud-NetHack.exe" "" "$INSTDIR\noegnud-NetHack.exe" 0
  CreateShortCut "$SMPROGRAMS\noegnud\NetHack\Darkarts Studios.lnk" "http://www.darkarts.co.za" "" "http://www.darkarts.co.za" 0
  CreateShortCut "$SMPROGRAMS\noegnud\NetHack\Uninstall noegnud NetHack.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-NetHack"
  DeleteRegKey HKLM SOFTWARE\noegnud-NetHack
  Delete $INSTDIR\*
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\noegnud\NetHack\Play noegnud NetHack.lnk"
  Delete "$SMPROGRAMS\noegnud\NetHack\Darkarts Studios.lnk"
  Delete "$SMPROGRAMS\noegnud\NetHack\Uninstall noegnud NetHack.lnk"
  RMDir "$SMPROGRAMS\noegnud\NetHack"
  Delete "$DESKTOP\noegnud NetHack.lnk"
SectionEnd
