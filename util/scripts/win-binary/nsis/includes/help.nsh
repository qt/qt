!ifndef HELP_INCLUDE
!define HELP_INCLUDE

; make it as a macro since it's only used twice (about the same overhead)
!macro InstallHelp HELP_PATH HELP_ROOT VS_VERSION
  SetOutPath "${HELP_PATH}"
  SetOverwrite ifnewer
  
  File "${MODULE_HELP_ROOT}\h2reg.exe"
  File "${MODULE_HELP_ROOT}\h2reg.ini"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*.HxS"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*C.HxA"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*C.HxC"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*C.HxT"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*KC.HxK"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*FC.HxK"
  File "${MODULE_HELP_ROOT}\${HELP_ROOT}_h2reg.ini"
  
  Push $0
  ${If} $VS_VERSION == "2003"
    StrCpy $0 "7.1"
  ${ElseIf} $VS_VERSION == "2005"
    StrCpy $0 "8.0"
  ${ElseIf} $VS_VERSION == "2008"
    StrCpy $0 "9.0"
  ${Else}
    StrCpy $0 ""
  ${EndIf}

  ${If} $0 != ""
    SetOutPath "$COMMONFILES\microsoft shared\VS Help Data\$0\Filters\1033"
    File "${MODULE_HELP_ROOT}\qt450.xml" 
    WriteRegDWORD HKLM "Software\Microsoft\VisualStudio\$0\Help\VisibleFilters" "qt450" 0x00000001
  ${EndIf}
  Pop $0

  !insertmacro InstallHelpInVS ${HELP_ROOT} ${VS_VERSION}
!macroend

!macro InstallHelpInVS HELP_ROOT VS_VERSION
  Push $0
  
  Push ${VS_VERSION}
  Call GetVSInstallationDir
  Pop $0
  StrCmp $0 "" +4
    SetOutPath "$0\HTML\XMLLinks\1033"
    SetOverwrite ifnewer
    File "${MODULE_HELP_ROOT}\${HELP_ROOT}_*.xml"
  ClearErrors
  
  Pop $0
!macroend

!macro RegisterHelp HELP_PATH HELP_ROOT
  SetOutPath "${HELP_PATH}"
  ClearErrors ; clear the error flag
  Push $0
  nsExec::ExecToLog '"${HELP_PATH}\h2reg.exe" -q -r cmdfile="${HELP_PATH}\${HELP_ROOT}_h2reg.ini"'
  Pop $0
  StrCmp $0 "error" 0 +2
    MessageBox MB_OK "Cannot register ${HELP_ROOT} help!"
  Pop $0
!macroend

!macro un.RegisterHelp HELP_PATH HELP_ROOT
  ClearErrors ; clear the error flag
  IfFileExists "${HELP_PATH}\h2reg.exe" 0 +5
    Push $0
    nsExec::ExecToLog '"${HELP_PATH}\h2reg.exe" -q -u cmdfile="${HELP_PATH}\${HELP_ROOT}_h2reg.ini"'
    Pop $0
    StrCmp $0 "error" 0 +3
      MessageBox MB_OK "Cannot unregister ${HELP_ROOT} help!"
      SetErrors
    Pop $0
!macroend

!macro un.InstallHelp HELP_PATH HELP_ROOT VS_VERSION
  Delete "${HELP_PATH}\${HELP_ROOT}_h2reg.ini"
  Delete "${HELP_PATH}\${HELP_ROOT}_*.HxS"
  Delete "${HELP_PATH}\${HELP_ROOT}_*C.HxA"
  Delete "${HELP_PATH}\${HELP_ROOT}_*C.HxC"
  Delete "${HELP_PATH}\${HELP_ROOT}_*C.HxT"
  Delete "${HELP_PATH}\${HELP_ROOT}_*KC.HxK"
  Delete "${HELP_PATH}\${HELP_ROOT}_*FC.HxK"

  Push $0
  ${If} $VS_VERSION == "2003"
    StrCpy $0 "7.1"
  ${ElseIf} $VS_VERSION == "2005"
    StrCpy $0 "8.0"
  ${ElseIf} $VS_VERSION == "2008"
    StrCpy $0 "9.0"
  ${Else}
    StrCpy $0 ""
  ${EndIf}

  ${If} $0 != ""
    Delete "$COMMONFILES\microsoft shared\VS Help Data\$0\Filters\qt450.xml"
    DeleteRegValue HKLM "Software\Microsoft\VisualStudio\$0\Help\VisibleFilters" "qt450"
  ${EndIf}
  Pop $0

  !insertmacro un.InstallHelpInVS ${HELP_ROOT} ${VS_VERSION}
!macroend

!macro un.InstallHelpInVS HELP_ROOT VS_VERSION
  Push $0

  Push ${VS_VERSION}
  Call un.GetVSInstallationDir

  Pop $0
  StrCmp $0 "" +2 0
    Delete "$0\HTML\XMLLinks\1033\${MODULE_HELP_QT_FILE_ROOT}_*.xml"

  ClearErrors
  
  Pop $0
!macroend

Function un.DeleteH2RegFiles
  Exch $0
  IfFileExists "$0\*.HxS" DeleteH2RegFiles_Done
    Delete "$0\h2reg.exe"
    Delete "$0\h2reg.ini"
    Delete "$0\h2reg_log.txt"
    RMDir "$0"
  DeleteH2RegFiles_Done:
  Pop $0
FunctionEnd

!endif ;HELP_INCLUDE
