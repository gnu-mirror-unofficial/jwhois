#!/bin/sh

# Check "--config" command line argument.
# Copyright (C) 2016 Free Software Foundation, Inc.
#
# This file is part of GNU JWhois.
#
# GNU JWhois is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GNU JWhois is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU JWhois.  If not, see <http://www.gnu.org/licenses/>.

. "${srcdir=.}/tests/init.sh"

ping -c 1 gnu.org || skip_ failed to connect to gnu.org

jwhois --config=${confdir}/jwhois.conf gnu.org || fail=1

jwhois -c ${confdir}/jwhois.conf gnu.org || fail=1

Exit $fail
