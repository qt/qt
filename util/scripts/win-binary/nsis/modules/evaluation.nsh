!ifdef MODULE_EVALUATION
!macro EVALUATION_INITIALIZE
  !ifndef MODULE_EVALUATION_QTDIR
    !ifdef MODULE_MINGW
      !define MODULE_EVALUATION_QTDIR $MINGW_INSTDIR
    !endif

    !ifdef MODULE_MSVC
      !define MODULE_EVALUATION_QTDIR $MSVC_INSTDIR
    !endif
    
    !ifdef MODULE_QTJAMBI
      !define MODULE_EVALUATION_QTDIR $QTJAMBI_INSTDIR
    !endif
  !endif
!macroend
!macro EVALUATION_SECTIONS
  Section -ModuleEvaluationSection
    !ifdef MODULE_MSVC_WIN32BUILDTREE & MODULE_MSVC_WIN32BUILDINSTALLDIRNAME
        push "$MSVC_INSTDIR\${MODULE_MSVC_WIN32BUILDINSTALLDIRNAME}"
        push "bin"
        call PatchKeyInBinary
    !endif
    !ifdef MODULE_MSVC_WM50BUILDTREE & MODULE_MSVC_WM50BUILDINSTALLDIRNAME
        push "$MSVC_INSTDIR\${MODULE_MSVC_WM50BUILDINSTALLDIRNAME}"
        push "lib"
        call PatchKeyInBinary
    !endif
    !ifdef MODULE_MSVC_SSDK50X86BUILDTREE & MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME
        push "$MSVC_INSTDIR\${MODULE_MSVC_SSDK50X86BUILDINSTALLDIRNAME}"
        push "lib"
        call PatchKeyInBinary
    !endif
    !ifndef MODULE_MSVC_WIN32BUILDTREE | MODULE_MSVC_WM50BUILDTREE | MODULE_MSVC_SSDK50X86BUILDTREE 
        push "${MODULE_EVALUATION_QTDIR}"
        push "bin"
        call PatchKeyInBinary
    !endif
  SectionEnd
  
  Function PatchKeyInBinary
    exch $3
    exch
    exch $2
    exch
    push $0
    push $1

    DetailPrint "Patching key in core ($2\$3)..."
    FindFirst $0 $1 "$2\$3\QtCore*.dll"
    StrCmp $1 "" ErrorPatchingCore
    qtnsisext::PatchBinary "$2\$3\$1" "qt_qevalkey=" "qt_qevalkey=$LICENSE_KEY"

    FindNext $0 $1
    StrCmp $1 "" ErrorPatchingCore
    qtnsisext::PatchBinary "$2\$3\$1" "qt_qevalkey=" "qt_qevalkey=$LICENSE_KEY"

    ErrorPatchingCore:

    DetailPrint "Patching key in gui ($2\$3)..."
    FindFirst $0 $1 "$2\$3\QtGui*.dll"
    StrCmp $1 "" ErrorPatchingGUI
    qtnsisext::PatchBinary "$2\$3\$1" "qt_qevalkey=" "qt_qevalkey=$LICENSE_KEY"

    FindNext $0 $1
    StrCmp $1 "" ErrorPatchingGUI
    qtnsisext::PatchBinary "$2\$3\$1" "qt_qevalkey=" "qt_qevalkey=$LICENSE_KEY"

    ErrorPatchingGUI:
    
    IfFileExists "$2\include\Qt\qconfig.h" 0 NoConfigFileFound
        CopyFiles /FILESONLY "$2\include\Qt\qconfig.h" "$2\include\QtCore"
    NoConfigFileFound:

    pop $1
    pop $0
    pop $3
    pop $2
  FunctionEnd
!macroend
!macro EVALUATION_DESCRIPTION
!macroend
!macro EVALUATION_STARTUP
!macroend
!macro EVALUATION_FINISH
!macroend
!macro EVALUATION_UNSTARTUP
!macroend
!macro EVALUATION_UNINSTALL
  Section un.-ModuleEvaluationSection
    Delete "${MODULE_EVALUATION_QTDIR}\include\QtCore\qconfig.h"
    RMDir "${MODULE_EVALUATION_QTDIR}\include\QtCore"
    RMDir "${MODULE_EVALUATION_QTDIR}\include"
  SectionEnd
!macroend
!macro EVALUATION_UNFINISH
!macroend
!else ;MODULE_EVALUATION
!macro EVALUATION_INITIALIZE
!macroend
!macro EVALUATION_SECTIONS
!macroend
!macro EVALUATION_DESCRIPTION
!macroend
!macro EVALUATION_STARTUP
!macroend
!macro EVALUATION_FINISH
!macroend
!macro EVALUATION_UNSTARTUP
!macroend
!macro EVALUATION_UNINSTALL
!macroend
!macro EVALUATION_UNFINISH
!macroend
!endif ;MODULE_EVALUATION

