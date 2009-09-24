#!/usr/bin/perl -w

use strict;

# print $ARGV[0]."\n";

my %funcs;
my %addrs;


sub print_functions() {
  my $sum = 0;
  foreach my $key (keys(%addrs)) {
    $sum += $addrs{$key};
  }
  printf "\n\n\n\n\n\n";

  #  printf "addr: %s, hexaddr: %x\n", $addr, $hexaddr;
#  chomp($result);
#  my ($func, $pos) = split /\n/,$result;
#  if (defined($pos)) {
#    if ($func eq "??") {
#      $func = "?? $addr";
#    }
#    my ($file, $line) = split /:/,$pos;
    #  print "func: $func, file: $file, line: $line\n";
#  }

  
  my @topaddrs = sort { $addrs{$b} <=> $addrs{$a} } keys(%addrs);
#  print "$addrstr\n";
#  my $cmd = "avr-addr2line -C -e ".$ARGV[0]." -f $addrstr";
#  my $result = `$cmd`;
#  my @lines = split(/\n/, $result);

  my $cnt = 0;
  foreach my $key (@topaddrs) {
    printf "%10d hits : %6.2f %%: %s\n",
      ($addrs{$key}, ($addrs{$key} / $sum * 100.0), $key);
    if ($cnt++ > 20) {
      last;
    }
  }
}

sub INT_handler {
#  print("Don't Interrupt!\n");
}

# $SIG{'INT'} = 'INT_handler';

my $cnt = 0;

while (<STDIN>) {
  chomp;

  my $func = $_;
  my $line = <STDIN>;
  chomp($line);

  if (defined($line) && defined($func)) {
    if (defined($addrs{$func})) {
      $addrs{$func}++;
    } else {
      $addrs{$func} = 1;
    }
    
    if (++$cnt > 100) {
      print_functions();
      $cnt = 0;
    }
  }
#  my $result = "";
}
