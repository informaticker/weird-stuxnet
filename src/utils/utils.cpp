/*      utils.c
*       Mischa Lehmann
*       ducksource@duckpond.ch
*       Version:1.0
*
*	utils.c
*       Require:
*		- see header
*
*
*	Licence:
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
*/

/*==============================*/
/*           includes           */
/*==============================*/

	#include "utils.h"


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

/*==============================*/
/*     function definitions     */
/*==============================*/


/*Imported from my own functionreference*/

void printerr(char *errmsg,int errcode){

        switch(errcode){
                default:
                case 0:
                        fprintf(stderr,"An Error has occurred: \"%s\"\n",errmsg);
                break;
		/*SDL-Errors (not used)*/
                case 1:
                        //fprintf(stderr,"An Error has occurred: \"%s\"\n\"%s\"\n",errmsg,SDL_GetError());
                break;
                case 2:
                        //fprintf(stderr,"An Error has occurred: \"%s\"\n\"%s\"\n",errmsg,TTF_GetError());
                break;
		/*Dll-errors (not used)*/
                case 3:
			/*dynamic linked library*/
			//fprintf(stderr,"An Error has occurred: \"%s\"\n\"%s\"\n",errmsg,dlerror());
                break;
                case 4: 
                        /*perror output*/
			fprintf(stderr,"An Error has occurred: \n");
                        perror(errmsg);
                break;
        }
}

boolean util_isbigendian(){
	unsigned int testint=0xFF;
	char *ptr = (char*)(&testint);
	if(ptr[0] == 0xFF){ // is this little endian?
		return false;
	}

	return true;
}


void util_convertendian(void *data, size_t size){
	char* byte_data = (char*)data;
	unsigned int hibyte = ((unsigned int) size)-1; //the highest byte address 
	unsigned int i;
	int iterations = (int)(size/2); // amount of iterations..

	for(i=0;i<iterations;i++){
		char buf = byte_data[i]; // copy the current switching bit into the buffer
		/*switch bytes*/
		byte_data[i] = byte_data[hibyte-i];
		byte_data[hibyte-i] = buf;
	}
}

int util_write_to_file(FILE *fd,void *data,size_t size,boolean save_bigendian){
	boolean is_bigendian = util_isbigendian();
	boolean convert_endian = false;
	void *data_bup;

	if((save_bigendian == true && is_bigendian == false)
	|| (save_bigendian == false && is_bigendian == true)){
		convert_endian = true;

		/*switch endianness..*/
		data_bup = malloc(size); // for this backup the data first (do not work on linked data) 
		memcpy(data_bup,data,size);
		util_convertendian(data_bup,size);

		data = data_bup;
		convert_endian = true;
	}

	if(size != fwrite((const void*)data,1, size, fd)){
		printerr("util_write_to_file(): Couldn't write to file!",0);
		return -1;
	}

	if(convert_endian == true)
		free(data_bup);

	return 1;
}

int util_read_from_file(FILE *fd,void* data,size_t size,boolean read_bigendian){
	boolean is_bigendian = util_isbigendian();

	if(size != fread(data,1, size, fd)){
		printerr("util_read_from_file(): Couldn't read from file!",0);
		return -1;
	}

	if((read_bigendian == true && is_bigendian == false)
	|| (read_bigendian == false && is_bigendian == true)){
		util_convertendian(data,size);
	}

	return 1;
}

inline int util_mk_2dmatrix(unsigned int l1_size,unsigned int l2_size,size_t element_size,void ***matrix){
	unsigned int i;
	char *field;
	
	if(l1_size <= 0
	|| l2_size <= 0
	|| element_size <= 0){
		printerr("util_mk_2dmatrix(): l1_size || l2_size || element_size <= 0",0);
		return -1;
	}

	(*matrix) = (void**)malloc(l1_size*sizeof(void*)); // first dimension (pointers)
	field = (char*)malloc(l1_size*l2_size*element_size);	// second dimension (data)

	if((*matrix) == NULL || field == NULL){
		printerr("util_mk_2dmatrix(): Malloc failed!",0);
		return -1;
	}

	for(i=0;i<l1_size;i++){
		(*matrix)[i] = (void*)(&(field[i*l2_size*element_size]));
	}

	return 1;
}

inline int util_set_bit(char *field,boolean state,unsigned char bitnum){
	unsigned char num = bitnum%8;
	if(state == true){ // set to 0
		switch(num){
			case 0:(*field) = (*field)|0x01;break;
			case 1:(*field) = (*field)|0x02;break;
			case 2:(*field) = (*field)|0x04;break;
			case 3:(*field) = (*field)|0x08;break;
			case 4:(*field) = (*field)|0x10;break;
			case 5:(*field) = (*field)|0x20;break;
			case 6:(*field) = (*field)|0x40;break;
			case 7:(*field) = (*field)|0x80;break;
		}
	}else{
		switch(num){
			case 0:(*field) = (*field)&0xFE;break;
			case 1:(*field) = (*field)&0xFD;break;
			case 2:(*field) = (*field)&0xFB;break;
			case 3:(*field) = (*field)&0xF7;break;
			case 4:(*field) = (*field)&0xEF;break;
			case 5:(*field) = (*field)&0xDF;break;
			case 6:(*field) = (*field)&0xBF;break;
			case 7:(*field) = (*field)&0x7F;break;
		}
	}
}



