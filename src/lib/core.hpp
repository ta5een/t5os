#pragma once

#ifndef USING_LIB_GLOBALLY
#    define USING_LIB_GLOBALLY 1
#endif

#define LIB_MAKE_NONCOPYABLE(T)                                                \
  private:                                                                     \
    T(const T &) = delete;                                                     \
    T &operator=(const T &) = delete;

#define LIB_MAKE_NONMOVABLE(T)                                                 \
  private:                                                                     \
    T(T &&) = delete;                                                          \
    T &operator=(T &&) = delete;

#define __LIB_STRINGIFY(s) #s
#define LIB_STRINGIFY(s) __LIB_STRINGIFY(s)
