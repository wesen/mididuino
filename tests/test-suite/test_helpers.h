/*
 * MidiCtrl - Unit test helper functions
 *
 * (c) 2010 - Manuel Odendahl - wesen@ruinwesen.com
 */

#ifndef UNIT_TEST_HELPERS_H__
#define UNIT_TEST_HELPERS_H__

#define M_CHECK_EQUAL(val1, val2) \
  if ((val1) != (val2)) { \
    printf("failure at %d in %s, %x should be %x\n", __LINE__, __FILE__, val1, val2); \
    return false; \
  }

#define M_CHECK_EQUAL_L(val1, val2) \
  if ((val1) != (val2)) { \
    printf("failure at %d in %s, %llx should be %llx\n", __LINE__, __FILE__, val1, val2); \
    return false; \
  }


#endif /* UNIT_TEST_HELPERS_H__ */
