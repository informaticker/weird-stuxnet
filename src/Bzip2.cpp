#include "Bzip2.hpp"

unsigned long Bzip2::compress( char *data ) {

	unsigned int data_len = strlen(data);
	unsigned int outbuf_len = data_len*((double)101/100)+600;

	// To guarantee that the compressed data will fit in its buffer, allocate an output buffer of size 1% larger than the uncompressed data, plus six hundred extra bytes.
	char *outbuf = new char[outbuf_len];

	if( BZ2_bzBuffToBuffCompress( outbuf, &outbuf_len, data, data_len, 9, 0, 1 ) != BZ_OK ) {
		delete [] outbuf;
		return 0;
	} else {
		delete [] outbuf;
		return outbuf_len;
	}

}
