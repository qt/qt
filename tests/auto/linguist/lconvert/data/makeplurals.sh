#! /bin/bash

function makeit2()
{
    for ((i = 0; i < (1 << $1); i++)); do
        echo
        test -n "$3" && echo "$3"
        echo "msgid \"singular $2 $i\""
        echo "msgid_plural \"plural $2 $i\""
        for ((j = 0; j < $1; j++)); do
            tr=
            if test $((i & (1 << j))) = 0; then
                tr="translated $2 $i $j"
            fi
            echo "msgstr[$j] \"$tr\""
        done
    done
}

function makeit()
{
    {
        cat <<EOF
msgid ""
msgstr ""
"X-FooBar: yup\n"
"X-Language: $2\n"
EOF
        makeit2 $1 one ""
        makeit2 $1 two "#, fuzzy
#| msgid \"old untranslated one\""
        makeit2 $1 three "#, fuzzy
#| msgid \"old untranslated two\"
#| msgid_plural \"old untranslated plural two\""
        makeit2 $1 four "#, fuzzy
#| msgid_plural \"old untranslated only plural three\""
    } > ${OUTDIR}plural-$1.po
}

OUTDIR=$1
makeit 1 zh_CN
makeit 2 de_DE
makeit 3 pl_PL
