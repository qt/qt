!include "StrFunc.nsh"
!include "includes\list.nsh"

${StrCase}
${StrTrimNewLines}
${StrStr}
${StrRep}
${UnStrRep}

var STARTMENU_STRING
var PRODUCT_UNIQUE_KEY
var RUNNING_AS_ADMIN

!define QT_VERSION_KEY_NAME "SOFTWARE\Trolltech\Versions"

!ifndef MODULE_MINGW
  !ifdef MODULE_MSVC_VC60
    !define INSTALL_COMPILER "vc60"
  !else
    !ifdef MODULE_MSVC_VS2002
      !define INSTALL_COMPILER "vs2002"
    !else
      !ifdef MODULE_MSVC_VS2005
        !define INSTALL_COMPILER "vs2005"
      !else
        !ifdef MODULE_MSVC_VS2005CE
          !define INSTALL_COMPILER "vs2005ce"
        !else
          !ifdef MODULE_MSVC_VS2008
            !define INSTALL_COMPILER "vs2008"
          !else
            !ifdef MODULE_MSVC_VS2008CE
              !define INSTALL_COMPILER "vs2008ce"
            !else
              !define INSTALL_COMPILER "vs2003"
            !endif
          !endif
        !endif
      !endif
    !endif
  !endif
!else
  !define INSTALL_COMPILER "mingw"
!endif

var VS_VERSION
var VS_VERSION_SHORT
var ADDIN_INSTDIR
var VSIP_INSTDIR
var HELP_INSTDIR
var ECLIPSE_INSTDIR
var QTJAMBI_INSTDIR
var QTJAMBIECLIPSE_INSTDIR

; LICENSECHECK
var LICENSE_KEY
var LICENSEE
var LICENSE_PRODUCT
var LICENSE_PLATFORM
var LICENSE_FILE

; MSVC
!ifdef MODULE_MSVC
  !define MSVC_ValidateDirectory
  var MSVC_INSTDIR
!endif

; MINGW
!ifdef MODULE_MINGW
  !define MINGW_ValidateDirectory
  var MINGW_INSTDIR
!endif

; QSA
var QSA_INSTDIR

; QTDIR PAGE
var QTDIR_SELECTED
var COMPILER_SELECTED

; used by addin7x and vsip
!ifndef MODULE_VSIP_ROOT
  !define MODULE_VSIP_ROOT "${INSTALL_ROOT}\vsip"
!endif

; add to confirm path
var UninstallerConfirmProduct

Function un.ConfirmOnDelete
  exch $0
  push $1

  push "$0"
  push "$UninstallerConfirmProduct"
  call un.ItemInList
  pop $1
  IntCmp $1 1 ConfirmOnDeleteDone
  
  strcmp "$UninstallerConfirmProduct" "" 0 +3
    strcpy $UninstallerConfirmProduct "$0"
  goto +2
    strcpy $UninstallerConfirmProduct "$UninstallerConfirmProduct$\r$\n$0"
    
  ConfirmOnDeleteDone:
  pop $1
  pop $0
FunctionEnd

!macro ConfirmOnRemove REG_KEY PRODUCT_NAME
  push $0
  ClearErrors
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "${REG_KEY}"
  intcmp $0 1 0 +3
    push "${PRODUCT_NAME}"
    call un.ConfirmOnDelete
  ClearErrors
  pop $0
!macroend

!define GLOBAL_SEC_SHORTCUTCREATION_TEXT "Create shortcuts"
!define GLOBAL_SEC_SHORTCUTCREATION_MAX_ID "100"

Function GetSecShortcutCreationId
  push $0
  push $1

  StrCpy $0 "0"
  loopStart:
    IntCmp $0 "${GLOBAL_SEC_SHORTCUTCREATION_MAX_ID}" notfound
    IntOp $0 $0 + 1
    SectionGetText $0 $1
    StrCmp $1 "${GLOBAL_SEC_SHORTCUTCREATION_TEXT}" done
    goto loopStart

  notfound:
  StrCpy $0 "-1"
  done:
  Pop $1
  Exch $0
FunctionEnd

Function GetCreateSchortcuts
!ifdef USE_OPTIONAL_SHORTCUTCREATION
  Push $0
  Push $1
  Call GetSecShortcutCreationId
  Pop $1
  IntCmp $1 -1 setfalse
  SectionGetFlags "$1" $0
  IntOp $0 $0 & ${SF_SELECTED}
  IntCmp $0 ${SF_SELECTED} settrue setfalse
  settrue:
    StrCpy $0 true
    Goto done
  setfalse:
    StrCpy $0 false
  done:
  Pop $1
  Exch $0
!else ;USE_OPTIONAL_SHORTCUTCREATION
  Push true
!endif ;USE_OPTIONAL_SHORTCUTCREATION
FunctionEnd

!macro CreateConditionalShortCutDirectory DIRECTORY
  Push $0
  Call GetCreateSchortcuts
  Pop $0
  StrCmp $0 true 0 +2
    CreateDirectory "${DIRECTORY}"
  Pop $0
!macroend

Function GLOBAL_CreateConditionalShortCutWithParameters
  Exch $0 ; PARAMETERS
  Exch
  Exch $1 ; TARGET
  Exch
  Exch 2
  Exch $2 ; LINK
  Exch 2
  Push $3

  Call GetCreateSchortcuts
  Pop $3
  StrCmp $3 true 0 +2
    CreateShortCut "$2" "$1" "$0"

  Pop $3
  Pop $0
  Pop $1
  Pop $2
FunctionEnd

!macro CreateConditionalShortCutWithParameters LINK TARGET PARAMETERS
  Push "${LINK}"
  Push "${TARGET}"
  Push "${PARAMETERS}"
  Call GLOBAL_CreateConditionalShortCutWithParameters
!macroend

!macro CreateConditionalShortCut LINK TARGET
  !insertmacro CreateConditionalShortCutWithParameters "${LINK}" "${TARGET}" ""
!macroend
