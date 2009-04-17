!ifndef REGSVR_INCLUDE
!define REGSVR_INCLUDE

; usage:
; push dll to register
; call RegSvr
Function RegSvr
  exch $0 ;filename
  push $1

  ClearErrors

  ; get regsvr location
  IfFileExists "$SYSDIR\regsvr32.exe" 0 RegSvrNotFoundError

  DetailPrint "Registering $0..."
  nsExec::ExecToLog '"$SYSDIR\regsvr32.exe" /s "$0"'
  pop $1
  strcmp "$1" "0" 0 RegSvrFailed
  DetailPrint "Registering of $0 succeeded!"
  goto RegSvrDone

  RegSvrNotFoundError:
    DetailPrint "Could not find regsvr32.exe!"
    SetErrors
    goto RegSvrDone

  RegSvrFailed:
    DetailPrint "Registering of $0 failed!$\r$\n(errorcode: $1)"
    SetErrors
    goto RegSvrDone

  RegSvrDone:

  pop $1
  pop $0
FunctionEnd

; usage:
; push dll to unregister
; call un.RegSvr
Function un.RegSvr
  exch $0 ;filename
  push $1

  ; get regsvr location
  IfFileExists "$SYSDIR\regsvr32.exe" 0 UnRegSvrDone

  DetailPrint "Unregistering $0..."
  nsExec::ExecToLog '"$SYSDIR\regsvr32.exe" /s /u "$0"'
  pop $1
  strcmp "$1" "0" 0 UnRegSvrDone
  DetailPrint "Unregistering of $0 succeeded!"
  UnRegSvrDone:
  pop $1
  pop $0
FunctionEnd

!endif ;REGSVR_INCLUDE