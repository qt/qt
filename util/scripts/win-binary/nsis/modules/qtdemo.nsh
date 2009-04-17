!ifdef MODULE_QTDEMO

;-------------------------------
; Test which version of Microsoft DirectX is installed
; Returns e.g. 900 for version 9
;
; Original code is from nsis.sourceforge.net
Function GetDXVersion
  Push $0
  Push $1

  ClearErrors
  ReadRegStr $0 HKLM "Software\Microsoft\DirectX" "Version"
  IfErrors noDirectX

  StrCpy $1 $0 2 5    ; get the minor version
  StrCpy $0 $0 2 2    ; get the major version
  IntOp $0 $0 * 100   ; $0 = major * 100 + minor
  IntOp $0 $0 + $1
  Goto done

noDirectX:
  StrCpy $0 0

done:
  Pop $1
  Exch $0
FunctionEnd

;-------------------------------
; Test if Visual Studio Redistributables 2005+ SP1 installed
; Returns -1 if there is no VC redistributables installed
; otherwise returns version number (e.g. 134274064 for VS2005 SP1)
;
; Original code is from nsis.sourceforge.net
Function CheckVCRedist
   Push $0
   ClearErrors
   ReadRegDword $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7299052b-02a4-4627-81f2-1818da5d550d}" "Version"

   IfErrors 0 done
   StrCpy $0 -1

done:
   Exch $0
FunctionEnd


!macro INSTALLVCREDIST VCREDIST_EXE
  Push $R0
  DetailPrint "Checking for Microsoft Visual C++ 2005 SP1 Redistributable Runtime ..."
  Call CheckVCRedist
  Pop $R0
  IntCmp $R0 134274064 vcexists +1 +1 ; 134274064 = 800dc10
  MessageBox MB_YESNO|MB_ICONINFORMATION "Qt demo requires the Microsoft Visual C++ 2005 SP1 Runtime to be installed. Do you want to install the Runtime now?" IDYES vcinstall IDNO vcdone
vcinstall:
  DetailPrint "Installing Microsoft Visual C++ 2005 SP1 Runtime ..."
  nsExec::Exec '"$INSTDIR\${VCREDIST_EXE}" /q:a /c:"VCREDI~1.EXE /q:a /c:""msiexec /i vcredist.msi /qb!"" "' $R0
  IntCmp $R0 1 vcdone vcdone +1       ; return value 0,1 -> done
  IntCmp $R0 3010 0 vcerror vcerror   ; other return values
  MessageBox MB_OK|MB_ICONINFORMATION "Please reboot after finishing this installation"
  Goto vcdone
vcerror:
  MessageBox MB_OK|MB_ICONEXCLAMATION "Installing the Microsoft Visual C++ 2005 SP1 Runtime failed (error code $R0)! Please try to run '$INSTDIR\${VCREDIST_EXE}' manually"
  Goto vcdone
vcexists:
  DetailPrint "Microsoft Visual C++ 2005 SP1 Runtime SP1 is already installed"
vcdone:
  Pop $R0
!macroend

!macro INSTALLDIRECTX DIRECTX_EXE
  Push $R0
  DetailPrint "Checking for Microsoft Direct X version 9 or higher ..."
  Call GetDXVersion
  Pop $R0
  IntCmp $R0 900 dxexists +1 dxexists
  MessageBox MB_YESNO|MB_ICONINFORMATION "The mediaplayer demo requires Microsoft Direct X 9 or higher to be installed. Do you want to download and install Direct X 9 now?" IDYES dxinstall IDNO dxdone
dxinstall:
  DetailPrint "Running Microsoft Direct X installer ..."
  nsExec::Exec '"$INSTDIR\${DIRECTX_EXE}"'
  Goto dxdone
dxexists:
  DetailPrint "Microsoft Direct X version 9 or higher is already installed (version detected: $R0)"
dxdone:
  Pop $R0
!macroend
  
!macro QTDEMO_STARTUP
  SectionSetFlags ${QTDEMO_SEC01} 17 ; selected & read-only
!macroend

!macro QTDEMO_SECTIONS
  Section "Prerequisites" QTDEMO_SEC01
  !ifdef MODULE_QTDEMO_VCREDIST
    !insertmacro INSTALLVCREDIST "${MODULE_QTDEMO_VCREDIST}"
  !endif
  !ifdef MODULE_QTDEMO_DIRECTX
    !insertmacro INSTALLDIRECTX "${MODULE_QTDEMO_DIRECTX}"
  !endif
  SectionEnd
!macroend

!else ;MODULE_QTDEMO

!macro QTDEMO_STARTUP
!macroend

!macro QTDEMO_SECTIONS
!macroend

!endif