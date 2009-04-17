!ifndef LIST_INCLUDE
!define LIST_INCLUDE

; usage:
; push item
; push list
; call ItemInList
; returns 1 or 0
!macro ItemInList UN
Function ${UN}ItemInList
  exch $0 ;list
  exch
  exch $1 ;item
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  
  strcpy $3 ""
  strcpy $2 "0"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +4
      strcmp "$3" "$1" found
      strcpy $3 "" ;reset substr
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "1"
    goto done
    
  atend:
    strcmp "$3" "$1" found
    strcpy $0 "0"

  done:
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd
!macroend

!insertmacro ItemInList ""
!insertmacro ItemInList "un."

Function GetItemInList
  exch $0 ;list
  exch
  exch $1 ;index
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  push $5 ;current index

  strcpy $3 ""
  strcpy $2 "0"
  strcpy $5 "1"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +5
      strcmp "$5" "$1" found
      strcpy $3 "" ;reset substr
      intop $5 $5 + 1
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "$3"
    goto done

  atend:
    strcmp "$5" "$1" found
    strcpy $0 ""

  done:
  pop $5
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd

!endif ;LIST_INCLUDE