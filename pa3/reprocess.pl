#!/usr/bin/env perl
use strict; use warnings;
use Data::Dumper;

my %links;
my %ranks;
my $title = "ERROR";

while(<>) {
  if(/(\d+) ([\d\.]+) ([\d ]+)/) {
    push(@{$links{$1}}, split(/ /, $3));
  }
}

# Now read in the new pageranks
while(<STDIN>) {
  if(/(\d+) ([\d\.]+)/) {
    $ranks{$2} = $1;
  }
}

foreach(sort(keys(%ranks))) {
  print "$ranks{$_} $_";
  #if(defined($links{$ranks{$_}})) {
    foreach(@{$links{$ranks{$_}}}) {
      print "$_ ";
    }
    #}
  print "\n";
}
