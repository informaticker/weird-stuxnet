#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <cstdio>
#include <iostream>
#include <ctime>
#include <cmath>
#include <cfloat>
#include <cstdlib>
#include <cstring>

#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

/**
 * This static class contains a bunch of useful methods for various purposes.
 *
 * @author Fabian Schläpfer
 * @date March 24th 2010
 */

class Util {

	public:

		/**
		 * This method tells you whether or not an element is in an array.
		 *
		 * @param h		The array to search
		 * @param size	The size of the array h
		 * @param n		The element to check for
		 *
		 * @return 1 if the element already is in the array, 0 otherwise.
		 */
		static int in_array( int h[], int size, int n );

		/**
		 * This method returns a random floating point number in a range.
		 *
		 * @param low	The lower bound
		 * @param high	The upper bound
		 *
		 * @return A random floating point number: low <= x <= high
		 */
		static double randdouble( double low, double high );

		/**
		 * This method returns a random floating point number of type double between 0 and 1.
		 *
		 * @return A random floating point number: 0 < x < 1
		 */
		static double drand( void );

		/**
		 * This method calculates the distance between two points.
		 *
		 * @param x1 The x-coordinate of the first point
		 * @param y1 The y-coordinate of the first point
		 * @param x2 The x-coordinate of the second point
		 * @param y2 The y-coordinate of the second point
		 *
		 * @return The distance between (x1,y1) and (x2,y2).
		 */
		static int distance( int x1, int y1, int x2, int y2 );

		/**
		 * This method restricts a floating-point value to be within a specified range.
		 *
		 * @param value	The value to be restricted
		 * @param min	The minimum value
		 * @param max	The maximum value
		 *
		 * @return		The clamped value
		 */
		static double clamp( double value, double min, double max );

		/**
		 * This method returns the current time in string representation.
		 *
		 * @return A string representing the current time formatted as "%d-%m-%Y_%H-%M-%S"
		 */
		static char *get_time( void );

		/**
		 * This method tells you whether or not a specified point lies within a specified rectangle.
		 *
		 * @param ax	The x-coordinate of the point
		 * @param ay	The y-coordinate of the point
		 * @param bx	The x-coordinate of the rectangle
		 * @param by	The y-coordinate of the rectangle
		 * @param w		The width of the rectangle
		 * @param h		The height of the rectangle
		 *
		 * @return True if the point lies within the rectangle, false otherwise.
		 */
		static bool point_in_rect( int ax, int ay, int bx, int by, int w, int h );

		/**
		 * This method checks whether or not a specified rectangle lies within another specified rectangle.
		 *
		 * @param x1	The x-coordinate of the top-left corner of the first rectangle.
		 * @param y1	The y-coordinate of the top-left corner of the first rectangle.
		 * @param x2	The x-coordinate of the bottom-right corner of the first rectangle.
		 * @param y2	The y-coordinate of the bottom-right corner of the first rectangle.
		 * @param xp2	The x-coordinate of the top-left corner of the second rectangle.
		 * @param yp2	The y-coordinate of the top-left corner of the second rectangle.
		 * @param xp1	The x-coordinate of the bottom-right corner of the second rectangle.
		 * @param yp1	The y-coordinate of the bottom-right corner of the second rectangle.
		 *
		 * @return True if the first rectangle lies within the second rectangle, false otherwise.
		 */
		static bool rect_in_rect(int x1, int y1, int x2, int y2, int xp2, int yp2, int xp1, int yp1);

};

#endif /* UTIL_HPP_ */
