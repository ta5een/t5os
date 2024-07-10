#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Computes the length of a fixed-size array.
 *
 * A well-formed use of this macro is to provide an lvalue as the argument. It
 * is INCORRECT to provide a pointer or a reference as the argument, as this
 * will compute the size of the pointer. This will be statically verified.
 *
 * Although not recommended, you may provide a string literal as the argument.
 * In this case, the computed length depends on the underlying encoding of the
 * string. For example, the length of a UTF-8 encoded string (where each element
 * is a char/uint8_t) represents the number of code points it contains, plus the
 * `NUL` character. Pay attention to this, as the majority of graphemes
 * available in the Unicode Standard do NOT fit inside one byte.
 */
#define LIBCORE_ARRAY_LENGTH($array)                                           \
    ({                                                                         \
        static_assert(                                                         \
            !__builtin_types_compatible_p(                                     \
                typeof($array), typeof(&($array)[0])                           \
            ),                                                                 \
            "The provided argument is not a fixed-size array"                  \
        );                                                                     \
        (sizeof($array) / sizeof(($array)[0]));                                \
    })

#define __LIBCORE_STRING_VIEW_WITH_CHAR_TYPE($str, $chtype)                    \
    ({                                                                         \
        static_assert(                                                         \
            __builtin_types_compatible_p(                                      \
                typeof($str), typeof(const $chtype[])                          \
            ),                                                                 \
            "The provided argument is not a string literal"                    \
        );                                                                     \
        string_view_t view = {                                                 \
            .stride = sizeof(($str)[0]),                                       \
            .length = LIBCORE_ARRAY_LENGTH($str) - 1,                          \
            .data = (const uint8_t *)($str)                                    \
        };                                                                     \
        view;                                                                  \
    })

/**
 * Creates a new instance of string_view_t given a fixed-size string.
 */
#define LIBCORE_STRING_VIEW($str)                                              \
    __LIBCORE_STRING_VIEW_WITH_CHAR_TYPE($str, char)

/**
 * Creates a new instance of string_view_t given a fixed-size wide string.
 */
#define LIBCORE_WIDE_STRING_VIEW($str)                                         \
    __LIBCORE_STRING_VIEW_WITH_CHAR_TYPE($str, wchar_t)

/**
 * Represents an immutable view into a sequence of bytes.
 *
 * The purpose of this data type is to facilitate passing around fixed-size
 * strings along with its length in a cheap-to-copy manner. By using the
 * accompanying macros `LIBCORE_STRING_VIEW` and `LIBCORE_WIDE_STRING_VIEW`, the
 * length and stride of the string can be computed at compile time. As such,
 * this data type is only usable for fixed-size strings (i.e. string literals
 * and variables that hold string literals).
 *
 * Internally, this data type is represented as a pointer to a contiguous
 * sequence of UTF-8 encoded bytes. The calculated length is guaranteed to
 * represent the total number of bytes, even if there are multiple `NUL`
 * characters interspersed within it. However, the contents of the string is
 * NOT guaranteed to contain values suitable for a string data type (for
 * example, the string may content nonsense bytes that translate to nonsense
 * characters). It is the responsibility of both the provider and recipient
 * to ensure the contents are valid.
 *
 * To support wide strings (e.g., UTF-16 and UTF-32), the `stride` property
 * can be used to determine the correct number of bytes per code point.
 *
 * TODO: Write about the expected lifetime of the underlying pointer.
 */
typedef struct string_view
{
    /**
     * The minimum number of bytes per code point.
     *
     * - For UTF-8 encoded strings, this will equal to 1.
     * - For UTF-16 encoded strings, this will equal to 2.
     * - For UTF-32 encoded strings, this will equal to 4.
     */
    const size_t stride;
    /**
     * The total number of bytes that make up this string, NOT including the
     * `NUL` character.
     */
    const size_t length;
    /**
     * A read-only view of the bytes that make up this string.
     */
    const uint8_t *data;
} string_view_t;

// The C++ Core Guidelines state that types with a size of two or three words
// are "cheap to copy" (F.16). This assertion ensures that `string_view_t` is at
// most 3 words big.
static_assert((bool)(sizeof(string_view_t) <= (3 * sizeof(void *))));
