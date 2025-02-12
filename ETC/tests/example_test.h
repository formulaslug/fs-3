#ifndef _EXAMPLE_TEST_H_
#define _EXAMPLE_TEST_H_


#include "unity.h"


void some_example_test_that_will_pass() {
    TEST_ASSERT_EQUAL(4, 2 * 2);
}


void some_example_test_that_will_fail() {
    TEST_ASSERT_EQUAL(3, 1 + 1);
}


#endif  // _EXAMPLE_TEST_H_
