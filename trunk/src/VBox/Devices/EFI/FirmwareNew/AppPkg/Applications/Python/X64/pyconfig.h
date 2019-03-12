/** @file
    Manually generated Python Configuration file for EDK II.

    Copyright (c) 2015, Daryl McDaniel. All rights reserved.<BR>
    Copyright (c) 2011 - 2016, Intel Corporation. All rights reserved.<BR>
    This program and the accompanying materials are licensed and made available under
    the terms and conditions of the BSD License that accompanies this distribution.
    The full text of the license may be found at
    http://opensource.org/licenses/bsd-license.

    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/
#ifndef Py_PYCONFIG_H
#define Py_PYCONFIG_H

#include  <Uefi.h>

#define PLATFORM    "uefi"

/* Define if building universal (internal helper macro) */
#undef AC_APPLE_UNIVERSAL_BUILD

/* Define for AIX if your compiler is a genuine IBM xlC/xlC_r and you want
   support for AIX C++ shared extension modules. */
#undef AIX_GENUINE_CPLUSPLUS

/* Define this if you have AtheOS threads. */
#undef ATHEOS_THREADS

/* Define this if you have BeOS threads. */
#undef BEOS_THREADS

/* Define if you have the Mach cthreads package */
#undef C_THREADS

/* Define if C doubles are 64-bit IEEE 754 binary format, stored in ARM
   mixed-endian order (byte order 45670123) */
#undef DOUBLE_IS_ARM_MIXED_ENDIAN_IEEE754

/* Define if C doubles are 64-bit IEEE 754 binary format, stored with the most
   significant byte first */
#undef DOUBLE_IS_BIG_ENDIAN_IEEE754

/* Define if C doubles are 64-bit IEEE 754 binary format, stored with the
   least significant byte first */
#define DOUBLE_IS_LITTLE_ENDIAN_IEEE754

/* Define if --enable-ipv6 is specified */
#undef ENABLE_IPV6

/* Define if flock needs to be linked with bsd library. */
#undef FLOCK_NEEDS_LIBBSD

/* Define if getpgrp() must be called as getpgrp(0). */
#undef GETPGRP_HAVE_ARG

/* Define if gettimeofday() does not have second (timezone) argument This is
   the case on Motorola V4 (R40V4.2) */
#undef GETTIMEOFDAY_NO_TZ

/* Define to 1 if you have the 'acosh' function. */
#undef HAVE_ACOSH

/* struct addrinfo (netdb.h) */
#undef HAVE_ADDRINFO

/* Define to 1 if you have the 'alarm' function. */
#undef HAVE_ALARM

/* Define to 1 if you have the <alloca.h> header file. */
#undef HAVE_ALLOCA_H

/* Define this if your time.h defines altzone. */
#undef HAVE_ALTZONE

/* Define to 1 if you have the 'asinh' function. */
#undef HAVE_ASINH

/* Define to 1 if you have the <asm/types.h> header file. */
#undef HAVE_ASM_TYPES_H

/* Define to 1 if you have the 'atanh' function. */
#undef HAVE_ATANH

/* Define if GCC supports __attribute__((format(PyArg_ParseTuple, 2, 3))) */
#undef HAVE_ATTRIBUTE_FORMAT_PARSETUPLE

/* Define to 1 if you have the 'bind_textdomain_codeset' function. */
#undef HAVE_BIND_TEXTDOMAIN_CODESET

/* Define to 1 if you have the <bluetooth/bluetooth.h> header file. */
#undef HAVE_BLUETOOTH_BLUETOOTH_H

/* Define to 1 if you have the <bluetooth.h> header file. */
#undef HAVE_BLUETOOTH_H

/* Define if nice() returns success/failure instead of the new priority. */
#undef HAVE_BROKEN_NICE

/* Define if the system reports an invalid PIPE_BUF value. */
#undef HAVE_BROKEN_PIPE_BUF

/* Define if poll() sets errno on invalid file descriptors. */
#undef HAVE_BROKEN_POLL

/* Define if the Posix semaphores do not work on your system */
#define HAVE_BROKEN_POSIX_SEMAPHORES  1

/* Define if pthread_sigmask() does not work on your system. */
#define HAVE_BROKEN_PTHREAD_SIGMASK   1

/* define to 1 if your sem_getvalue is broken. */
#define HAVE_BROKEN_SEM_GETVALUE      1

/* Define if 'unsetenv' does not return an int. */
#undef HAVE_BROKEN_UNSETENV

/* Define this if you have the type _Bool. */
#define HAVE_C99_BOOL                 1

/* Define to 1 if you have the 'chflags' function. */
#undef HAVE_CHFLAGS

/* Define to 1 if you have the 'chown' function. */
#undef HAVE_CHOWN

/* Define if you have the 'chroot' function. */
#undef HAVE_CHROOT

/* Define to 1 if you have the 'clock' function. */
#define HAVE_CLOCK                    1

/* Define to 1 if you have the 'confstr' function. */
#undef HAVE_CONFSTR

/* Define to 1 if you have the <conio.h> header file. */
#undef HAVE_CONIO_H

/* Define to 1 if you have the 'copysign' function. */
#undef HAVE_COPYSIGN

/* Define to 1 if you have the 'ctermid' function. */
#undef HAVE_CTERMID

/* Define if you have the 'ctermid_r' function. */
#undef HAVE_CTERMID_R

/* Define to 1 if you have the <curses.h> header file. */
#undef HAVE_CURSES_H

/* Define if you have the 'is_term_resized' function. */
#undef HAVE_CURSES_IS_TERM_RESIZED

/* Define if you have the 'resizeterm' function. */
#undef HAVE_CURSES_RESIZETERM

/* Define if you have the 'resize_term' function. */
#undef HAVE_CURSES_RESIZE_TERM

/* Define to 1 if you have the declaration of 'isfinite', and to 0 if you
   don't. */
#define HAVE_DECL_ISFINITE    0

/* Define to 1 if you have the declaration of 'isinf', and to 0 if you don't.
   */
#define HAVE_DECL_ISINF       1

/* Define to 1 if you have the declaration of 'isnan', and to 0 if you don't.
   */
#define HAVE_DECL_ISNAN       1

/* Define to 1 if you have the declaration of 'tzname', and to 0 if you don't.
   */
#define HAVE_DECL_TZNAME      0

/* Define to 1 if you have the device macros. */
#undef HAVE_DEVICE_MACROS

/* Define to 1 if you have the /dev/ptc device file. */
#undef HAVE_DEV_PTC

/* Define to 1 if you have the /dev/ptmx device file. */
#undef HAVE_DEV_PTMX

/* Define to 1 if you have the <direct.h> header file. */
#undef HAVE_DIRECT_H

/* Define to 1 if you have the <dirent.h> header file, and it defines 'DIR'.
   */
#define HAVE_DIRENT_H   1

/* Define to 1 if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define to 1 if you have the 'dlopen' function. */
#undef HAVE_DLOPEN

/* Define to 1 if you have the 'dup2' function. */
#define HAVE_DUP2       1

/* Defined when any dynamic module loading is enabled. */
#undef HAVE_DYNAMIC_LOADING

/* Define if you have the 'epoll' functions. */
#undef HAVE_EPOLL

/* Define to 1 if you have the 'erf' function. */
#undef HAVE_ERF

/* Define to 1 if you have the 'erfc' function. */
#undef HAVE_ERFC

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H    1

/* Define to 1 if you have the 'execv' function. */
#undef HAVE_EXECV

/* Define to 1 if you have the 'expm1' function. */
#undef HAVE_EXPM1

/* Define if you have the 'fchdir' function. */
#undef HAVE_FCHDIR

/* Define to 1 if you have the 'fchmod' function. */
#undef HAVE_FCHMOD

/* Define to 1 if you have the 'fchown' function. */
#undef HAVE_FCHOWN

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H    1

/* Define if you have the 'fdatasync' function. */
#undef HAVE_FDATASYNC

/* Define to 1 if you have the 'finite' function. */
#define HAVE_FINITE     1

/* Define to 1 if you have the 'flock' function. */
#undef HAVE_FLOCK

/* Define to 1 if you have the 'fork' function. */
#undef HAVE_FORK

/* Define to 1 if you have the 'forkpty' function. */
#undef HAVE_FORKPTY

/* Define to 1 if you have the 'fpathconf' function. */
#undef HAVE_FPATHCONF

/* Define to 1 if you have the 'fseek64' function. */
#undef HAVE_FSEEK64

/* Define to 1 if you have the 'fseeko' function. */
#define HAVE_FSEEKO     1

/* Define to 1 if you have the 'fstatvfs' function. */
#undef HAVE_FSTATVFS

/* Define if you have the 'fsync' function. */
#undef HAVE_FSYNC

/* Define to 1 if you have the 'ftell64' function. */
#undef HAVE_FTELL64

/* Define to 1 if you have the 'ftello' function. */
#define HAVE_FTELLO     1

/* Define to 1 if you have the 'ftime' function. */
#undef HAVE_FTIME

/* Define to 1 if you have the 'ftruncate' function. */
#undef HAVE_FTRUNCATE

/* Define to 1 if you have the 'gai_strerror' function. */
#undef HAVE_GAI_STRERROR

/* Define to 1 if you have the 'gamma' function. */
#undef HAVE_GAMMA

/* Define if we can use gcc inline assembler to get and set x87 control word
*/
#if defined(__GNUC__)
  #define HAVE_GCC_ASM_FOR_X87  1
#else
  #undef HAVE_GCC_ASM_FOR_X87
#endif

/* Define if you have the getaddrinfo function. */
//#undef HAVE_GETADDRINFO
#define HAVE_GETADDRINFO  1

/* Define to 1 if you have the 'getcwd' function. */
#define HAVE_GETCWD   1

/* Define this if you have flockfile(), getc_unlocked(), and funlockfile() */
#undef HAVE_GETC_UNLOCKED

/* Define to 1 if you have the 'getentropy' function. */
#undef HAVE_GETENTROPY

/* Define to 1 if you have the 'getgroups' function. */
#undef HAVE_GETGROUPS

/* Define to 1 if you have the 'gethostbyname' function. */
//#undef HAVE_GETHOSTBYNAME
#define HAVE_GETHOSTBYNAME  1

/* Define this if you have some version of gethostbyname_r() */
#undef HAVE_GETHOSTBYNAME_R

/* Define this if you have the 3-arg version of gethostbyname_r(). */
#undef HAVE_GETHOSTBYNAME_R_3_ARG

/* Define this if you have the 5-arg version of gethostbyname_r(). */
#undef HAVE_GETHOSTBYNAME_R_5_ARG

/* Define this if you have the 6-arg version of gethostbyname_r(). */
#undef HAVE_GETHOSTBYNAME_R_6_ARG

/* Define to 1 if you have the 'getitimer' function. */
#undef HAVE_GETITIMER

/* Define to 1 if you have the 'getloadavg' function. */
#undef HAVE_GETLOADAVG

/* Define to 1 if you have the 'getlogin' function. */
#undef HAVE_GETLOGIN

/* Define to 1 if you have the 'getnameinfo' function. */
//#undef HAVE_GETNAMEINFO
#define HAVE_GETNAMEINFO 1

/* Define if you have the 'getpagesize' function. */
#undef HAVE_GETPAGESIZE

/* Define to 1 if you have the 'getpeername' function. */
#define HAVE_GETPEERNAME  1

/* Define to 1 if you have the 'getpgid' function. */
#undef HAVE_GETPGID

/* Define to 1 if you have the 'getpgrp' function. */
#undef HAVE_GETPGRP

/* Define to 1 if you have the 'getpid' function. */
#undef HAVE_GETPID

/* Define to 1 if you have the 'getpriority' function. */
#undef HAVE_GETPRIORITY

/* Define to 1 if you have the 'getpwent' function. */
#undef HAVE_GETPWENT

/* Define to 1 if you have the 'getresgid' function. */
#undef HAVE_GETRESGID

/* Define to 1 if you have the 'getresuid' function. */
#undef HAVE_GETRESUID

/* Define to 1 if you have the 'getsid' function. */
#undef HAVE_GETSID

/* Define to 1 if you have the 'getspent' function. */
#undef HAVE_GETSPENT

/* Define to 1 if you have the 'getspnam' function. */
#undef HAVE_GETSPNAM

/* Define to 1 if you have the 'gettimeofday' function. */
#undef HAVE_GETTIMEOFDAY

/* Define to 1 if you have the 'getwd' function. */
#undef HAVE_GETWD

/* Define to 1 if you have the <grp.h> header file. */
#undef HAVE_GRP_H

/* Define if you have the 'hstrerror' function. */
#undef HAVE_HSTRERROR

/* Define to 1 if you have the 'hypot' function. */
#undef HAVE_HYPOT

/* Define to 1 if you have the <ieeefp.h> header file. */
#undef HAVE_IEEEFP_H

/* Define if you have the 'inet_aton' function. */
#define HAVE_INET_ATON    1

/* Define if you have the 'inet_pton' function. */
#define HAVE_INET_PTON    1

/* Define to 1 if you have the 'initgroups' function. */
#undef HAVE_INITGROUPS

/* Define if your compiler provides int32_t. */
#undef HAVE_INT32_T

/* Define if your compiler provides int64_t. */
#undef HAVE_INT64_T

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H   1

/* Define to 1 if you have the <io.h> header file. */
#undef HAVE_IO_H

/* Define to 1 if you have the 'kill' function. */
#undef HAVE_KILL

/* Define to 1 if you have the 'killpg' function. */
#undef HAVE_KILLPG

/* Define if you have the 'kqueue' functions. */
#undef HAVE_KQUEUE

/* Define to 1 if you have the <langinfo.h> header file. */
#undef HAVE_LANGINFO_H   /* non-functional in EFI. */

/* Defined to enable large file support when an off_t is bigger than a long
   and long long is available and at least as big as an off_t. You may need to
   add some flags for configuration and compilation to enable this mode. (For
   Solaris and Linux, the necessary defines are already defined.) */
#undef HAVE_LARGEFILE_SUPPORT

/* Define to 1 if you have the 'lchflags' function. */
#undef HAVE_LCHFLAGS

/* Define to 1 if you have the 'lchmod' function. */
#undef HAVE_LCHMOD

/* Define to 1 if you have the 'lchown' function. */
#undef HAVE_LCHOWN

/* Define to 1 if you have the 'lgamma' function. */
#undef HAVE_LGAMMA

/* Define to 1 if you have the 'dl' library (-ldl). */
#undef HAVE_LIBDL

/* Define to 1 if you have the 'dld' library (-ldld). */
#undef HAVE_LIBDLD

/* Define to 1 if you have the 'ieee' library (-lieee). */
#undef HAVE_LIBIEEE

/* Define to 1 if you have the <libintl.h> header file. */
#undef HAVE_LIBINTL_H

/* Define if you have the readline library (-lreadline). */
#undef HAVE_LIBREADLINE

/* Define to 1 if you have the 'resolv' library (-lresolv). */
#undef HAVE_LIBRESOLV

/* Define to 1 if you have the <libutil.h> header file. */
#undef HAVE_LIBUTIL_H

/* Define if you have the 'link' function. */
#undef HAVE_LINK

/* Define to 1 if you have the <linux/netlink.h> header file. */
#undef HAVE_LINUX_NETLINK_H

/* Define to 1 if you have the <linux/tipc.h> header file. */
#undef HAVE_LINUX_TIPC_H

/* Define to 1 if you have the 'log1p' function. */
#undef HAVE_LOG1P

/* Define this if you have the type long double. */
#undef HAVE_LONG_DOUBLE

/* Define this if you have the type long long. */
#define HAVE_LONG_LONG  1

/* Define to 1 if you have the 'lstat' function. */
#define HAVE_LSTAT      1

/* Define this if you have the makedev macro. */
#undef HAVE_MAKEDEV

/* Define to 1 if you have the 'memmove' function. */
#define HAVE_MEMMOVE    1

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the 'mkfifo' function. */
#undef HAVE_MKFIFO

/* Define to 1 if you have the 'mknod' function. */
#undef HAVE_MKNOD

/* Define to 1 if you have the 'mktime' function. */
#define HAVE_MKTIME     1

/* Define to 1 if you have the 'mmap' function. */
#undef HAVE_MMAP

/* Define to 1 if you have the 'mremap' function. */
#undef HAVE_MREMAP

/* Define to 1 if you have the <ncurses.h> header file. */
#undef HAVE_NCURSES_H

/* Define to 1 if you have the <ndir.h> header file, and it defines 'DIR'. */
#undef HAVE_NDIR_H

/* Define to 1 if you have the <netpacket/packet.h> header file. */
#undef HAVE_NETPACKET_PACKET_H

/* Define to 1 if you have the 'nice' function. */
#undef HAVE_NICE

/* Define to 1 if you have the 'openpty' function. */
#undef HAVE_OPENPTY

/* Define if compiling using MacOS X 10.5 SDK or later. */
#undef HAVE_OSX105_SDK

/* Define to 1 if you have the 'pathconf' function. */
#undef HAVE_PATHCONF

/* Define to 1 if you have the 'pause' function. */
#undef HAVE_PAUSE

/* Define to 1 if you have the 'plock' function. */
#undef HAVE_PLOCK

/* Define to 1 if you have the 'poll' function. */
#define HAVE_POLL         1

/* Define to 1 if you have the <poll.h> header file. */
#undef HAVE_POLL_H

/* Define to 1 if you have the <process.h> header file. */
#undef HAVE_PROCESS_H

/* Define if your compiler supports function prototype */
#define HAVE_PROTOTYPES   1

/* Define if you have GNU PTH threads. */
#undef HAVE_PTH

/* Define to 1 if you have the 'pthread_atfork' function. */
#undef HAVE_PTHREAD_ATFORK

/* Defined for Solaris 2.6 bug in pthread header. */
#undef HAVE_PTHREAD_DESTRUCTOR

/* Define to 1 if you have the <pthread.h> header file. */
#undef HAVE_PTHREAD_H

/* Define to 1 if you have the 'pthread_init' function. */
#undef HAVE_PTHREAD_INIT

/* Define to 1 if you have the 'pthread_sigmask' function. */
#undef HAVE_PTHREAD_SIGMASK

/* Define to 1 if you have the <pty.h> header file. */
#undef HAVE_PTY_H

/* Define to 1 if you have the 'putenv' function. */
#undef HAVE_PUTENV

/* Define if the libcrypto has RAND_egd */
#undef HAVE_RAND_EGD

/* Define to 1 if you have the 'readlink' function. */
#undef HAVE_READLINK

/* Define to 1 if you have the 'realpath' function. */
#define HAVE_REALPATH   1

/* Define if you have readline 2.1 */
#undef HAVE_RL_CALLBACK

/* Define if you can turn off readline's signal handling. */
#undef HAVE_RL_CATCH_SIGNAL

/* Define if you have readline 2.2 */
#undef HAVE_RL_COMPLETION_APPEND_CHARACTER

/* Define if you have readline 4.0 */
#undef HAVE_RL_COMPLETION_DISPLAY_MATCHES_HOOK

/* Define if you have readline 4.2 */
#undef HAVE_RL_COMPLETION_MATCHES

/* Define if you have rl_completion_suppress_append */
#undef HAVE_RL_COMPLETION_SUPPRESS_APPEND

/* Define if you have readline 4.0 */
#undef HAVE_RL_PRE_INPUT_HOOK

/* Define to 1 if you have the 'round' function. */
#undef HAVE_ROUND

/* Define to 1 if you have the 'select' function. */
#define HAVE_SELECT       1

/* Define to 1 if you have the 'sem_getvalue' function. */
#undef HAVE_SEM_GETVALUE

/* Define to 1 if you have the 'sem_open' function. */
#undef HAVE_SEM_OPEN

/* Define to 1 if you have the 'sem_timedwait' function. */
#undef HAVE_SEM_TIMEDWAIT

/* Define to 1 if you have the 'sem_unlink' function. */
#undef HAVE_SEM_UNLINK

/* Define to 1 if you have the 'setegid' function. */
#undef HAVE_SETEGID

/* Define to 1 if you have the 'seteuid' function. */
#undef HAVE_SETEUID

/* Define to 1 if you have the 'setgid' function. */
#undef HAVE_SETGID

/* Define if you have the 'setgroups' function. */
#undef HAVE_SETGROUPS

/* Define to 1 if you have the 'setitimer' function. */
#undef HAVE_SETITIMER

/* Define to 1 if you have the 'setlocale' function. */
#define HAVE_SETLOCALE    1

/* Define to 1 if you have the 'setpgid' function. */
#undef HAVE_SETPGID

/* Define to 1 if you have the 'setpgrp' function. */
#undef HAVE_SETPGRP

/* Define to 1 if you have the 'setregid' function. */
#undef HAVE_SETREGID

/* Define to 1 if you have the 'setresgid' function. */
#undef HAVE_SETRESGID

/* Define to 1 if you have the 'setresuid' function. */
#undef HAVE_SETRESUID

/* Define to 1 if you have the 'setreuid' function. */
#undef HAVE_SETREUID

/* Define to 1 if you have the 'setsid' function. */
#undef HAVE_SETSID

/* Define to 1 if you have the 'setuid' function. */
#undef HAVE_SETUID

/* Define to 1 if you have the 'setvbuf' function. */
#define HAVE_SETVBUF    1

/* Define to 1 if you have the <shadow.h> header file. */
#undef HAVE_SHADOW_H

/* Define to 1 if you have the 'sigaction' function. */
#undef HAVE_SIGACTION

/* Define to 1 if you have the 'siginterrupt' function. */
#undef HAVE_SIGINTERRUPT

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H   1

/* Define to 1 if you have the 'sigrelse' function. */
#undef HAVE_SIGRELSE

/* Define to 1 if you have the 'snprintf' function. */
#define HAVE_SNPRINTF   1

/* Define if sockaddr has sa_len member */
#undef HAVE_SOCKADDR_SA_LEN

/* struct sockaddr_storage (sys/socket.h) */
#undef HAVE_SOCKADDR_STORAGE

/* Define if you have the 'socketpair' function. */
#undef HAVE_SOCKETPAIR

/* Define to 1 if you have the <spawn.h> header file. */
#undef HAVE_SPAWN_H

/* Define if your compiler provides ssize_t */
#define HAVE_SSIZE_T    1

/* Define to 1 if you have the 'statvfs' function. */
#undef HAVE_STATVFS

/* Define if you have struct stat.st_mtim.tv_nsec */
#undef HAVE_STAT_TV_NSEC

/* Define if you have struct stat.st_mtimensec */
#undef HAVE_STAT_TV_NSEC2

/* Define if your compiler supports variable length function prototypes (e.g.
   void fprintf(FILE *, char *, ...);) *and* <stdarg.h> */
#define HAVE_STDARG_PROTOTYPES          1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H   1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H   1

/* Define to 1 if you have the 'strdup' function. */
#define HAVE_STRDUP     1

/* Define to 1 if you have the 'strftime' function. */
#define HAVE_STRFTIME   1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H   1

/* Define to 1 if you have the <stropts.h> header file. */
#undef HAVE_STROPTS_H

/* Define to 1 if 'st_birthtime' is a member of 'struct stat'. */
#define HAVE_STRUCT_STAT_ST_BIRTHTIME   1

/* Define to 1 if 'st_blksize' is a member of 'struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE     1

/* Define to 1 if 'st_blocks' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_BLOCKS

/* Define to 1 if 'st_flags' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_FLAGS

/* Define to 1 if 'st_gen' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_GEN

/* Define to 1 if 'st_rdev' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_RDEV

/* Define to 1 if 'st_dev' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_DEV

/* Define to 1 if 'st_ino' is a member of 'struct stat'. */
#undef HAVE_STRUCT_STAT_ST_INO

/* Define to 1 if 'tm_zone' is a member of 'struct tm'. */
#undef HAVE_STRUCT_TM_TM_ZONE

/* Define to 1 if your 'struct stat' has 'st_blocks'. Deprecated, use
   'HAVE_STRUCT_STAT_ST_BLOCKS' instead. */
#undef HAVE_ST_BLOCKS

/* Define if you have the 'symlink' function. */
#undef HAVE_SYMLINK

/* Define to 1 if you have the 'sysconf' function. */
#undef HAVE_SYSCONF

/* Define to 1 if you have the <sysexits.h> header file. */
#undef HAVE_SYSEXITS_H

/* Define to 1 if you have the <sys/audioio.h> header file. */
#undef HAVE_SYS_AUDIOIO_H

/* Define to 1 if you have the <sys/bsdtty.h> header file. */
#undef HAVE_SYS_BSDTTY_H

/* Define to 1 if you have the <sys/dir.h> header file, and it defines 'DIR'.
   */
#undef HAVE_SYS_DIR_H

/* Define to 1 if you have the <sys/epoll.h> header file. */
#undef HAVE_SYS_EPOLL_H

/* Define to 1 if you have the <sys/event.h> header file. */
#undef HAVE_SYS_EVENT_H

/* Define to 1 if you have the <sys/file.h> header file. */
#undef HAVE_SYS_FILE_H

/* Define to 1 if you have the <sys/loadavg.h> header file. */
#undef HAVE_SYS_LOADAVG_H

/* Define to 1 if you have the <sys/lock.h> header file. */
#undef HAVE_SYS_LOCK_H

/* Define to 1 if you have the <sys/mkdev.h> header file. */
#undef HAVE_SYS_MKDEV_H

/* Define to 1 if you have the <sys/modem.h> header file. */
#undef HAVE_SYS_MODEM_H

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines 'DIR'.
   */
#undef HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H    1

/* Define to 1 if you have the <sys/poll.h> header file. */
#define HAVE_SYS_POLL_H                 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#define HAVE_SYS_RESOURCE_H   1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H               1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H               1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#undef HAVE_SYS_STATVFS_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H   1

/* Define to 1 if you have the <sys/termio.h> header file. */
#undef HAVE_SYS_TERMIO_H

/* Define to 1 if you have the <sys/times.h> header file. */
#undef HAVE_SYS_TIMES_H

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H     1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H    1

/* Define to 1 if you have the <sys/un.h> header file. */
#undef HAVE_SYS_UN_H

/* Define to 1 if you have the <sys/utsname.h> header file. */
#undef HAVE_SYS_UTSNAME_H

/* Define to 1 if you have the <sys/wait.h> header file. */
#undef HAVE_SYS_WAIT_H

/* Define to 1 if you have the system() command. */
#define HAVE_SYSTEM       1

/* Define to 1 if you have the 'tcgetpgrp' function. */
#undef HAVE_TCGETPGRP

/* Define to 1 if you have the 'tcsetpgrp' function. */
#undef HAVE_TCSETPGRP

/* Define to 1 if you have the 'tempnam' function. */
#define HAVE_TEMPNAM      1

/* Define to 1 if you have the <termios.h> header file. */
#undef HAVE_TERMIOS_H

/* Define to 1 if you have the <term.h> header file. */
#undef HAVE_TERM_H

/* Define to 1 if you have the 'tgamma' function. */
#undef HAVE_TGAMMA

/* Define to 1 if you have the <thread.h> header file. */
#undef HAVE_THREAD_H

/* Define to 1 if you have the 'timegm' function. */
#undef HAVE_TIMEGM

/* Define to 1 if you have the 'times' function. */
#undef HAVE_TIMES

/* Define to 1 if you have the 'tmpfile' function. */
#define HAVE_TMPFILE    1

/* Define to 1 if you have the 'tmpnam' function. */
#define HAVE_TMPNAM     1

/* Define to 1 if you have the 'tmpnam_r' function. */
#undef HAVE_TMPNAM_R

/* Define to 1 if your 'struct tm' has 'tm_zone'. Deprecated, use
   'HAVE_STRUCT_TM_TM_ZONE' instead. */
#undef HAVE_TM_ZONE

/* Define to 1 if you have the 'truncate' function. */
#undef HAVE_TRUNCATE

/* Define to 1 if you don't have 'tm_zone' but do have the external array
   'tzname'. */
#undef HAVE_TZNAME

/* Define this if you have tcl and TCL_UTF_MAX==6 */
#undef HAVE_UCS4_TCL

/* Define if your compiler provides uint32_t. */
#undef HAVE_UINT32_T

/* Define if your compiler provides uint64_t. */
#undef HAVE_UINT64_T

/* Define to 1 if the system has the type 'uintptr_t'. */
#define HAVE_UINTPTR_T    1

/* Define to 1 if you have the 'uname' function. */
#undef HAVE_UNAME

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H                   1

/* Define to 1 if you have the 'unsetenv' function. */
#undef HAVE_UNSETENV

/* Define if you have a useable wchar_t type defined in wchar.h; useable means
   wchar_t must be an unsigned type with at least 16 bits. (see
   Include/unicodeobject.h). */
#define HAVE_USABLE_WCHAR_T             1

/* Define to 1 if you have the <util.h> header file. */
#undef HAVE_UTIL_H

/* Define to 1 if you have the 'utimes' function. */
#undef HAVE_UTIMES

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H    1

/* Define to 1 if you have the 'wait3' function. */
#undef HAVE_WAIT3

/* Define to 1 if you have the 'wait4' function. */
#undef HAVE_WAIT4

/* Define to 1 if you have the 'waitpid' function. */
#undef HAVE_WAITPID

/* Define if the compiler provides a wchar.h header file. */
#define HAVE_WCHAR_H    1

/* Define to 1 if you have the 'wcscoll' function. */
#define HAVE_WCSCOLL    1

/* Define if tzset() actually switches the local timezone in a meaningful way.
   */
#undef HAVE_WORKING_TZSET

/* Define if the zlib library has inflateCopy */
#undef HAVE_ZLIB_COPY

/* Define to 1 if you have the '_getpty' function. */
#undef HAVE__GETPTY

/* Define if you are using Mach cthreads directly under /include */
#undef HURD_C_THREADS

/* Define if you are using Mach cthreads under mach / */
#undef MACH_C_THREADS

/* Define to 1 if 'major', 'minor', and 'makedev' are declared in <mkdev.h>.
   */
#undef MAJOR_IN_MKDEV

/* Define to 1 if 'major', 'minor', and 'makedev' are declared in
   <sysmacros.h>. */
#undef MAJOR_IN_SYSMACROS

/* Define if mvwdelch in curses.h is an expression. */
#undef MVWDELCH_IS_EXPRESSION

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT   "edk2-devel@lists.01.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME    "EDK II Python 2.7.10 Package"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING  "EDK II Python 2.7.10 Package V0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME   "EADK_Python"

/* Define to the home page for this package. */
#define PACKAGE_URL   "http://www.tianocore.org/"

/* Define to the version of this package. */
#define PACKAGE_VERSION  "V0.1"

/* Define if POSIX semaphores aren't enabled on your system */
#define POSIX_SEMAPHORES_NOT_ENABLED    1

/* Defined if PTHREAD_SCOPE_SYSTEM supported. */
#undef PTHREAD_SYSTEM_SCHED_SUPPORTED

/* Define as the preferred size in bits of long digits */
#undef PYLONG_BITS_IN_DIGIT

/* Define to printf format modifier for long long type */
#define PY_FORMAT_LONG_LONG   "ll"

/* Define to printf format modifier for Py_ssize_t */
#define PY_FORMAT_SIZE_T    "z"

/* Define as the integral type used for Unicode representation. */
#define PY_UNICODE_TYPE     wchar_t

/* Define if you want to build an interpreter with many run-time checks. */
#undef Py_DEBUG

/* Defined if Python is built as a shared library. */
#undef Py_ENABLE_SHARED

/* Define as the size of the unicode type. */
#define Py_UNICODE_SIZE   2

/* Define if you want to have a Unicode type. */
#define Py_USING_UNICODE

/* assume C89 semantics that RETSIGTYPE is always void */
#undef RETSIGTYPE

/* Define if setpgrp() must be called as setpgrp(0, 0). */
#undef SETPGRP_HAVE_ARG

/* Define this to be extension of shared libraries (including the dot!). */
#undef SHLIB_EXT

/* Define if i>>j for signed int i does not extend the sign bit when i < 0 */
#undef SIGNED_RIGHT_SHIFT_ZERO_FILLS

/* The size of 'double', as computed by sizeof. */
#define SIZEOF_DOUBLE     8

/* The size of 'float', as computed by sizeof. */
#define SIZEOF_FLOAT      4

/* The size of 'fpos_t', as computed by sizeof. */
#define SIZEOF_FPOS_T     8

/* The size of 'int', as computed by sizeof. */
#define SIZEOF_INT        4

/* The size of 'long', as computed by sizeof. */
#if defined(_MSC_VER)           /* Handle Microsoft VC++ compiler specifics. */
#define SIZEOF_LONG       4
#else
#define SIZEOF_LONG       8
#endif

/* The size of 'long double', as computed by sizeof. */
#undef SIZEOF_LONG_DOUBLE

/* The size of 'long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG  8

/* The size of 'off_t', as computed by sizeof. */
#define SIZEOF_OFF_T      8

/* The size of 'pid_t', as computed by sizeof. */
#define SIZEOF_PID_T      4

/* The size of 'pthread_t', as computed by sizeof. */
#undef SIZEOF_PTHREAD_T

/* The size of 'short', as computed by sizeof. */
#define SIZEOF_SHORT      2

/* The size of 'size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T     8

/* The size of 'time_t', as computed by sizeof. */
#define SIZEOF_TIME_T     4

/* The size of 'uintptr_t', as computed by sizeof. */
#define SIZEOF_UINTPTR_T  8

/* The size of 'void *', as computed by sizeof. */
#define SIZEOF_VOID_P     8

/* The size of 'wchar_t', as computed by sizeof. */
#define SIZEOF_WCHAR_T    2

/* The size of '_Bool', as computed by sizeof. */
#define SIZEOF__BOOL      1

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS    1

/* Define if you can safely include both <sys/select.h> and <sys/time.h>
   (which you can't on SCO ODT 3.0). */
#undef SYS_SELECT_WITH_SYS_TIME

/* Define if tanh(-0.) is -0., or if platform doesn't have signed zeros */
#undef TANH_PRESERVES_ZERO_SIGN

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#undef TIME_WITH_SYS_TIME

/* Define to 1 if your <sys/time.h> declares 'struct tm'. */
#undef TM_IN_SYS_TIME

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# undef _ALL_SOURCE
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# undef _GNU_SOURCE
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# undef _POSIX_PTHREAD_SEMANTICS
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# undef _TANDEM_SOURCE
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# undef __EXTENSIONS__
#endif


/* Define if you want to use MacPython modules on MacOSX in unix-Python. */
#undef USE_TOOLBOX_OBJECT_GLUE

/* Define if a va_list is an array of some kind */
#undef VA_LIST_IS_ARRAY

/* Define if you want SIGFPE handled (see Include/pyfpe.h). */
#undef WANT_SIGFPE_HANDLER

/* Define if you want wctype.h functions to be used instead of the one
   supplied by Python itself. (see Include/unicodectype.h). */
#define WANT_WCTYPE_FUNCTIONS   1

/* Define if WINDOW in curses.h offers a field _flags. */
#undef WINDOW_HAS_FLAGS

/* Define if you want documentation strings in extension modules */
#undef WITH_DOC_STRINGS

/* Define if you want to use the new-style (Openstep, Rhapsody, MacOS) dynamic
   linker (dyld) instead of the old-style (NextStep) dynamic linker (rld).
   Dyld is necessary to support frameworks. */
#undef WITH_DYLD

/* Define to 1 if libintl is needed for locale functions. */
#undef WITH_LIBINTL

/* Define if you want to produce an OpenStep/Rhapsody framework (shared
   library plus accessory files). */
#undef WITH_NEXT_FRAMEWORK

/* Define if you want to compile in Python-specific mallocs */
#undef WITH_PYMALLOC

/* Define if you want to compile in rudimentary thread support */
#undef WITH_THREAD

/* Define to profile with the Pentium timestamp counter */
#undef WITH_TSC

/* Define if you want pymalloc to be disabled when running under valgrind */
#undef WITH_VALGRIND

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* Define if arithmetic is subject to x87-style double rounding issue */
#undef X87_DOUBLE_ROUNDING

/* Define on OpenBSD to activate all library features */
#undef _BSD_SOURCE

/* Define on Irix to enable u_int */
#undef _BSD_TYPES

/* Define on Darwin to activate all library features */
#undef _DARWIN_C_SOURCE

/* This must be set to 64 on some systems to enable large file support. */
#undef _FILE_OFFSET_BITS

/* Define on Linux to activate all library features */
#undef _GNU_SOURCE

/* This must be defined on some systems to enable large file support. */
#undef _LARGEFILE_SOURCE

/* This must be defined on AIX systems to enable large file support. */
#undef _LARGE_FILES

/* Define to 1 if on MINIX. */
#undef _MINIX

/* Define on NetBSD to activate all library features */
  #define _NETBSD_SOURCE  1

/* Define _OSF_SOURCE to get the makedev macro. */
#undef _OSF_SOURCE

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
#undef _POSIX_1_SOURCE

/* Define to activate features from IEEE Stds 1003.1-2001 */
#undef _POSIX_C_SOURCE

/* Define to 1 if you need to in order for 'stat' and other things to work. */
#undef _POSIX_SOURCE

/* Define if you have POSIX threads, and your system does not define that. */
#undef _POSIX_THREADS

/* Define to force use of thread-safe errno, h_errno, and other functions */
#undef _REENTRANT

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
#undef _UINT32_T

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
#undef _UINT64_T

/* Define to the level of X/Open that your system supports */
#undef _XOPEN_SOURCE

/* Define to activate Unix95-and-earlier features */
#undef _XOPEN_SOURCE_EXTENDED

/* Define on FreeBSD to activate all library features */
#undef __BSD_VISIBLE

/* Define to 1 if type 'char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
# undef __CHAR_UNSIGNED__
#endif

/* Defined on Solaris to see additional function prototypes. */
#undef __EXTENSIONS__

/* Define to 'long' if <time.h> doesn't define. */
//#undef clock_t

/* Define to empty if 'const' does not conform to ANSI C. */
//#undef const

/* Define to 'int' if <sys/types.h> doesn't define. */
//#undef gid_t

/* Define to the type of a signed integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
//#undef int32_t

/* Define to the type of a signed integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
//#undef int64_t

/* Define to 'int' if <sys/types.h> does not define. */
//#undef mode_t

/* Define to 'long int' if <sys/types.h> does not define. */
//#undef off_t

/* Define to 'int' if <sys/types.h> does not define. */
//#undef pid_t

/* Define to empty if the keyword does not work. */
//#undef signed

/* Define to 'unsigned int' if <sys/types.h> does not define. */
//#undef size_t

/* Define to 'int' if <sys/socket.h> does not define. */
//#undef socklen_t

/* Define to 'int' if <sys/types.h> doesn't define. */
//#undef uid_t

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
//#undef uint32_t

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
//#undef uint64_t

/* Define to empty if the keyword does not work. */
//#undef volatile

#endif /*Py_PYCONFIG_H*/

