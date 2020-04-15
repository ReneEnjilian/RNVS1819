#ifndef TIMEVAL_MACROS_H_
#define TIMEVAL_MACROS_H_

#include <time.h>

#ifndef timerisset
#define timerisset(tvp)        ((tvp)->tv_sec || (tvp)->tv_usec)
#endif /* timerisset */

#ifndef timerclear
#define timerclear(tvp)        ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#endif /* timerclear */

#ifndef timercmp
#define timercmp(a, b, CMP)                           \
  (((a)->tv_sec == (b)->tv_sec) ?                     \
   ((a)->tv_usec CMP (b)->tv_usec) :                  \
   ((a)->tv_sec CMP (b)->tv_sec))
#endif /* timercmp */

#ifndef timeradd
#define timeradd(a, b, result)                        \
  do {                                                \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;     \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;  \
    if ((result)->tv_usec >= 1000000)                 \
      {                                               \
        ++(result)->tv_sec;                           \
        (result)->tv_usec -= 1000000;                 \
      }                                               \
  } while (0)
#endif /* timeradd */

#ifndef timersub
#define timersub(a, b, result)                        \
  do {                                                \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;     \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;  \
    if ((result)->tv_usec < 0) {                      \
      --(result)->tv_sec;                             \
      (result)->tv_usec += 1000000;                   \
    }                                                 \
  } while (0)
#endif /* timersub */

#endif /* TIMEVAL_MACROS_H_ */
