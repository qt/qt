!ifdef MODULE_QSA
!macro QSA_INITIALIZE
  !include "includes\qtenv.nsh"
  !define MODULE_QSA_OPTIONPAGE "qsa.ini"
  !ifndef MODULE_QSA_NAME
    !define MODULE_QSA_NAME "QSA"
  !endif
  !ifndef MODULE_QSA_VERSION
    !define MODULE_QSA_VERSION ${PRODUCT_VERSION}
  !endif
  !ifndef MODULE_QSA_ROOT
    !define MODULE_QSA_ROOT "${INSTALL_ROOT}\qsa"
  !endif
  !ifndef MODULE_QSA_INTEGRATE
    !include "includes\qtdir.nsh"
    !insertmacro QTDIR_INITIALIZE
  !endif
  
  var MODULE_QSA_NOIDE
  var MODULE_QSA_NOEDITOR
  
  LangString ModuleQSATitle ${LANG_ENGLISH} "${MODULE_QSA_NAME} ${MODULE_QSA_VERSION} Build Options"
  LangString ModuleQSADescription ${LANG_ENGLISH} "Do you want build the QSA Workbench and the QSA Editor?"
  Page custom ModuleQSAOptionsPageEnter ModuleQSAOptionsPageExit
!macroend
!macro QSA_SECTIONS
  Section "${MODULE_QSA_NAME} ${MODULE_QSA_VERSION}" QSA_SEC01
    push $0
    strcmp "$QSA_INSTDIR" "" 0 +2
      StrCpy $QSA_INSTDIR "$INSTDIR\${MODULE_QSA_NAME} ${MODULE_QSA_VERSION}"

    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QSAInstalled" 1

    SetOutPath "$QSA_INSTDIR"
    SetOverwrite ifnewer
    File /r "${MODULE_QSA_ROOT}\*.*"
    
    call QsaModuleWriteBuildFile
    
    DetailPrint "Please wait while building ${MODULE_QSA_NAME} ${MODULE_QSA_VERSION}..."
    nsExec::ExecToLog "$QSA_INSTDIR\qsabuild.bat"
    pop $0
    strcmp "$0" "0" +2
      MessageBox MB_OK|MB_ICONEXCLAMATION "Building ${MODULE_QSA_NAME} ${MODULE_QSA_VERSION} failed.$\r$\nLook at the output, resolve the problem, and try to rebuild it manually.$\r$\n(errorcode: $0)"
    
    call ModuleQSAAddShortCuts
    
    pop $0
  SectionEnd
  
  !ifndef MODULE_QSA_INTEGRATE
    !insertmacro QTDIR_FUNCTIONS
  !endif

  Function ModuleQSAOptionsPageEnter
    !insertmacro MUI_HEADER_TEXT "$(ModuleQSATitle)" "$(ModuleQSADescription)"
    push $0    
    ${strstr} $0 "$QTDIR_SELECTED" "4.0.0"
    strcmp $0 "" +2 
      MessageBox MB_OK|MB_ICONEXCLAMATION "QSA 1.2.0 requires Qt 4.0.1 snapshots or later"
    pop $0


    strcmp "$MODULE_QSA_NOIDE" "" +3
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${MODULE_QSA_OPTIONPAGE} "Field 2" "State" "1"
    goto +2
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${MODULE_QSA_OPTIONPAGE} "Field 2" "State" "0"

    strcmp "$MODULE_QSA_NOEDITOR" "" +3
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${MODULE_QSA_OPTIONPAGE} "Field 3" "State" "1"
    goto +2
      !insertmacro MUI_INSTALLOPTIONS_WRITE ${MODULE_QSA_OPTIONPAGE} "Field 3" "State" "0"

   !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_QSA_OPTIONPAGE}"
  FunctionEnd
  
Function ModuleQSAAddShortCuts
  push $0
  push $1
  push $2
  push $3

  !insertmacro CreateConditionalShortCutDirectory "$SMPROGRAMS\$STARTMENU_STRING\Examples"

  FindFirst $0 $1 "$QSA_INSTDIR\examples\*.*"
loop:
    StrCmp $1 "" done
    IfFileExists "$QSA_INSTDIR\examples\$1\release\$1.exe" 0 +3
      strcpy $2 "release"
    goto +3
      IfFileExists "$QSA_INSTDIR\examples\$1\debug\$1.exe" 0 example_shortcuts_next
      strcpy $2 "debug"

      FileOpen $3 "$QSA_INSTDIR\examples\$1\$1.bat" w
      IfErrors example_shortcuts_next
      FileWrite $3 "@rem ----------------------$\r$\n"
      FileWrite $3 "@rem This file is generated$\r$\n"
      FileWrite $3 "@rem ----------------------$\r$\n"
      FileWrite $3 "$\r$\n"
      FileWrite $3 "set PATH=$QTDIR_SELECTED\bin;%PATH%$\r$\n"
      FileWrite $3 "start $2\$1.exe$\r$\n"
      FileClose $3

      SetOutPath "$QSA_INSTDIR\examples\$1"
      !insertmacro CreateConditionalShortCutWithParameters "$SMPROGRAMS\$STARTMENU_STRING\Examples\$1.lnk" "%COMSPEC%" "/c $1.bat"
example_shortcuts_next:
    FindNext $0 $1
    Goto loop

  done:
  SetOutPath "$QSA_INSTDIR"
  pop $3
  pop $2
  pop $1
  pop $0
FunctionEnd

  
  Function ModuleQSAOptionsPageExit
    push $0
    !insertmacro MUI_INSTALLOPTIONS_READ $0 ${MODULE_QSA_OPTIONPAGE} "Field 2" "State"
    strcmp "$0" "1" +3
      strcpy $MODULE_QSA_NOIDE ""
    goto +2
      strcpy $MODULE_QSA_NOIDE "-no-ide "

    !insertmacro MUI_INSTALLOPTIONS_READ $0 ${MODULE_QSA_OPTIONPAGE} "Field 3" "State"
    strcmp "$0" "1" +3
      strcpy $MODULE_QSA_NOEDITOR ""
    goto +2
      strcpy $MODULE_QSA_NOEDITOR "-no-editor "
      
    pop $0
  FunctionEnd

  Function QsaModuleWriteBuildFile
    push $0 ; file handle
    push $1 ; compiler
    push $2 ; qmakespec
    push $3 ; vars file

    push $COMPILER_SELECTED
    call GetShortCompilerName
    pop $1
    
    push $1
    call GetMkSpec
    pop $2
    
    push $1
    call GetVSVarsFile
    pop $3

    ClearErrors
    FileOpen $0 "$QSA_INSTDIR\qsabuild.bat" w
    IfErrors done
    FileWrite $0 "@rem ----------------------$\r$\n"
    FileWrite $0 "@rem This file is generated$\r$\n"
    FileWrite $0 "@rem ----------------------$\r$\n"
    FileWrite $0 "$\r$\n"
    FileWrite $0 "@echo Setting QTDIR to $QTDIR_SELECTED$\r$\n"
    FileWrite $0 "@set QTDIR=$QTDIR_SELECTED$\r$\n"
    FileWrite $0 "$\r$\n"
    FileWrite $0 "@echo Adding %QTDIR%\bin to PATH$\r$\n"
    FileWrite $0 "@set PATH=%QTDIR%\bin;%PATH%$\r$\n"
    FileWrite $0 "$\r$\n"
    FileWrite $0 "@echo Setting QMAKESPEC to $2$\r$\n"
    FileWrite $0 "@set QMAKESPEC=$2$\r$\n"
    FileWrite $0 "$\r$\n"
    strcmp "$3" "" +3
      FileWrite $0 '@call "$3"$\r$\n'
      FileWrite $0 "$\r$\n"
    FileWrite $0 "@echo Running configure in $QSA_INSTDIR$\r$\n"
    FileWrite $0 "@cd $QSA_INSTDIR$\r$\n"
    FileWrite $0 "@configure $MODULE_QSA_NOIDE$MODULE_QSA_NOEDITOR-automake$\r$\n"
    FileWrite $0 "$\r$\n"
    FileClose $0
    done:
    
    pop $3
    pop $2
    pop $1
    pop $0
  FunctionEnd
!macroend
!macro QSA_CHECKLICENSEPRODUCT
  push $0
  qtnsisext::HasLicenseProduct $LICENSE_KEY "QSA"
  pop $0
  strcmp $0 "1" QSALicenseOK
    SectionSetText ${QSA_SEC01} "${MODULE_QSA_NAME} (No License)"
    SectionSetFlags ${QSA_SEC01} 16
  QSALicenseOK:
  pop $0
!macroend
!macro QSA_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${QSA_SEC01} "This installs ${MODULE_QSA_NAME} version ${MODULE_QSA_VERSION} on your system."
!macroend
!macro QSA_STARTUP
  !ifndef MODULE_QSA_INTEGRATE
    !insertmacro QTDIR_STARTUP
  !endif
  strcpy $QSA_INSTDIR "C:\${MODULE_QSA_NAME}\${MODULE_QSA_VERSION}"
  strcpy $MODULE_QSA_NOIDE ""
  strcpy $MODULE_QSA_NOEDITOR ""
  
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_QSA_OPTIONPAGE}"
!macroend
!macro QSA_FINISH
!macroend
!macro QSA_RUN_FUNCTION
!macroend
!macro QSA_UNSTARTUP
  strcmp "$QSA_INSTDIR" "" 0 +2
    StrCpy $QSA_INSTDIR "$INSTDIR\${MODULE_QSA_NAME} ${MODULE_QSA_VERSION}"

  !insertmacro ConfirmOnRemove "QSAInstalled" "$QSA_INSTDIR"
!macroend
!macro QSA_UNINSTALL
  Section un."${MODULE_QSA_NAME} ${MODULE_QSA_VERSION}"
    ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "QSAInstalled"
    intcmp $0 1 0 DoneUnInstallQSA
    
    DetailPrint "Removing installation directory..."
    RMDir /r "$QSA_INSTDIR"
    RmDir /r "$SMPROGRAMS\$STARTMENU_STRING\Examples"
    
    DoneUnInstallQSA:
  SectionEnd
!macroend
!macro QSA_UNFINISH
!macroend
!else ;MODULE_QSA
!macro QSA_INITIALIZE
!macroend
!macro QSA_SECTIONS
!macroend
!macro QSA_CHECKLICENSEPRODUCT
!macroend
!macro QSA_DESCRIPTION
!macroend
!macro QSA_STARTUP
!macroend
!macro QSA_FINISH
!macroend
!macro QSA_RUN_FUNCTION
!macroend
!macro QSA_UNSTARTUP
!macroend
!macro QSA_UNINSTALL
!macroend
!macro QSA_UNFINISH
!macroend
!endif ;MODULE_QSA

