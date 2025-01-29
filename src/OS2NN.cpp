#include "OS2NN.hpp"

#define inverse_char_scale(x) (((unsigned long)x)*10 + 20)

OS2NN::OS2NN(unsigned long ws, unsigned long nb){
	//strictly speaking, the minimum should always be zero, but this error message will still
	//work if we change that in future.
	if(nb<=0 || nb > max_nb || ws <=0 || ws > max_ws)
		if( DEBUG_LEVEL > 2)  fprintf(stderr, "OS2NN::OS2NN(): Warning: Attempt to construct interface to the opperating system with invalid paramters\nUsing min/max values and attempting to continue\n");

	if(nb <= 0) nb = 1;
	if(nb > max_nb) nb = max_nb;
	if(ws <= 0) ws = 1;
	if(ws > max_ws) ws = max_ws;
	
	this->word_size = ws;
	this->n_buf = nb;
	buffers = (char**) malloc(nb*sizeof(char*)); 
	for(unsigned int i =0; i < this->n_buf; i++)
		buffers[i] = (char*)calloc(BUFSIZ, sizeof(char));
	
}

OS2NN::~OS2NN(){
	for(unsigned int i =0; i < this->n_buf; i++)
		free(buffers[i]);
	free(buffers);
}

int OS2NN::retrieve_input(unsigned long which_buf, unsigned long which_word, unsigned long offset, double* op){
	static char tmpbuf[BUFSIZ];
	if(which_buf >= this->n_buf){
		if( DEBUG_LEVEL > 2 )printf("retrieve_input 1: Warning: Attempt to access invalid buffer. Using modular arithmatic and attempting to continue.\n");
		which_buf = which_buf%n_buf;
	}	

	//first, count the number of words in the buffer. We need this to make sure we can access the desired word.
	unsigned long word_counter = 1;
	int inaword = 0;
	for(unsigned int i = 0; i < strlen(buffers[which_buf]); i++){
		if(buffers[which_buf][i] == ' ' || buffers[which_buf][i] == '\t' || buffers[which_buf][i] == '\n'){
			if(inaword){
				word_counter++;
				inaword = 0;
			}
		} else {
			inaword = 1;
		}
	}
	strncpy(tmpbuf, buffers[which_buf], BUFSIZ);
	
	if(which_word >= word_counter){
		if( DEBUG_LEVEL > 2 )printf("retrieve_input 2: Warning: Attempt to access invalid word in buffer. Using modular arithmatic and attempting to continue.\n");
		which_word = which_word%word_counter;
	}

	char *x;
	for(unsigned int i = 0; i <= which_word;i++)
		x = strtok(i==0?tmpbuf:NULL, "\t\n ");
	
	unsigned int i;
	unsigned int start = offset * word_size;
	for(i = start; (i-start) < word_size && i < (x==NULL?0:strlen(x)); i++)
		op[i-start] = inverse_char_scale(x[i]);

	for(i -= start; i < word_size; i++)
		op[i] = '\0';
	
	// 0 indicates that there is no more data in this word, 10 indicates that there is.
	op[this->word_size] = start+this->word_size < (x==NULL?0:strlen(x)) ? 10 : 0; 
	return 0;
}

int OS2NN::append_buffer(unsigned long which_buf, char* to_what){
	if(which_buf >= this->n_buf){
		if( DEBUG_LEVEL > 2 ) printf("append_buffer: Warning: Attempt to access invalid buffer. Using modular arithmatic and attempting to continue.\n");
		which_buf = which_buf % this->n_buf;
	}
	if(strlen(to_what) + strlen(buffers[which_buf]) > BUFSIZ)
		strncat(buffers[which_buf], to_what, BUFSIZ - strlen(buffers[which_buf]));
	else
		strncat(buffers[which_buf], to_what, strlen(to_what));
	buffers[which_buf][strlen(to_what) > BUFSIZ? BUFSIZ-1 : strlen(to_what)] = '\0'; //what is this for... very strange.
	return 0;	
}

int OS2NN::set_buffer(unsigned long which_buf, char* to_what){
	if(which_buf >= this->n_buf){
		if( DEBUG_LEVEL > 2 )printf("set_buffer: Warning: Attempt to access invalid buffer. Using modular arithmatic and attempting to continue.\n");
		which_buf = which_buf % this->n_buf;
	}
	if(strlen(to_what) > BUFSIZ)
		strncpy(buffers[which_buf], to_what, BUFSIZ);
	else
		strncpy(buffers[which_buf], to_what, strlen(to_what));
	buffers[which_buf][strlen(to_what) > BUFSIZ? BUFSIZ-1 : strlen(to_what)] = '\0';
	return 0;	
}

int OS2NN::fetch_buffer(unsigned long which, char* tmpbuf){
	strcpy(tmpbuf, buffers[which%this->n_buf]);
	return 0;
}
