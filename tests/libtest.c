/*  $Id$
**
**  Some utility routines for writing tests.
**
**  Herein are a variety of utility routines for writing tests.  All
**  routines of the form ok*() take a test number and some number of
**  appropriate arguments, check to be sure the results match the expected
**  output using the arguments, and print out something appropriate for that
**  test number.  Other utility routines help in constructing more complex
**  tests.
**
**  Copyright 2006
**      Board of Trustees, Leland Stanford Jr. University
**  Copyright (c) 2004, 2005, 2006
**      by Internet Systems Consortium, Inc. ("ISC")
**  Copyright (c) 1991, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001,
**      2002, 2003 by The Internet Software Consortium and Rich Salz
**
**  This code is derived from software contributed to the Internet Software
**  Consortium by Rich Salz.
**
**  Permission to use, copy, modify, and distribute this software for any
**  purpose with or without fee is hereby granted, provided that the above
**  copyright notice and this permission notice appear in all copies.
**
**  THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
**  REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
**  MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY
**  SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
**  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
**  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
**  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <config.h>
#include <system.h>

#include <sys/time.h>
#include <sys/wait.h>

#include <tests/libtest.h>
#include <util/util.h>

/* A global buffer into which message_log_buffer stores error messages. */
char *errors = NULL;


/*
**  Initialize things.  Turns on line buffering on stdout and then prints out
**  the number of tests in the test suite.
*/
void
test_init(int count)
{
    if (setvbuf(stdout, NULL, _IOLBF, BUFSIZ) != 0)
        syswarn("cannot set stdout to line buffered");
    printf("%d\n", count);
}


/*
**  Takes a boolean success value and assumes the test passes if that value
**  is true and fails if that value is false.
*/
void
ok(int n, int success)
{
    printf("%sok %d\n", success ? "" : "not ", n);
}


/*
**  Takes an expected integer and a seen integer and assumes the test passes
**  if those two numbers match.
*/
void
ok_int(int n, int wanted, int seen)
{
    if (wanted == seen)
        printf("ok %d\n", n);
    else
        printf("not ok %d\n  wanted: %d\n    seen: %d\n", n, wanted, seen);
}


/*
**  Takes a string and what the string should be, and assumes the test
**  passes if those strings match (using strcmp).
*/
void
ok_string(int n, const char *wanted, const char *seen)
{
    if (wanted == NULL)
        wanted = "(null)";
    if (seen == NULL)
        seen = "(null)";
    if (strcmp(wanted, seen) != 0)
        printf("not ok %d\n  wanted: %s\n    seen: %s\n", n, wanted, seen);
    else
        printf("ok %d\n", n);
}


/*
**  Takes an expected integer and a seen integer and assumes the test passes
**  if those two numbers match.
*/
void
ok_double(int n, double wanted, double seen)
{
    if (wanted == seen)
        printf("ok %d\n", n);
    else
        printf("not ok %d\n  wanted: %g\n    seen: %g\n", n, wanted, seen);
}


/*
**  Skip a test.
*/
void
skip(int n, const char *reason)
{
    printf("ok %d # skip", n);
    if (reason != NULL)
        printf(" - %s", reason);
    putchar('\n');
}


/*
**  Report the same status on the next count tests.
*/
void
ok_block(int n, int count, int status)
{
    int i;

    for (i = 0; i < count; i++)
        ok(n++, status);
}


/*
**  Skip the next count tests.
*/
void
skip_block(int n, int count, const char *reason)
{
    int i;

    for (i = 0; i < count; i++)
        skip(n++, reason);
}


/*
**  An error handler that appends all errors to the errors global.  Used by
**  error_capture.
*/
static void
message_log_buffer(int len, const char *fmt, va_list args, int error UNUSED)
{
    char *message;

    message = xmalloc(len + 1);
    vsnprintf(message, len + 1, fmt, args);
    if (errors == NULL) {
        errors = concat(message, "\n", (char *) 0);
    } else {
        char *new_errors;

        new_errors = concat(errors, message, "\n", (char *) 0);
        free(errors);
        errors = new_errors;
    }
    free(message);
}


/*
**  Turn on the capturing of errors.  Errors will be stored in the global
**  errors variable where they can be checked by the test suite.  Capturing is
**  turned off with errors_uncapture.
*/
void
errors_capture(void)
{
    if (errors != NULL) {
        free(errors);
        errors = NULL;
    }
    message_handlers_warn(1, message_log_buffer);
    message_handlers_notice(1, message_log_buffer);
}


/*
**  Turn off the capturing of errors again.
*/
void
errors_uncapture(void)
{
    message_handlers_warn(1, message_log_stderr);
    message_handlers_notice(1, message_log_stdout);
}