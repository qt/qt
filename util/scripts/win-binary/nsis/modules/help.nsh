; Help Module

!ifdef MODULE_HELP

;------------------------------------------------------------------------------------------------
!macro HELP_INITIALIZE
!ifndef MODULE_HELP_NAME
  !define MODULE_HELP_NAME "Help Integration"
!endif
!ifndef MODULE_HELP_ROOT
  !define MODULE_HELP_ROOT "${INSTALL_ROOT}\vsip\help"
!endif
!ifndef MODULE_HELP_QT_FILE_ROOT
  !define MODULE_HELP_QT_FILE_ROOT "qt"
!endif

!include "includes\system.nsh"
!include "includes\help.nsh"

!macroend

;------------------------------------------------------------------------------------------------

!macro HELP_SECTIONS

Section -PreHelpSection
  ; use default instdir if not set
  strcmp "$HELP_INSTDIR" "" 0 +2
    StrCpy $HELP_INSTDIR "$INSTDIR\help"
SectionEnd

SectionGroup "Help Integration"

!ifndef MODULE_HELP_NOVS2008
Section "Visual Studio 2008" HELP_SEC01
  !insertmacro InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "9.0"
  WriteRegDWORD SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 9.0" 1
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp90Installed" 1
SectionEnd
!endif

!ifndef MODULE_HELP_NOVS2005
Section "Visual Studio 2005" HELP_SEC02
  !insertmacro InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "8.0"
  WriteRegDWORD SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 8.0" 1
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp80Installed" 1
SectionEnd
!endif

!ifndef MODULE_HELP_NOVS2003
Section "Visual Studio 2003" HELP_SEC03
  !insertmacro InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "7.1"
  WriteRegDWORD SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.1" 1
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp71Installed" 1
SectionEnd
!endif

!ifndef MODULE_HELP_NOVS2002
Section "Visual Studio 2002" HELP_SEC04
  !insertmacro InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "7.0"
  WriteRegDWORD SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.0" 1
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp70Installed" 1
SectionEnd
!endif

SectionGroupEnd

Section -PostHelpSection
  IfFileExists "$HELP_INSTDIR\h2reg.exe" 0 PostHelpSectionFinished
  !insertmacro RegisterHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT}
PostHelpSectionFinished:
SectionEnd

!macroend

;------------------------------------------------------------------------------------------------
!macro HELP_DESCRIPTION
!ifdef HELP_SEC01
  !insertmacro MUI_DESCRIPTION_TEXT ${HELP_SEC01} "This installs the Qt Help Integration for Visual Studio 2008"
!endif
!ifdef HELP_SEC02
  !insertmacro MUI_DESCRIPTION_TEXT ${HELP_SEC02} "This installs the Qt Help Integration for Visual Studio 2005"
!endif
!ifdef HELP_SEC03
  !insertmacro MUI_DESCRIPTION_TEXT ${HELP_SEC03} "This installs the Qt Help Integration for Visual Studio 2003"
!endif
!ifdef HELP_SEC04
  !insertmacro MUI_DESCRIPTION_TEXT ${HELP_SEC04} "This installs the Qt Help Integration for Visual Studio 2002"
!endif
!macroend

;------------------------------------------------------------------------------------------------
!macro HELP_STARTUP
  Push $0
  Push $1
  Push $2
  
  StrCmp $RUNNING_AS_ADMIN "false" 0 Module_Help_CheckForInstallation
!ifdef HELP_SEC01
    SectionSetFlags ${HELP_SEC01} "16"
!endif
!ifdef HELP_SEC02
    SectionSetFlags ${HELP_SEC02} "16"
!endif
!ifdef HELP_SEC03
    SectionSetFlags ${HELP_SEC03} "16"
!endif
    Goto Module_Help_Done

  Module_Help_CheckForInstallation:
!ifdef HELP_SEC01
    ReadRegDWORD $0 SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 9.0"
    intcmp $0 1 0 +2
      SectionSetFlags ${HELP_SEC01} "16"
!endif
      
!ifdef HELP_SEC02
    ReadRegDWORD $0 SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 8.0"
    intcmp $0 1 0 +2
      SectionSetFlags ${HELP_SEC02} "16"
!endif
     
!ifdef HELP_SEC03
    ReadRegDWORD $0 SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.1"
    intcmp $0 1 0 +2
      SectionSetFlags ${HELP_SEC03} "16"
!endif
      
!ifdef HELP_SEC04
    ReadRegDWORD $0 SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.0"
    intcmp $0 1 0 +2
      SectionSetFlags ${HELP_SEC04} "16"
!endif

!ifdef HELP_SEC01
    ClearErrors
    Push "9.0"
    Call GetVSInstallationDir
    Pop $2
    IfErrors 0 +2
      SectionSetFlags ${HELP_SEC01} "16"
!endif
      
!ifdef HELP_SEC02
    ClearErrors
    Push "8.0"
    Call GetVSInstallationDir
    Pop $2
    IfErrors 0 +2
      SectionSetFlags ${HELP_SEC02} "16"
!endif
      
!ifdef HELP_SEC03
    ClearErrors
    Push "7.1"
    Call GetVSInstallationDir
    Pop $2
    IfErrors 0 +2
      SectionSetFlags ${HELP_SEC03} "16"
!endif

!ifdef HELP_SEC04
    ClearErrors
    Push "7.0"
    Call GetVSInstallationDir
    Pop $2
    IfErrors 0 +2
      SectionSetFlags ${HELP_SEC04} "16"
!endif

  Module_Help_Done:
  Pop $2
  Pop $1
  Pop $0
!macroend

;------------------------------------------------------------------------------------------------

!macro HELP_FINISH
!macroend

;------------------------------------------------------------------------------------------------

!macro HELP_UNSTARTUP
  ; use default instdir if not set
  strcmp "$HELP_INSTDIR" "" 0 +2
    StrCpy $HELP_INSTDIR "$INSTDIR\help"
!macroend

;------------------------------------------------------------------------------------------------

!macro HELP_UNINSTALL
Section un."${MODULE_HELP_NAME}"
  push $0
  push $1
  push $2
  push $3
  push $4
  
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp90Installed"
  ReadRegDWORD $1 SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp80Installed"
  ReadRegDWORD $2 SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp71Installed"
  ReadRegDWORD $3 SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp70Installed"
  
  IntOp $4 $0 | $1
  IntOp $4 $4 | $2
  IntOp $4 $4 | $3
  IntCmp $4 0 Module_Help_End
  
  !insertmacro un.RegisterHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT}

  IntCmp $0 0 Module_Help_NoVS2008
  !insertmacro un.InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "9.0"
  DeleteRegValue SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp90Installed"
  DeleteRegValue SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 9.0"
  Module_Help_NoVS2008:

  IntCmp $1 0 Module_Help_NoVS2005
  !insertmacro un.InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "8.0"
  DeleteRegValue SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp80Installed"
  DeleteRegValue SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 8.0"
  Module_Help_NoVS2005:
  
  IntCmp $2 0 Module_Help_NoVS2003
  !insertmacro un.InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "7.1"
  DeleteRegValue SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp71Installed"
  DeleteRegValue SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.1"
  Module_Help_NoVS2003:

  IntCmp $3 0 Module_Help_NoVS2002
  !insertmacro un.InstallHelp "$HELP_INSTDIR" ${MODULE_HELP_QT_FILE_ROOT} "7.0"
  DeleteRegValue SHCTX "$PRODUCT_UNIQUE_KEY" "QtHelp70Installed"
  DeleteRegValue SHCTX "SOFTWARE\Trolltech\QtHelp" "${PRODUCT_NAME} ${PRODUCT_VERSION} - 7.0"
  Module_Help_NoVS2002:

  DeleteRegKey /ifempty SHCTX "SOFTWARE\Trolltech\QtHelp"

  push "$HELP_INSTDIR"
  Call un.DeleteH2RegFiles

  Module_Help_End:
  pop $4
  pop $3
  pop $2
  pop $1
  pop $0
SectionEnd
!macroend

;------------------------------------------------------------------------------------------------

!macro HELP_UNFINISH
!macroend

;------------------------------------------------------------------------------------------------

!else
!macro HELP_INITIALIZE
!macroend
!macro HELP_SECTIONS
!macroend
!macro HELP_DESCRIPTION
!macroend
!macro HELP_STARTUP
!macroend
!macro HELP_FINISH
!macroend
!macro HELP_UNSTARTUP
!macroend
!macro HELP_UNINSTALL
!macroend
!macro HELP_UNFINISH
!macroend
!endif
