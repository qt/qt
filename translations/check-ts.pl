#! /usr/bin/perl -w

use strict;

# "qt" must come last to avoid prefix matching.
my @groups = ("assistant", "designer", "linguist", "qt_help", "qtconfig", "qvfb", "qt");

my %scores = ();
my %langs = ();

my $files = join("\n", <*.ts>);
my $res = `xmlpatterns -param files=\"$files\" check-ts.xq`;
for my $i (split(/ /, $res)) {
  $i =~ /^([^.]+).ts:(.*)$/;
  my ($fn, $pc) = ($1, $2);
  for my $g (@groups) {
    if ($fn =~ /^${g}_(.*)$/) {
      my $lang = $1;
      $scores{$g}{$lang} = $pc;
      $langs{$lang} = 1;
      last;
    }
  }
}

# now we move "qt" to the front, as it should be the first column.
pop @groups;
unshift @groups, "qt";

my $code = "";

print "L10n  ";
for my $g (@groups) {
  print " ".$g." ";
}
print "\n";
for my $lang (sort(keys(%langs))) {
  printf "%-5s ", $lang;
  my $qt = 1;
  my $rest = 1;
  my $line = "";
  for my $g (@groups) {
    my $pc = $scores{$g}{$lang};
    $pc = "0" if !defined($pc);
    if (int($pc) < 98 or !$qt) {
      if ($g eq "qt") {
        $qt = 0;
      } else {
        $rest = 0;
      }
    } else {
      $line .= " ".$g."_".$lang.".ts";
    }
    printf " %-".(length($g)+1)."s", $pc;
  }
  if ($qt) {
    $code .= " \\\n   ".$line;
    if (!$rest) {
      print "   (partial)";
    }
  } else {
    print "   (excluded)";
  }
  print "\n";
}

my $fn = "translations.pro";
my $nfn = $fn."new";
open IN, $fn or die;
open OUT, ">".$nfn or die;
while (1) {
  $_ = <IN>;
  last if (/^TRANSLATIONS /);
  print OUT $_;
}
while ($_ =~ /\\\n$/) {
  $_ = <IN>;
}
print OUT "TRANSLATIONS =".$code."\n";
while (<IN>) {
  print OUT $_;
}
close OUT;
close IN;
rename $nfn, $fn;
