#!/usr/bin/perl -w

foreach (sort keys %ENV) {
	warn $_ => $ENV{$_};
}

my @args = qw(	WSGeneratedStub.h WSGeneratedStub.cp WSGeneratedObj.h WSGeneratedObj.m );

foreach (@args) {
	my $fn = $_;
	my $vn = $fn;
	
	$vn =~ s/\./_/g;
	
	print qq~const char* k$vn =\n~;
	
	open (IN, "<Tests/WebServices/$_") || die "Can't open $_ : $!";
	while (<IN>) {
		chop;
		my $line = $_;
		$_ =~ s/\\n/\\\\n/g;
		$_ =~ s/\"/\\\"/g;
		print qq~"$_\\n"\n~;
	}
	close (IN);
	
	print qq~;\n\n~;
}