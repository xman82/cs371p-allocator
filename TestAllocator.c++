// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls -al /usr/include/gtest/
    ...
    gtest.h
    ...

    % locate libgtest.a
    /usr/lib/libgtest.a

    % locate libpthread.a
    /usr/lib/x86_64-linux-gnu/libpthread.a
    /usr/lib32/libpthread.a

    % locate libgtest_main.a
    /usr/lib/libgtest_main.a

    % g++ -pedantic -std=c++0x -Wall TestAllocator.c++ -o TestAllocator -lgtest -lpthread -lgtest_main

    % valgrind TestAllocator > TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator
#include <iostream>  //cout

#include "gtest/gtest.h"

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A                  allocator_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int, 100>,
            Allocator<double, 100> >
        my_types;

TYPED_TEST_CASE(TestAllocator, my_types);

TYPED_TEST(TestAllocator, One) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    if (p != 0) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator, Ten) {
    typedef typename TestFixture::allocator_type  allocator_type;
    typedef typename TestFixture::value_type      value_type;
    typedef typename TestFixture::difference_type difference_type;
    typedef typename TestFixture::pointer         pointer;

    allocator_type x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != 0) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

//-------------
//valid() tests
//-------------

TEST(TestAllocator, valid_1){
  Allocator<double, 50> x;
  ASSERT_EQ(x.isValid(), true);
}

TEST(TestAllocator, valid_2){
  Allocator<char, 100> x;
  char* p1 = x.allocate(84);
  ASSERT_EQ(x.isValid(), true);
}

TEST(TestAllocator, valid_3){
  Allocator<int, 100> x;
  int* p1 = x.allocate(5);
  int* p2 = x.allocate(10);
  int* p3 = x.allocate(4);
  ASSERT_EQ(x.isValid(), true);
}

//----------------
//allocate() tests
//----------------

TEST(TestAllocator, allocate_1){
  Allocator<char, 50> x;
  try{
    char* p1 = x.allocate(50);
    ASSERT_EQ(0,5);
  }
  catch(std::bad_alloc&){
    ASSERT_EQ(0,0);}
}

TEST(TestAllocator, allocate_2){
  Allocator<int, 50> x;
  try{
    int* p1 = x.allocate(1);
    ASSERT_EQ(0,0);
  }
  catch(std::bad_alloc&) {
    ASSERT_EQ(1,0);
  }
}

TEST(TestAllocator, allocate_3){
  Allocator<char, 50> x;
  char* p1 = x.allocate(34);
  try{
    char* p2 = x.allocate(1);
    ASSERT_EQ(0,1);  
  }
  catch(std::bad_alloc&) {
    ASSERT_EQ(0,0);
  }
}

//------------------
//deallocate() tests
//------------------

TEST(TestAllocator, deallocate_1) {
  Allocator<int,100> x;
  int* p1 = x.allocate(3);
  x.deallocate(p1, 3);
  int* p2 = x.allocate(2);
  ASSERT_EQ(p1, p2);
}

TEST(TestAllocator, deallocate_2) {
  Allocator<int, 100> x;
  int* p1 = x.allocate(20);
  x.deallocate(p1, 20);
  int* p2 = x.allocate(3);

  ASSERT_EQ(p1, p2);
}

TEST(TestAllocator, deallocate_3) {
  Allocator<char, 100> x;
  char* p1 = x.allocate(30);
  char* p2 = x.allocate(2);
  x.deallocate(p2, 2);
  char* p3 = x.allocate(5);
  
  ASSERT_EQ(p2, p3);
}
