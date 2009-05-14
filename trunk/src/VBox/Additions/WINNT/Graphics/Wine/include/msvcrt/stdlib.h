/*
 * Standard library definitions
 *
 * Derived from the mingw header written by Colin Peters.
 * Modified for Wine use by Jon Griffiths and Francois Gouget.
 * This file is in the public domain.
 */

/*
 * Sun LGPL Disclaimer: For the avoidance of doubt, except that if any license choice
 * other than GPL or LGPL is available it will apply instead, Sun elects to use only
 * the Lesser General Public License version 2.1 (LGPLv2) at this time for any software where
 * a choice of LGPL license versions is made available with the language indicating
 * that LGPLv2 or any later version may be used, or where a choice of which version
 * of the LGPL is applied is otherwise unspecified.
 */

#ifndef __WINE_STDLIB_H
#define __WINE_STDLIB_H

#include <crtdefs.h>

#include <pshpack8.h>

#ifndef NULL
#ifdef __cplusplus
#define NULL  0
#else
#define NULL  ((void*)0)
#endif
#endif

typedef struct
{
    float f;
} _CRT_FLOAT;

typedef struct
{
    double x;
} _CRT_DOUBLE;

typedef struct
{
    unsigned char ld[10];
} _LDOUBLE;

#define EXIT_SUCCESS        0
#define EXIT_FAILURE        -1
#define RAND_MAX            0x7FFF

#ifndef _MAX_PATH
#define _MAX_DRIVE          3
#define _MAX_FNAME          256
#define _MAX_DIR            _MAX_FNAME
#define _MAX_EXT            _MAX_FNAME
#define _MAX_PATH           260
#endif


typedef struct _div_t {
    int quot;
    int rem;
} div_t;

typedef struct _ldiv_t {
    long quot;
    long rem;
} ldiv_t;

#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#ifndef __cplusplus
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif

/* _set_error_mode() constants */
#define _OUT_TO_DEFAULT      0
#define _OUT_TO_STDERR       1
#define _OUT_TO_MSGBOX       2
#define _REPORT_ERRMODE      3


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __i386__

extern unsigned int* __cdecl __p__osver(void);
#define _osver             (*__p__osver())
extern unsigned int* __cdecl __p__winver(void);
#define _winver            (*__p__winver())
extern unsigned int* __cdecl __p__winmajor(void);
#define _winmajor          (*__p__winmajor())
extern unsigned int* __cdecl __p__winminor(void);
#define _winminor          (*__p__winminor())
extern int*          __cdecl __p___argc(void);
#define __argc             (*__p___argc())
extern char***       __cdecl __p___argv(void);
#define __argv             (*__p___argv())
extern wchar_t***    __cdecl __p___wargv(void);
#define __wargv            (*__p___wargv())
extern char***       __cdecl __p__environ(void);
#define _environ           (*__p__environ())
extern wchar_t***    __cdecl __p__wenviron(void);
#define _wenviron          (*__p__wenviron())
extern unsigned int* __cdecl __p__fmode(void);
#define _fmode             (*__p__fmode())

#else  /* __i386__ */

extern unsigned int _osver;
extern unsigned int _winver;
extern unsigned int _winmajor;
extern unsigned int _winminor;
extern int __argc;
extern char **__argv;
extern wchar_t **__wargv;
extern char **_environ;
extern wchar_t **_wenviron;
extern unsigned int _fmode;

#endif  /* __i386__ */

extern int*           __cdecl ___mb_cur_max_func(void);
#define __mb_cur_max        (*___mb_cur_max_func())
extern unsigned long* __cdecl __doserrno(void);
#define _doserrno           (*__doserrno())
extern int*           __cdecl _errno(void);
#define errno               (*_errno())

/* FIXME: We need functions to access these:
 * int _sys_nerr;
 * char** _sys_errlist;
 */


typedef int (__cdecl *_onexit_t)(void);


int           __cdecl _atodbl(_CRT_DOUBLE*,char*);
int           __cdecl _atoflt(_CRT_FLOAT*,char*);
__int64       __cdecl _atoi64(const char*);
long double   __cdecl _atold(const char*);
int           __cdecl _atoldbl(_LDOUBLE*,char*);
void          __cdecl _beep(unsigned int,unsigned int);
char*         __cdecl _ecvt(double,int,int*,int*);
char*         __cdecl _fcvt(double,int,int*,int*);
char*         __cdecl _fullpath(char*,const char*,size_t);
char*         __cdecl _gcvt(double,int,char*);
char*         __cdecl _i64toa(__int64,char*,int);
char*         __cdecl _itoa(int,char*,int);
char*         __cdecl _ltoa(long,char*,int);
unsigned long __cdecl _lrotl(unsigned long,int);
unsigned long __cdecl _lrotr(unsigned long,int);
void          __cdecl _makepath(char*,const char*,const char*,const char*,const char*);
size_t        __cdecl _mbstrlen(const char*);
_onexit_t     __cdecl _onexit(_onexit_t);
int           __cdecl _putenv(const char*);
unsigned int  __cdecl _rotl(unsigned int,int);
unsigned int  __cdecl _rotr(unsigned int,int);
void          __cdecl _searchenv(const char*,const char*,char*);
int           __cdecl _set_error_mode(int);
void          __cdecl _seterrormode(int);
void          __cdecl _sleep(unsigned long);
void          __cdecl _splitpath(const char*,char*,char*,char*,char*);
long double   __cdecl _strtold(const char*,char**);
void          __cdecl _swab(char*,char*,int);
char*         __cdecl _ui64toa(unsigned __int64,char*,int);
char*         __cdecl _ultoa(unsigned long,char*,int);

void          __cdecl _exit(int);
void          __cdecl abort(void);
int           __cdecl abs(int);
int           __cdecl atexit(void (*)(void));
double        __cdecl atof(const char*);
int           __cdecl atoi(const char*);
long          __cdecl atol(const char*);
void*         __cdecl calloc(size_t,size_t);
#ifndef __i386__
div_t  __cdecl div(int,int);
ldiv_t __cdecl ldiv(long,long);
#endif
void          __cdecl exit(int);
void          __cdecl free(void*);
char*         __cdecl getenv(const char*);
long          __cdecl labs(long);
void*         __cdecl malloc(size_t);
int           __cdecl mblen(const char*,size_t);
void          __cdecl perror(const char*);
int           __cdecl rand(void);
void*         __cdecl realloc(void*,size_t);
void          __cdecl srand(unsigned int);
double        __cdecl strtod(const char*,char**);
long          __cdecl strtol(const char*,char**,int);
unsigned long __cdecl strtoul(const char*,char**,int);
int           __cdecl system(const char*);
void*         __cdecl bsearch(const void*,const void*,size_t,size_t,int (*)(const void*,const void*));
void          __cdecl qsort(void*,size_t,size_t,int (*)(const void*,const void*));

#ifndef _WSTDLIB_DEFINED
#define _WSTDLIB_DEFINED
wchar_t*      __cdecl _itow(int,wchar_t*,int);
wchar_t*      __cdecl _i64tow(__int64,wchar_t*,int);
wchar_t*      __cdecl _ltow(long,wchar_t*,int);
wchar_t*      __cdecl _ui64tow(unsigned __int64,wchar_t*,int);
wchar_t*      __cdecl _ultow(unsigned long,wchar_t*,int);
wchar_t*      __cdecl _wfullpath(wchar_t*,const wchar_t*,size_t);
wchar_t*      __cdecl _wgetenv(const wchar_t*);
void          __cdecl _wmakepath(wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*,const wchar_t*);
void          __cdecl _wperror(const wchar_t*);
int           __cdecl _wputenv(const wchar_t*);
void          __cdecl _wsearchenv(const wchar_t*,const wchar_t*,wchar_t*);
void          __cdecl _wsplitpath(const wchar_t*,wchar_t*,wchar_t*,wchar_t*,wchar_t*);
int           __cdecl _wsystem(const wchar_t*);
int           __cdecl _wtoi(const wchar_t*);
__int64       __cdecl _wtoi64(const wchar_t*);
long          __cdecl _wtol(const wchar_t*);

size_t        __cdecl mbstowcs(wchar_t*,const char*,size_t);
int           __cdecl mbtowc(wchar_t*,const char*,size_t);
double        __cdecl wcstod(const wchar_t*,wchar_t**);
long          __cdecl wcstol(const wchar_t*,wchar_t**,int);
size_t        __cdecl wcstombs(char*,const wchar_t*,size_t);
unsigned long __cdecl wcstoul(const wchar_t*,wchar_t**,int);
int           __cdecl wctomb(char*,wchar_t);
#endif /* _WSTDLIB_DEFINED */

#ifdef __cplusplus
}
#endif


#define environ _environ
#define onexit_t _onexit_t

static inline char* ecvt(double value, int ndigit, int* decpt, int* sign) { return _ecvt(value, ndigit, decpt, sign); }
static inline char* fcvt(double value, int ndigit, int* decpt, int* sign) { return _fcvt(value, ndigit, decpt, sign); }
static inline char* gcvt(double value, int ndigit, char* buf) { return _gcvt(value, ndigit, buf); }
static inline char* itoa(int value, char* str, int radix) { return _itoa(value, str, radix); }
static inline char* ltoa(long value, char* str, int radix) { return _ltoa(value, str, radix); }
static inline _onexit_t onexit(_onexit_t func) { return _onexit(func); }
static inline int putenv(const char* str) { return _putenv(str); }
static inline void swab(char* src, char* dst, int len) { _swab(src, dst, len); }
static inline char* ultoa(unsigned long value, char* str, int radix) { return _ultoa(value, str, radix); }

#ifdef __i386__
static inline div_t __wine_msvcrt_div(int num, int denom)
{
    extern unsigned __int64 div(int,int);
    div_t ret;
    unsigned __int64 res = div(num,denom);
    ret.quot = (int)res;
    ret.rem  = (int)(res >> 32);
    return ret;
}
static inline ldiv_t __wine_msvcrt_ldiv(long num, long denom)
{
    extern unsigned __int64 ldiv(long,long);
    ldiv_t ret;
    unsigned __int64 res = ldiv(num,denom);
    ret.quot = (long)res;
    ret.rem  = (long)(res >> 32);
    return ret;
}
#define div(num,denom) __wine_msvcrt_div(num,denom)
#define ldiv(num,denom) __wine_msvcrt_ldiv(num,denom)
#endif

#include <poppack.h>

#endif /* __WINE_STDLIB_H */
