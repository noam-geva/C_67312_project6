#include <iostream>
#include <vector>
#include <algorithm>
#include "vl_vector.h"
#include "helpers.h"

#ifdef BONUS_IMPLEMENTED
# include "vl_string.h"
#endif

#define PRESUBMISSION_ASSERT(testCase) __ASSERT_TEST("Presubmit", testCase)

//-------------------------------------------------------
// Tests
//-------------------------------------------------------

int TestCreateVector() {
  VLVector<int, STATIC_CAP> int_vec{};
  VLVector<std::string, STATIC_CAP> string_vec{};
  VLVector<double, 4> double_vec{};

  ASSERT_VLA_PROPERTIES(int_vec, false, STATIC_CAP, 0)
  ASSERT_VLA_PROPERTIES(string_vec, false, STATIC_CAP, 0)
  ASSERT_VLA_PROPERTIES(double_vec, false, 4, 0)

  auto dynamic_allocated_vec = new VLVector<int, STATIC_CAP>();
  delete dynamic_allocated_vec;

  return 1;
}

int TestPushBack() {
  VLVector<int, STATIC_CAP> vec;
  ASSERT_VLA_PROPERTIES(vec, false, STATIC_CAP, 0)
  vec.PushBack(1);
  ASSERT_VLA_PROPERTIES(vec, false, STATIC_CAP, 1)

  return 1;
}

int TestSize() {
  VLVector<int, STATIC_CAP> vec;
  ASSERT_SIZE(vec, 0)

  for (int i = 0; i < 5; ++i) {
    vec.PushBack(i);
  }

  ASSERT_SIZE(vec, 5)

  for (int i = 0; i < 15; ++i) {
    vec.PushBack(i);
  }

  ASSERT_SIZE(vec, 20)
  return 1;
}

int TestCapacity() {
  VLVector<int, STATIC_CAP> vec;
  ASSERT_CAPACITY(vec, STATIC_CAP)

  // Inserting [0, 15) (15 items)
  for (int i = 0; i < 15; ++i) {
    vec.PushBack(i);
  }

  ASSERT_CAPACITY(vec, STATIC_CAP)

  // Inserting [15, 30) (15 items)
  for (int i = 15; i < 30; ++i) {
    vec.PushBack(i);
  }

  ASSERT_CAPACITY(vec, 39)

  // Inserting [30, 39) (15 items)
  for (int i = 30; i < 39; ++i) {
    vec.PushBack(i);
  }

  ASSERT_CAPACITY(vec, 39)
  return 1;
}

int TestEmpty() {
  VLVector<int, STATIC_CAP> vec;
  ASSERT_TRUE(vec.Empty())
  vec.PushBack(1);
  RETURN_ASSERT_TRUE(!vec.Empty())
}

int TestClear() {
  VLVector<int, STATIC_CAP> vec;

  vec.PushBack(1);
  vec.Clear();

  ASSERT_SIZE(vec, 0)
  ASSERT_CAPACITY(vec, STATIC_CAP)

  return 1;
}

int TestPopBack() {
  VLVector<int, STATIC_CAP> vec;
  if (!vec.Empty()) {
    return 0;
  }

  vec.PushBack(1);
  ASSERT_SIZE(vec, 1)
  ASSERT_CAPACITY(vec, STATIC_CAP)

  vec.PopBack();
  ASSERT_SIZE(vec, 0)
  ASSERT_CAPACITY(vec, STATIC_CAP)

  return 1;
}

int TestGetElement() {
  VLVector<int, STATIC_CAP> vec;
  vec.PushBack(1);

  ASSERT_TRUE(vec.At(0) == 1)  // using VLVector::At
  RETURN_ASSERT_TRUE(vec[0] == 1) // using VLVector::operator[]
}

int TestData() {
  VLVector<int, STATIC_CAP> vec;
  UNUSED(vec.Data());
  return 1;
}

int TestComparison() {
  VLVector<int, STATIC_CAP> vec1{};
  VLVector<int, STATIC_CAP> vec2{};

  RETURN_ASSERT_TRUE(vec1 == vec2)
}

int TestAssignment() {
  VLVector<int, STATIC_CAP> vec1{};
  vec1.PushBack(1);

  VLVector<int, STATIC_CAP> vec2 = vec1;
  UNUSED(vec2);
  return 1;
}

int TestInsertSingleValue() {
  VLVector<int, STATIC_CAP> vec;
  vec.PushBack(0);
  vec.PushBack(2);
  auto it = vec.begin();
  ++it;
  vec.Insert(it, 1);

  RETURN_ASSERT_TRUE(vec[1] == 1)
}

int TestInsertIterator() {
  VLVector<int, STATIC_CAP> vec;
  vec.PushBack(0);

  std::vector<int> values{1, 2, 3, 4};
  auto it = vec.begin();
  ++it;
  vec.Insert(it, values.begin(), values.end());
  RETURN_ASSERT_TRUE(vec.Size() == 5)
}

int TestResize() {
  VLVector<int> vec;
  ASSERT_VLA_PROPERTIES(vec, false, STATIC_CAP, 0)

  // Add 1 item
  vec.PushBack(1);
  ASSERT_VLA_PROPERTIES(vec, false, STATIC_CAP, 1)

  // Add 16 item
  for (int i = 2; i <= 17; ++i) {
    vec.PushBack(i);
  }
  ASSERT_VLA_PROPERTIES(vec, true, /* Capacity: */ 25, /* Size: */17)

  // Add another 12 items
  std::vector<int> entries(13, 1);
  vec.Insert(vec.begin(), entries.cbegin(), entries.cend());
  ASSERT_VLA_PROPERTIES(vec, true, /* Capacity: */ 45, /* Size: */30)

  // Remove those 12 items (note that the Capacity won't be decreased).
  for (int i = 18; i <= 30; ++i) {
    vec.Erase(vec.end() - 1);
  }
  ASSERT_VLA_PROPERTIES(vec, true, /* Capacity: */ 45, /* Size: */17)

  // Clear
  vec.Clear();
  ASSERT_VLA_PROPERTIES(vec, false, /* Capacity: */ STATIC_CAP, /* Size: */0)

  // Adding 17 items (note that we have a Capacity of 25 instead of 39, as we did Clear, which returned
  // us to static memory and free'd the dynamic memory).
  for (int i = 1; i <= 17; ++i) {
    vec.PushBack(i);
  }
  ASSERT_VLA_PROPERTIES(vec, true, /* Capacity: */ 25, /* Size: */17)

  return 1;
}


//-------------------------------------------------------
//  Bonus
//-------------------------------------------------------
#ifdef BONUS_IMPLEMENTED

int TestVLStringCreateString() {
  VLString<> str1;
  ASSERT_TRUE(str1.Size() == 0 && str1.Capacity() == STATIC_CAP)

  VLString<> str2("Hello");
  ASSERT_TRUE(str2.Size() == 5 && str2.Capacity() == STATIC_CAP)

  VLString<> str2_copy(str2);
  ASSERT_TRUE(str2_copy.Size() == 5 && str2_copy.Capacity() == STATIC_CAP)

  return 1;
}

int TestVLStringConcat() {
  VLString<> str;
  ASSERT_TRUE(str.Size() == 0 && str.Capacity() == STATIC_CAP)

  str += "Hello";
  ASSERT_TRUE(str.Size() == 5 && str.Capacity() == STATIC_CAP)

  str += ", World from C++";
  ASSERT_TRUE(str.Size() == 21 && str.Capacity() == 33)

  str += '!';
  ASSERT_TRUE(str.Size() == 22 && str.Capacity() == 33)

  return 1;
}

#endif

//-------------------------------------------------------
//  The main entry point
//-------------------------------------------------------
int RunPresubmitChecks() {
  PRESUBMISSION_ASSERT(TestCreateVector)
  PRESUBMISSION_ASSERT(TestPushBack)
  PRESUBMISSION_ASSERT(TestSize)
  PRESUBMISSION_ASSERT(TestCapacity)
  PRESUBMISSION_ASSERT(TestEmpty)
  PRESUBMISSION_ASSERT(TestClear)
  PRESUBMISSION_ASSERT(TestPopBack)
  PRESUBMISSION_ASSERT(TestGetElement)
  PRESUBMISSION_ASSERT(TestData)
  PRESUBMISSION_ASSERT(TestComparison)
  PRESUBMISSION_ASSERT(TestAssignment)
  PRESUBMISSION_ASSERT(TestInsertSingleValue)
  PRESUBMISSION_ASSERT(TestInsertIterator)
  PRESUBMISSION_ASSERT(TestResize)

#ifdef BONUS_IMPLEMENTED
  PRESUBMISSION_ASSERT(TestVLStringCreateString)
  PRESUBMISSION_ASSERT(TestVLStringConcat)
#endif

  return 1;
}

int main() {
  return RunPresubmitChecks() ? EXIT_SUCCESS : EXIT_FAILURE;
}