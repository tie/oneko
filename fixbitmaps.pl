#!/usr/contributed/bin/perl
foreach $elem( @ARGV) {
    if ($elem =~ /\.xbm$/io) {
	rename ($elem, $elem . '.bak');
	open (OUTFILE, ">$elem");
	$oldfile = $elem . '.bak';
	open (INFILE, $oldfile);
	print "Working on $elem\n";
	@data = <INFILE>;
	$name = $elem;
	$name =~ s/\.xbm$//;
	@parts = split(/\//,$name);
	$count = @parts;
	$name = @parts[$count - 1];
	print "name = $name count = $count\n";
	foreach $line (@data) {
	    $line =~ s/ \S*width/' ' . $name . '_width'/e;
	    $line =~ s/ \S*height/' ' . $name . '_height'/e;
	    $line =~ s/ \S*x_hot/' ' . $name . '_x_hot'/e;
	    $line =~ s/ \S*y_hot/' ' . $name . '_y_hot'/e;
	    $line =~ s/ \S*bits/' ' . $name . '_bits'/e;
	}
	print OUTFILE @data;
	unlink ($oldfile);
    }
}
