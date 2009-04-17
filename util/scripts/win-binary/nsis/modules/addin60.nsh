!ifdef MODULE_ADDIN60
;------------------------------------------------------------------------------------------------
!macro ADDIN60_INITIALIZE
!ifndef MODULE_ADDIN60_NAME
  !define MODULE_ADDIN60_NAME "QMsDev Add-In"
!endif
!ifndef MODULE_ADDIN60_VERSION
  !define MODULE_ADDIN60_VERSION ${PRODUCT_VERSION}
!endif
!ifndef MODULE_ADDIN60_ROOT
  !define MODULE_ADDIN60_ROOT "${INSTALL_ROOT}\addin60"
!endif

!define QMSDEV_GUID "{73036FFA-7B76-47d4-999F-0F9424F7DC12}"
!include "includes\regsvr.nsh"

!macroend ;ADDIN60_INITIALIZE

;------------------------------------------------------------------------------------------------
!macro ADDIN60_SECTIONS
SectionGroup "${MODULE_ADDIN60_NAME}"
Section "Visual C++ 6.0" ADDIN60_SEC01
  SetOutPath "$ADDIN_INSTDIR\6.0"
  File "${MODULE_ADDIN60_ROOT}\q4msdev.dll"

  call RegisterDevAddin

  SetOutPath "$ADDIN_INSTDIR\6.0"
  File "${MODULE_ADDIN60_ROOT}\doc\usage.rtf"
  !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\QMsDev Usage.lnk" "$ADDIN_INSTDIR\6.0\usage.rtf"

  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QMsDevInstalled" 1
SectionEnd
SectionGroupEnd

Function RegisterDevAddin
  push $0
  
  ClearErrors
  push "$ADDIN_INSTDIR\6.0\q4msdev.dll"
  call RegSvr
  IfErrors 0 +3
    MessageBox MB_OK|MB_ICONEXCLAMATION "Could not register q4msdev.dll"
    goto done

  WriteRegStr HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "" "0"
  WriteRegStr HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Filename" "$ADDIN_INSTDIR\6.0\q4msdev.dll"

  ReadRegStr $0 HKCR "CLSID\${QMSDEV_GUID}" ""
  WriteRegStr HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "DisplayName" $0
  
  ReadRegStr $0 HKCR "CLSID\${QMSDEV_GUID}\Description" ""
  WriteRegStr HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Description" $0

  done:
  pop $0
FunctionEnd

Function un.RegisterDevAddin
  push "$ADDIN_INSTDIR\6.0\q4msdev.dll"
  call un.RegSvr
  DeleteRegKey HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1"
FunctionEnd
!macroend ;ADDIN60_SECTIONS

;------------------------------------------------------------------------------------------------
!macro ADDIN60_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${ADDIN60_SEC01} "This installs the QMsDev Add-in ${MODULE_ADDIN60_VERSION} for Visual Studio 6.0"
!macroend

;------------------------------------------------------------------------------------------------
!macro ADDIN60_STARTUP
  push $0
  SectionSetFlags ${ADDIN60_SEC01} 1
  
  Call IsQMsDevInstalled
  pop $0
  IntCmp $0 0 +3
    SectionSetFlags ${ADDIN60_SEC01} 16
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer has detected that a Qt Add-In for Visual Studio 6.0 is already installed.$\r$\nUninstall the previous version before you reinstall it."

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  strcmp $0 "" 0 +2
    SectionSetFlags ${ADDIN60_SEC01} 16

  strcpy $ADDIN_INSTDIR "$PROGRAMFILES\Trolltech\QMsAddin"
  
  pop $0
!macroend ;ADDIN60_STATUP

;------------------------------------------------------------------------------------------------
!macro ADDIN60_FINISH
!macroend

;------------------------------------------------------------------------------------------------
!macro ADDIN60_UNSTARTUP
  !insertmacro ConfirmOnRemove "QMsDevInstalled" "QMsDev Add-In"
!macroend

;------------------------------------------------------------------------------------------------
!macro ADDIN60_UNINSTALL
  push $0
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "QMsDevInstalled"

  intcmp $0 1 0 DoneUnInstall60
    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QMsDevInstalled" 0
    call un.RegisterDevAddin
    Delete "$ADDIN_INSTDIR\6.0\q4msdev.dll"
    Delete "$ADDIN_INSTDIR\6.0\usage.rtf"
    Delete "$SMPROGRAMS\$STARTMENU_STRING\QMsDev Usage.lnk"
    RmDir "$ADDIN_INSTDIR\6.0"
    RmDir "$ADDIN_INSTDIR"
  DoneUnInstall60:
  pop $0
!macroend ;ADDIN60_UNINSTALL

;------------------------------------------------------------------------------------------------
!macro ADDIN60_UNFINISH
!macroend

!else ;MODULE_ADDIN60
!macro ADDIN60_INITIALIZE
!macroend
!macro ADDIN60_SECTIONS
!macroend
!macro ADDIN60_DESCRIPTION
!macroend
!macro ADDIN60_STARTUP
!macroend
!macro ADDIN60_FINISH
!macroend
!macro ADDIN60_UNSTARTUP
!macroend
!macro ADDIN60_UNINSTALL
!macroend
!macro ADDIN60_UNFINISH
!macroend
!endif ;MODULE_ADDIN60

