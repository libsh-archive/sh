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
        s/DEBUG_PRINT/SH_DEBUG_PRINT/g;
        s/STR_SH_DEBUG_PRINT/SH_STR_DEBUG_PRINT/g;
        s/CC_SH_DEBUG_PRINT/SH_CC_DEBUG_PRINT/g;
        s/DEBUG_WARN/SH_DEBUG_WARN/g;
        s/DEBUG_ERROR/SH_DEBUG_ERROR/g;
        s/DEBUG_ASSERT/SH_DEBUG_ASSERT/g;
        s/GL_CHECK_ERROR/SH_GL_CHECK_ERROR/g;
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
