#!/usr/bin/perl

use strict;

use File::Copy;
use File::Temp;

# When changing the copyright notice, one must also change the appropriate
# string in src/scripts/common.py
my $copyright_text = <<END_OF_COPYRIGHT_TEXT;
// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must
// not claim that you wrote the original software. If you use this
// software in a product, an acknowledgment in the product documentation
// would be appreciated but is not required.
// 
// 2. Altered source versions must be plainly marked as such, and must
// not be misrepresented as being the original software.
// 
// 3. This notice may not be removed or altered from any source
// distribution.
//////////////////////////////////////////////////////////////////////////////
END_OF_COPYRIGHT_TEXT

die "Must specify a filename." if @ARGV == 0;
my $filename = $ARGV[0];
open INPUT, "$filename" or die "Cannot open $filename.";

my $in_copyright = 0; # 0 = before the header
my @lines;
while (<INPUT>) {
    if (m|^// Sh: A GPU metaprogramming language|) {
	$in_copyright = 1; # 1 = inside the header
    }
    elsif (m|^///////////////////////////////////////////////////////////////////////|) {
	$in_copyright = 2; # 2 = after the header
	push @lines, $copyright_text;
    } 
    elsif ($in_copyright == 0 or $in_copyright == 2) {
	push @lines, $_;
    } 
    elsif (!m|^//|) {
	die "$filename: Unexpected header\n";
    }
}
close INPUT;

if ($in_copyright == 0) {
    # No header was found, insert copyright text at the front
    unshift @lines, $copyright_text;
}

die "$filename: Nothing was found after the header." if $in_copyright == 1;

open OUTPUT, ">$filename" or die "Cannot open $filename.";
print OUTPUT foreach @lines;
close OUTPUT;
