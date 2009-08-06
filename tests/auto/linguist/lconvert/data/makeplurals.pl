#! /usr/bin/env perl

sub makeit2($$$)
{
    for (my $i = 0; $i < (1 << $_[0]); $i++) {
        print OUTFILE "\n";
        print OUTFILE "$_[2]\n" unless $3 eq "";
        print OUTFILE "msgid \"singular $_[1] $i\"\n";
        print OUTFILE "msgid_plural \"plural $_[1] $i\"\n";
        for (my $j = 0; $j < $_[0]; $j++) {
            my $tr;
            if (($i & (1 << $j)) == 0) {
                $tr = "translated $_[1] $i $j";
            }
            print OUTFILE "msgstr[$j] \"$tr\"\n";
        }
    }
}

sub makeit($$)
{
    open OUTFILE, ">${OUTDIR}plural-$_[0].po" || die "cannot write file in $OUTDIR";
    print OUTFILE <<EOF;
msgid ""
msgstr ""
"X-FooBar: yup\\n"
"X-Language: $_[1]\\n"
EOF
    makeit2($_[0], "one", "");
    makeit2($_[0], "two", "#, fuzzy
#| msgid \"old untranslated one\"");
    makeit2($_[0], "three", "#, fuzzy
#| msgid \"old untranslated two\"
#| msgid_plural \"old untranslated plural two\"");
    makeit2($_[0], "four", "#, fuzzy
#| msgid_plural \"old untranslated only plural three\"");
}

$OUTDIR = $ARGV[0];
makeit(1, "zh_CN");
makeit(2, "de_DE");
makeit(3, "pl_PL");
