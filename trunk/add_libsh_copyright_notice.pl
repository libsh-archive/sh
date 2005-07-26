#!/usr/bin/perl

use strict;

use File::Copy;
use File::Temp;

# When changing the copyright notice, one must also change the appropriate
# strings in:
#    - COPYING
#    - src/scripts/common.py
#    - backend/cc/CcTexturesString.hpp
my $copyright_text = <<END_OF_COPYRIGHT_TEXT;
// Sh: A GPU metaprogramming language.
//
// Copyright 2003-2005 Serious Hack Inc.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
// MA  02110-1301, USA
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
