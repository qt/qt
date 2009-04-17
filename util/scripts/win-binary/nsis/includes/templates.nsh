!ifndef TEMPLATE_INCLUDE
!define TEMPLATE_INCLUDE

Var ProjectInstDir
Var WizardInstDir
Var ItemInstDir

!macro InstallProjectTemplate SD TT
  SetOutPath "$ProjectInstDir"
  SetOverwrite ifnewer

  ; Create the vsz file
  push ${TT}             ;WIZ_NAME
  push "$ProjectInstDir" ;PRO_INST_DIR
  push "$WizardInstDir"  ;WIZ_INST_DIR
  call CreateVszFile

  File "${SD}\${TT}.ico"
  File "${SD}\${TT}.vsdir"
  
  CreateDirectory "$WizardInstDir\${TT}"

  CreateDirectory "$WizardInstDir\${TT}\1033"
  SetOutPath "$WizardInstDir\${TT}\1033"
  SetOverwrite ifnewer
  File "${SD}\1033\styles.css"

  CreateDirectory "$WizardInstDir\${TT}\scripts\"
  CreateDirectory "$WizardInstDir\${TT}\scripts\1033"
  SetOutPath "$WizardInstDir\${TT}\scripts\1033"
  SetOverwrite ifnewer
  File "${SD}\scripts\1033\default.js"

  CreateDirectory "$WizardInstDir\${TT}\templates"
  CreateDirectory "$WizardInstDir\${TT}\templates\1033"
  SetOutPath "$WizardInstDir\${TT}\templates\1033"
  SetOverwrite ifnewer
  File "${SD}\templates\1033\*"

  CreateDirectory "$WizardInstDir\${TT}\images"
  SetOutPath "$WizardInstDir\${TT}\images"
  SetOverwrite ifnewer
  File "${SD}\images\*"
  
  CreateDirectory "$WizardInstDir\${TT}\html"
  CreateDirectory "$WizardInstDir\${TT}\html\1033"
  SetOutPath "$WizardInstDir\${TT}\html\1033"
  SetOverwrite ifnewer
  File "${SD}\html\1033\*"
  
  CreateDirectory "$WizardInstDir\${TT}\html\1031"
  SetOutPath "$WizardInstDir\${TT}\html\1031"
  SetOverwrite ifnewer
  File /nonfatal "${SD}\html\1031\*"
!macroend

!macro UnInstallProjectTemplate TT
  Delete "$ProjectInstDir\${TT}.vsz"
  Delete "$ProjectInstDir\${TT}.ico"
  Delete "$ProjectInstDir\${TT}.vsdir"

  Delete "$WizardInstDir\${TT}\1033\styles.css"
  RmDir "$WizardInstDir\${TT}\1033"

  Delete "$WizardInstDir\${TT}\scripts\1033\default.js"
  RmDir "$WizardInstDir\${TT}\scripts\1033"
  RmDir "$WizardInstDir\${TT}\scripts"

  Delete "$WizardInstDir\${TT}\templates\1033\*"
  RmDir "$WizardInstDir\${TT}\templates\1033"
  RmDir "$WizardInstDir\${TT}\templates"

  Delete "$WizardInstDir\${TT}\images\*"
  RmDir "$WizardInstDir\${TT}\images"

  Delete "$WizardInstDir\${TT}\html\1033\*"
  RmDir "$WizardInstDir\${TT}\html\1033"
  
  Delete "$WizardInstDir\${TT}\html\1031\*"
  RmDir "$WizardInstDir\${TT}\html\1031"

  RmDir "$WizardInstDir\${TT}\html"

  RmDir "$ProjectInstDir\${TT}"
  RmDir "$WizardInstDir\${TT}"
!macroend

Function InstallProjectTemplates
  exch $0
  strcpy $ProjectInstDir "$0\projects\$VS_VERSION_SHORT"
  strcpy $WizardInstDir "$0\wizards"
  CreateDirectory "$ProjectInstDir"
  CreateDirectory "$WizardInstDir"

  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4GuiProject" "Qt4GuiProject"
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4ConsoleProject" "Qt4ConsoleProject"
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4LibProject" "Qt4LibProject"
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4ActiveQtServerProject" "Qt4ActiveQtServerProject"
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4DesignerPluginProject" "Qt4DesignerPluginProject"

  # Check whether we have a Qt/WinCE license.
  !ifdef MODULE_LICENSECHECK
    qtnsisext::IsValidWinCELicense $LICENSE_KEY
    IfErrors skipCETemplates
    pop $1
    strcmp $1 "0" skipCETemplates
  !endif
  # Check whether we have VS 2008 or VS 2005.
  strcmp "8.0" $VS_VERSION_SHORT installCETemplates 0
  strcmp "9.0" $VS_VERSION_SHORT installCETemplates 0
  goto skipCETemplates
  installCETemplates:
    !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4WinCEProject" "Qt4WinCEProject"
    !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Projects\Qt4WinCELibProject" "Qt4WinCELibProject"
  skipCETemplates:

  !ifndef MODULE_OLD_ADDIN7X
  # install the class wizard
  strcpy $ProjectInstDir "$0\wizards\$VS_VERSION_SHORT" ### why do the .vsz file go here?
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Items\Qt4GuiClass" "Qt4GuiClass"
  !insertmacro InstallProjectTemplate "${MODULE_VSIP_ROOT}\Items\Qt4Class" "Qt4Class"
  !endif

  pop $0
FunctionEnd

Function InstallItemTemplates
  exch $0
  
  !ifdef MODULE_OLD_ADDIN7X
  strcpy $ItemInstDir "$0\qt4items"
  !else
  strcpy $ItemInstDir "$0\uiItems"
  !endif
  CreateDirectory "$ItemInstDir"
  SetOutPath "$ItemInstDir"
  SetOverwrite ifnewer
  !ifndef MODULE_OLD_ADDIN7X
  File "${MODULE_VSIP_ROOT}\Items\uiItems\uiFiles.vsdir"
  !endif
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiDialogButtonsRight.ui"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiDialogButtonsBottom.ui"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiWidget.ui"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiMainWindow.ui"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiDialogButtonsRight.ico"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiDialogButtonsBottom.ico"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiWidget.ico"
  File "${MODULE_VSIP_ROOT}\Items\uiItems\newUiMainWindow.ico"
  
  !ifndef MODULE_OLD_ADDIN7X
  strcpy $ItemInstDir "$0\qrcItems"
  !endif
  CreateDirectory "$ItemInstDir"
  SetOutPath "$ItemInstDir"
  SetOverwrite ifnewer
  !ifndef MODULE_OLD_ADDIN7X
  File "${MODULE_VSIP_ROOT}\Items\qrcItems\qrcFiles.vsdir"
  !endif
  File "${MODULE_VSIP_ROOT}\Items\qrcItems\newQrcFile.qrc"
  File "${MODULE_VSIP_ROOT}\Items\qrcItems\newQrcFile.ico"

  pop $0
FunctionEnd

Function un.InstallProjectTemplates
  exch $0
  strcpy $ProjectInstDir "$0\projects\$VS_VERSION_SHORT"
  strcpy $WizardInstDir "$0\wizards"
  
  !insertmacro UnInstallProjectTemplate "Qt4GuiProject"
  !insertmacro UnInstallProjectTemplate "Qt4ConsoleProject"
  !insertmacro UnInstallProjectTemplate "Qt4LibProject"
  !insertmacro UnInstallProjectTemplate "Qt4ActiveQtServerProject"
  !insertmacro UnInstallProjectTemplate "Qt4DesignerPluginProject"
  !insertmacro UnInstallProjectTemplate "Qt4WinCEProject"
  !insertmacro UnInstallProjectTemplate "Qt4WinCELibProject"

  RmDir "$ProjectInstDir"
  RmDir "$0\projects"

  !ifndef MODULE_OLD_ADDIN7X
  # install the class wizard
  strcpy $ProjectInstDir "$0\wizards\$VS_VERSION_SHORT"
  !insertmacro UnInstallProjectTemplate "Qt4GuiClass"
  !insertmacro UnInstallProjectTemplate "Qt4Class"
  RmDir "$ProjectInstDir"
  !endif

  RmDir "$WizardInstDir"
  pop $0
FunctionEnd

Function un.InstallItemTemplates
  exch $0
  
  !ifdef MODULE_OLD_ADDIN7X
  strcpy $ItemInstDir "$0\qt4items"
  !else
  strcpy $ItemInstDir "$0\uiItems"
  Delete "$ItemInstDir\uiFiles.vsdir"
  !endif
  Delete "$ItemInstDir\newUiDialogButtonsRight.ui"
  Delete "$ItemInstDir\newUiDialogButtonsBottom.ui"
  Delete "$ItemInstDir\newUiWidget.ui"
  Delete "$ItemInstDir\newUiMainWindow.ui"
  Delete "$ItemInstDir\newUiDialogButtonsRight.ico"
  Delete "$ItemInstDir\newUiDialogButtonsBottom.ico"
  Delete "$ItemInstDir\newUiWidget.ico"
  Delete "$ItemInstDir\newUiMainWindow.ico"
  RmDir "$ItemInstDir"
  
  !ifndef MODULE_OLD_ADDIN7X
  strcpy $ItemInstDir "$0\qrcItems"
  Delete "$ItemInstDir\qrcFiles.vsdir"
  !endif
  Delete "$ItemInstDir\newQrcFile.qrc"
  Delete "$ItemInstDir\newQrcFile.ico"
  RmDir "$ItemInstDir"

  pop $0
FunctionEnd

#
# usage:
# push WIZ_NAME
# push PRO_INST_DIR
# push WIZ_INST_DIR
# call CreateVszFile
#
Function CreateVszFile
  exch $1 ;WIZ_INST_DIR
  exch 2
  exch $2 ;WIZ_NAME
  exch
  exch $3 ;PRO_INST_DIR
  push $0
  
  ClearErrors
  FileOpen $0 "$3\$2.vsz" w
  IfErrors done
  FileWrite $0 "VSWIZARD 7.0$\r$\n"

; vs2002 don't use version number at all...
strcmp "$VS_VERSION_SHORT" "7.0" +3
  FileWrite $0 "Wizard=VsWizard.VsWizardEngine.$VS_VERSION_SHORT$\r$\n"
goto +2
  FileWrite $0 "Wizard=VsWizard.VsWizardEngine$\r$\n"

  FileWrite $0 "$\r$\n"
  FileWrite $0 'Param="WIZARD_NAME = $2"$\r$\n'
  FileWrite $0 'Param="ABSOLUTE_PATH = $1\$2"$\r$\n'
  FileWrite $0 'Param="FALLBACK_LCID = 1033"$\r$\n'
  FileClose $0

  done:
  pop $0
  pop $3
  pop $2
  pop $1
FunctionEnd

!endif ;TEMPLATE_INCLUDE
