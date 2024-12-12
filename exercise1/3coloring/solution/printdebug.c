/**
 * @file	printdebug.c
 * @author	Boris Dimov, 12220025
 * @brief	Implements 'printdebug.h'
 * @details	Contains implementations for functions defined in 'printdebug.h'.
 * @date	13-11-2023
 */
#include "printdebug.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief	Prints an error message to **stderr**.
 * @details	Prints an error message using the callee's process name (derived from first parameter), converting _errno_ into the respective text message.
 * @param	ctx Program context of the calling process.
 * @param	msg	Message to add before the error description.
 * @param	errnum Error number, copy of _errno_.
 */
void print_error(struct program_ctx* ctx, const char* msg, int errnum)
{
	fprintf(stderr, "%s: %s: %s\n", ctx->progname, msg, strerror(errnum));
}