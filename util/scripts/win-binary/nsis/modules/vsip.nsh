; Integration Module

!ifdef MODULE_VSIP

;------------------------------------------------------------------------------------------------
!macro VSIP_INITIALIZE

!ifndef MODULE_VSIP_NAME
  !define MODULE_VSIP_NAME "Qt Visual Studio Integration"
!endif
!ifndef MODULE_VSIP_VERSION
  !define MODULE_VSIP_VERSION ${PRODUCT_VERSION}
!endif
!ifndef MODULE_VSIP_ROOT
  !define MODULE_VSIP_ROOT "${INSTALL_ROOT}\vsip"
!endif
!ifndef MODULE_HELP_ROOT
  !define MODULE_HELP_ROOT "${MODULE_VSIP_ROOT}\help"
!endif


!include "includes\templates.nsh"
!include "includes\system.nsh"
!include "includes\help.nsh"
!include "includes\regsvr.nsh"
!include "includes\qtcommon.nsh"
!include "WinMessages.nsh"

!macroend ;VSIP_INITIALIZE

;------------------------------------------------------------------------------------------------

!macro VSIP_SECTIONS

SectionGroup "Qt Visual Studio Integration"
!ifndef MODULE_VSIP_NO2003
Section "Visual Studio 2003" VSIP_SEC01
  WriteRegStr SHCTX "SOFTWARE\\Trolltech\\Qt4VS2003" "LicenseKey" $LICENSE_KEY
  
  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  
  WriteRegDWord SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2003" "1"

  StrCpy $VS_VERSION_SHORT "7.1"
  StrCpy $VS_VERSION "2003"
  Push $VS_VERSION_SHORT
  Call InstallVSIP
  
  !insertmacro InstallHelp "$VSIP_INSTDIR\help" "qt4vs" "$VS_VERSION_SHORT"
  
  ;install readme file
  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  File "${MODULE_VSIP_ROOT}\Readme.txt"
  File "${MODULE_VSIP_ROOT}\ui.ico"
  File "${MODULE_VSIP_ROOT}\Changes-${MODULE_VSIP_VERSION}"
SectionEnd
!endif

!ifndef MODULE_VSIP_NO2005
Section "Visual Studio 2005" VSIP_SEC02
  WriteRegStr SHCTX "SOFTWARE\\Trolltech\\Qt4VS2005" "LicenseKey" $LICENSE_KEY

  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  
  WriteRegDWord SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2005" "1"

  StrCpy $VS_VERSION_SHORT "8.0"
  StrCpy $VS_VERSION "2005"
  Push $VS_VERSION_SHORT
  Call InstallVSIP
  
  !insertmacro InstallHelp "$VSIP_INSTDIR\help" "qt4vs" "$VS_VERSION_SHORT"
  
  ;install readme file
  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  File "${MODULE_VSIP_ROOT}\Readme.txt"
  File "${MODULE_VSIP_ROOT}\ui.ico"
  File "${MODULE_VSIP_ROOT}\Changes-${MODULE_VSIP_VERSION}"
SectionEnd
!endif

!ifndef MODULE_VSIP_NO2008
Section "Visual Studio 2008" VSIP_SEC03
  WriteRegStr SHCTX "SOFTWARE\\Trolltech\\Qt4VS2008" "LicenseKey" $LICENSE_KEY

  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  
  WriteRegDWord SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2008" "1"

  StrCpy $VS_VERSION_SHORT "9.0"
  StrCpy $VS_VERSION "2008"
  Push $VS_VERSION_SHORT
  Call InstallVSIP
  
  !insertmacro InstallHelp "$VSIP_INSTDIR\help" "qt4vs" "$VS_VERSION_SHORT"
  
  ;install readme file
  SetOutPath "$VSIP_INSTDIR"
  SetOverwrite ifnewer
  File "${MODULE_VSIP_ROOT}\Readme.txt"
  File "${MODULE_VSIP_ROOT}\ui.ico"
  File "${MODULE_VSIP_ROOT}\Changes-${MODULE_VSIP_VERSION}"
SectionEnd
!endif

SectionGroupEnd

Section -PostVSIPSection
  IfFileExists "$VSIP_INSTDIR\help\h2reg.exe" 0 PostVSIPSectionHelp_Done
  !insertmacro RegisterHelp "$VSIP_INSTDIR\help" "qt4vs"
  PostVSIPSectionHelp_Done:
  
  IfFileExists "$VSIP_INSTDIR\Readme.txt" 0 PostVSIPSection_Done
  !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\Visual Studio Integration Readme.lnk" "$VSIP_INSTDIR\Readme.txt"
  PostVSIPSection_Done:
SectionEnd

Function InstallVSIP
  Exch $0
  
  Push $0
  Call InstallIntegration

  Push $0
  Call RegisterIntegration

  Push $VSIP_INSTDIR
  Call InstallProjectTemplates

  Push $VSIP_INSTDIR
  Call InstallItemTemplates

  Call InstallResources
  
  Call InstallSamples

  Push $0
  Call InstallAddin

  Pop $0
FunctionEnd


Function InstallIntegration
  Exch $0
  Push $1
  
  CreateDirectory "$VSIP_INSTDIR\bin"
  SetOutPath "$VSIP_INSTDIR\bin"
  SetOverwrite ifnewer

  ; Install common files
  File "${MODULE_VSIP_ROOT}\bin\QtCore4.dll"
  File "${MODULE_VSIP_ROOT}\bin\QtGui4.dll"
  File "${MODULE_VSIP_ROOT}\bin\QtXml4.dll"
  File "${MODULE_VSIP_ROOT}\bin\QtScript4.dll"  

  File "${MODULE_VSIP_ROOT}\bin\QtDesigner4.dll"
  File "${MODULE_VSIP_ROOT}\bin\QtDesignerComponents4.dll"
  File "${MODULE_VSIP_ROOT}\bin\FormEditor1.dll"
  
  DetailPrint "Patching $VSIP_INSTDIR\bin\QtCore4.dll..."
  Push "$VSIP_INSTDIR"
  Push "$VSIP_INSTDIR\bin\QtCore4.dll"
  Call PatchBinaryPaths

  CreateDirectory "$VSIP_INSTDIR\bin\$0"
  SetOutPath "$VSIP_INSTDIR\bin\$0"
  SetOverwrite ifnewer

  StrCmp $0 "7.1" 0 MODULE_VSIP_2005_1
    File "${MODULE_VSIP_ROOT}\bin\7.1\Qt4VSa.dll"
    SetFileAttributes "$VSIP_INSTDIR\bin\7.1\Qt4VSa.dll" HIDDEN
    ClearErrors
  
    ; Install Integration Libs
    File "${MODULE_VSIP_ROOT}\bin\7.1\Axformeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\formeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\QtProjectLib.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\Microsoft.VisualStudio.dll"

    File "${MODULE_VSIP_ROOT}\bin\7.1\Qt4VS2003.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\Trolltech.Qt4VS2003Base.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\Microsoft.VisualStudio.Designer.Interfaces.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\RegQt4VS2003.exe"

    File "${MODULE_VSIP_ROOT}\bin\7.1\QtProjectEngineLib.dll"

    ; Install MS Interop Assemblies
    File "${MODULE_VSIP_ROOT}\bin\7.1\Microsoft.VisualStudio.OLE.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\Microsoft.VisualStudio.Shell.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\Microsoft.VisualStudio.TextManager.Interop.dll"

    Goto MODULE_VSIP_ALL
    
  MODULE_VSIP_2005_1:
  StrCmp $0 "8.0" 0 MODULE_VSIP_2008
    File "${MODULE_VSIP_ROOT}\bin\8.0\Qt4VSa.dll"
    SetFileAttributes "$VSIP_INSTDIR\bin\8.0\Qt4VSa.dll" HIDDEN
    ClearErrors

    File "${MODULE_VSIP_ROOT}\bin\8.0\Axformeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\formeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\QtProjectLib.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\Microsoft.VisualStudio.dll"
    
    File "${MODULE_VSIP_ROOT}\bin\8.0\Qt4VS2005.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\Trolltech.Qt4VS2005Base.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\RegQt4VS2005.exe"
    
    File "${MODULE_VSIP_ROOT}\bin\8.0\QtProjectEngineLib.dll"

    ; Install MS Interop Assemblies
    File "${MODULE_VSIP_ROOT}\bin\8.0\Microsoft.VisualStudio.OLE.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\Microsoft.VisualStudio.Shell.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\Microsoft.VisualStudio.TextManager.Interop.dll"
    Goto MODULE_VSIP_ALL

  MODULE_VSIP_2008:
  StrCmp $0 "9.0" 0 MODULE_VSIP_ALL
    File "${MODULE_VSIP_ROOT}\bin\9.0\Qt4VSa.dll"
    SetFileAttributes "$VSIP_INSTDIR\bin\9.0\Qt4VSa.dll" HIDDEN
    ClearErrors

    File "${MODULE_VSIP_ROOT}\bin\9.0\Axformeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\formeditor1Lib.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\QtProjectLib.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\Microsoft.VisualStudio.dll"
    
    File "${MODULE_VSIP_ROOT}\bin\9.0\Qt4VS2008.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\Trolltech.Qt4VS2008Base.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\RegQt4VS2008.exe"
    
    File "${MODULE_VSIP_ROOT}\bin\9.0\QtProjectEngineLib.dll"

    ; Install MS Interop Assemblies
    File "${MODULE_VSIP_ROOT}\bin\9.0\Microsoft.VisualStudio.OLE.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\Microsoft.VisualStudio.Shell.Interop.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\Microsoft.VisualStudio.TextManager.Interop.dll"


  MODULE_VSIP_ALL:
  !insertmacro InstallUnManagedTranslation $0 "1033"
  !insertmacro InstallUnManagedTranslation $0 "1031"
  !insertmacro InstallUnManagedTranslation $0 "1041"
  !insertmacro InstallManagedTranslation $0 "de"
  !insertmacro InstallManagedTranslation $0 "ja"
  
  ;CreateDirectory "$VSIP_INSTDIR\bin\$0\1033"
  ;SetOutPath "$VSIP_INSTDIR\bin\$0\1033"
  ;SetOverwrite ifnewer
  
  ;StrCmp $0 "8.0" 0 +2
  ;  File "${MODULE_VSIP_ROOT}\bin\8.0\1033\Qt4VS2005UI.dll"

  ;StrCmp $0 "7.1" 0 +2
  ;File "${MODULE_VSIP_ROOT}\bin\7.1\1033\Qt4VS2003UI.dll"

  SetOutPath "$VSIP_INSTDIR\plugins\designer"
  SetOverwrite ifnewer
  File "${MODULE_VSIP_ROOT}\plugins\designer\qaxwidget.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\qt3supportwidgets.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\customwidgetplugin.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\worldtimeclockplugin.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\containerextension.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\taskmenuextension.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\QtNetwork4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\Qt3Support4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\designer\QtSql4.dll"
  ${If} $0 != "7.1"
    File "${MODULE_VSIP_ROOT}\plugins\designer\phonon4.dll"
    File "${MODULE_VSIP_ROOT}\plugins\designer\QtWebKit4.dll"
    File "${MODULE_VSIP_ROOT}\plugins\designer\qwebview.dll"
  ${EndIf}
  
  SetOutPath "$VSIP_INSTDIR\plugins\imageformats"
  SetOverwrite ifnewer
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\QtSvg4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\qsvg4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\qgif4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\qjpeg4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\qtiff4.dll"
  File "${MODULE_VSIP_ROOT}\plugins\imageformats\qico4.dll"
 
  ; Install VC redistributable
  Push $0
  Call IsVS2008RedistInstalled
  Pop $0
  ${If} $0 == 0
    SetOutPath "$VSIP_INSTDIR"
    SetOverwrite ifnewer
    File "${MODULE_VSIP_ROOT}\redist\vcredist_x86.exe"
    ExecWait '"$VSIP_INSTDIR\vcredist_x86.exe" /q' $0
    Delete "$VSIP_INSTDIR\vcredist_x86.exe"
  ${EndIf}
  Pop $0

  ; Install usertype.dat

  Push $0
  Call GetVSInstallationDir
  Pop $1
  SetOutPath "$1"
  SetOverwrite off ;ifnewer
  File "${MODULE_VSIP_ROOT}\usertype.dat"

  Pop $1
  Pop $0
FunctionEnd


Function InstallAddin
  Exch $0
  CreateDirectory "$VSIP_INSTDIR\bin\$0"
  ClearErrors
  SetOutPath "$VSIP_INSTDIR\bin\$0"
  SetOverwrite ifnewer

  StrCmp $0 "7.1" MODULE_VSIP_ADDIN_71
  StrCmp $0 "8.0" MODULE_VSIP_ADDIN_80
    File "${MODULE_VSIP_ROOT}\bin\9.0\StartQtVSIP.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\StartQtVSIP2008.AddIn"
    WriteRegStr SHCTX "Software\Microsoft\VisualStudio\9.0\AutomationOptions\LookInFolders" "$VSIP_INSTDIR\bin\$0" ""
    Goto MODULE_VSIP_ADDIN_End

  MODULE_VSIP_ADDIN_80:
    File "${MODULE_VSIP_ROOT}\bin\8.0\StartQtVSIP.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\StartQtVSIP2005.AddIn"
    WriteRegStr SHCTX "Software\Microsoft\VisualStudio\8.0\AutomationOptions\LookInFolders" "$VSIP_INSTDIR\bin\$0" ""
    Goto MODULE_VSIP_ADDIN_End
    
  MODULE_VSIP_ADDIN_71:
  Push $1
  Push $2
  
  Push $0
  Call IsDotNETInstalled
  Pop $1
  
  File "${MODULE_VSIP_ROOT}\bin\7.1\StartQtVSIP.dll"
  
  ClearErrors
  DetailPrint "Registering StartQtVSIP..."
  nsExec::ExecToLog '"$1\regasm.exe" /codebase "StartQtVSIP.dll"'
  Pop $2
  StrCmp $2 "error" 0 +3
    MessageBox MB_OK 'The command $\n"$1\regasm.exe" /codebase "StartQtVSIP.dll"$\n failed.'
    Goto MODULE_VSIP_ADDIN_71_End

  WriteRegDWORD SHCTX "Software\Microsoft\VisualStudio\7.1\Addins\StartQtVSIP" "CommandLineSafe" 0x00000001
  WriteRegDWORD SHCTX "Software\Microsoft\VisualStudio\7.1\Addins\StartQtVSIP" "CommandPreload" 0x00000000
  WriteRegDWORD SHCTX "Software\Microsoft\VisualStudio\7.1\Addins\StartQtVSIP" "LoadBehavior" 0x00000003

  MODULE_VSIP_ADDIN_71_End:
  Pop $2
  Pop $1

  MODULE_VSIP_ADDIN_End:
  Pop $0
FunctionEnd


Function RegisterIntegration
  Exch $0
  Push $1
  Push $2
  Push $3

  Push $0
  Call GetVSInstallationDir
  Pop $1
  
  Push $0
  Call IsDotNETInstalled
  Pop $2
  
  SetOutPath "$VSIP_INSTDIR\bin"

;  MessageBox MB_OK '"$2\regasm.exe" /codebase "$0\bin\QtProjectEngineLib.dll"'
;  MessageBox MB_OK '"$0\bin\RegQt4VS${MODULE_VSIP_VS_VERSION}.exe" /templatepath:"$0//" "$0\bin\Qt4VS${MODULE_VSIP_VS_VERSION}.dll"'
;  MessageBox MB_OK "Can not setup devenv! The command $\n$1\devenv.exe /setup$\n failed. Try to run in manually!"
  
  ClearErrors ; clear the error flag
  nsExec::ExecToLog '"$2\regasm.exe" /codebase "$VSIP_INSTDIR\bin\$0\QtProjectEngineLib.dll"'
  Pop $3
  StrCmp $3 "error" 0 Module_VSIP_RegFormEditor
    MessageBox MB_OK "Can not register QtProjectEngineLib.dll!"
    MessageBox MB_OK '"$2\regasm.exe" /codebase "$VSIP_INSTDIR\bin\$0\QtProjectEngineLib.dll"'

  Module_VSIP_RegFormEditor:
  ClearErrors
  push "$VSIP_INSTDIR\bin\formeditor1.dll"
  call RegSvr
  IfErrors 0 Module_VSIP_RegPackage
    MessageBox MB_OK "Can not register formeditor1.dll!"

  Module_VSIP_RegPackage:
  StrCpy $VS_VERSION "2008"
  StrCmp $0 "8.0" 0 +2
    StrCpy $VS_VERSION "2005"
  StrCmp $0 "7.1" 0 +2
    StrCpy $VS_VERSION "2003"
  nsExec::ExecToLog '"$VSIP_INSTDIR\bin\$0\RegQt4VS$VS_VERSION.exe" /root:Software\Microsoft\VisualStudio\$0 /templatepath:"$VSIP_INSTDIR//" "$VSIP_INSTDIR\bin\$0\Qt4VS$VS_VERSION.dll"'
  Pop $3
  StrCmp $3 "error" 0 Module_VSIP_SetupVS
    MessageBox MB_OK "Can not register package!"

  Module_VSIP_SetupVS:
  DetailPrint "Calling devenv /setup ..."
  nsExec::Exec '"$1\devenv.exe" /setup'
  Pop $3
  StrCmp $3 "error" 0 Module_VSIP_SetupOK
    MessageBox MB_OK "Can not setup devenv! The command $\n$1\devenv.exe /setup$\n failed. Try to run in manually!"
    goto Module_VSIP_EndRegisterPackage
  Module_VSIP_SetupOK:
    DetailPrint "Running setup was successfull."
  Module_VSIP_EndRegisterPackage:
  
  Pop $3
  Pop $2
  Pop $1
  Pop $0
FunctionEnd


Function InstallSamples
  CreateDirectory "$VSIP_INSTDIR\samples\AddressBook"
  SetOutPath "$VSIP_INSTDIR\samples\AddressBook"
  SetOverwrite ifnewer

  File "${MODULE_VSIP_ROOT}\samples\AddressBook\adddialog.cpp"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\adddialog.h"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\adddialog.ui"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\addressbook.cpp"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\addressbook.h"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\AddressBook.ico"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\AddressBook.rc"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\addressbook.ui"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\AddressBook.vcproj"
  File "${MODULE_VSIP_ROOT}\samples\AddressBook\main.cpp"
FunctionEnd

Function InstallResources
  CreateDirectory "$VSIP_INSTDIR\resources"

  SetOutPath "$VSIP_INSTDIR\resources"
  SetOverWrite ifnewer

  !insertmacro InstallResourceFiles "bmp" "bitmap"
  !insertmacro InstallResourceFiles "txt" "text"
  !insertmacro InstallResourceFiles "htm" "page"
  !insertmacro InstallResourceFiles "xml" "xmlfile"
  !insertmacro InstallResourceFiles "png" "image"
  !insertmacro InstallResourceFiles "ui" "form"
FunctionEnd

Function un.InstallVSIP
  Exch $0
  
  Push $0
  Call un.InstallAddin

  Call un.InstallSamples
  
  Call un.InstallResources

  Push $VSIP_INSTDIR
  Call un.InstallProjectTemplates

  Push $VSIP_INSTDIR
  Call un.InstallItemTemplates

  Push $0
  Call un.RegisterIntegration

  Push $0
  Call un.InstallIntegration
  
  Pop $0
FunctionEnd


Function un.InstallIntegration
  Exch $0
  Push $1
  
  Delete "$VSIP_INSTDIR\bin\$0\Microsoft.VisualStudio.TextManager.Interop.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Microsoft.VisualStudio.Shell.Interop.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Microsoft.VisualStudio.OLE.Interop.dll"
  Delete "$VSIP_INSTDIR\bin\$0\QtProjectEngineLib.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Microsoft.VisualStudio.dll"

  StrCmp $0 "7.1" 0 MODULE_VSIP_UNINST_2005
  ;Delete "$VSIP_INSTDIR\bin\$0\1033\Qt4VS2003UI.dll"
  Delete "$VSIP_INSTDIR\bin\$0\RegQt4VS2003.exe"
  Delete "$VSIP_INSTDIR\bin\$0\Microsoft.VisualStudio.Designer.Interfaces.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Trolltech.Qt4VS2003Base.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Qt4VS2003.dll"
  Goto MODULE_VSIP_CONTINUE
  
  MODULE_VSIP_UNINST_2005:
  StrCmp $0 "8.0" 0 MODULE_VSIP_UNINST_2008
  ;Delete "$VSIP_INSTDIR\bin\$0\1033\Qt4VS2005UI.dll"
  Delete "$VSIP_INSTDIR\bin\$0\RegQt4VS2005.exe"
  Delete "$VSIP_INSTDIR\bin\$0\Trolltech.Qt4VS2005Base.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Qt4VS2005.dll"
  Goto MODULE_VSIP_CONTINUE

  MODULE_VSIP_UNINST_2008:
  ;Delete "$VSIP_INSTDIR\bin\$0\1033\Qt4VS2008UI.dll"
  Delete "$VSIP_INSTDIR\bin\$0\RegQt4VS2008.exe"
  Delete "$VSIP_INSTDIR\bin\$0\Trolltech.Qt4VS2008Base.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Qt4VS2008.dll"

  MODULE_VSIP_CONTINUE:
  !insertmacro UnInstallUnManagedTranslation $0 "1033"
  !insertmacro UnInstallUnManagedTranslation $0 "1031"
  !insertmacro UnInstallUnManagedTranslation $0 "1041"
  !insertmacro UnInstallManagedTranslation $0 "de"
  !insertmacro UnInstallManagedTranslation $0 "ja"
  
  ;RmDir "$VSIP_INSTDIR\bin\$0\1033"
  
  Delete "$VSIP_INSTDIR\bin\$0\QtProjectLib.dll"
  Delete "$VSIP_INSTDIR\bin\$0\formeditor1Lib.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Axformeditor1Lib.dll"
  Delete "$VSIP_INSTDIR\bin\$0\Qt4VSa.dll"
  RmDir "$VSIP_INSTDIR\bin\$0"
  
  Delete "$VSIP_INSTDIR\bin\FormEditor1.dll"
  Delete "$VSIP_INSTDIR\bin\QtDesigner4.dll"
  Delete "$VSIP_INSTDIR\bin\QtDesignerComponents4.dll"
  Delete "$VSIP_INSTDIR\bin\QtCore4.dll"
  Delete "$VSIP_INSTDIR\bin\QtGui4.dll"
  Delete "$VSIP_INSTDIR\bin\QtXml4.dll"
  Delete "$VSIP_INSTDIR\bin\QtScript4.dll"
  RmDir "$VSIP_INSTDIR\bin"

  Push $0
  Call un.GetVSInstallationDir
  Pop $1
  ;Delete "$1\usertype.dat"
  
  Delete "$VSIP_INSTDIR\plugins\imageformats\QtSvg4.dll"
  Delete "$VSIP_INSTDIR\plugins\imageformats\qsvg4.dll"
  Delete "$VSIP_INSTDIR\plugins\imageformats\qgif4.dll"
  Delete "$VSIP_INSTDIR\plugins\imageformats\qjpeg4.dll"
  Delete "$VSIP_INSTDIR\plugins\imageformats\qtiff4.dll"
  Delete "$VSIP_INSTDIR\plugins\imageformats\qico4.dll"
  RmDir "$VSIP_INSTDIR\plugins\imageformats"

  Delete "$VSIP_INSTDIR\plugins\designer\qaxwidget.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\qt3supportwidgets.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\customwidgetplugin.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\worldtimeclockplugin.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\containerextension.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\taskmenuextension.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\qwebview.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\QtSql4.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\QtNetwork4.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\phonon4.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\QtWebKit4.dll"
  Delete "$VSIP_INSTDIR\plugins\designer\Qt3Support4.dll"
  RmDir "$VSIP_INSTDIR\plugins\designer"
  RmDir "$VSIP_INSTDIR\plugins"

  Pop $1
  Pop $0
FunctionEnd


Function un.InstallAddin
  Exch $0

  StrCmp $0 "7.1" MODULE_VSIP_ADDIN_71 0
  StrCmp $0 "8.0" MODULE_VSIP_ADDIN_80 0
    Delete "$VSIP_INSTDIR\bin\$0\StartQtVSIP2008.AddIn"
    DeleteRegValue SHCTX "Software\Microsoft\VisualStudio\9.0\AutomationOptions\LookInFolders" "$VSIP_INSTDIR\bin\$0"
    Goto MODULE_VSIP_ADDIN_End

  MODULE_VSIP_ADDIN_80:
    Delete "$VSIP_INSTDIR\bin\$0\StartQtVSIP2005.AddIn"
    DeleteRegValue SHCTX "Software\Microsoft\VisualStudio\8.0\AutomationOptions\LookInFolders" "$VSIP_INSTDIR\bin\$0"
    Goto MODULE_VSIP_ADDIN_End
    
  MODULE_VSIP_ADDIN_71:
  Push $1
  Push $2
  
  Push $0
  Call un.IsDotNETInstalled
  Pop $1

  ClearErrors
  DetailPrint "Unregistering StartQtVSIP..."
  nsExec::ExecToLog '"$1\regasm.exe" /unregister "$VSIP_INSTDIR\bin\$0\StartQtVSIP.dll"'
  Pop $2
  StrCmp $2 "error" 0 +3
    MessageBox MB_OK 'The command $\n"$1\regasm.exe" /unregister "$VSIP_INSTDIR\bin\$0\StartQtVSIP.dll"$\n failed.'
    Goto MODULE_VSIP_ADDIN_71_End

  DeleteRegKey SHCTX "Software\Microsoft\VisualStudio\7.1\Addins\StartQtVSIP"

  MODULE_VSIP_ADDIN_71_End:
  Pop $2
  Pop $1

  MODULE_VSIP_ADDIN_End:

  Delete "$VSIP_INSTDIR\bin\$0\StartQtVSIP.dll"
  Pop $0
FunctionEnd


Function un.RegisterIntegration
  Exch $0
  Push $1
  Push $2
  Push $3

  Push $0
  Call un.GetVSInstallationDir
  Pop $1

  Push $0
  Call un.IsDotNETInstalled
  Pop $2
  
;  UnRegPackage:
  IfErrors 0 ; clear the error flag
  StrCpy $VS_VERSION "2003"
  StrCmp $0 "8.0" 0 +2
    StrCpy $VS_VERSION "2005"
  StrCmp $0 "9.0" 0 +2
    StrCpy $VS_VERSION "2008"

  IfFileExists "$VSIP_INSTDIR\bin\$0\RegQt4VS$VS_VERSION.exe" 0 Module_VSIP_UnRegProjectEngine
  nsExec::ExecToLog '"$VSIP_INSTDIR\bin\$0\RegQt4VS$VS_VERSION.exe" /root:Software\Microsoft\VisualStudio\$0 /unregister "$VSIP_INSTDIR\bin\$0\Qt4VS$VS_VERSION.dll"'
  Pop $3
  StrCmp $3 "error" 0 Module_VSIP_UnRegProjectEngine
    MessageBox MB_OK "Can not unregister Package!"

  Module_VSIP_UnRegProjectEngine:
  IfFileExists "$VSIP_INSTDIR\bin\$0\QtProjectEngineLib.dll" 0 Module_VSIP_UnRegFormEditor
  nsExec::ExecToLog '"$2\regasm.exe" /unregister "$VSIP_INSTDIR\bin\$0\QtProjectEngineLib.dll"'
  Pop $3
  StrCmp $3 "error" 0 Module_VSIP_UnRegFormEditor
    MessageBox MB_OK "Can not unregister QtProjectEngineLib.dll!"

  Module_VSIP_UnRegFormEditor:
  IfFileExists "$VSIP_INSTDIR\bin\formeditor1.dll" 0 Module_VSIP_SetupVS

  ClearErrors
  SetOutPath "$VSIP_INSTDIR\bin"
  push "$VSIP_INSTDIR\bin\formeditor1.dll"
  call un.RegSvr

  Module_VSIP_SetupVS:
  SetOutPath "$1" ; don't stay in .\bin
  StrCmp $1 "" Module_VSIP_EndUnReg 0
    DetailPrint "Calling devenv /setup ..."
    nsExec::Exec '"$1\devenv.exe" /setup'
    Pop $3
    StrCmp $3 "error" 0 Module_VSIP_SetupOK
      MessageBox MB_OK "Can not setup devenv! The command $\n$1\devenv.exe /setup$\n failed. Try to run in manually!"
      Goto Module_VSIP_EndUnReg
  Module_VSIP_SetupOK:
    DetailPrint "Running setup was successfull."
  Module_VSIP_EndUnReg:
    Pop $3
    Pop $2
    Pop $1
    Pop $0
FunctionEnd


Function un.InstallSamples
  Delete "$VSIP_INSTDIR\samples\AddressBook\adddialog.cpp"
  Delete "$VSIP_INSTDIR\samples\AddressBook\adddialog.h"
  Delete "$VSIP_INSTDIR\samples\AddressBook\adddialog.ui"
  Delete "$VSIP_INSTDIR\samples\AddressBook\addressbook.cpp"
  Delete "$VSIP_INSTDIR\samples\AddressBook\addressbook.h"
  Delete "$VSIP_INSTDIR\samples\AddressBook\AddressBook.ico"
  Delete "$VSIP_INSTDIR\samples\AddressBook\AddressBook.rc"
  Delete "$VSIP_INSTDIR\samples\AddressBook\addressbook.ui"
  Delete "$VSIP_INSTDIR\samples\AddressBook\AddressBook.vcproj"
  Delete "$VSIP_INSTDIR\samples\AddressBook\main.cpp"
  RMDir "$VSIP_INSTDIR\samples\AddressBook"
  RMDir "$VSIP_INSTDIR\samples"
FunctionEnd

Function un.InstallResources
  !insertmacro UnInstallResourceFiles "bmp" "bitmap"
  !insertmacro UnInstallResourceFiles "txt" "text"
  !insertmacro UnInstallResourceFiles "htm" "page"
  !insertmacro UnInstallResourceFiles "xml" "xmlfile"
  !insertmacro UnInstallResourceFiles "png" "image"
  !insertmacro UnInstallResourceFiles "ui" "form"

  RmDir "$VSIP_INSTDIR\resources"
FunctionEnd
!macroend

!macro InstallResourceFiles TYPE TMPLNAME
  File "${MODULE_VSIP_ROOT}\resources\${TYPE}.xml"
  File "${MODULE_VSIP_ROOT}\resources\${TYPE}icon.bmp"
  File "${MODULE_VSIP_ROOT}\resources\${TMPLNAME}.${TYPE}"
!macroend

!macro UnInstallResourceFiles TYPE TMPLNAME
  Delete "$VSIP_INSTDIR\resources\${TYPE}.xml"
  Delete "$VSIP_INSTDIR\resources\${TYPE}icon.bmp"
  Delete "$VSIP_INSTDIR\resources\${TMPLNAME}.${TYPE}"
!macroend

!macro InstallUnManagedTranslation VS_SHORT LOCALE
  CreateDirectory "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
  SetOutPath "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
  SetOverwrite ifnewer

  StrCmp ${VS_SHORT} "9.0" 0 +2
    File "${MODULE_VSIP_ROOT}\bin\9.0\${LOCALE}\Qt4VS2008UI.dll"

  StrCmp ${VS_SHORT} "8.0" 0 +2
    File "${MODULE_VSIP_ROOT}\bin\8.0\${LOCALE}\Qt4VS2005UI.dll"

  StrCmp ${VS_SHORT} "7.1" 0 +2
    File "${MODULE_VSIP_ROOT}\bin\7.1\${LOCALE}\Qt4VS2003UI.dll"
!macroend

!macro UnInstallUnManagedTranslation VS_SHORT LOCALE
  StrCmp ${VS_SHORT} "9.0" 0 +2
    Delete "$VSIP_INSTDIR\bin\9.0\${LOCALE}\Qt4VS2008UI.dll"

  StrCmp ${VS_SHORT} "8.0" 0 +2
    Delete "$VSIP_INSTDIR\bin\8.0\${LOCALE}\Qt4VS2005UI.dll"

  StrCmp ${VS_SHORT} "7.1" 0 +2
    Delete "$VSIP_INSTDIR\bin\7.1\${LOCALE}\Qt4VS2003UI.dll"

  RMDir "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
!macroend

!macro InstallManagedTranslation VS_SHORT LOCALE
  CreateDirectory "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
  SetOutPath "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
  SetOverwrite ifnewer

  StrCmp ${VS_SHORT} "9.0" 0 +3
    File "${MODULE_VSIP_ROOT}\bin\9.0\${LOCALE}\Qt4VS2008.resources.dll"
    File "${MODULE_VSIP_ROOT}\bin\9.0\${LOCALE}\QtProjectLib.resources.dll"

  StrCmp ${VS_SHORT} "8.0" 0 +3
    File "${MODULE_VSIP_ROOT}\bin\8.0\${LOCALE}\Qt4VS2005.resources.dll"
    File "${MODULE_VSIP_ROOT}\bin\8.0\${LOCALE}\QtProjectLib.resources.dll"

  StrCmp ${VS_SHORT} "7.1" 0 +3
    File "${MODULE_VSIP_ROOT}\bin\7.1\${LOCALE}\Qt4VS2003.resources.dll"
    File "${MODULE_VSIP_ROOT}\bin\7.1\${LOCALE}\QtProjectLib.resources.dll"
!macroend

!macro UnInstallManagedTranslation VS_SHORT LOCALE
  StrCmp ${VS_SHORT} "9.0" 0 +2
    Delete "$VSIP_INSTDIR\bin\9.0\${LOCALE}\Qt4VS2008.resources.dll"

  StrCmp ${VS_SHORT} "8.0" 0 +2
    Delete "$VSIP_INSTDIR\bin\8.0\${LOCALE}\Qt4VS2005.resources.dll"

  StrCmp ${VS_SHORT} "7.1" 0 +2
    Delete "$VSIP_INSTDIR\bin\7.1\${LOCALE}\Qt4VS2003.resources.dll"

  Delete "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}\QtProjectLib.resources.dll"
  RMDir "$VSIP_INSTDIR\bin\${VS_SHORT}\${LOCALE}"
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${VSIP_SEC01} "Qt Integration for Visual Studio .NET 2003 v${MODULE_VSIP_VERSION}."
  !insertmacro MUI_DESCRIPTION_TEXT ${VSIP_SEC02} "Qt Integration for Visual Studio .NET 2005 v${MODULE_VSIP_VERSION}."
  !insertmacro MUI_DESCRIPTION_TEXT ${VSIP_SEC03} "Qt Integration for Visual Studio .NET 2008 v${MODULE_VSIP_VERSION}."
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_STARTUP
  Push $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  Push $6

  ClearErrors
  StrCmp "$RUNNING_AS_ADMIN" "false" 0 Module_VSIP_CheckForInstallation
!ifndef MODULE_VSIP_NO2003
    SectionSetFlags ${VSIP_SEC01} "16"
!endif
!ifndef MODULE_VSIP_NO2005
    SectionSetFlags ${VSIP_SEC02} "16"
!endif
!ifndef MODULE_VSIP_NO2008
    SectionSetFlags ${VSIP_SEC03} "16"
!endif
  Module_VSIP_CheckForInstallation:
  
  StrCpy $2 "0"
  StrCpy $4 "0"
  StrCpy $5 "0"
  
  ClearErrors
  
  Push "7.1"
  Call IsIntegrationInstalled
  Pop $1

!ifndef MODULE_VSIP_NO2003
  Push "7.1"
  Call IsQMsNetInstalled
  Pop $4

  IntCmp $1 0 +2
    SectionSetFlags ${VSIP_SEC01} "16"
    
  Push "7.1"
  Call GetVSInstallationDir
  Pop $3
  StrCmp $3 "" 0 +2
    SectionSetFlags ${VSIP_SEC01} "16"
!endif

  ClearErrors

  Push "8.0"
  Call IsIntegrationInstalled
  Pop $1

!ifndef MODULE_VSIP_NO2005
  Push "8.0"
  Call IsQMsNetInstalled
  Pop $2

  IntCmp $1 0 +2
    SectionSetFlags ${VSIP_SEC02} "16"

  Push "8.0"
  Call GetVSInstallationDir
  Pop $3
  StrCmp $3 "" 0 +3
    SectionSetFlags ${VSIP_SEC02} "16"
    Goto VSIP_STARTUP_2008

  Call IsVS2005SP1Installed
  Pop $1
  ${If} $1 == 0
    SectionSetFlags ${VSIP_SEC02} ${SF_RO}
    MessageBox MB_OK|MB_ICONEXCLAMATION "Service Pack 1 for Visual Studio 2005 is not installed.$\n${PRODUCT_NAME} for VS 2005 will not work properly without it.$\nYou will not be able to choose the ${PRODUCT_NAME} for VS 2005 component for installation."
  ${EndIf}

  Call IsDotNet2SP1Installed
  Pop $1
  ${If} $1 == 0
    StrCpy $6 "1"
    SectionSetFlags ${VSIP_SEC02} ${SF_RO}
    MessageBox MB_OK|MB_ICONEXCLAMATION "Service Pack 1 for .NET Framework 2.0 is not installed.$\n${PRODUCT_NAME} for VS 2005 will not work properly without it.$\nYou will not be able to choose the ${PRODUCT_NAME} for VS 2005 component for installation."
  ${Else}
    StrCpy $6 "0"
  ${EndIf}
VSIP_STARTUP_2008:
!endif

  ClearErrors

  Push "9.0"
  Call IsIntegrationInstalled
  Pop $1

!ifndef MODULE_VSIP_NO2008
  Push "9.0"
  Call IsQMsNetInstalled
  Pop $5

  IntCmp $1 0 +2
    SectionSetFlags ${VSIP_SEC03} "16"

  Push "9.0"
  Call GetVSInstallationDir
  Pop $3
  StrCmp $3 "" 0 +2
    SectionSetFlags ${VSIP_SEC03} "16"

  ${If} $6 == "0"
      Call IsDotNet2SP1Installed
      Pop $1
      ${If} $1 == 0
        SectionSetFlags ${VSIP_SEC02} ${SF_RO}
        MessageBox MB_OK|MB_ICONEXCLAMATION "Service Pack 1 for .NET Framework 2.0 is not installed.$\n${PRODUCT_NAME} for VS 2008 will not work properly without it.$\nYou will not be able to choose the ${PRODUCT_NAME} for VS 2008 component for installation."
      ${EndIf}
  ${EndIf}
!endif

  IntOp $1 $2 + $4
  IntOp $1 $1 + $5
  IntCmp $1 0 +2
    MessageBox MB_OK|MB_ICONEXCLAMATION "The installer has detected that the Qt Visual Studio Add-In is installed.$\r$\nThe Add-In conflicts with the integration."

  StrCpy $VSIP_INSTDIR "$PROGRAMFILES\Nokia\Qt VS Integration"

!ifndef MODULE_VSIP_NO2003
  SectionSetSize ${VSIP_SEC01} 7000
!endif
!ifndef MODULE_VSIP_NO2005
  SectionSetSize ${VSIP_SEC02} 7000
!endif
!ifndef MODULE_VSIP_NO2008
  SectionSetSize ${VSIP_SEC03} 7000
!endif

  Pop $6
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Pop $0
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_FINISH
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_UNSTARTUP
  !insertmacro ConfirmOnRemove "Qt4VS2003" "Qt Visual Studio Integration (VS2003)"
  !insertmacro ConfirmOnRemove "Qt4VS2005" "Qt Visual Studio Integration (VS2005)"
  !insertmacro ConfirmOnRemove "Qt4VS2008" "Qt Visual Studio Integration (VS2008)"
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_UNINSTALL

  Push $0
  Push $1
  Push $2

  ReadRegDWord $0 SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2003"
  IntCmp $0 1 0 MODULE_VSIP_UNINSTALL2005
    StrCpy $VS_VERSION_SHORT "7.1"
    StrCpy $VS_VERSION "2003"
    Push $VS_VERSION_SHORT
    Call un.InstallVSIP
    !insertmacro un.InstallHelpInVS "qt4vs" "$VS_VERSION_SHORT"
    
  MODULE_VSIP_UNINSTALL2005:
    ReadRegDWord $1 SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2005"
    IntCmp $1 1 0 MODULE_VSIP_UNINSTALL2008
      StrCpy $VS_VERSION_SHORT "8.0"
      StrCpy $VS_VERSION "2005"
      Push $VS_VERSION_SHORT
      Call un.InstallVSIP
      !insertmacro un.InstallHelpInVS "qt4vs" "$VS_VERSION_SHORT"

  MODULE_VSIP_UNINSTALL2008:
    ReadRegDWord $2 SHCTX "$PRODUCT_UNIQUE_KEY" "Qt4VS2008"
    IntCmp $2 1 0 MODULE_VSIP_UNINSTALL_COMMON
      StrCpy $VS_VERSION_SHORT "9.0"
      StrCpy $VS_VERSION "2008"
      Push $VS_VERSION_SHORT
      Call un.InstallVSIP
      !insertmacro un.InstallHelpInVS "qt4vs" "$VS_VERSION_SHORT"
    
  MODULE_VSIP_UNINSTALL_COMMON:
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Visual Studio Integration Readme.lnk"
  
  IfFileExists "$VSIP_INSTDIR\help\h2reg.exe" 0 MODULE_VSIP_UNINSTALL_HELP_DONE
  !insertmacro un.RegisterHelp "$VSIP_INSTDIR\help" "qt4vs"
  !insertmacro un.InstallHelp "$VSIP_INSTDIR\help" "qt4vs" "0.0"
  Push "$VSIP_INSTDIR\help"
  Call un.DeleteH2RegFiles
  MODULE_VSIP_UNINSTALL_HELP_DONE:

  Delete "$VSIP_INSTDIR\Readme.txt"
  Delete "$VSIP_INSTDIR\ui.ico"
  Delete "$VSIP_INSTDIR\Changes-${MODULE_VSIP_VERSION}"
  RmDir "$VSIP_INSTDIR"
      
  Pop $2
  Pop $1
  Pop $0
  
!macroend

;------------------------------------------------------------------------------------------------

!macro VSIP_UNFINISH
!macroend

;------------------------------------------------------------------------------------------------

!else
!macro VSIP_INITIALIZE
!macroend
!macro VSIP_SECTIONS
!macroend
!macro VSIP_MERGE_HELP_NAMESPACE_SECTIONS
!macroend
!macro VSIP_DESCRIPTION
!macroend
!macro VSIP_STARTUP
!macroend
!macro VSIP_FINISH
!macroend
!macro VSIP_UNSTARTUP
!macroend
!macro VSIP_CLEANUP_HELP_NAMESPACE
!macroend
!macro VSIP_UNINSTALL
!macroend
!macro VSIP_UNFINISH
!macroend
!endif
