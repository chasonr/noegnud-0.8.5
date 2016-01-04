Name "noegnud SlashEM"
OutFile "noegnud-a.b.c-SlashEM_win32-d.exe"
InstallDir "$PROGRAMFILES\noegnud\SlashEM"

;--------------------------------

Page directory
Page instfiles

;--------------------------------

Section ""
  SetOutPath $INSTDIR
  File ..\..\build\slashem\*

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-SlashEM" "DisplayName" "noegnud SlashEM"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-SlashEM" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-SlashEM" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-SlashEM" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Desktop icon" SecDesktop
  SetOutPath $INSTDIR
  CreateShortcut "$DESKTOP\noegnud SlashEM.lnk" "$INSTDIR\noegnud-SlashEM.exe" "" "$INSTDIR\noegnud-SlashEM.exe" 0
SectionEnd

Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\noegnud\SlashEM"
  CreateShortCut "$SMPROGRAMS\noegnud\SlashEM\Play noegnud SlashEM.lnk" "$INSTDIR\noegnud-SlashEM.exe" "" "$INSTDIR\noegnud-SlashEM.exe" 0
  CreateShortCut "$SMPROGRAMS\noegnud\SlashEM\Darkarts Studios.lnk" "http://www.darkarts.co.za" "" "http://www.darkarts.co.za" 0
  CreateShortCut "$SMPROGRAMS\noegnud\SlashEM\Uninstall noegnud SlashEM.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-SlashEM"
  DeleteRegKey HKLM SOFTWARE\noegnud-SlashEM
  Delete $INSTDIR\*
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\noegnud\SlashEM\Play noegnud SlashEM.lnk"
  Delete "$SMPROGRAMS\noegnud\SlashEM\Darkarts Studios.lnk"
  Delete "$SMPROGRAMS\noegnud\SlashEM\Uninstall noegnud SlashEM.lnk"
  RMDir "$SMPROGRAMS\noegnud\SlashEM"
  Delete "$DESKTOP\noegnud SlashEM.lnk"
SectionEnd
