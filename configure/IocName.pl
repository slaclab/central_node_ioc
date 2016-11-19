#!/usr/bin/env perl

use strict;
use Cwd qw(cwd);

    my $IocName = cwd();
    $IocName =~ s|\\|/|go;
    $IocName =~ s/^.*\///;  # iocname is last component of directory name
print "$IocName\n";

