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
