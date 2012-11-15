;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
;; All rights reserved.
;; Contact: http://www.qt-project.org/legal
;;
;; This file is part of the tools applications of the Qt Toolkit.
;;
;; $QT_BEGIN_LICENSE:LGPL$
;; Commercial License Usage
;; Licensees holding valid commercial Qt licenses may use this file in
;; accordance with the commercial license agreement provided with the
;; Software or, alternatively, in accordance with the terms contained in
;; a written agreement between you and Digia.  For licensing terms and
;; conditions see http://qt.digia.com/licensing.  For further information
;; use the contact form at http://qt.digia.com/contact-us.
;;
;; GNU Lesser General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU Lesser
;; General Public License version 2.1 as published by the Free Software
;; Foundation and appearing in the file LICENSE.LGPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU Lesser General Public License version 2.1 requirements
;; will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;;
;; In addition, as a special exception, Digia gives you certain additional
;; rights.  These rights are described in the Digia Qt LGPL Exception
;; version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
;;
;; GNU General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU
;; General Public License version 3.0 as published by the Free Software
;; Foundation and appearing in the file LICENSE.GPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU General Public License version 3.0 requirements will be
;; met: http://www.gnu.org/copyleft/gpl.html.
;;
;;
;; $QT_END_LICENSE$
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

