!ifdef MODULE_QTJAMBI
;------------------------------------------------------------------------------------------------
!macro QTJAMBI_INITIALIZE
!include "includes\qtcommon.nsh"
!ifndef MODULE_QTJAMBI_NAME
  !define MODULE_QTJAMBI_NAME "Qt Jambi"
!endif
!ifndef MODULE_QTJAMBI_VERSION
  !define MODULE_QTJAMBI_VERSION ${PRODUCT_VERSION}
!endif

!macroend ;QTJAMBI_INITIALIZE

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_SECTIONS
SectionGroup "${MODULE_QTJAMBI_NAME}"
Section "QtJambi" QTJAMBI_SEC01
  SetOutPath "$QTJAMBI_INSTDIR"
  SetOverwrite ifnewer
  !insertmacro MODULE_QTJAMBI_INSTALLFILES

  !insertmacro CreateConditionalShortCutWithParameters "$SMPROGRAMS\$STARTMENU_STRING\Readme.lnk" "notepad" "$QTJAMBI_INSTDIR\README"
  !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\Examples and Demos.lnk" "$QTJAMBI_INSTDIR\qtjambi.exe"

  Push "bin"
  push $QTJAMBI_INSTDIR
  call PatchCommonBinaryFiles

  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtJambiInstalled" 1
SectionEnd
SectionGroupEnd
!macroend ;QTJAMBI_SECTIONS

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${QTJAMBI_SEC01} "This installs the Qt Jambi ${MODULE_QTJAMBI_VERSION} for Visual Studio 6.0"
!macroend

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_STARTUP
  push $0
  SectionSetFlags ${QTJAMBI_SEC01} 1
!ifdef MODULE_ENVIRONMENT
  strcpy $QTJAMBI_INSTDIR "C:\Qt\qtjambi-win32-eval-${MODULE_QTJAMBI_VERSION}"
!else
  strcpy $QTJAMBI_INSTDIR "C:\Qt\qtjambi-win32-commercial-${MODULE_QTJAMBI_VERSION}"
!endif

  pop $0
!macroend ;QTJAMBI_STATUP

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_FINISH
!macroend

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_UNSTARTUP
  !insertmacro ConfirmOnRemove "QtJambiInstalled" "Qt Jambi"
!macroend

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_UNINSTALL
  push $0
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "QtJambiInstalled"

  intcmp $0 1 0 DoneUnInstallQtJambi
    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtJambiInstalled" 0
    Delete "$SMPROGRAMS\$STARTMENU_STRING\Readme.lnk"
    Delete "$SMPROGRAMS\$STARTMENU_STRING\Examples and Demos.lnk"
    !insertmacro MODULE_QTJAMBI_REMOVE "$QTJAMBI_INSTDIR"
    RMDir $QTJAMBI_INSTDIR ;removes it if empty
  DoneUnInstallQtJambi:
  pop $0
!macroend ;QTJAMBI_UNINSTALL

;------------------------------------------------------------------------------------------------
!macro QTJAMBI_UNFINISH
!macroend

!else ;MODULE_QTJAMBI
!macro QTJAMBI_INITIALIZE
!macroend
!macro QTJAMBI_SECTIONS
!macroend
!macro QTJAMBI_DESCRIPTION
!macroend
!macro QTJAMBI_STARTUP
!macroend
!macro QTJAMBI_FINISH
!macroend
!macro QTJAMBI_UNSTARTUP
!macroend
!macro QTJAMBI_UNINSTALL
!macroend
!macro QTJAMBI_UNFINISH
!macroend
!endif ;MODULE_QTJAMBI

