#!/usr/bin/perl

use strict;
use warnings;
use Test::More;

# t\file.asm : input file, can contain "options: xxx" in line 1
# t\file.out : expected .i file
# t\file.err : if exists, expected stderr output

for my $asm (<t/*.asm>) {
	(my $out    = $asm) =~ s/\.asm$/.out/;			# should
	(my $i      = $asm) =~ s/\.asm$/.i/;			# got
	(my $err    = $asm) =~ s/\.asm$/.err/;			# should
	(my $goterr = $asm) =~ s/\.asm$/.err.got/;		# got
	
	# read options
	my $opts = "";
	open(my $f, "<", $asm) or die "$asm: $!";
	$_ = <$f>; defined($_) and /options:(.*)/ and $opts = $1;
	close($f);
	
	# check for error
	my $shall_fail;
	$shall_fail = 1 if -f $err;

	# run
	my $cmd = "asmpp $opts $asm 2> $goterr";
	my $rv1 = system($cmd);

	# diff result
	if ($shall_fail) {
		ok $rv1 != 0, $cmd;
		
		$cmd = "diff --strip-trailing-cr $err $goterr";
		my $rv2 = system($cmd);
		ok $rv2 == 0, $cmd;	
	
		if ($rv1 != 0 && $rv2 == 0) { unlink($i, $goterr); }
	}
	else {
		ok $rv1 == 0, $cmd;
		
		$cmd = "diff --strip-trailing-cr $out $i";
		my $rv2 = system($cmd);
		ok $rv2 == 0, $cmd;	

		if ($rv1 == 0 && $rv2 == 0) { unlink($i, $goterr); }
	}
}

done_testing();
