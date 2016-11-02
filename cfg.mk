# Customize maint.mk                           -*- makefile -*-
# Copyright (C) 2016 Free Software Foundation, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Used in maint.mk's web-manual rule
manual_title = GNU JWhois

gnulib_dir = .gnulib

# Tools used to bootstrap this package, used for "announcement".
bootstrap-tools = autoconf,automake,gnulib

# 'sc_makefile_at_at_check' fails for @CODE_COVERAGE_RULES@.
local-checks-to-skip = \
  sc_makefile_at_at_check \
  sc_prohibit_atoi_atof \
  sc_prohibit_strncpy \
  sc_trailing_blank

old_NEWS_hash = f850f73b8fc429fa4cc4fbd10a4adcce
