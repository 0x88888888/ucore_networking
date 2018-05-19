#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef NULLPTR
#define	NULLPTR	((void *)0)
#endif


#define __always_inline __attribute__((__always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))

#define CHAR_BIT        8

/*Represent true-or-false values */
typedef int bool;
enum { false, true };

/* Explicitly-sized version of integer types */
typedef	unsigned char	byte;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

/**
 * Pointers and address are 32 bits long
 * We use to pointer types to represent address,
 * uintptr_t to represent the numerical values of address. 
 **/
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint32_t physaddr_t;

/* size_t is used for memory object sizes */
typedef uintptr_t size_t;

// ssize_t is a signed version of ssize_t, used in case there might be an
// error return.
typedef int32_t ssize_t;

/* off_t is used for file offsets and lengths */
typedef intptr_t off_t;

/* used for page numbers*/
typedef size_t ppn_t;

/**
 * Rounding operations (efficent when n is a power of 2)
 * Round down to the nearest multiple of n
 **/
#define ROUNDDOWN(a, n) ({                                          \
            size_t __a = (size_t)(a);                               \
            (typeof(a))(__a - __a % (n));                           \
        })

/* Round up to the nearest multiple of n */
#define ROUNDUP(a, n) ({                                            \
            size_t __n = (size_t)(n);                               \
            (typeof(a))(ROUNDDOWN((size_t)(a) + __n - 1, __n));     \
        })

/* Round up the result of dividing of n */
#define ROUNDUP_DIV(a, n) ({                                        \
uint32_t __n = (uint32_t)(n);                           \
(typeof(a))(((a) + __n - 1) / __n);                     \
})

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

/* Return the offset of 'member' relative to the beginning of a struct type */
#define offsetof(type, member)                                      \
    ((size_t)(&((type *)0)->member))

/**
 * to_struct - get the struct from a ptr
 * @ptr:    a struct pointer of member
 * @type:   the value of the struct this is embedded in
 * @member  the name of the member within the struct   
 **/
#define to_struct(ptr, type, member)                               \
    ((type *)((char *)(ptr) - offsetof(type, member)))


/***********************************************************************************/

#define	_U	0001
#define	_L	0002
#define	_N	0004
#define	_S	0010
#define _P	0020
#define _C	0040
#define	_X	0100

extern	char	_ctype_[];    

#define	isalpha(c)	((_ctype_+1)[c]&(_U|_L))
#define	isupper(c)	((_ctype_+1)[c]&_U)
#define	islower(c)	((_ctype_+1)[c]&_L)
#define	isdigit(c)	((_ctype_+1)[c]&_N)
#define	isxdigit(c)	((_ctype_+1)[c]&(_N|_X))
#define	isspace(c)	((_ctype_+1)[c]&_S)
#define ispunct(c)	((_ctype_+1)[c]&_P)
#define isalnum(c)	((_ctype_+1)[c]&(_U|_L|_N))
#define isprshort(c)	((_ctype_+1)[c]&(_P|_U|_L|_N))
#define isprint(c)	((_ctype_+1)[c]&(_P|_U|_L|_N|_S))
#define iscntrl(c)	((_ctype_+1)[c]&_C)
#define isascii(c)	((unsigned)(c)<=0177)
#define toupper(c)	((c)-'a'+'A')
#define tolower(c)	((c)-'A'+'a')
#define toascii(c)	((c)&0177)


#endif /* !__LIBS_DEFS_H__ */

