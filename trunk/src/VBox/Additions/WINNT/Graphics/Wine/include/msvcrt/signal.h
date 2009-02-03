/*
 * Signal definitions
 *
 * Copyright 2005 Juan Lang
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

/*
 * Sun LGPL Disclaimer: For the avoidance of doubt, except that if any license choice
 * other than GPL or LGPL is available it will apply instead, Sun elects to use only
 * the Lesser General Public License version 2.1 (LGPLv2) at this time for any software where
 * a choice of LGPL license versions is made available with the language indicating
 * that LGPLv2 or any later version may be used, or where a choice of which version
 * of the LGPL is applied is otherwise unspecified.
 */
#ifndef _WINE_SIGNAL_H
#define _WINE_SIGNAL_H
#ifndef __WINE_USE_MSVCRT
#define __WINE_USE_MSVCRT
#endif

#define SIGINT   2
#define SIGILL   4
#define SIGFPE   8
#define SIGSEGV  11
#define SIGTERM  15
#define SIGBREAK 21
#define SIGABRT  22

#define NSIG     (SIGABRT + 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*__sighandler_t)(int);

#define SIG_DFL ((__sighandler_t)0)
#define SIG_IGN ((__sighandler_t)1)
#define SIG_ERR ((__sighandler_t)-1)

__sighandler_t signal(int sig, __sighandler_t func);
int raise(int sig);

#ifdef __cplusplus
}
#endif

#endif /* _WINE_SIGNAL_H */
