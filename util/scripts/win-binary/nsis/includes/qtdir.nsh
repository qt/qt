!macro QTDIR_INITIALIZE
  !include "includes\qtenv.nsh"
  !include "includes\list.nsh"
  !define QTDIR_PAGE "qtdir.ini"

  Page custom QtDirPageEnter QtDirPageExit
  
  LangString QtDirPageTitle ${LANG_ENGLISH} "Select Qt Version"
  LangString QtDirPageDescription ${LANG_ENGLISH} "Select the detected Qt Version, or choose a QTDIR manually."
!macroend

!macro QTDIR_FUNCTIONS
  Function QtDirPageEnter
    push $0
    push $1
    push $2
    !insertmacro MUI_HEADER_TEXT "$(QtDirPageTitle)" "$(QtDirPageDescription)"

    StrCpy $2 ""
    StrCpy $0 0
    loopHKLM:
      EnumRegKey $1 HKLM "SOFTWARE\trolltech\Versions" $0
      StrCmp $1 "" doneHKLM
      IntOp $0 $0 + 1
      ReadRegStr $1 HKLM "SOFTWARE\trolltech\Versions\$1\" "InstallDir"
      StrCmp $1 "" +2
      StrCpy $2 "$2$1|"
      goto loopHKLM
    doneHKLM:

    StrCpy $0 0
    loopHKCU:
      EnumRegKey $1 HKCU "SOFTWARE\trolltech\Versions" $0
      StrCmp $1 "" doneHKCU
      IntOp $0 $0 + 1
      ReadRegStr $1 HKCU "SOFTWARE\trolltech\Versions\$1\" "InstallDir"
      StrCmp $1 "" +2
      StrCpy $2 "$2$1|"
      goto loopHKCU
    doneHKCU:

    StrLen $0 $2
    IntOp $0 $0 - 1
    StrCpy $1 $2 $0
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 3" "ListItems" "$1"

    ExpandEnvStrings $0 "%QTDIR%"
    strcmp "$QTDIR_SELECTED" "" 0 noQtDirEnv
    strcmp "$0" "%QTDIR%" noQtDirEnv
    strcmp "$0" "" noQtDirEnv
      strcpy $QTDIR_SELECTED $0
    noQtDirEnv:
    
    strcmp "$QTDIR_SELECTED" "" 0 noFirstItemInList
      push "1"
      push "$1"
      call GetItemInList
      pop $0
      strcpy $QTDIR_SELECTED $0
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 3" "State" "$0"
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 5" "State" "$0"
    noFirstItemInList:

    !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 3" "State" "$QTDIR_SELECTED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 5" "State" "$QTDIR_SELECTED"
    
    call AutoDetectCompilers
    pop $1
    !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 8" "ListItems" "$1"
    
    strcmp "$COMPILER_SELECTED" "" 0 noFirstCompilerInList
      push "1"
      push "$1"
      call GetItemInList
      pop $0
      strcpy $COMPILER_SELECTED $0
    noFirstCompilerInList:

    !insertmacro MUI_INSTALLOPTIONS_WRITE ${QTDIR_PAGE} "Field 8" "State" "$COMPILER_SELECTED"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${QTDIR_PAGE}"

    pop $2
    pop $1
    pop $0
  FunctionEnd

  Function QtDirPageExit
    push $0
    push $1
    push $2
    !insertmacro MUI_INSTALLOPTIONS_READ $0 "${QTDIR_PAGE}" "Settings" "State"
    strcmp "$0" "3" 0 NoNotify
      !insertmacro MUI_INSTALLOPTIONS_READ $0 "${QTDIR_PAGE}" "Field 3" "State"
      FindWindow $2 "#32770" "" $HWNDPARENT
      GetDlgItem $1 $2 1204 ;line edit (DirRequest)
      SendMessage $1 ${WM_SETTEXT} 0 "STR:$0"
      abort
    NoNotify:

    !insertmacro MUI_INSTALLOPTIONS_READ $QTDIR_SELECTED "${QTDIR_PAGE}" "Field 5" "State"
    IfFileExists "$QTDIR_SELECTED\bin\qmake.exe" +3
      MessageBox MB_OK|MB_ICONEXCLAMATION "This is not a valid QTDIR, please try another one."
      abort
      
    !insertmacro MUI_INSTALLOPTIONS_READ $COMPILER_SELECTED "${QTDIR_PAGE}" "Field 8" "State"
    strcmp "$COMPILER_SELECTED" "" 0 +3
      MessageBox MB_OK|MB_ICONEXCLAMATION "Please select a compiler."
      abort

    pop $2
    pop $1
    pop $0
  FunctionEnd
!macroend

!macro QTDIR_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${QTDIR_PAGE}"
  strcpy $QTDIR_SELECTED ""
  strcpy $COMPILER_SELECTED ""
!macroend