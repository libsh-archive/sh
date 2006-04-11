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

my %prefixed_types = (
# binding types:
    "INPUT"    => "SH_INPUT",
    "OUTPUT"   => "SH_OUTPUT",
    "INOUT"    => "SH_INOUT",
    "TEMP"     => "SH_TEMP",
    "CONST"    => "SH_CONST",
    "TEXTURE"  => "SH_TEXTURE",
    "STREAM"   => "SH_STREAM",
    "PALETTE"  => "SH_PALETTE",
# semantic types:
    "ATTRIB"   => "SH_ATTRIB",
    "POINT"    => "SH_POINT",
    "VECTOR"   => "SH_VECTOR",
    "NORMAL"   => "SH_VECTOR",
    "COLOR"    => "SH_COLOR",
    "TEXCOORD" => "SH_TEXCOORD",
    "POSITION" => "SH_POSITION",
    "PLANE"    => "SH_PLANE",
# storage types:
    "HALF"     => "SH_HALF",
    "FLOAT"    => "SH_FLOAT",
    "DOUBLE"   => "SH_DOUBLE",
    "BYTE"     => "SH_BYTE",
    "SHORT"    => "SH_SHORT",
    "INT"      => "SH_INT",
    "UBYTE"    => "SH_UBYTE",
    "USHORT"   => "SH_USHORT",
    "UINT"     => "SH_UINT",
    "FBYTE"    => "SH_FBYTE",
    "FSHORT"   => "SH_FSHORT",
    "FINT"     => "SH_FINT",
    "FUBYTE"   => "SH_FUBYTE",
    "FUSHORT"  => "SH_FUSHORT",
    "FUINT"    => "SH_FUINT",
);

sub add_prefix
{
    $_ = shift;
    $_ = $prefixed_types{$_} if exists $prefixed_types{$_};
    return $_;
}

# Directories which contain files to be processed
my @SRC_DIRS = ('backends', 'src', 'test', 'examples');

sub process_file
{
    my $filename = shift;
    print "Processing $filename... ";
    open INPUT, "$filename" or die "Cannot open $filename as input.";

    my @lines;
    while (<INPUT>) {
        s/([^A-Z_])([A-Z]+)([^A-Z])/$1 . &add_prefix($2) . $3/eg;
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
