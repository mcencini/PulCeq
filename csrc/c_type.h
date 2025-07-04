/**
 * @file c_type.h
 * @brief Public API for custom c_type (avoid including <ctype.h>).
 *
 */
#ifndef C_TYPE_H
#define C_TYPE_H

/**
 * @brief Check whether a character is a whitespace character.
 *
 * This custom implementation matches the standard definition of isspace(),
 * including space, horizontal tab, newline, vertical tab, form feed, and carriage return.
 *
 * @param c The character to check (typically as unsigned char or casted to int).
 * @return Non-zero if the character is a whitespace character; 0 otherwise.
 */
static int is_space(int c);

#endif /* C_TYPE_H */