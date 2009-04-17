!ifdef MODULE_MSVC

!include "includes\system.nsh"

!macro MSVC_INITIALIZE
!include "includes\qtcommon.nsh"
!ifndef MODULE_MSVC_NAME
  !define MODULE_MSVC_NAME "Qt"
!endif
!ifndef MODULE_MSVC_VERSION
  !define MODULE_MSVC_VERSION "${PRODUCT_VERSION}"
!endif
!ifndef MODULE_MSVC_BUILDDIR
  !error "MODULE_MSVC_BUILDDIR not defined!"
!endif
var MSVC_QtDesktopInstallBuildTree
!include "includes\qtenv.nsh"
!macroend

!macro MSVC_PATCH_QT_INSTALLATION QtBuildTree QtInstallBuildTree QtSourceTree QtInstallSourceTree DLLPDBDirName
  Push "${QtInstallBuildTree}"
  Call DeleteFloatingLicenseProgram

  Push "${QtInstallBuildTree}\bin"
  Call AddStartmenuApplication

  Push "${QtBuildTree}"
  Push "${QtInstallBuildTree}"
  Call PatchPrlFiles

  ${If} ${FileExists} "${QtInstallBuildTree}\.qmake.cache"
    Push "${QtInstallBuildTree}\.qmake.cache"
    Push "${QtBuildTree}"
    Push "${QtInstallBuildTree}"
    Call PatchPath
    ${If} "${QtInstallBuildTree}" != "${QtInstallSourceTree}"
      Push "${QtInstallBuildTree}\.qmake.cache"
      Push "${QtSourceTree}"
      Push "${QtInstallSourceTree}"
      Call PatchPath
    ${EndIf}
  ${EndIf}

  ${If} ${FileExists} "${QtInstallBuildTree}\mkspecs\default\qmake.conf"
    Push "${QtInstallBuildTree}\mkspecs\default\qmake.conf"
    Push "${QtSourceTree}"
    Push "${QtInstallSourceTree}"
    Call PatchPath
  ${EndIf}

  Push "${DLLPDBDirName}"
  Push "${QtInstallBuildTree}"
  Call PatchCommonBinaryFiles

!ifndef MODULE_MSVC_SKIPPATCHLICENSEINFORMATION
  Push "${QtInstallBuildTree}"
  Call PatchLicenseInformation
!endif  

  Push "${DLLPDBDirName}"
  Push "${QtBuildTree}"
  Push "${QtInstallBuildTree}"
  Call PatchMSVCBinaryFiles
  ${If} "${QtInstallBuildTree}" != "${QtInstallSourceTree}"
    Push "${DLLPDBDirName}"
    Push "${QtSourceTree}"
    Push "${QtInstallSourceTree}"
    Call PatchMSVCBinaryFiles
  ${EndIf}
!macroend ;MSVC_PATCH_QT_INSTALLATION

!macro MSVC_POST_WIN32_INSTALLFILES QtBuildTree QtInstallBuildTree QtSourceTree QtInstallSourceTree VersionName
  !insertmacro MSVC_PATCH_QT_INSTALLATION \
    "${QtBuildTree}" \
    "${QtInstallBuildTree}" \
    "${QtSourceTree}" \
    "${QtInstallSourceTree}" \
    "bin"

  Push "${VersionName}"
  Push "${QtInstallBuildTree}"
  Call MSVC_AddQtVersionToRegistry

  Push "${INSTALL_COMPILER}"
  Push "${QtInstallBuildTree}"
  Push ""
  Push "${VersionName}"
  Call MSVC_CreateQtVarsBatAndShortcut

  StrCpy $MSVC_QtDesktopInstallBuildTree "${QtInstallBuildTree}"
!macroend ;MSVC_POST_WIN32_INSTALLFILES

!macro MSVC_POST_WINCE_INSTALLFILES QtBuildTree QtInstallBuildTree QtSourceTree QtInstallSourceTree VersionName WinCESDK
  !insertmacro MSVC_PATCH_QT_INSTALLATION \
    "${QtBuildTree}" \
    "${QtInstallBuildTree}" \
    "${QtSourceTree}" \
    "${QtInstallSourceTree}" \
    "lib"

  Push "${VersionName}"
  Push "${QtInstallBuildTree}"
  Call MSVC_AddQtVersionToRegistry

  Push "${INSTALL_COMPILER}ce"
  Push "${QtInstallBuildTree}"
  Push "${WinCESDK}"
  Push "${VersionName}"
  Call MSVC_CreateQtVarsBatAndShortcut
!macroend ;MSVC_POST_WINCE_INSTALLFILES

!macro MSVC_PATCH_MSVC_BINARY_FILE BinaryFile QtBuildDir QtInstallDir
  ${If} ${FileExists} "${BinaryFile}"
    DetailPrint "Patching ${BinaryFile} (${QtInstallDir})..."
!ifdef MODULE_MSVC_VC60
    qtnsisext::PatchVC6Binary "${BinaryFile}" "${QtBuildDir}" "${QtInstallDir}"
!else
    qtnsisext::PatchVC7Binary "${BinaryFile}" "${QtBuildDir}" "${QtInstallDir}"
!endif
  ${EndIf}
!macroend

!macro MSVC_SECTIONS
Section "${MODULE_MSVC_NAME} ${MODULE_MSVC_VERSION}" MSVC_SEC01
  ${If} "$MSVC_INSTDIR" == ""
    StrCpy $MSVC_INSTDIR "$INSTDIR\${MODULE_MSVC_NAME} ${MODULE_MSVC_VERSION}"
    Push $MSVC_INSTDIR
    Call MakeQtDirectory
    Pop $MSVC_INSTDIR
  ${EndIf}

  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "MSVCInstalled" 1

  SetOutPath "$MSVC_INSTDIR"
  SetOverwrite ifnewer
  !insertmacro MODULE_MSVC_INSTALLFILES

!ifdef MODULE_MSVC_WIN32BUILDTREE & MODULE_MSVC_WIN32BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
  !insertmacro MSVC_POST_WIN32_INSTALLFILES \
    "${MODULE_MSVC_WIN32BUILDTREE}"                           /* QtBuildTree         */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_WIN32BUILDINSTALLDIRNAME}"   /* QtInstallBuildTree  */ \
    "${MODULE_MSVC_QTSOURCETREE}"                             /* QtSourceTree        */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_QTSOURCEINSTALLDIRNAME}"     /* QtInstallSourceTree */ \
    "${MODULE_MSVC_VERSION} Desktop"                          /* VersionName         */
!endif
!ifdef MODULE_MSVC_WM50BUILDTREE & MODULE_MSVC_WM50BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
  !insertmacro MSVC_POST_WINCE_INSTALLFILES \
    "${MODULE_MSVC_WM50BUILDTREE}"                            /* QtBuildTree         */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_WM50BUILDINSTALLDIRNAME}"    /* QtInstallBuildTree  */ \
    "${MODULE_MSVC_QTSOURCETREE}"                             /* QtSourceTree        */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_QTSOURCEINSTALLDIRNAME}"     /* QtInstallSourceTree */ \
    "${MODULE_MSVC_VERSION} ${MODULE_MSVC_WM50VERSIONPRETTY}" /* VersionName         */ \
    ${MODULE_MSVC_WM50SDK}                                    /* WinCESDK            */
!endif
!ifdef MODULE_MSVC_SSDK50X86BUILDTREE & MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
  !insertmacro MSVC_POST_WINCE_INSTALLFILES \
    "${MODULE_MSVC_SSDK50X86BUILDTREE}"                            /* QtBuildTree         */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME}"    /* QtInstallBuildTree  */ \
    "${MODULE_MSVC_QTSOURCETREE}"                                  /* QtSourceTree        */ \
    "$MSVC_INSTDIR\${MODULE_MSVC_QTSOURCEINSTALLDIRNAME}"          /* QtInstallSourceTree */ \
    "${MODULE_MSVC_VERSION} ${MODULE_MSVC_SSDK50X86VERSIONPRETTY}" /* VersionName         */ \
    ${MODULE_MSVC_SSDK50X86SDK}                                    /* WinCESDK            */
!endif
!ifndef MODULE_MSVC_WIN32BUILDTREE | MODULE_MSVC_WM50BUILDTREE | MODULE_MSVC_QTSOURCETREE | MODULE_MSVC_QTSOURCEINSTALLDIRNAME
  !insertmacro MSVC_POST_WIN32_INSTALLFILES \
    "${MODULE_MSVC_BUILDDIR}"                                 /* QtBuildTree         */ \
    "$MSVC_INSTDIR"                                           /* QtInstallBuildTree  */ \
    "${MODULE_MSVC_BUILDDIR}"                                 /* QtSourceTree        */ \
    "$MSVC_INSTDIR"                                           /* QtInstallSourceTree */ \
    "${MODULE_MSVC_VERSION}"                                  /* VersionName         */
!endif

!ifdef MODULE_LICENSECHECK
  WriteRegStr SHCTX "SOFTWARE\Trolltech\Common\${MODULE_MSVC_VERSION}\$LICENSE_PRODUCT" "Key" "$LICENSE_KEY"
  call MSVC_UpdateExistingVsipLicenseKeys
!endif

SectionEnd

Function PatchMSVCBinaryFiles
  Exch $0 ; Absolute path to where Qt is installed
  Exch
  Exch $1 ; Absolute path to where Qt was built
  Exch
  Exch 2
  Exch $2 ; Name of directory containing PDBs
  Exch 2
  Push $3 ; FindFirst/FindNext handle
  Push $4 ; Found .pdb file name

  FindFirst $3 $4 "$0\$2\*.pdb"
  ${DoWhile} $4 != ""
    !insertmacro MSVC_PATCH_MSVC_BINARY_FILE "$0\$2\$4" "$1" "$0"
    FindNext $3 $4
  ${Loop}

  !insertmacro MSVC_PATCH_MSVC_BINARY_FILE "$0\lib\qtmaind.lib" "$1" "$0"

  Pop $4
  Pop $3
  Pop $0
  Pop $1
  Pop $2
FunctionEnd

Function MSVC_AddQtVersionToRegistry
  Exch $0 ; VersionDir
  Exch
  Exch $1 ; VersionName
  Exch
  Push $2

  WriteRegStr HKCU "${QT_VERSION_KEY_NAME}\$1\" "InstallDir" "$0"
  ${If} "$RUNNING_AS_ADMIN" == true
    WriteRegStr HKLM "${QT_VERSION_KEY_NAME}\$1\" "InstallDir" "$0"
  ${EndIf}

  ReadRegStr $2 HKCU "${QT_VERSION_KEY_NAME}" "DefaultQtVersion"
  ${If} $2 == ""
    WriteRegStr HKCU "${QT_VERSION_KEY_NAME}" "DefaultQtVersion" "$1"
  ${EndIf}

  Pop $2
  Pop $0
  Pop $1
FunctionEnd ;MSVC_AddQtVersionToRegistry

Function MSVC_CreateQtVarsBatAndShortcut
  Exch $0 ; VersionName
  Exch
  Exch $1 ; WinCESDK
  Exch
  Exch 2
  Exch $2 ; QtBuildTree
  Exch 2
  Exch 3
  Exch $3 ; Compiler
  Exch 3
  Push $4 ; Found substring "command.com"
  Push $5 ; Expanded %COMSPEC%

  ${If} ${FileExists} "$2\bin\qmake.exe"
    !insertmacro MAKE_QTVARS_FILE "$3" "$2" "$1"
    !insertmacro CreateConditionalShortCutWithParameters "$SMPROGRAMS\$STARTMENU_STRING\Visual Studio with ${MODULE_MSVC_NAME} $0.lnk" "$2\bin\qtvars.bat" "vsstart"

    ExpandEnvStrings $5 "%COMSPEC%"
    ${StrStr} $4 "$5" "command.com"
    ${If} "$4" == ""
      !insertmacro CreateConditionalShortCutWithParameters "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MSVC_NAME} $0 Command Prompt.lnk" "%COMSPEC%" "/k $\"$2\bin\qtvars.bat vsvars$\""
    ${Else}
      !insertmacro CreateConditionalShortCutWithParameters "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MSVC_NAME} $0 Command Prompt.lnk" "%COMSPEC%" "/e:4096 /k $\"$2\bin\qtvars.bat vsvars$\""
    ${EndIf}
  ${EndIf}

  Pop $5
  Pop $4
  Pop $0
  Pop $1
  Pop $2
  Pop $3
FunctionEnd ;MSVC_CreateQtVarsBatAndShortcut

Function MSVC_ValidateDirectoryFunc
  push "${MODULE_MSVC_BUILDDIR}"
  push $MSVC_INSTDIR
  call CommonCheckDirectory
FunctionEnd

Function MSVC_UpdateExistingVsipLicenseKeys
  Push $0 ; EnumRegKey loop index
  Push $1 ; Reg key
  Push $2 ; StrStr result

  StrCpy $0 0
  loop:
    EnumRegKey $1 SHCTX SOFTWARE\Trolltech $0
    StrCmp $1 "" no_reg_key_found
    ${StrStr} $2 $1 "Qt4VS"
    StrCmp $2 $1 0 +1
      WriteRegStr SHCTX "SOFTWARE\Trolltech\$2" "LicenseKey" "$LICENSE_KEY"
    IntOp $0 $0 + 1
    goto loop
  no_reg_key_found:

  Pop $2
  Pop $1
  Pop $0
FunctionEnd
!macroend

!macro MSVC_DESCRIPTION
  !insertmacro MUI_DESCRIPTION_TEXT ${MSVC_SEC01} "This installs ${MODULE_MSVC_NAME} version ${MODULE_MSVC_VERSION} on your system."
!macroend

!macro MSVC_STARTUP
  !ifdef MODULE_MSVC_VS2005
    Call IsVS2005SP1Installed
    Pop $1
    ${If} $1 == 0
      MessageBox MB_OK|MB_ICONEXCLAMATION "Service Pack 1 for Visual Studio 2005 is not installed.$\n${PRODUCT_NAME} for VS 2005 will not work properly without it."
      Abort
    ${EndIf}
  !endif

  !ifndef MODULE_MSVC_NODEFAULT
    SectionSetFlags ${MSVC_SEC01} 17
  !endif
  strcpy $MSVC_INSTDIR "C:\Qt\${MODULE_MSVC_VERSION}"
  push $MSVC_INSTDIR
  call MakeQtDirectory
  pop $MSVC_INSTDIR
!macroend

!macro MSVC_FINISH
!macroend

!macro MSVC_RUN_FUNCTION
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MSVCInstalled"
  ${If} $0 == 1
    ${If} ${FileExists} "$MSVC_QtDesktopInstallBuildTree\bin\qtdemo.exe"
      Exec '$MSVC_QtDesktopInstallBuildTree\bin\qtdemo.exe'
    ${EndIf}
    Goto DoneRunFunction ;don't run more applications
  ${EndIf}
!macroend

!macro MSVC_README_FUNCTION
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MSVCInstalled"
  ${If} $0 == 1
    ${If} ${FileExists} "$MSVC_QtDesktopInstallBuildTree\bin\assistant.exe"
!ifndef README_STARTPAGE
      Exec "$MSVC_QtDesktopInstallBuildTree\bin\assistant.exe"
!else
      Exec "$MSVC_QtDesktopInstallBuildTree\bin\assistant.exe -showurl $\"${README_STARTPAGE}$\""
!endif
    ${EndIf}
    Goto DoneReadmeFunction ;don't run more applications
  ${EndIf}
!macroend

!macro MSVC_UNSTARTUP
  ${If} "$MSVC_INSTDIR" == ""
    StrCpy $MSVC_INSTDIR "$INSTDIR\${MODULE_MSVC_NAME} ${MODULE_MSVC_VERSION}"
    Push $MSVC_INSTDIR
    Call un.MakeQtDirectory
    Pop $MSVC_INSTDIR
  ${EndIf}

  !insertmacro ConfirmOnRemove "MSVCInstalled" "- ${MODULE_MSVC_NAME} ${MODULE_MSVC_VERSION} in $MSVC_INSTDIR"
!macroend

!macro MSVC_POST_UNINSTALLFILES QtInstallBuildTree VersionName
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Visual Studio with ${MODULE_MSVC_NAME} ${VersionName}.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\${MODULE_MSVC_NAME} ${VersionName} Command Prompt.lnk"
  Delete "${QtInstallBuildTree}\bin\qtvars.bat"

  Push "${VersionName}"
  Push "${QtInstallBuildTree}"
  Call un.MSVC_RemoveQtVersionFromRegistry
!macroend ;MSVC_POST_UNINSTALLFILES

!macro MSVC_UNINSTALL
Section un."${MODULE_MSVC_NAME} ${MODULE_MSVC_VERSION}"
  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "MSVCInstalled"
  ${If} $0 == 1
    DetailPrint "Removing start menu shortcuts and registry entries"
    Call un.RemoveStartmenuApplication
!ifdef MODULE_MSVC_WIN32BUILDTREE & MODULE_MSVC_WIN32BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
    !insertmacro MSVC_POST_UNINSTALLFILES \
      "$MSVC_INSTDIR\${MODULE_MSVC_WIN32BUILDINSTALLDIRNAME}" \
      "${MODULE_MSVC_VERSION} Desktop"
!endif
!ifdef MODULE_MSVC_WM50BUILDTREE & MODULE_MSVC_WM50BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
    !insertmacro MSVC_POST_UNINSTALLFILES \
      "$MSVC_INSTDIR\${MODULE_MSVC_WM50BUILDINSTALLDIRNAME}" \
      "${MODULE_MSVC_VERSION} ${MODULE_MSVC_WM50VERSIONPRETTY}"
!endif
!ifdef MODULE_MSVC_SSDK50X86BUILDTREE & MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME & MODULE_MSVC_QTSOURCETREE & MODULE_MSVC_QTSOURCEINSTALLDIRNAME
    !insertmacro MSVC_POST_UNINSTALLFILES \
      "$MSVC_INSTDIR\${MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME}" \
      "${MODULE_MSVC_VERSION} ${MODULE_MSVC_SSDK50X86VERSIONPRETTY}"
!endif
!ifndef MODULE_MSVC_WIN32BUILDTREE | MODULE_MSVC_WM50BUILDTREE | MODULE_MSVC_QTSOURCETREE | MODULE_MSVC_QTSOURCEINSTALLDIRNAME
    !insertmacro MSVC_POST_UNINSTALLFILES \
      "$MSVC_INSTDIR" \
      "${MODULE_MSVC_VERSION}"
!endif
    !insertmacro MODULE_MSVC_REMOVE "$MSVC_INSTDIR"
    RMDir $MSVC_INSTDIR ;removes it if empty
  ${EndIf}
SectionEnd

Function un.MSVC_RemoveQtVersionFromRegistry
  Exch $0 ; QtInstallBuildTree
  Exch
  Exch $1 ; VersionName
  Exch
  Push $2 ; Temp

  ReadRegStr $2 HKCU "${QT_VERSION_KEY_NAME}\$1" "InstallDir"
  ${If} "$2" == "$0"
    DeleteRegKey HKCU "${QT_VERSION_KEY_NAME}\$1"
  ${EndIf}

  ReadRegStr $2 HKLM "${QT_VERSION_KEY_NAME}\$1" "InstallDir"
  ${If} "$2" == "$0"
    DeleteRegKey HKLM "${QT_VERSION_KEY_NAME}\$1"
  ${EndIf}

  Pop $2
  Pop $0
  Pop $1
FunctionEnd
!macroend
!macro MSVC_UNFINISH
!macroend
!else ;MODULE_MSVC
!macro MSVC_INITIALIZE
!macroend
!macro MSVC_SECTIONS
!macroend
!macro MSVC_DESCRIPTION
!macroend
!macro MSVC_STARTUP
!macroend
!macro MSVC_FINISH
!macroend
!macro MSVC_README_FUNCTION
!macroend
!macro MSVC_RUN_FUNCTION
!macroend
!macro MSVC_UNSTARTUP
!macroend
!macro MSVC_UNINSTALL
!macroend
!macro MSVC_UNFINISH
!macroend
!endif ;MODULE_MSVC

