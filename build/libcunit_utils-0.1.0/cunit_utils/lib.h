#ifndef CUNIT_UTILS_H
#define CUNIT_UTILS_H

#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include <stdbool.h>
#include <inttypes.h>

/// attempts to add suite to registry. reports failure otherwise and clears registry to prevent memory leaks in CUnit lib
/// returns CU_get_error() in case of failure, else CUE_SUCCESS.
CU_ErrorCode CU_utils_try_add_suite(CU_pSuite *out, const char *suite_name);

/// attempts to add a test to a suite. reports failure otherwise and clears registry to prevent memory leaks in CUnit lib
/// returns CU_get_error() in case of failure, else CUE_SUCCESS.
CU_ErrorCode CU_utils_try_add_test(CU_pSuite suite, void (*test_callback)(void), const char *msg);

bool CU_impl_assert_eq_u32(uint32_t actual, uint32_t expected, const char *actual_expr, const char *expected_expr, int line, const char *file);

#define CU_ASSERT_EQ_U32(actual, expected) { \
    CU_impl_assert_eq_u32((actual), (expected), #actual, #expected, __LINE__, __FILE__); \
}

#endif // header guard
