#include "../include/math_operations.h" // Include the header file for the functions to be tested
#include <check.h>
#include <stdlib.h>

Suite *math_operations_suite(void);

// Define test cases
START_TEST(
    test_addition_positive_integers) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
  // Test add function with two positive integers
  ck_assert_int_eq(add(3, 4), 7);
}

START_TEST(
    test_addition_positive_and_negative_integer) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
  // Test add function with one positive and one negative integer
  ck_assert_int_eq(add(3, -4), -1);
}

START_TEST(
    test_subtraction_positive_integers) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
  // Test subtract function with two positive integers
  ck_assert_int_eq(subtract(10, 4), 6);
}

START_TEST(
    test_subtraction_positive_and_negative_integer) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
  // Test subtract function with one positive and one negative integer
  ck_assert_int_eq(subtract(10, -4), 14);
}

START_TEST(
    test_subtraction_negative_result) // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
{
  // Test subtract function with result being negative
  ck_assert_int_eq(subtract(4, 10), -6);
}

// Create test suite
Suite *math_operations_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Math Operations");
  tc_core = tcase_create("Core");

  // Add test cases to the test suite
  tcase_add_test(tc_core, test_addition_positive_integers);
  tcase_add_test(tc_core, test_addition_positive_and_negative_integer);
  tcase_add_test(tc_core, test_subtraction_positive_integers);
  tcase_add_test(tc_core, test_subtraction_positive_and_negative_integer);
  tcase_add_test(tc_core, test_subtraction_negative_result);
  suite_add_tcase(s, tc_core);

  return s;
}

// Main function to run the tests
int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  // Create test suite
  s = math_operations_suite();

  // Create test runner
  sr = srunner_create(s);

  // Run the tests
  srunner_run_all(sr, CK_NORMAL);

  // Get number of failed tests
  number_failed = srunner_ntests_failed(sr);

  // Clean up
  srunner_free(sr);

  // Return status
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
