
/**
 * @file c_type.c
 * @brief Implementation of custom c_type (avoid including <ctype.h>).
 *
 */

#include "c_type.h"

static int is_space(int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}