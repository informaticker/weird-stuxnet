/*
 * File: Util.cpp
 *
 * Project: eco
 *
 * * * * * * * * * * * * * * *
 *
 *  Created on: Mar 24, 2010
 *      Author: fabian
 *
 */

#include "Util.hpp"


int Util::in_array( int h[], int size, int n ) {
	int i;

	for( i=0; i<size; i++ ) {
		if( h[i] == n ) {
			return 1;
		}
	}

	return 0;
}

double Util::randdouble( double low, double high ) {
    static int init = 0;

    if(!init) {
        init = 1;
        srand(time(NULL));
    }

    return ( high - low ) * (rand() / (double)(RAND_MAX)) + low;
}

double Util::drand( void ) {
    static int init = 0;

    if(!init) {
        init = 1;
        srand(time(NULL));
    }

    double d;
    do {
       d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
    } while (d >= 1); /* Round off */

    return d;
}

int Util::distance( int x1, int y1, int x2, int y2 ) {
    return sqrt( pow( x2 - x1, 2) + pow( y2 - y1, 2) );
}

bool Util::point_in_rect( int ax, int ay, int bx, int by, int w, int h ) {

	if( (ax >= bx) &&
		(ax <= bx + w) &&
		(ay >= by) &&
		(ay <= by + h) ) {
		return true;
	} else {
		return false;
	}
}

double Util::clamp( double value, double min, double max ) {

	if( value < min ) return min;
	if( value > max ) return max;
	return value;

}

char * Util::get_time( void ) {

	// string format
	const char *fmt = "%d-%m-%Y_%H-%M-%S";

	// get current time
	time_t t = time(NULL);

	// convert to tm struct
	struct tm *tinfo = localtime( &t );

	// length of buffer
	unsigned int len = 100;

	// allocate memory for the result
	char *buf = (char *)malloc( sizeof(char) * (len+1) );

	// generate string to buffer
	size_t l = strftime( buf, len, fmt, tinfo );

	char *result = (char *)malloc( sizeof(char) * (l+1) );
	strncpy( result, buf, l );
	result[l] = '\0';

	free( buf );

	// return result
	return result;
}

bool Util::rect_in_rect(int x1, int y1, int x2, int y2, int xp2, int yp2, int xp1, int yp1){
		int x3 = x2;
		int y3 = y1;
		int x4 = x1;
		int y4 = y2;

            //Do they have overlapping corners?
                if((xp2 <= x2 && x2 <= xp1) && (yp2 <= y2 && y2 <= yp1))
			return true;
                else if((xp2 <= x1 && x1 <= xp1) && (yp2 <= y1 && y1 <= yp1))
			return true;
                else if((xp2 <= x3 &&x3 <= xp1) && (yp2 <= y3 && y3 <= yp1))
			return true;
                else if((xp2 <= x4 &&x4 <= xp1) && (yp2 <= y4 && y4 <= yp1))
			return true;

             //do they have overlapping middles?
                else if(xp2 <= x1 && x1 <= xp1 && xp2 <= x2 && x2 <= xp1 &&
                                ((yp2 >= y1 && y2 >= yp1) || (yp2 <= y1 && y2 <= yp1)) )
			return true;
                else if(yp2 <= y1 && y1 <= yp1 && yp2 <= y2 && y2 <= yp1 &&
                                ( (xp2 >= x1 && x2 >= xp1) || (xp2 <= x1 && x2 <= xp1)) )
			return true;
		
        return false;
}
