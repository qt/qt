!ifndef SYSTEM_INCLUDE
!define SYSTEM_INCLUDE

!define QT4VSADDIN_GUID "{6A7385B4-1D62-46e0-A4E3-AED4475371F0}"

!define QTVSIP2003_OLD2_GUID "{789202F4-94F5-4f0a-AA00-73295FEBFD68}"
!define QTVSIP2005_OLD2_GUID "{789202F4-94F5-4f0a-AA00-73295FEBFD69}"
!define QTVSIP2008_OLD2_GUID "{7E61B999-F2CC-4DC8-8120-EF2E90FDF713}"

!define QTVSIP2003_OLD_GUID "{789212F4-94F5-4f0a-AA00-73295FEBFD68}"
!define QTVSIP2005_OLD_GUID "{789212F4-94F5-4f0a-AA00-73295FEBFD69}"
!define QTVSIP2008_OLD_GUID "{789212F4-94F5-4f0a-AA00-73295FEBFD70}"
!define QTVSIP_GUID         "{789212F4-94F5-4f0a-AFFE-73295BE14F10}"

!define QMSNET2002_GUID "{C174ACCD-D856-4B60-9887-0FF9E841E0EC}"
!define QMSNET2003_GUID "{C174ACCE-D857-4B61-9888-0FF9E841E0ED}"
!define QMSNET2005_GUID "{14E98DB4-A232-49a4-8EC1-8CE4F6985C73}"
!define QMSNET2008_GUID "{A40CF53F-35D5-460F-9871-A3F0599309A4}"

!macro GetVSInstallationDir UN
; Usage:
;
;   push "7.0"
;   call GetVSInstallationDir
;   pop $0
;
; If the requested VS version can be found, its
; installation directory is returned.
Function ${UN}GetVSInstallationDir
  Exch $0
  Push $1
  ReadRegStr $1 HKLM "Software\Microsoft\VisualStudio\$0\Packages\{1B027A40-8F43-11D0-8D11-00A0C91BC942}" "About"
  StrCmp $1 "" +2
  ReadRegStr $1 HKLM "Software\Microsoft\VisualStudio\$0" "InstallDir"
  StrCpy $0 $1
  StrCmp $0 "" 0 +2
    SetErrors
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro GetVSInstallationDir ""
!insertmacro GetVSInstallationDir "un."


!macro IsDotNETInstalled UN
; Usage:
;
;  push "8.0"
;  call IsDotNETInstalled
;  pop $0
;
;  $0 contains the path where the .NET framework is installed.
;  If not installation can be found $0 is empty.
Function ${UN}IsDotNETInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4
  Push $5
  
  StrCpy $5 $0

  ReadRegStr $4 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework" "InstallRoot"
  Push $4
  Exch $EXEDIR
  Exch $EXEDIR
  Pop $4

  IfFileExists $4 0 noDotNET
  StrCpy $0 0

  EnumStart:
  EnumRegKey $2 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework\Policy"  $0
  IntOp $0 $0 + 1
  StrCmp $2 "" noDotNET
  StrCpy $1 0

  EnumPolicy:
  EnumRegValue $3 HKEY_LOCAL_MACHINE "Software\Microsoft\.NETFramework\Policy\$2" $1
  IntOp $1 $1 + 1
  StrCmp $3 "" EnumStart

  StrCmp $5 "8.0" greaterThan80
  StrCmp $5 "9.0" greaterThan80 testDotNet
  greaterThan80:
    StrCmp $2 "v2.0" 0 EnumPolicy

  testDotNet:
  IfFileExists "$4\$2.$3" foundDotNET EnumPolicy

  noDotNET:
  StrCpy $0 0
  Goto done

  foundDotNET:
  StrCpy $0 "$4\$2.$3"

  done:
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsDotNETInstalled ""
!insertmacro IsDotNETInstalled "un."

!macro IsQt4VSAddinInstalled UN
; Usage:
;
;  push "8.0"
;  call IsQt4VSAddinInstalled
;  pop $0
Function ${UN}IsQt4VSAddinInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
 
  StrCmp $0 "8.0" 0 +2
    StrCpy $2 "${QT4VSADDIN_GUID}"
  StrCmp $0 "9.0" 0 +2
    StrCpy $2 "${QT4VSADDIN_GUID}"

  StrCpy $3 0

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +3
    StrCpy $3 1
    goto done

  ReadRegStr $1 HKCU "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +2
    StrCpy $3 1

done:
  StrCpy $0 $3

  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsQt4VSAddinInstalled ""
!insertmacro IsQt4VSAddinInstalled "un."


!macro IsQMsNetInstalled UN
; Usage:
;
;  push "8.0"
;  call IsQMsNetInstalled
;  pop $0
Function ${UN}IsQMsNetInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
  
  StrCmp $0 "7.0" 0 +2
    StrCpy $2 "${QMSNET2002_GUID}"
  StrCmp $0 "7.1" 0 +2
    StrCpy $2 "${QMSNET2003_GUID}"
  StrCmp $0 "8.0" 0 +2
    StrCpy $2 "${QMSNET2005_GUID}"
  StrCmp $0 "9.0" 0 +2
    StrCpy $2 "${QMSNET2008_GUID}"

  StrCpy $3 0

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +3
    StrCpy $3 1
    goto done

  ReadRegStr $1 HKCU "SOFTWARE\Microsoft\VisualStudio\$0\NewProjectTemplates\TemplateDirs\$2\/2" "TemplatesDir"
  StrCmp $1 "" +2
    StrCpy $3 1

done:
  StrCpy $0 $3

  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsQMsNetInstalled ""
!insertmacro IsQMsNetInstalled "un."

!macro IsQMsDevInstalled UN
; Usage:
;
;  call IsQMsDevInstalled
;  pop $0
Function ${UN}IsQMsDevInstalled
  Push $0
  Push $1
  Push $2

  StrCpy $0 0

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Filename"
  StrCmp $1 "" +3
    StrCpy $0 1
    goto done

  ReadRegStr $1 HKCU "SOFTWARE\Microsoft\DevStudio\6.0\AddIns\q4msdev.Q4MsDev.1" "Filename"
  StrCmp $1 "" +2
    StrCpy $0 1

done:
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsQMsDevInstalled ""
!insertmacro IsQMsDevInstalled "un."

!macro IsIntegrationInstalled UN
; Usage:
;
;  push "8.0"
;  call IsIntegrationInstalled
;  pop $0
Function ${UN}IsIntegrationInstalled
  Exch $0
  Push $1
  Push $2
  Push $3
  Push $4

  StrCpy $2 "${QTVSIP_GUID}"
  StrCmp $0 "7.1" 0 +3
    StrCpy $3 "${QTVSIP2003_OLD_GUID}"
    StrCpy $4 "${QTVSIP2003_OLD2_GUID}"
  StrCmp $0 "8.0" 0 +3
    StrCpy $3 "${QTVSIP2005_OLD_GUID}"
    StrCpy $4 "${QTVSIP2005_OLD2_GUID}"
  StrCmp $0 "9.0" 0 +3
    StrCpy $3 "${QTVSIP2008_OLD_GUID}"
    StrCpy $4 "${QTVSIP2008_OLD2_GUID}"

  StrCpy $0 0
  
  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\Packages\$2" "ProductName"
  StrCmp $1 "" +3
    StrCpy $0 1
    goto done
    
  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\Packages\$3" "ProductName"
  StrCmp $1 "" +3
    StrCpy $0 1
    goto done

  ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\$0\Packages\$4" "ProductName"
  StrCmp $1 "" done
    StrCpy $0 1

done:
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd
!macroend

!insertmacro IsIntegrationInstalled ""
!insertmacro IsIntegrationInstalled "un."

!macro AdministratorRights UN
Function ${UN}HasAdminRights
  push $0
  ClearErrors
  UserInfo::GetAccountType
  IfErrors Yes ;It's probably Win95
  pop $0
  StrCmp $0 "Admin" Yes
  StrCmp $0 "Power" Yes

  StrCpy $0 false
  goto Done

  Yes:
  StrCpy $0 true

  Done:
  exch $0
FunctionEnd
!macroend
!insertmacro AdministratorRights ""
!insertmacro AdministratorRights "un."

; Pushes 1 on the stack if VS 2005 SP1 is installed, otherwise 0.
; Usage:
; call IsVS2005SP1Installed
; pop $0
Function IsVS2005SP1Installed
  Push $0
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\DevDiv\VS\Servicing\8.0" "SP"
  ${If} $0 >= 1
    Pop $0
    Push 1
    Return
  ${EndIf}
; try again for Visual Studio 2005 Express
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\DevDiv\VC\Servicing\8.0" "SP"
  ${If} $0 >= 1
    Pop $0
    Push 1
    Return
  ${EndIf}
  Pop $0
  Push 0
FunctionEnd

; Pushes 1 on the stack if .NET 2 SP1 is installed, otherwise 0.
; Usage:
; call IsDotNet2SP1Installed
; pop $0
Function IsDotNet2SP1Installed
  Push $0
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\NET Framework Setup\NDP\v2.0.50727" "SP"
  ${If} $0 >= 1
    Pop $0
    Push 1
    Return
  ${EndIf}
  Pop $0
  Push 0
FunctionEnd


; Pushes 1 on the stack if VS 2008 redistributable is installed, otherwise 0.
; Usage:
; call IsVS2008RedistInstalled
; pop $0
Function IsVS2008RedistInstalled
  Push $0
  ClearErrors
  ReadRegDword $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{FF66E9F6-83E7-3A3E-AF14-8DE9A809A6A4}" "Version"
  ${If} $0 == 151015966 ; 151015966 = 0x900521e
    Pop $0
    Push 1
    Return
  ${EndIf}
  Pop $0
  Push 0
FunctionEnd

!endif ;SYSTEM_INCLUDE
