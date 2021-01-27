#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include "lib.h"

int init_suite(void);
int clean_suite(void);

/// in case of failure, CUnit registery must be cleaned to avoid leaks
#define CLEANUP_RETURN() do {\
    CU_cleanup_registry();\
    return CU_get_error();\
} while(0)

/// attempts to add suite to registry. reports failure otherwise and clears registry to prevent memory leaks in CUnit lib
/// returns CU_get_error() in case of failure, else CUE_SUCCESS.
CU_ErrorCode CUU_utils_try_add_suite(CU_pSuite *out, const char *suite_name) {
	*out = CU_add_suite(suite_name, init_suite, clean_suite);
	if (*out == NULL) CLEANUP_RETURN();
	return CUE_SUCCESS;
}

/// attempts to add a test to a suite. reports failure otherwise and clears registry to prevent memory leaks in CUnit lib
/// returns CU_get_error() in case of failure, else CUE_SUCCESS.
CU_ErrorCode CUU_utils_try_add_test(CU_pSuite suite, void (*test_callback)(void), const char *msg) {
	if (CU_add_test(suite, msg, test_callback) == NULL) CLEANUP_RETURN();
	return CUE_SUCCESS;
}

bool CUU_impl_assert_eq_u32(uint32_t actual, uint32_t expected, const char *actual_expr, const char *expected_expr, int line, const char *func_name, const char *file) {
    if (actual != expected) {
        char *buf = NULL;
        char msg[100];
        int size = snprintf(msg, sizeof(msg)/sizeof(char), "CUU_ASSERT_EQ_U32(%s, %s): %d != %d", actual_expr, expected_expr, actual, expected);

        // faled to guess sufficient space for message -- dynamically allocate string
        if (size < 0 || size >= sizeof(msg)/sizeof(char)) {
            buf = (char *)malloc(size+1); // +'\0'
            snprintf(buf, size, "CUU_ASSERT_EQ_U32(%s, %s): %d != %d", actual_expr, expected_expr, actual, expected);
        } else {
            buf = msg;
        }

        return CU_assertImplementation(((actual) == (expected)), line, (buf), file, __func__, false);
    } 
    return true;
}
