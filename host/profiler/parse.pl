#!/usr/bin/perl -w

use strict;

print $ARGV[0]."\n";

my %funcs;

sub INT_handler {
#  print("Don't Interrupt!\n");
  my $sum = 0;
  foreach my $key (keys(%funcs)) {
    $sum += $funcs{$key};
  }
  printf "\n\n\n\n\n\n";
  foreach my $key (sort { $funcs{$b} <=> $funcs{$a} } keys(%funcs) ){
    printf "%10d hits : %6.2f %%: %s\n",
      ($funcs{$key}, ($funcs{$key} / $sum * 100.0), $key);
  }
}

$SIG{'INT'} = 'INT_handler';

while (<STDIN>) {
  chomp;

  my $cmd = "avr-addr2line -C -e ".$ARGV[0]." -f $_";
  my $result = `$cmd`;
  chomp($result);
  my ($func, $pos) = split /\n/,$result;
#  print "cmd: $cmd, $func; \n";
  if (defined($pos)) {
    my ($file, $line) = split /:/,$pos;
    #  print "func: $func, file: $file, line: $line\n";
    if (defined($funcs{$func})) {
      $funcs{$func}++;
    } else {
      $funcs{$func} = 1;
    }
  }
#  my $result = "";
}
