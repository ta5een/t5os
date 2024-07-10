#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Computes the length of the provide array.
 *
 * A well-formed use of this macro is to provide an lvalue as the argument. It
 * is INCORRECT to provide a reference to an array as the argument, as this will
 * compute the size of the data being pointed to.
 *
 * Although not recommended, you may provide a string literal as the argument.
 * In this case, the computed length depends on the underlying encoding of the
 * string. For example, the length of a UTF-8 encoded string (where each element
 * is a char/uint8_t) represents the number of code points it contains, plus the
 * NULL character. Pay attention to this, as the majority of graphemes available
 * in the Unicode Standard does NOT fit inside one byte.
 */
#define LIBCORE_ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

/**
 * Resizes the provided string view with the computed details of the provided
 * NULL-terminated string.
 *
 * TODO: Write about how string length is computed.
 * TODO: Show example of how to use this macro.
 * FIXME: This macro currently accepts any array with elements that may be
 * casted to uint8_t. There should be some mechanism to prevent this misuse.
 */
#define LIBCORE_RESIZE_STRING_VIEW(identifier, string)                         \
    do                                                                         \
    {                                                                          \
        identifier.stride = sizeof(string[0]);                                 \
        identifier.length = LIBCORE_ARRAY_LENGTH(string);                      \
        identifier.data = (const uint8_t *)string;                             \
    } while (0)

/**
 * Represents an immutable view into a sequence of bytes.
 *
 * This data type facilitates passing a string along with its length as a
 * function argument. It clearly indicates that a string is expected,
 * not just a pointer to a single char, and it provides pre-computed details
 * of the string without needing iteration.
 *
 * To construct this data type, use the `LIBCORE_RESIZE_STRING_VIEW` macro. This
 * data type is relatively cheap to copy, and thus it is expected to be passed
 * by value.
 *
 * Internally, this data type is represented as a pointer to a contiguous
 * sequence of UTF-8 encoded bytes. The sequence is NOT guaranteed to end with a
 * NULL character, nor is it guaranteed to contain values suitable for a string
 * data type. It is the responsibility of both the provider and recipient to
 * ensure the data's validity.
 *
 * To support wide strings (e.g., UTF-16 and UTF-32), the `stride` property can
 * be used to determine the correct number of bytes per character when
 * iterating.
 *
 * TODO: Write about the expected lifetime of the underlying pointer.
 * TODO: Write about how this data type is limited to statically-sized strings.
 */
typedef struct string_view
{
    size_t stride;
    size_t length;
    const uint8_t *data;
} string_view_t;

// The C++ Core Guidelines state that types with a size of two or three words
// are "cheap to copy" (F.16). This assertion ensures that `string_view_t` is at
// most 3 words big.
static_assert((bool)(sizeof(string_view_t) <= (3 * sizeof(void *))));
