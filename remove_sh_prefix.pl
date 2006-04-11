#!/usr/bin/perl

use strict;

use File::Copy;
use File::Find;
use File::Temp;

# Directories which contain files to be "licensified"
my @SRC_DIRS = ('backends', 'src', 'test', 'examples');

my %special_macros = (
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
        s/ SH([A-Z]+)_HPP/ XYZXYZ\1_HPP/g;
        
        # Macros/enums
        s/(^|[^a-zA-Z])SH_?([A-Z]+)/$1 . &substituted_macro($2)/eg;

        # Class names
        s/(^|[^a-zA-Z])Sh([A-Z]+[a-z]+)/$1 . &substituted_classname($2)/eg;

        # Function names (e.g. nibbles)
        s/(^|[^a-zA-Z])sh([A-Z]+[a-z]+)/$1 . &to_lower($2)/eg;

	# Restore header guard
        s/ XYZXYZ([A-Z]+)_HPP/ SH\1_HPP/g;

        push @lines, $_;
    }
    close INPUT;

    open OUTPUT, ">$filename" or die "Cannot open $filename as output.";
    print OUTPUT foreach @lines;
    close OUTPUT;
    print "done.\n";
}

sub process_standard_files
{
    # Find all .hpp and .cpp files in the standard directories
    File::Find::find sub { /^.*\.[ch]pp\z/s && process_file($_) }, @SRC_DIRS;
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
