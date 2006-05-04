#!/usr/bin/perl

use strict;

use File::Copy;
use File::Find;
use File::Temp;

my $COLOR_GREEN  = "[32m";
my $COLOR_RED    = "[31m";
my $COLOR_BLUE   = "[34m";
my $COLOR_YELLOW = "[33m";
my $COLOR_NORMAL = "[0m";

# Directories which contain files to be processed
my @SRC_DIRS = ('backends', 'src', 'test', 'examples');

sub process_file
{
    my $filename = shift;
    print "Processing $filename... ";
    open INPUT, "$filename" or die "Cannot open $filename as input.";

    my @lines;
    while (<INPUT>) {
        s/PUSH_ARG_QUEUE/SH_PUSH_ARG_QUEUE/g;
        s/PUSH_ARG/SH_PUSH_ARG/g;
        s/PROCESS_ARG/SH_PROCESS_ARG/g;
        s/ENDIF/SH_ENDIF/g;
        s/NAME/SH_NAME/g;
        s/DECL/SH_DECL/g;
        s/NAMEDECL/SH_NAMEDECL/g;
        s/SH_NAMESH_DECL/SH_NAMEDECL/g;
        push @lines, $_;
    }
    close INPUT;

    if (open OUTPUT, ">$filename") {
        print OUTPUT foreach @lines;
        close OUTPUT;
        print $COLOR_GREEN . "done" . $COLOR_NORMAL . ".\n";
    } else {
        print $COLOR_YELLOW . "skipped" . $COLOR_NORMAL . ".\n";
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
