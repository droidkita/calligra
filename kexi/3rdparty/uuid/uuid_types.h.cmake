/* 
 * If linux/types.h is already been included, assume it has defined
 * everything we need.  (cross fingers)  Other header files may have 
 * also defined the types that we need.
 */
#if (!defined(_LINUX_TYPES_H) && !defined(_BLKID_TYPES_H) && \
	!defined(_UUID_TYPES) && !defined(_EXT2_TYPES_H))
#define _UUID_TYPES_H

typedef unsigned char __u8;
typedef signed char __s8;

#if (@SIZEOF_INT@ == 8)
typedef int		__s64;
typedef unsigned int	__u64;
#elif (@SIZEOF_LONG@ == 8)
typedef long		__s64;
typedef unsigned long	__u64;
#elif (@SIZEOF_LONG_LONG@ == 8)
#if defined(__GNUC__)
typedef __signed__ long long 	__s64;
#else
typedef signed long long 	__s64;
#endif
typedef unsigned long long	__u64;
#endif

#if (@SIZEOF_INT@ == 2)
typedef	int		__s16;
typedef	unsigned int	__u16;
#elif (@SIZEOF_SHORT@ == 2)
typedef	short		__s16;
typedef	unsigned short	__u16;
#else
  ?==error: undefined 16 bit type
#endif

#if (@SIZEOF_INT@ == 4)
typedef	int		__s32;
typedef	unsigned int	__u32;
#elif (@SIZEOF_LONG@ == 4)
typedef	long		__s32;
typedef	unsigned long	__u32;
#elif (@SIZEOF_SHORT@ == 4)
typedef	short		__s32;
typedef	unsigned short	__u32;
#else
 ?== error: undefined 32 bit type
#endif

#endif /* _*_TYPES_H */
