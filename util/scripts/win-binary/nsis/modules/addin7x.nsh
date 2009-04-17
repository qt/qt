!ifdef MODULE_ADDIN7X

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_INITIALIZE
!ifndef MODULE_ADDIN7X_NAME
  !define MODULE_ADDIN7X_NAME "Qt4 Visual Studio Add-In"
!endif
!ifndef MODULE_ADDIN7X_VERSION
  !define MODULE_ADDIN7X_VERSION ${PRODUCT_VERSION}
!endif
!ifndef MODULE_ADDIN7X_ROOT
  !define MODULE_ADDIN7X_ROOT "${INSTALL_ROOT}\addin7x"
!endif
!ifndef MODULE_VSIP_ROOT
  !error "MODULE_VSIP_ROOT must be in the .conf file..."
!endif

!include "includes\system.nsh"
!include "includes\templates.nsh"
!include "includes\regsvr.nsh"
!macroend ;ADDIN7X_INITIALIZE

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_SECTIONS

SectionGroup "${MODULE_ADDIN7X_NAME}"

!ifndef MODULE_ADDIN7X_NO2008
Section "Visual Studio 2008" ADDIN7X_SEC01
  strcpy $VS_VERSION "2008"
  strcpy $VS_VERSION_SHORT "9.0"

  SetOutPath "$ADDIN_INSTDIR\$VS_VERSION_SHORT"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2008\Qt4VSAddin.AddIn"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2008\Qt4VSAddin.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2008\QtProjectLib.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2008\QtProjectEngineLib.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2008\qmakewrapper1Lib.dll"
  WriteRegStr SHCTX "Software\Microsoft\VisualStudio\9.0\AutomationOptions\LookInFolders" "$ADDIN_INSTDIR\$VS_VERSION_SHORT" ""

  !insertmacro InstallHelp "$ADDIN_INSTDIR\help" "qt4vs" "$VS_VERSION_SHORT"

  call InstallCommonFiles
  call RegisterAddin
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2008" 1
SectionEnd
!endif

!ifndef MODULE_ADDIN7X_NO2005
Section "Visual Studio 2005" ADDIN7X_SEC02
  strcpy $VS_VERSION "2005"
  strcpy $VS_VERSION_SHORT "8.0"

  SetOutPath "$ADDIN_INSTDIR\$VS_VERSION_SHORT"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\Qt4VSAddin.AddIn"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\Qt4VSAddin.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\QtProjectLib.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\QtProjectEngineLib.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\qmakewrapper1Lib.dll"

  SetOutPath "$ADDIN_INSTDIR\$VS_VERSION_SHORT\en"
  File "${MODULE_ADDIN7X_ROOT}\src\bin2005\en\Qt4VSAddin.resources.dll"

  WriteRegStr SHCTX "Software\Microsoft\VisualStudio\8.0\AutomationOptions\LookInFolders" "$ADDIN_INSTDIR\$VS_VERSION_SHORT" ""

  !insertmacro InstallHelp "$ADDIN_INSTDIR\help" "qt4vs" "$VS_VERSION_SHORT"

  call InstallCommonFiles
  call RegisterAddin
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2005" 1
SectionEnd
!endif

SectionGroupEnd

Function InstallCommonFiles
  ; make sure one of the addins is installed
  IfFileExists "$ADDIN_INSTDIR\8.0\Qt4VSAddin.dll" installCommon
  IfFileExists "$ADDIN_INSTDIR\9.0\Qt4VSAddin.dll" installCommon done
  
  installCommon:
  SetOverwrite ifnewer

  push $ADDIN_INSTDIR
  call InstallProjectTemplates
  push $ADDIN_INSTDIR
  call InstallItemTemplates

  ; Install additional tools and stuff
  SetOutPath "$ADDIN_INSTDIR"
  File "${MODULE_ADDIN7X_ROOT}\src\common\qmakewrapper1.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\common\qrceditor.exe"
  File "${MODULE_ADDIN7X_ROOT}\src\common\QtCore4.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\common\QtGui4.dll"
  File "${MODULE_ADDIN7X_ROOT}\src\common\QtXml4.dll"

  ; register help
  IfFileExists "$ADDIN_INSTDIR\help\h2reg.exe" 0 AddinCommonFilesHelp_Done
  !insertmacro RegisterHelp "$ADDIN_INSTDIR\help" "qt4vs"
  AddinCommonFilesHelp_Done:

  ; Install VC redistributable
  Push $0
  Call IsVS2008RedistInstalled
  Pop $0
  ${If} $0 == 0
    SetOutPath "$ADDIN_INSTDIR"
    SetOverwrite ifnewer
    File "${MODULE_VSIP_ROOT}\redist\vcredist_x86.exe"
    ExecWait '"$ADDIN_INSTDIR\vcredist_x86.exe" /q' $0
    Delete "$ADDIN_INSTDIR\vcredist_x86.exe"
  ${EndIf}
  Pop $0

  SetOverwrite off ;ifnewer
  done:
FunctionEnd

Function RegisterAddin
  push $2
  push $3

  push "$VS_VERSION_SHORT"
  call IsDotNETInstalled
  Pop $3
  strcpy $2 "$3\regasm.exe"

  ClearErrors
  push "$ADDIN_INSTDIR\qmakewrapper1.dll"
  call RegSvr
  IfErrors 0 Module_Addin_RegisterWizards
    MessageBox MB_OK "Can not register qmakewrapper1.dll!"

  Module_Addin_RegisterWizards:
  ClearErrors
  DetailPrint "Registering the Qt Wizard Engine."
  nsExec::ExecToLog '"$2" /codebase "$ADDIN_INSTDIR\$VS_VERSION_SHORT\QtProjectEngineLib.dll"'
  pop $3
  strcmp "$3" "error" 0 RegEngineDone
    MessageBox MB_OK "Can not register QtProjectEngineLib.dll!"
    MessageBox MB_OK '"$2" /codebase "$ADDIN_INSTDIR\$VS_VERSION_SHORT\QtProjectEngineLib.dll"'
  RegEngineDone:
  
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "" "Qt4 Projects"
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "SortPriority" 16
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "TemplatesDir" "$ADDIN_INSTDIR\projects\$VS_VERSION_SHORT"
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/2" "SortPriority" 16

  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "" "Qt Forms"
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "TemplatesDir" "$ADDIN_INSTDIR\uiItems"
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "SortPriority" 16
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/2" "" "Qt Resources"
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/2" "TemplatesDir" "$ADDIN_INSTDIR\qrcItems"
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/2" "SortPriority" 16

  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "" "Qt4 Classes"
  WriteRegStr SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "TemplatesDir" "$ADDIN_INSTDIR\wizards\$VS_VERSION_SHORT"
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/1" "SortPriority" 16
  WriteRegDWORD SHCTX "SOFTWARE\Microsoft\VisualStudio\$VS_VERSION_SHORT\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}\/2" "SortPriority" 16

  Done:

  pop $3
  pop $2
FunctionEnd

Function un.RegisterAddin
  push $2
  push $3

  push "$VS_VERSION_SHORT"
  call un.IsDotNETInstalled
  Pop $3
  strcpy $2 "$3\regasm.exe"

  ClearErrors
  IfFileExists "$ADDIN_INSTDIR\$VS_VERSION_SHORT\QtProjectEngineLib.dll" 0 UnRegEngineDone
  nsExec::ExecToLog '"$2" /unregister "$ADDIN_INSTDIR\$VS_VERSION_SHORT\QtProjectEngineLib.dll"'
  pop $3
  strcmp "$3" "error" 0 UnRegEngineDone
    MessageBox MB_OK "Can not unregister QtProjectEngineLib.dll!"
  UnRegEngineDone:

  pop $3
  pop $2
FunctionEnd

Function GetVSVersion
  push $0

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\9.0\Setup\VS" "ProductDir"
  StrCmp $0 "" 0 foundVS2008 ; found msvc.net 2008

  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\8.0\Setup\VS" "ProductDir"
  StrCmp $0 "" 0 foundVS2005 ; found msvc.net 2005

  push "" ;empty string if not found
  goto done

  foundVS2008:
  push "2008"
  goto done

  foundVS2005:
  push "2005"
  goto done

  done:
  exch
  pop $0
FunctionEnd
!macroend ;ADDIN7X_SECTIONS

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_DESCRIPTION
!ifdef ADDIN7X_SEC01
  !insertmacro MUI_DESCRIPTION_TEXT ${ADDIN7X_SEC01} "This installs the Qt4 Add-in ${MODULE_ADDIN7X_VERSION} for Visual Studio 2008"
!endif
!ifdef ADDIN7X_SEC02
  !insertmacro MUI_DESCRIPTION_TEXT ${ADDIN7X_SEC02} "This installs the Qt4 Add-in ${MODULE_ADDIN7X_VERSION} for Visual Studio 2005"
!endif
!macroend

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_STARTUP
  push $0
  push $1

!ifdef ADDIN7X_SEC01
  SectionSetFlags ${ADDIN7X_SEC01} 0
!endif
!ifdef ADDIN7X_SEC02
  SectionSetFlags ${ADDIN7X_SEC02} 0
!endif

!ifdef ADDIN7X_SEC02
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\8.0" "InstallDir"
  strcmp $0 "" +3
    SectionSetFlags ${ADDIN7X_SEC02} 1
  goto +2
    SectionSetFlags ${ADDIN7X_SEC02} 16
!endif

!ifdef ADDIN7X_SEC01
  ReadRegStr $0 HKLM "Software\Microsoft\VisualStudio\9.0" "InstallDir"
  strcmp $0 "" +3
    SectionSetFlags ${ADDIN7X_SEC01} 1
  goto +2
    SectionSetFlags ${ADDIN7X_SEC01} 16
!endif

  strcpy $ADDIN_INSTDIR "$PROGRAMFILES\Nokia\Qt4VSAddin"

  Push "8.0"
  Call IsIntegrationInstalled
  Pop $0
  IntCmp $0 1 Addin7x_Warn
  
  Push "9.0"
  Call IsIntegrationInstalled
  Pop $0
  IntCmp $0 1 Addin7x_Warn
  
  Goto Addin7x_Done
  Addin7x_Warn:
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer has detected that a version of the Qt Visual Studio Integration is installed.$\r$\nThe Add-In's conflict with the integration."
  Addin7x_Done:
  
  StrCpy $1 ""
  Push "8.0"
  Call IsQMsNetInstalled
  Pop $0
  IntCmp $0 0 +2
    StrCpy $1 "$1$\r$\n - Visual Studio 2005"
    
  Push "9.0"
  Call IsQMsNetInstalled
  Pop $0
  IntCmp $0 0 +2
    StrCpy $1 "$1$\r$\n - Visual Studio 2008"

  StrCpy $1 ""
  Push "8.0"
  Call IsQt4VSAddinInstalled
  Pop $0
  IntCmp $0 0 +2
    StrCpy $1 "$1$\r$\n - Visual Studio 2005"
    
  Push "9.0"
  Call IsQt4VSAddinInstalled
  Pop $0
  IntCmp $0 0 +2
    StrCpy $1 "$1$\r$\n - Visual Studio 2008"

  StrCmp $1 "" +2
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer has detected that the following versions of the Add-In are already installed:$1$\r$\nUninstall the previous versions before you reinstall them."

  pop $1
  pop $0
!macroend ;ADDIN7X_STATUP

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_FINISH
!macroend

!macro ADDIN7X_UNSTARTUP
  !insertmacro ConfirmOnRemove "Qt4VSAddin2008" "$ADDIN_INSTDIR"
  !insertmacro ConfirmOnRemove "Qt4VSAddin2005" "$ADDIN_INSTDIR"
!macroend

;------------------------------------------------------------------------------------------------
!macro ADDIN7X_UNINSTALL
  push $0
  
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2008"
  intcmp $0 1 0 DoneUnInstall2008
    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2008" 0
    strcpy $VS_VERSION "2008"
    strcpy $VS_VERSION_SHORT "9.0"
    call un.RegisterAddin
    Delete "$ADDIN_INSTDIR\9.0\Qt4VSAddin.AddIn"
    Delete "$ADDIN_INSTDIR\9.0\Qt4VSAddin.dll"
    Delete "$ADDIN_INSTDIR\9.0\QtProjectLib.dll"
    Delete "$ADDIN_INSTDIR\9.0\QtProjectEngineLib.dll"
    Delete "$ADDIN_INSTDIR\9.0\qmakewrapper1Lib.dll"
    RmDir "$ADDIN_INSTDIR\9.0"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\9.0\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\9.0\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\9.0\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegValue SHCTX "Software\Microsoft\VisualStudio\9.0\AutomationOptions\LookInFolders" "$ADDIN_INSTDIR\9.0"
    !insertmacro un.InstallHelpInVS "qt4vs" "$VS_VERSION_SHORT"

    push "$ADDIN_INSTDIR"
    call un.InstallProjectTemplates
    strcpy $WizardInstDir "$ADDIN_INSTDIR\wizards"
    RmDir "$ProjectInstDir"
  DoneUnInstall2008:

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2005"
  intcmp $0 1 0 DoneUnInstall2005
    WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VSAddin2005" 0
    strcpy $VS_VERSION "2005"
    strcpy $VS_VERSION_SHORT "8.0"
    call un.RegisterAddin
    Delete "$ADDIN_INSTDIR\8.0\en\Qt4VSAddin.resources.dll"
    RmDir "$ADDIN_INSTDIR\8.0\en"
    Delete "$ADDIN_INSTDIR\8.0\Qt4VSAddin.AddIn"
    Delete "$ADDIN_INSTDIR\8.0\Qt4VSAddin.dll"
    Delete "$ADDIN_INSTDIR\8.0\QtProjectLib.dll"
    Delete "$ADDIN_INSTDIR\8.0\QtProjectEngineLib.dll"
    Delete "$ADDIN_INSTDIR\8.0\qmakewrapper1Lib.dll"
    RmDir "$ADDIN_INSTDIR\8.0"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\8.0\NewProjectTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\8.0\Projects\{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegKey SHCTX "SOFTWARE\Microsoft\VisualStudio\8.0\Projects\{8BC9CEBA-8B4A-11D0-8D11-00A0C91BC942}\AddItemTemplates\TemplateDirs\${QT4VSADDIN_GUID}"
    DeleteRegValue SHCTX "Software\Microsoft\VisualStudio\8.0\AutomationOptions\LookInFolders" "$ADDIN_INSTDIR\8.0"
    !insertmacro un.InstallHelpInVS "qt4vs" "$VS_VERSION_SHORT"

    push "$ADDIN_INSTDIR"
    call un.InstallProjectTemplates
    strcpy $WizardInstDir "$ADDIN_INSTDIR\wizards"
    RmDir "$ProjectInstDir"
  DoneUnInstall2005:

  strcmp $VS_VERSION "2005" DoUninstallAddinCommon
  strcmp $VS_VERSION "2008" DoUninstallAddinCommon DoneUninstallAddinCommon

  DoUninstallAddinCommon:

  IfFileExists "$ADDIN_INSTDIR\help\h2reg.exe" 0 DoneUninstallAddinHelp
  !insertmacro un.RegisterHelp "$ADDIN_INSTDIR\help" "qt4vs"
  !insertmacro un.InstallHelp "$ADDIN_INSTDIR\help" "qt4vs" "0.0"
  Push "$ADDIN_INSTDIR\help"
  Call un.DeleteH2RegFiles
  DoneUninstallAddinHelp:

  Delete "$ADDIN_INSTDIR\qmakewrapper1.dll"
  Delete "$ADDIN_INSTDIR\qrceditor.exe"
  Delete "$ADDIN_INSTDIR\QtCore4.dll"
  Delete "$ADDIN_INSTDIR\QtGui4.dll"
  Delete "$ADDIN_INSTDIR\QtXml4.dll"

  RmDir "$ADDIN_INSTDIR\projects"
  RmDir "$ADDIN_INSTDIR\wizards"

  Delete "$ADDIN_INSTDIR\uiItems\Qt4Files.vsdir"
  push $ADDIN_INSTDIR
  call un.InstallItemTemplates

  RmDir "$ADDIN_INSTDIR"
  
  DoneUninstallAddinCommon:
  pop $0
!macroend ;ADDIN7X_UNINSTALL

!macro ADDIN7X_UNFINISH
!macroend

!else ;MODULE_ADDIN7X
!macro ADDIN7X_INITIALIZE
!macroend
!macro ADDIN7X_SECTIONS
!macroend
!macro ADDIN7X_DESCRIPTION
!macroend
!macro ADDIN7X_STARTUP
!macroend
!macro ADDIN7X_FINISH
!macroend
!macro ADDIN7X_UNSTARTUP
!macroend
!macro ADDIN7X_UNINSTALL
!macroend
!macro ADDIN7X_UNFINISH
!macroend
!endif ;MODULE_ADDIN7X

