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

my %special_macros = (
# control flow:
    "BEGIN"    => "SH_BEGIN",
    "END"      => "SH_END",
    "IF"       => "SH_IF",
    "ELSE"     => "SH_ELSE",
    "ELSEIF"   => "SH_ELSEIF",
    "FOR"      => "SH_FOR",
    "ENDFOR"   => "SH_ENDFOR",
    "DO"       => "SH_DO",
    "UNTIL"    => "SH_UNTIL",
    "WHILE"    => "SH_WHILE",
    "ENDWHILE" => "SH_ENDWHILE",
    "WHEN"     => "SH_WHEN",
    "BREAK"    => "SH_BREAK",
    "CONTINUE" => "SH_CONTINUE",
    "RETURN"   => "SH_RETURN",
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
    "NORMAL"   => "SH_NORMAL",
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
# other:
    "ORT"      => "SHORT",
    "ADER"     => "SHADER",
    "LIB"      => "SHLIB",
    "INSTALL"  => "SH_INSTALL",
    "REGRESS"  => "SH_REGRESS",
    "DLLEXPORT"=> "SH_DLLEXPORT",
);

my %reserved_words = (
    "if"       => "internal_if",
    "else"     => "internal_else",
    "break"    => "internal_break",
    "continue" => "internal_continue",
    "while"    => "internal_while",
    "for"      => "internal_for",
    "do"       => "internal_do",
    "and"      => "logical_and",
    "or"       => "logical_or",
    "not"      => "logical_not",
);

my %reserved_classnames = (
    "Util"       => "ShUtil",
);

sub substituted_macro
{
    $_ = shift;
    $_ = $special_macros{$_} if exists $special_macros{$_};
    return $_;
}

sub substituted_classname
{
    $_ = shift;
    $_ = $reserved_classnames{$_} if exists $reserved_classnames{$_};
    return $_;
}

sub to_lower
{
    $_ = shift;
    $_ =~ tr/[A-Z]/[a-z]/;
    $_ = $reserved_words{$_} if exists $reserved_words{$_};
    return $_;
}

sub process_file
{
    my $filename = shift;
    print "Processing $filename... ";
    open INPUT, "$filename" or die "Cannot open $filename as input.";

    my @lines;
    while (<INPUT>) {
        # Header guards
        s/([ "])SH([A-Z]+)_HPP/\1XYZheaderguardXYZ\2_HPP/g;
        s/([ "])SHUTIL_([A-Z]+)_HPP/\1ZZZheaderguardZZZ\2_HPP/g;
        
        # Macros/enums
        s/(^|[^a-zA-Z_])SH_?([A-Z]+)/$1 . &substituted_macro($2)/eg;

        # Class names
        s/(^|[^a-zA-Z])Sh([A-Z]+[a-z]+)/$1 . &substituted_classname($2)/eg;

        # Function names (e.g. nibbles)
        s/(^|[^a-zA-Z])sh([A-Z]+[a-z]+)/$1 . &to_lower($2)/eg;

	# Restore header guard
        s/XYZheaderguardXYZ([A-Z]+)_HPP/SH\1_HPP/g;
        s/ZZZheaderguardZZZ([A-Z]+)_HPP/SHUTIL_\1_HPP/g;

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
