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
        s/ SH([A-Z]+)_HPP/ \1_HPP/g;
        s/ ([A-Z]+)_HPP/ SH\1_HPP/g;
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
