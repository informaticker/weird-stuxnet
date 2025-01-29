#ifndef OS2NN_H
#define OS2NN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * This class provides access to buffer data for a neural network.
 * It allows new data to be written and data to be read from a number of buffers, each of size BUFSIZ.
 *
 * @author Fabian Schläpfer
 * @date July 12th 2010
 */

class OS2NN {

protected:
	
	/**
	 * This two-dimensional array holds all the data.
	 */
	char **buffers;

	/**
	 * This variable specifies the word size that the neural network uses to access specific words or buffers at a specific offset.
	 */
	unsigned long word_size;

	/**
	 * This variable holds the number of buffers.
	 */
	unsigned long n_buf;	

public:

	/**
	 * This variable is used to store information about the internal state of the object. It is set by NN2OS and read by Individual.
	 */
	unsigned int which_buf_next;

	/**
	 * This variable is used to store information about the internal state of the object. It is set by NN2OS and read by Individual.
	 */
	unsigned int which_word_next;

	/**
	 * This variable is used to store information about the internal state of the object. It is set by NN2OS and read by Individual.
	 */
	unsigned int which_offset_next;

	/**
	 * This constant defines the maximum word size.
	 */
	const static unsigned long max_ws = 50;

	/**
	 * This constant defines the maximum number of buffers.
	 */
	const static unsigned long max_nb = 20;

	/**
	 * This constructor sets up an instance of this class using the supplied parameters and allocates memory for the buffers.
	 *
	 * @param ws	The word size to use.
	 * @param nb	The number of buffers to use, each of size BUFSIZ.
	 */
	OS2NN(unsigned long ws, unsigned long nb);

	/**
	 * This destructor simply frees the memory for the buffers.
	 */
	~OS2NN();

	/**
	 * This method is used to retrieve a specific part of a word stored in a buffer.
	 * It is usually called before the neural network is calculated and writes its result to an input vector supplied.
	 * After this the neural network can access single characters of the word requested using its input neurons.
	 *
	 * @param which_buf		The number of the buffer that contains the word desired.
	 * @param which_word	The number of the word in the buffer.
	 * @param offset		The offset in the word.
	 * @param op			A pointer to an array of doubles, at least of the size #word_size.
	 * 						It is used to store the word requested.
	 *
	 * @return The return value is always zero and should not be used.
	 */
	int retrieve_input(unsigned long which_buf, unsigned long which_word, unsigned long offset, double* op);

	/**
	 * This method is used to write a null-terminated sequence of characters to a specific buffer.
	 *
	 * @param which_buf	The number of the buffer to be accessed.
	 * @param to_what	The null-terminated string of characters containing the data to be written.
	 *
	 * @return The return value is always zero and should not be used.
	 */
	int set_buffer(unsigned long which_buf, char* to_what);

	/**
	 * This method simply appends the entire buffer w to tmpbuf.
	 *
	 * @param w			The number of the buffer we want to read data from.
	 * @param tmpbuf	A pointer to a memory location where the buffer data should be appended to.
	 * 					It should be at least BUFSIZ bytes large.
	 *
 	 * @return The return value is always zero and should not be used.
	 */
	int fetch_buffer(unsigned long w, char *tmpbuf);

	/**
	 * This method simply appends at most BUFSIZE bytes from tmpbuf to buffer number w.
	 *
	 * @param w			The number of the buffer we want to append to.
	 * @param tmpbuf	A pointer to a memory location that contains the data which should be appended to the buffer.
	 * 					It should be at most BUFSIZ bytes long, data after tmpbuf[BUFSIZ] is discarded.
	 *
	 * @return The return value is always zero and should not be used.
	 */
	int append_buffer(unsigned long w, char *tmpbuf);

};



#endif
