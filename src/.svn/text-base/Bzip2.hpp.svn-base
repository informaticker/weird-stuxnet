#ifndef _BZIP2_H
#define _BZIP2_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include <bzlib.h> // BZ2_bzBuffToBuffCompress

using namespace std;

/**
 * Interfaces with bzlib to facilitate data compression without system calls.
 * Used to compute the normalized information distance approximation of the
 * Kolmogorov complexity.
 *
 * @author Fabian Schläpfer
 * @data June 11th 2010
*/
class Bzip2 {
	
	public:

		/**
		 * Computes the length of the input string when compressed
		 * with the bzip2 algorithm.
		 *
		 * @param data	A null terminated string of any length.
		 *
		 * @return A fairly reliable approximation of the Kolmogorov complexity of the input data.
		*/
		static unsigned long compress( char *data );
	
};

#endif
