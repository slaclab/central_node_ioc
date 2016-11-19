#!/usr/bin/env perl

use strict;

my $IocStCmd;
if (-f "st.cmd") {
    $IocStCmd = "st.cmd";
} else {
    $IocStCmd = "";
}
print "$IocStCmd\n";

