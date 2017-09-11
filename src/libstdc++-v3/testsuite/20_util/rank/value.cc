// { dg-options "-std=gnu++11" }
// { dg-do compile }

// Copyright (C) 2013-2016 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

#include <type_traits>
#include <testsuite_tr1.h>

void test01()
{
  using std::rank;
  using namespace __gnu_test;

  static_assert(test_property<rank, int>(0), "");
  static_assert(test_property<rank, int[2]>(1), "");
  static_assert(test_property<rank, int[][4]>(2), "");
  static_assert(test_property<rank, int[2][2][4][4][6][6]>(6), "");
  static_assert(test_property<rank, ClassType>(0), "");
  static_assert(test_property<rank, ClassType[2]>(1), "");
  static_assert(test_property<rank, ClassType[][4]>(2), "");
  static_assert(test_property<rank, ClassType[2][2][4][4][6][6]>(6), "");
}
