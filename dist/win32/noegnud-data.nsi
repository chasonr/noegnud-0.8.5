Name "noegnud Data"
OutFile "noegnud-a.b.c-Data_win32-d.exe"
InstallDir "$PROGRAMFILES\noegnud\data"

;--------------------------------

Page directory
Page instfiles

;--------------------------------

Section ""
  SetOutPath $INSTDIR
  File /r ..\..\build\data\*

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-Data" "DisplayName" "noegnud Data"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-Data" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-Data" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-Data" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
SectionEnd

Section "Start Menu Shortcuts"
  SetOutPath $INSTDIR
  CreateDirectory "$SMPROGRAMS\noegnud"
  CreateShortCut "$SMPROGRAMS\noegnud\Uninstall noegnud Data.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\noegnud-Data"
  DeleteRegKey HKLM SOFTWARE\noegnud-Data
  Delete $INSTDIR\*
  RMDir "$INSTDIR"
  Delete "$SMPROGRAMS\noegnud\Uninstall noegnud Data.lnk"
  RMDir "$SMPROGRAMS\noegnud"
SectionEnd
