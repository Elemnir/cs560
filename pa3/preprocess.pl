#!/usr/bin/env perl
use strict; use warnings;
use Data::Dumper;

my %ids;
my %links;
my $title = "ERROR";
while(<>) {
  if(/^    <title>(.+)<\/title>/) {
    $title = $1;
  } elsif(/^    <id>(\d+)<\/id>/) {
    $ids{$title} = $1;
  } elsif(/\[\[([A-Za-z0-9 ]+)\]\]/g) {
    push(@{$links{$title}}, $1);
  }
}

foreach(keys(%links)) {
  print "$ids{$_} 1.0 ";
  foreach(@{$links{$_}}) {
    if(defined($ids{$_})) {
      print "$ids{$_} ";
    }
  }
  print "\n";
}
