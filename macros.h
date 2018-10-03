/************************************************************************
* Title : MACROS common utility			  			*
*						  			*
* Author : Jagdish Prajapati			  			*
*						  			*
* Discription : 				  			*
*	common macros are available to make 	  			*
* programming easy fast and optimized.		  			*
*						  			*
*************************************************************************/

#ifndef __MACROS_H__
#define __MACROS_H__

#define V1X(_arg)     _arg
#define V2X(_arg)     V1X(_arg),  V1X(_arg)
#define V4X(_arg)     V2X(_arg),  V2X(_arg)
#define V8X(_arg)     V4X(_arg),  V4X(_arg)
#define V16X(_arg)    V8X(_arg),  V8X(_arg)
#define V32X(_arg)    V16X(_arg), V16X(_arg)
#define V64X(_arg)    V32X(_arg), V32X(_arg)

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(_A) (sizeof(_A) / sizeof((_A)[0]))
#endif /* ARRAY_SIZE */

#define DIV_ROUND_UP(x, y)  (((x) + (y) - 1) / (y))
#define max_t(x, y)  ((x) > (y) ? (x) : (y))
#define min_t(x, y)  ((x) < (y) ? (x) : (y))
#ifdef DBG_ENABLE
	#define printf(__str, __args... )  printf("%s : %s : %d " __str, \
			__FILE__, __func__, __LINE__, ##__args )
#endif

#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
	#define container_of(ptr, type, member) ({                	\
        	const typeof(((type *)0)->member)*__mptr = (ptr);	\
        	(type *)((char *)__mptr - offset_of(type, member)); })
#endif


#ifndef MACROS_DISABLE
	#define _printv(_arg, _tchrs) printf(#_arg " = %" #_tchrs  	\
			"\n", _arg)

	#define _progress(_per,dchr,pchr) ({  				\
		static int _p = -1;	      				\
		static char _a[101] = {0};		\
		if ((_per)%101 == 0) for(_p=0;_p<100;_p++) _a[_p]=dchr; \
		if ((_per)%101 != _p) {	      				\
		  if((_per)%101 != 0) _a[(_per)%101 - 1] = pchr;  		\
		  printf("<%3d %%>[%s]\r", (_per), _a);fflush(stdout);	\
		  _p = _per%101; 	      				\
		}			      				\
		})

#else

#endif

#endif
