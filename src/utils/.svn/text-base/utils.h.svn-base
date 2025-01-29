/*     	utils.h
*       Mischa Lehmann
*       ducksource@duckpond.ch
*       Version:1.0
*
*       Utilities for daily work
*       Require:
*		- stdio.h
*		- stdlib.h
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
#ifndef _UTILS_H
	#define _UTILS_H 1

	/*all Libraries*/
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

/*==============================*/
/*  System dependend stuff      */
/*==============================*/
#ifndef CURSystem
	#ifdef __unix__ 
		/*Unix-Systes*/
		#define CURSystem printf("Hello Unix-user\n You're elite!\n");
	#elif __BORLANDC__ && __MSDOS__
		/*Borland c-compiler and MSDOS*/
		#define CURSystem printf("Hello BorlandC-complier or MSDOS user!\n");
	#elif __WIN32__ || _MSC_VER
		/*Windows 32-Syste or visual C*/
		#define CURSystem printf("Hello windows user!\n");
	#else
		#define CURSystem printf("This programm is not compatible with your System, sorry -> EXIT\n");return EXIT_FAILURE;
	#endif
#endif

/*==============================*/
/*enums declaration + definition*/
/*==============================*/


#ifndef __cplusplus
/*define for using boolean-like-datatype (true and false)*/
typedef enum {
	false = -1,
	FALSE = -1,
	true = 1,
	TRUE = 1
}boolean;
#else
#define boolean bool
#endif

/*==============================*/
/*     global declarations      */
/*==============================*/

/*==============================*/
/*    function declarations     */
/*==============================*/

/*
*	funcname(args): <desc>
*	Arguments:
*		- <arg1> : <desc1> 
*/

/*
*	printerr(char *errmsg,int errcode): Prints an error
*	Arguments:
*		errmsg = the message
*		errcode = the error code
*/
void printerr(char *errmsg,int errcode); // Error-Printing

/*
*	util_isbigendian(): Deremits of the systems endianness
*	Arguments:
*	Return: true = is big endian system
*		false = is little endian system
*/
boolean util_isbigendian();

/*
*	void util_convertendian(void *data, size_t size): switch between endian
*	Arguments:
*		- data : the switching data
*		- size : size of the switching data 
*/
void util_convertendian(void *data, size_t size);

/*
*	int util_write_to_file(FILE *fd,void *data,size_t size,boolean save_bigendian): saves data to a file (swtich endian if needed)
*	Arguments:
*		- fd: the filedescriptor
*		- data: the data
*		- size: size of the data (in bytes)
*		- save_bigendian: if true saves in bigendian if not saves in little-endian
*/
int util_write_to_file(FILE *fd,void *data,size_t size,boolean save_bigendian);


/*
*	int util_read_from_file(FILE *fd,void* data,size_t size,boolean read_bigendian): read data from a file (swtich endian if needed)
*	Arguments:
*		- fd: the filedescriptor
*		- data: the data
*		- size: size of the data (in bytes)
*		- read_bigendian: if true data read is in bigendian if not data is in little-endian
*/
int util_read_from_file(FILE *fd,void* data,size_t size,boolean read_bigendian);

/*
*	util_mk_2dmatrix: malloc a 2d matrix
*	Arguments:
*		- l1_size : size of first dimension
*		- l2_size : size of second dimension
*		- element_size : size of an element of the matrix
*		- matrix : pointer to the returning matrix
*/
inline int util_mk_2dmatrix(unsigned int l1_size,unsigned int l2_size,size_t element_size,void ***matrix);
#endif
