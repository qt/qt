!ifdef MODULE_QTJAMBIECLIPSE

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_INITIALIZE

!define ECLIPSE_LOCATION_KEY "EclipseLocation"
var ECLIPSE_LOCATION

!ifndef MODULE_QTJAMBIECLIPSE_INSTALLER
  !define MODULE_QTJAMBIECLIPSE_INSTALLER "${PRODUCT_NAME} v${PRODUCT_VERSION}"
!endif
!ifndef MODULE_QTJAMBIECLIPSE_ROOT
  !define MODULE_QTJAMBIECLIPSE_ROOT "${INSTALL_ROOT}\qtjambieclipse"
!endif
!ifndef MODULE_QTJAMBIECLIPSE_VERSION
  !define MODULE_QTJAMBIECLIPSE_VERSION ${PRODUCT_VERSION}
!endif

!define MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION "1.0.0"

!define MODULE_QTJAMBIECLIPSE_QT_ID "com.trolltech.qt_${MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION}"
!define MODULE_QTJAMBIECLIPSE_QTJAMBI_ID "com.trolltech.qtjambi_${MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION}"
!define MODULE_QTJAMBIECLIPSE_QTDESIGNER_ID "com.trolltech.qtdesigner_${MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION}"
!define MODULE_QTJAMBIECLIPSE_QTDESIGNER_QTJAMBI_ID "com.trolltech.qtdesigner.qtjambi_${MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION}"
!define MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID "com.trolltech.qtdesignerplugins"

!define MODULE_QTJAMBIECLIPSE_BINARIES "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\com.trolltech.qtdesigner.win32.x86_${MODULE_QTJAMBIECLIPSE_QTJAMBIVERSION}"

LangString ModuleQtjambieclipsePageTitle ${LANG_ENGLISH} "Eclipse Installation Location"
LangString ModuleQtjambieclipsePageDescription ${LANG_ENGLISH} "Select where eclipse is installed."

!define MODULE_QTJAMBIECLIPSE_PAGE "qtjambieclipse.ini"
Page custom ModuleQtjambieclipsePageEnter ModuleQtjambieclipsePageExit

!include "includes\regsvr.nsh"

!define MODULE_QTJAMBIECLIPSE_INCOMPATIBLEPRODUCT "Qt Eclipse Integration"

!macroend ;QTJAMBIECLIPSE_INITIALIZE

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_SECTIONS

Section -PreEclipseSection
  WriteRegStr SHCTX "$PRODUCT_UNIQUE_KEY" "${ECLIPSE_LOCATION_KEY}" $ECLIPSE_LOCATION
SectionEnd

SectionGroup "Eclipse Integration"

Section "Qt Jambi Eclipse Integration" QTJAMBIECLIPSE_SEC01
  SetOutPath "$ECLIPSE_LOCATION\plugins\"
  SetOverwrite ifnewer
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QT_ID}.jar"
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QTJAMBI_ID}.jar"
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNER_ID}.jar"
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNER_QTJAMBI_ID}.jar"

  SetOutPath "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}"
  SetOverwrite ifnewer
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}\JambiCustomWidget.dll"
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}\JambiLanguage.dll"
  
  SetOutPath "$QTJAMBIECLIPSE_INSTDIR"
  SetOverwrite ifnewer
  File "${MODULE_QTJAMBIECLIPSE_ROOT}\LICENSE.QT_JAMBI_ECLIPSE_INTEGRATION"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\msvcp71.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\msvcr71.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtCore4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtGui4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtXml4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtScript4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtDesigner4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\QtDesignerComponents4.dll"
  File "${MODULE_QTJAMBIECLIPSE_BINARIES}\qtdesigner.dll"
  
  push "$QTJAMBIECLIPSE_INSTDIR\qtdesigner.dll"
  call RegEclipseSvr
  
  push "$QTJAMBIECLIPSE_INSTDIR\qtdesigner.dll"
  call RegEclipseSvr
  
  Rename "$QTJAMBIECLIPSE_INSTDIR\LICENSE.QT_JAMBI_ECLIPSE_INTEGRATION" "$QTJAMBIECLIPSE_INSTDIR\LICENSE.txt"
  !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\License.lnk" "$QTJAMBIECLIPSE_INSTDIR\LICENSE.txt"
SectionEnd

SectionGroupEnd

; usage:
; push dll to register
; call RegEclipseSvr
Function RegEclipseSvr
  exch $0 ;filename

  ClearErrors
  push $0
  call RegSvr
  IfErrors 0 +2
    MessageBox MB_OK|MB_ICONEXCLAMATION 'Could not register "$0"'

  pop $0
FunctionEnd

Function ModuleQtjambieclipsePageEnter
  !insertmacro MUI_HEADER_TEXT "$(ModuleQtjambieclipsePageTitle)" "$(ModuleQtjambieclipsePageDescription)"
  !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_QTJAMBIECLIPSE_PAGE}"
FunctionEnd

Function ModuleQtjambieclipsePageExit
  push $0
  push $1

  !insertmacro MUI_INSTALLOPTIONS_READ $0 "${MODULE_QTJAMBIECLIPSE_PAGE}" "Field 2" "State"
  IfFileExists "$0\eclipse.exe" eclipse_found
  MessageBox MB_OK|MB_ICONSTOP "$0\eclipse.exe not found!$\nPlease select a valid installation directory."
  Goto failed
  eclipse_found:
  
  ClearErrors
  FileOpen $1 "$0\plugins\com.trolltech.writetest" a
  IfErrors 0 has_write_access
  MessageBox MB_OK|MB_ICONSTOP "Can't write to $0\plugins.$\nPlease select a valid installation directory."
  Goto failed
  has_write_access:

  FileClose $1
  Delete "$0\plugins\com.trolltech.writetest"
  StrCpy $ECLIPSE_LOCATION $0
  
  Goto done
  failed:
  pop $1
  pop $0
  Abort
  
  done:
  pop $1
  pop $0
FunctionEnd

!macroend ;QTJAMBIECLIPSE_SECTIONS

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_DESCRIPTION
!ifdef QTJAMBIECLIPSE_SEC01
  !insertmacro MUI_DESCRIPTION_TEXT ${QTJAMBIECLIPSE_SEC01} "This installs the Qt Jambi plugin into Eclipse."
!endif
!macroend

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_STARTUP
  push "${MODULE_QTJAMBIECLIPSE_INCOMPATIBLEPRODUCT}"
  call WarnIfInstalledProductDetected
  
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_QTJAMBIECLIPSE_PAGE}"
  SectionSetFlags ${QTJAMBIECLIPSE_SEC01} 17
  
  strcpy $QTJAMBIECLIPSE_INSTDIR "$PROGRAMFILES\Trolltech\Qt Jambi Eclipse Integration"
!macroend ;QTJAMBIECLIPSE_STATUP

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_FINISH
!macroend ;QTJAMBIECLIPSE_FINISH

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_UNSTARTUP
  ReadRegStr $ECLIPSE_LOCATION SHCTX "$PRODUCT_UNIQUE_KEY" "${ECLIPSE_LOCATION_KEY}"
!macroend ;QTJAMBIECLIPSE_UNSTARTUP

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_UNINSTALL
Section un."Eclipse Integration"
  push "$QTJAMBIECLIPSE_INSTDIR\qtdesigner.dll"
  call un.RegSvr

  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QT_ID}.jar"
  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTJAMBI_ID}.jar"
  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNER_ID}.jar"
  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNER_QTJAMBI_ID}.jar"

  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}\JambiCustomWidget.dll"
  Delete "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}\JambiLanguage.dll"
  RMDir "$ECLIPSE_LOCATION\plugins\${MODULE_QTJAMBIECLIPSE_QTDESIGNERPLUGINS_ID}"

  Delete "$SMPROGRAMS\$STARTMENU_STRING\License.lnk"
  
  Delete "$QTJAMBIECLIPSE_INSTDIR\LICENSE.txt"
  Delete "$QTJAMBIECLIPSE_INSTDIR\msvcp71.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\msvcr71.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtCore4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtGui4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtXml4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtScript4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtDesigner4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\QtDesignerComponents4.dll"
  Delete "$QTJAMBIECLIPSE_INSTDIR\qtdesigner.dll"
  
  RMDir "$QTJAMBIECLIPSE_INSTDIR"
SectionEnd
!macroend ;QTJAMBIECLIPSE_UNINSTALL

;------------------------------------------------------------------------------------------------
!macro QTJAMBIECLIPSE_UNFINISH
!macroend

!else ;MODULE_QTJAMBIECLIPSE
!macro QTJAMBIECLIPSE_INITIALIZE
!macroend
!macro QTJAMBIECLIPSE_SECTIONS
!macroend
!macro QTJAMBIECLIPSE_DESCRIPTION
!macroend
!macro QTJAMBIECLIPSE_STARTUP
!macroend
!macro QTJAMBIECLIPSE_FINISH
!macroend
!macro QTJAMBIECLIPSE_UNSTARTUP
!macroend
!macro QTJAMBIECLIPSE_UNINSTALL
!macroend
!macro QTJAMBIECLIPSE_UNFINISH
!macroend
!endif ;MODULE_QTJAMBIECLIPSE

