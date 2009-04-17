!ifdef MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
  !define MODULE_OPENSOURCE_PAGE "opensource.ini"
  page custom ModuleOpenSourceShowPage
!macroend
!macro OPENSOURCE_SECTIONS
  Section -ModuleOpenSourceSection
    !ifdef MODULE_OPENSOURCE_ROOT
      SetOutPath "$INSTDIR"
      File "${MODULE_OPENSOURCE_ROOT}\OPENSOURCE-NOTICE.TXT"
    !endif
    !insertmacro CreateConditionalShortCut "$SMPROGRAMS\$STARTMENU_STRING\OpenSource Notice.lnk" "$INSTDIR\OPENSOURCE-NOTICE.TXT"
  SectionEnd

  Function ModuleOpenSourceShowPage
    !insertmacro MUI_HEADER_TEXT "Open Source Edition" " "
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_OPENSOURCE_PAGE}"
    strcpy "$LICENSEE" "Open Source"
    strcpy "$LICENSE_PRODUCT" "OpenSource"
  FunctionEnd
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_OPENSOURCE_PAGE}"
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
  Section -un.ModuleOpenSourceSection
    Delete "$SMPROGRAMS\$STARTMENU_STRING\OpenSource Notice.lnk"
  SectionEnd
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!else ;MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
!macroend
!macro OPENSOURCE_SECTIONS
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!endif ;MODULE_OPENSOURCE

