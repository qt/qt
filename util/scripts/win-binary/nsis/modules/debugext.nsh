!ifdef MODULE_DEBUGEXT

var DEBUGEXT_INITIALIZED

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_INITIALIZE
!ifndef MODULE_DEBUGEXT_INSTALLER
  !define MODULE_DEBUGEXT_INSTALLER "${PRODUCT_NAME} v${PRODUCT_VERSION}"
!endif
!ifndef MODULE_DEBUGEXT_ROOT
  !define MODULE_DEBUGEXT_ROOT "${INSTALL_ROOT}\debugext"
!endif
!macroend ;DEBUGEXT_INITIALIZE


;------------------------------------------------------------------------------------------------
!macro CleanAutoExp UN
Function ${UN}CleanAutoExp
  exch $1 ;filename autoexp
  exch
  exch $0 ;version
  push $3 ;readline
  push $4 ;handle autoexp
  push $5 ;tmpfile handle
  push $6 ;tmp copy of autoexp.dat
  push $7

  ClearErrors
  IfErrors done
  
  StrCpy $6 "$1_tmp"
  CopyFiles /FILESONLY "$1" "$6"

  FileOpen $5 $1 w
  IfErrors done
  
  FileOpen $4 $6 r
  IfErrors done

nextline:
  FileRead $4 $3
  IfErrors renameFile

  strcmp $0 "" 0 +4
    strcpy $7 $3 15
    strcmp $7 ";QT_DEBUG_START" nowrite
  goto +3
    strcmp $3 ";QT_DEBUG_START ($0)$\n" nowrite
    strcmp $3 ";QT_DEBUG_START ($0)$\r$\n" nowrite

  strcpy $7 $3 14 ;qt3 (always remove qt3 stuff)
  strcmp $7 "; Trolltech Qt" nowrite write
  nowrite:
    FileRead $4 $3
    IfErrors renameFile
    strcpy $7 $3 13
    strcmp $7 ";QT_DEBUG_END" nextline
    strcpy $7 $3 14 ;qt3
    strcmp $7 "QObject =class" nextline nowrite    
  write:
    FileWrite $5 $3
    goto nextline

renameFile:
  FileClose $5
  FileClose $4
  SetDetailsPrint none
  Delete $6
  SetDetailsPrint both

done:
  pop $7
  pop $6
  pop $5
  pop $4
  pop $3
  pop $0
  pop $1
FunctionEnd
!macroend ;CleanAutoExp

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_SECTIONS

Section -PreDebugExtSection
  StrCpy $DEBUGEXT_INITIALIZED "0"

  SetOutPath $TEMP
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat_entries.txt"
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat-autoexpand2003"
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat-autoexpand2005"
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat-autoexpand2008"
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat-visualizer2005"
  File "${MODULE_DEBUGEXT_ROOT}\autoexp.dat-visualizer2008"
SectionEnd

SectionGroup "Debugger Extension"

!ifndef MODULE_DEBUGEXT_NOVS2008
Section "Visual Studio 2008" DEBUGEXT_SEC01
  push $0
  
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\9.0" "InstallDir"
  strcmp $0 "" done90
  push "$0..\Packages\Debugger\"
  push "2008"
  call DoUpdateDatFile
  push "$0usertype.dat"
  call UpdateUserType
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2008DebugExtInstalled" 1
  StrCpy $DEBUGEXT_INITIALIZED "1"
  done90:
  
  pop $0
SectionEnd
!endif

!ifndef MODULE_DEBUGEXT_NOVS2005
Section "Visual Studio 2005" DEBUGEXT_SEC02
  push $0
  
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\8.0" "InstallDir"
  strcmp $0 "" done80
  push "$0..\Packages\Debugger\"
  push "2005"
  call DoUpdateDatFile
  push "$0usertype.dat"
  call UpdateUserType
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2005DebugExtInstalled" 1
  StrCpy $DEBUGEXT_INITIALIZED "1"
  done80:
  
  pop $0
SectionEnd
!endif

!ifndef MODULE_DEBUGEXT_NOVS2003
Section "Visual Studio 2003" DEBUGEXT_SEC03
  push $0
  
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.1" "InstallDir"
  strcmp $0 "" done71
  push "$0..\Packages\Debugger\"
  push "2003"
  call DoUpdateDatFile
  push "$0usertype.dat"
  call UpdateUserType
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2003DebugExtInstalled" 1
  StrCpy $DEBUGEXT_INITIALIZED "1"
  done71:

  pop $0
SectionEnd
!endif

!ifndef MODULE_DEBUGEXT_NOVS2002
Section "Visual Studio 2002" DEBUGEXT_SEC04
  push $0
  
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.0" "InstallDir"
  strcmp $0 "" done70
  push "$0..\Packages\Debugger\"
  push "2002"
  call DoUpdateDatFile
  push "$0usertype.dat"
  call UpdateUserType
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2002DebugExtInstalled" 1
  StrCpy $DEBUGEXT_INITIALIZED "1"
  done70:

  pop $0
SectionEnd
!endif

!ifndef MODULE_DEBUGEXT_NOVC60
Section "Visual C++ 6.0" DEBUGEXT_SEC05
  push $0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  strcmp $0 "" done60
  push "$0\MsDev98\bin\"
  push "6.0"
  call DoUpdateDatFile
  push "$0\MsDev98\bin\usertype.dat"
  call UpdateUserType
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VC60DebugExtInstalled" 1
  StrCpy $DEBUGEXT_INITIALIZED "1"
  done60:

  pop $0
SectionEnd
!endif

SectionGroupEnd

Section -PostDebugExtSection
  Delete "$TEMP\autoexp.dat_entries.txt"
  Delete "$TEMP\autoexp.dat-autoexpand2003"
  Delete "$TEMP\autoexp.dat-autoexpand2005"
  Delete "$TEMP\autoexp.dat-autoexpand2008"
  Delete "$TEMP\autoexp.dat-visualizer2005"
  Delete "$TEMP\autoexp.dat-visualizer2008"

#  IntCmp $DEBUGEXT_INITIALIZED 0 nodebugext

#  SetOutPath "$INSTDIR\debugext"
#  File "${MODULE_DEBUGEXT_ROOT}\doc\usage.rtf"
#  !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\Debugger Extension Readme.lnk" "$INSTDIR\debugext\usage.rtf"

  nodebugext:
SectionEnd

Function DoUpdateDatFile
  exch $0 ; vs version
  exch
  exch $1 ;dir
  push $2
  
  strcpy $2 "$1\autoexp.dat"
  
  push $2
  call BackupFile

  
  push "" ;remove all qt related stuff
  push $2
  call CleanAutoExp

  push "[AutoExpand]"
  push $2
  ${If} $0 == "6.0"
  ${OrIf} $0 == "2002"
    push "$TEMP\autoexp.dat_entries.txt"
  ${Else}    
    push "$TEMP\autoexp.dat-autoexpand$0"
  ${EndIf}  
  call InsertAutoExp
  
  ${If} $0 == "2005"
  ${OrIf} $0 == "2008"
    push "[Visualizer]"
    push $2
    push "$TEMP\autoexp.dat-visualizer$0"    
    call InsertAutoExp
  ${EndIf}
  
  pop $2
  pop $1
  pop $0
FunctionEnd

Function UpdateUserType
  exch $0 ;usertype filename
  push $1

  push $0
  call BackupFile

  ClearErrors
  FileOpen $1 $0 a
  IfErrors done

  push "Q_OBJECT"
  push $1
  call AddUserType
  push "Q_ENUMS"
  push $1
  call AddUserType
  push "Q_FLAGS"
  push $1
  call AddUserType
  push "Q_INTERFACES"
  push $1
  call AddUserType
  push "Q_PROPERTY"
  push $1
  call AddUserType
  push "Q_SETS"
  push $1
  call AddUserType
  push "Q_CLASSINFO"
  push $1
  call AddUserType
  push "emit"
  push $1
  call AddUserType
  push "SIGNAL"
  push $1
  call AddUserType
  push "SLOT"
  push $1
  call AddUserType
  push "signals:"
  push $1
  call AddUserType
  push "slots:"
  push $1
  call AddUserType
  push "qobject_cast"
  push $1
  call AddUserType
  push "qvariant_cast"
  push $1
  call AddUserType

done:
  FileClose $1

  pop $1
  pop $0
FunctionEnd

Function AddUserType
  exch $0 ;file handle
  exch
  exch $1 ;value
  push $2

  FileSeek $0 0 ;beginning

scann:
  FileRead $0 $2
  IfErrors insert
  strcmp "$2" "$1" done
  strcmp "$2" "$1$\r" done
  strcmp "$2" "$1$\n" done
  strcmp "$2" "$1$\r$\n" done
  goto scann

insert:
  FileSeek $0 -2 CUR ;previous char
  ClearErrors
  FileRead $0 $2 2
  IfErrors +3
  strcmp $2 "$\r$\n" +2
  FileWrite $0 "$\r$\n"
  FileWrite $0 "$1$\r$\n"

done:
  pop $2
  pop $1
  pop $0
FunctionEnd

Function BackupFile
  exch $0
  push $1
  push $2

  IfFileExists $0 0 done

  ; remove extension
  strlen $1 $0
  intop $1 $1 - 3
  strcpy $2 $0 $1

  IfFileExists "$2qto" writeqte writeqto

  writeqto:
    copyfiles /FILESONLY "$0" "$2qto"
    goto done

  writeqte:
    SetDetailsPrint none
    IfFileExists "$2qte" 0 +2
    Delete "$2qte"
    SetDetailsPrint both
    copyfiles /FILESONLY "$0" "$2qte"

  done:
  pop $2
  pop $1
  pop $0
FunctionEnd

Function InsertAutoExp
  exch $0 ;filename qtrules
  exch
  exch $1 ;filename autoexp
  exch
  exch 2
  exch $8 ; section name in autoexp file "[AutoExpand]"
  exch 2
  push $2 ;handle qtrules
  push $3 ;readline
  push $4 ;handle autoexp
  push $5 ;tmpfile handle
  push $6 ;tmp copy of autoexp.dat
  push $7

  ClearErrors
  IfErrors done

  StrCpy $6 "$1_tmp"
  CopyFiles /FILESONLY "$1" "$6"
  
  FileOpen $5 $1 a
  IfErrors done
  
  FileOpen $4 $6 r
  IfErrors done
  
  FileOpen $2 $0 r
  IfErrors done

nextline:
  FileRead $4 $3
  IfErrors insertloop

  strcpy $7 $3 12
  strcmp $7 $8 insert copyline
  copyline:
    FileWrite $5 $3
    goto nextline

  insert:
  FileWrite $5 $3
  insertloop:
    FileRead $2 $3
    IfErrors copyrest
    strcpy $7 $3 15
    strcmp $7 ";QT_DEBUG_START" 0 +2
      strcpy $3 "$7 (${MODULE_DEBUGEXT_INSTALLER})$\r$\n"
    FileWrite $5 $3
    goto insertloop

  copyrest:
    FileRead $4 $3
    IfErrors renameFile
    FileWrite $5 $3
    goto copyrest

renameFile:
  FileClose $5
  FileClose $4
  FileClose $2
  SetDetailsPrint none  
  Delete $6
  SetDetailsPrint both

done:
  pop $7
  pop $6
  pop $5
  pop $4
  pop $3
  pop $2
  pop $0
  pop $1
  pop $8
FunctionEnd

!insertmacro CleanAutoExp ""
!insertmacro CleanAutoExp "un."
!macroend ;DEBUGEXT_SECTIONS

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_DESCRIPTION
!ifndef MODULE_DEBUGEXT_NOVS2008
  !insertmacro MUI_DESCRIPTION_TEXT ${DEBUGEXT_SEC01} "This installs Qt 4 debug extensions. This makes it easier to debug Qt objects in Visual Studio (Overwrites Qt 3 debug extensions)."
!endif
!ifndef MODULE_DEBUGEXT_NOVS2005
  !insertmacro MUI_DESCRIPTION_TEXT ${DEBUGEXT_SEC02} "This installs Qt 4 debug extensions. This makes it easier to debug Qt objects in Visual Studio (Overwrites Qt 3 debug extensions)."
!endif
!ifndef MODULE_DEBUGEXT_NOVS2003
  !insertmacro MUI_DESCRIPTION_TEXT ${DEBUGEXT_SEC03} "This installs Qt 4 debug extensions. This makes it easier to debug Qt objects in Visual Studio (Overwrites Qt 3 debug extensions)."
!endif
!ifndef MODULE_DEBUGEXT_NOVS2002
  !insertmacro MUI_DESCRIPTION_TEXT ${DEBUGEXT_SEC04} "This installs Qt 4 debug extensions. This makes it easier to debug Qt objects in Visual Studio (Overwrites Qt 3 debug extensions)."
!endif
!ifndef MODULE_DEBUGEXT_NOVC60
  !insertmacro MUI_DESCRIPTION_TEXT ${DEBUGEXT_SEC05} "This installs Qt 4 debug extensions. This makes it easier to debug Qt objects in Visual Studio (Overwrites Qt 3 debug extensions)."
!endif
!macroend

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_STARTUP
  push $0
  push $1
  push $3
  
!ifndef MODULE_DEBUGEXT_NOVS2008
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\9.0" "InstallDir"
  strcmp $0 "" debugext_startup_not90
  strcpy $1 "$0..\Packages\Debugger\autoexp.dat"
  ClearErrors
  FileOpen $3 $1 a
  IfErrors debugext_startup_not90
  FileClose $3
  
  Goto debugext_startup_done90
  debugext_startup_not90:
  SectionSetFlags ${DEBUGEXT_SEC01} "16"
  debugext_startup_done90:
!endif

!ifndef MODULE_DEBUGEXT_NOVS2005
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\8.0" "InstallDir"
  strcmp $0 "" debugext_startup_not80
  strcpy $1 "$0..\Packages\Debugger\autoexp.dat"
  ClearErrors
  FileOpen $3 $1 a
  IfErrors debugext_startup_not80
  FileClose $3
  
  Goto debugext_startup_done80
  debugext_startup_not80:
  SectionSetFlags ${DEBUGEXT_SEC02} "16"
  debugext_startup_done80:
!endif

!ifndef MODULE_DEBUGEXT_NOVS2003
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.1" "InstallDir"
  strcmp $0 "" debugext_startup_not71
  strcpy $1 "$0..\Packages\Debugger\autoexp.dat"
  ClearErrors
  FileOpen $3 $1 a
  IfErrors debugext_startup_not71
  FileClose $3
  
  Goto debugext_startup_done71
  debugext_startup_not71:
  SectionSetFlags ${DEBUGEXT_SEC03} "16"
  debugext_startup_done71:
!endif

!ifndef MODULE_DEBUGEXT_NOVS2002
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.0" "InstallDir"
  strcmp $0 "" debugext_startup_not70
  strcpy $1 "$0..\Packages\Debugger\autoexp.dat"
  ClearErrors
  FileOpen $3 $1 a
  IfErrors debugext_startup_not70
  FileClose $3
  
  Goto debugext_startup_done70
  debugext_startup_not70:
  SectionSetFlags ${DEBUGEXT_SEC04} "16"
  debugext_startup_done70:
!endif

!ifndef MODULE_DEBUGEXT_NOVC60
  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  strcmp $0 "" debugext_startup_not60
  strcpy $1 "$0\MsDev98\bin\autoexp.dat"
  ClearErrors
  FileOpen $3 $1 a
  IfErrors debugext_startup_not60
  FileClose $3
  
  Goto debugext_startup_done60
  debugext_startup_not60:
  SectionSetFlags ${DEBUGEXT_SEC05} "16"
  debugext_startup_done60:
!endif

  pop $3
  pop $1
  pop $0
!macroend ;DEBUGEXT_STATUP

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_FINISH
!macroend

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_UNSTARTUP
  !insertmacro ConfirmOnRemove "VS2008DebugExtInstalled" "Qt Debugger Extension (VS2008)"
  !insertmacro ConfirmOnRemove "VS2005DebugExtInstalled" "Qt Debugger Extension (VS2005)"
  !insertmacro ConfirmOnRemove "VS2003DebugExtInstalled" "Qt Debugger Extension (VS2003)"
  !insertmacro ConfirmOnRemove "VS2002DebugExtInstalled" "Qt Debugger Extension (VS2002)"
  !insertmacro ConfirmOnRemove "VC60DebugExtInstalled"   "Qt Debugger Extension (VC60)"
!macroend

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_UNINSTALL
Section un."Debugger Extension"
  push $0

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "VS2008DebugExtInstalled"
  intcmp $0 1 0 done90
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2008DebugExtInstalled" 0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\9.0" "InstallDir"
  strcmp $0 "" done90
  push "$0..\Packages\Debugger\"
  call un.DoCleanDatFile
  done90:

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "VS2005DebugExtInstalled"
  intcmp $0 1 0 done80
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2005DebugExtInstalled" 0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\8.0" "InstallDir"
  strcmp $0 "" done80
  push "$0..\Packages\Debugger\"
  call un.DoCleanDatFile
  done80:

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "VS2003DebugExtInstalled"
  intcmp $0 1 0 done71
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2003DebugExtInstalled" 0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.1" "InstallDir"
  strcmp $0 "" done71
  push "$0..\Packages\Debugger\"
  call un.DoCleanDatFile
  done71:

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "VS2002DebugExtInstalled"
  intcmp $0 1 0 done70
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VS2002DebugExtInstalled" 0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\7.0" "InstallDir"
  strcmp $0 "" done70
  push "$0..\Packages\Debugger\"
  call un.DoCleanDatFile
  done70:

  ReadRegDWORD $0 SHCTX "$PRODUCT_UNIQUE_KEY" "VC60DebugExtInstalled"
  intcmp $0 1 0 done60
  WriteRegDWORD SHCTX "$PRODUCT_UNIQUE_KEY" "VC60DebugExtInstalled" 0

  ReadRegStr $0 SHCTX "Software\Microsoft\VisualStudio\6.0\Setup" "VsCommonDir"
  strcmp $0 "" done60
  push "$0\MsDev98\bin\"
  call un.DoCleanDatFile
  done60:
  
#  Delete "$INSTDIR\debugext\usage.rtf"
#  RmDir "$INSTDIR\debugext"
#  Delete "$SMPROGRAMS\$STARTMENU_STRING\Debugger Extension Readme.lnk"

  pop $0
SectionEnd

Function un.DoCleanDatFile
  exch $0 ;dir
  push "${MODULE_DEBUGEXT_INSTALLER}"
  push "$0autoexp.dat"
  call un.CleanAutoExp
  pop $0
FunctionEnd

!macroend ;DEBUGEXT_UNINSTALL

;------------------------------------------------------------------------------------------------
!macro DEBUGEXT_UNFINISH
!macroend

!else ;MODULE_DEBUGEXT
!macro DEBUGEXT_INITIALIZE
!macroend
!macro DEBUGEXT_SECTIONS
!macroend
!macro DEBUGEXT_DESCRIPTION
!macroend
!macro DEBUGEXT_STARTUP
!macroend
!macro DEBUGEXT_FINISH
!macroend
!macro DEBUGEXT_UNSTARTUP
!macroend
!macro DEBUGEXT_UNINSTALL
!macroend
!macro DEBUGEXT_UNFINISH
!macroend
!endif ;MODULE_DEBUGEXT

