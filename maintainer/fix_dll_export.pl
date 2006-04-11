#!/usr/bin/perl

use strict;

use File::Copy;
use File::Find;
use File::Temp;

# Directories which contain files to be "licensified"
my @SRC_DIRS = ('backends', 'src', 'test', 'examples');

sub process_file
{
    my $filename = shift;
    print "Processing $filename... ";
    open INPUT, "$filename" or die "Cannot open $filename as input.";

    my @lines;
    while (<INPUT>) {
        s/(^| )DLLEXPORT/\1SH_DLLEXPORT/g;
        push @lines, $_;
    }
    close INPUT;

    if (open OUTPUT, ">$filename") {
        print OUTPUT foreach @lines;
        close OUTPUT;
        print "done.\n";
    } else {
        print "skipped.\n";
    }
}

sub process_standard_files
{
    # Find all .hpp and .cpp files in the standard directories
    File::Find::find sub { /^.*\.[ch]pp(\.py)?\z/s && process_file($_) }, @SRC_DIRS;
}

sub main
{
    if (@ARGV > 0) {
        process_file($ARGV[0]);
    } else {
        &process_standard_files;
    }
}

&main;
