/*      inc_stdint.h
*       Mischa Lehmann
*       ducksource@duckpond.ch
*       Version:1.0
*
*       standard integers includer
*       Require:
*
*
*       Licence:
*       You're allowed to edit and publish my source in all of your free and open-source projects.
*       Please send me an e-mail if you'll implement this source in one of your commercial or proprietary projects.
*       Leave this Header untouched!
*
*       Warranty:
*       Warranty void if signet is broken
*       =================== / /===================
*       [       Warranty   / /   Signet          ]
*       ==================/ /=====================
*	!!NOWAY!!
*
*/

/*==============================*/
/*             TODO's           */
/*==============================*/
/*
*
*
*
*
*
*
*/

/*==============================*/
/*           includes           */
/*==============================*/
#ifndef _INC_STDINT_H
	#define _INC_STDINT_H 1
	
	#include <stdint.h>

/*==============================*/
/*  System dependend stuff      */
/*==============================*/

//check if this is posix-standard
#if !(defined(_POSIX_V6_ILP32_OFFBIG) || defined(_POSIX_V6_LP64_OFF64) || defined(_POSIX_V6_LPBIG_OFFBIG))
	#warning INC_STDINT: POSIX STANDARD NOT SUPPORTED!
#endif

/*==============================*/
/*enums declaration + definition*/
/*==============================*/

/*==============================*/
/*     global declarations      */
/*==============================*/

/*==============================*/
/*    function declarations     */
/*==============================*/

#endif
