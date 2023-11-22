#pragma once

#include <cstddef>

namespace MM {
namespace Utils {
#define OFFSET_OF(class_, member) offsetof(class_, member)

#define MM_CAT_IMP(a, b) a##b

#define MM_CAT(a, b) MM_CAT_IMP(a, b)

#define MM_STR_CAT_IMP_IMP(str) #str

#define MM_STR_CAT_IMP(str1, str2) MM_STR_CAT_IMP_IMP(str1##str2)

#define MM_STR_CAT(str1, str2) MM_STR_CAT_IMP(str1, str2)

#define MM_STR_IMP(str) #str

#define MM_STR(str) MM_STR_IMP(str)

#define MM_CODE_LOCATION_IMP_IMP(line)                                 \
  (std::string("[File:") + __FILE__ + " || Function:" + __FUNCTION__ + \
   " || Line:" + #line + "]")

#define MM_CODE_LOCATION_IMP(line) MM_CODE_LOCATION_IMP_IMP(line)

#define MM_CODE_LOCATION MM_CODE_LOCATION_IMP(__LINE__)

#define MM_ADD_COUNTER_SUFFIX_IMP_IMP(name, counter) name##counter

#define MM_ADD_COUNTER_SUFFIX_IMP(name, counter) \
  MM_ADD_COUNTER_SUFFIX_IMP_IMP(name, counter)

#define MM_ADD_COUNTER_SUFFIX(name) MM_ADD_COUNTER_SUFFIX_IMP(name, __COUNTER__)

#define MM_Print(object) std::cout << (object) << std::endl

#define RUN_TIME_IMP(expression, describe, start, end)                       \
  std::chrono::system_clock::time_point start =                              \
      std::chrono::system_clock::now();                                      \
  expression std::chrono::system_clock::time_point end =                     \
      std::chrono::system_clock::now();                                      \
  std::cout << (describe)                                                    \
            << std::chrono::duration_cast<std::chrono::nanoseconds>((end) -  \
                                                                    (start)) \
                   .count()                                                  \
            << std::endl;

#define RUN_TIME(expression, describe)                             \
  RUN_TIME_IMP(expression, describe, MM_ADD_COUNTER_SUFFIX(start), \
               MM_ADD_COUNTER_SUFFIX(end))

#if defined(_MSC_VER)
#define likely(x)       (x)
#define unlikely(x)     (x)
#else
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#endif
}  // namespace Utils
}  // namespace MM
