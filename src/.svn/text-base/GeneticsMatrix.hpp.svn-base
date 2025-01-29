#ifndef GENETICSMATRIX_HPP_
#define GENETICSMATRIX_HPP_

#include <iostream>
#include <exception>
using namespace std;

#include "Util.hpp"

/**
 * This class provides methods for manipulation of two-dimensional arrays of double representing weights matrices using genetic operators.
 *
 * Operators include one-point crossover of two elements resulting in either one or two children and
 * mutation (turn connections on or off or modify weights).
 *
 * @author Fabian Schläpfer
 * @date March 26th 2010
 *
 */

class GeneticsMatrix {

public:

	/**
	 * The constructor sets the mutation- and crossover-rates and value ranges for future operations.
	 *
	 * @param mutation_rate			The probability of mutating a matrix: 0 <= x <= 1
	 * @param mutation_rate_element	The probability of mutating an element of the matrix: 0 <= x <= 1
	 * @param crossover_rate		The probability of a crossover: 0 <= x <= 1
	 *
	 * @param min					The minimum value of a weight in the matrix.
	 * 								@see #random_value
	 * 								@see #random_matrix
	 * @param max					The maximum value of a weight in the matrix.
	 * 								@see #random_value
	 * 								@see #random_matrix
	 * @param factor				The factor to multiply a mutation with (smaller=less impact, bigger=more impact)
	 * 								@see #mutate
	 */
	GeneticsMatrix( double mutation_rate, double muation_rate_element, double crossover_rate, double min, double max, double factor );

	/**
	 * Perform a one-point crossover of mum and dad, resulting in one child "child1"
	 * with a probability of crossover_rate.
	 *
	 * @param mum		A two-dimensional array of doubles
	 * @param dad		A two-dimensional array of doubles
	 * @param child1	A pointer to a two-dimensional array of doubles
	 */
	void crossover( double **mum, double **dad, double ***child1 );

	/**
	 * Perform a one-point crossover of mum and dad, resulting in two children "child1" and "child2"
	 * with a probability of crossover_rate.
	 *
	 * @param mum		A two-dimensional array of doubles
	 * @param dad		A two-dimensional array of doubles
	 * @param child1	A pointer to a two-dimensional array of doubles
	 * @param child2	A pointer to a two-dimensional array of doubles
	 */
	void crossover( double **mum, double **dad, double ***child1, double ***child2 );

	/**
	 * Generate a random two-dimensional array of doubles
	 *
	 * @return A two-dimensional array of doubles containing random values within the range: min_val <= x <= max_val
	 * @see #random_value
	 */
	double **random_matrix( void );

	/**
	 * Dump a two-dimensional array of doubles to stdout.
	 *
	 * @param m		The two-dimensional array of doubles to dump
	 */
	void dump( double **m );

	/**
	 * Set the size of the two-dimensional arrays of doubles.
	 *
	 * @param cols	The number of columns
	 * @param rows	The number of rows
	 */
	void set_size( unsigned int cols, unsigned int rows );

	/**
	 * The number of rows of the two-dimensional arrays.
	 */
	unsigned int rows;

	/**
	 * The number of columns of the two-dimensional arrays.
	 */
	unsigned int cols;




	/**
	 * The mutation rate to use.
	 *
	 * @see #mutate
	 */
	double mutation_rate;

	/**
	 * The mutation rate of an element to use.
	 *
	 * @see #mutate
	 */
	double mutation_rate_element;

	/**
	 * The crossover rate to use.
	 *
	 * @see #crossover
	 */
	double crossover_rate;

	/**
	 * The mutation factor to use.
	 *
	 * @see #mutate
	 */
	double mfactor;

	/**
	 * The minimum value of a weight.
	 *
	 * @see #random_matrix
	 * @see #random_value
	 */
	double min_val;

	/**
	 * The maximum value of a weight.
	 *
	 * @see #random_matrix
	 * @see #random_value
	 */
	double max_val;





	/**
	 * Mutate a two-dimensional array of doubles.
	 * With a probability of mutation_rate, this method either sets weights to zero, inverts them (*(-1))
	 * or simply adds or substracts a random value with a probability of mutation_rate_element.
	 *
	 * @param matrix	The two-dimensional array of doubles to mutate.
	 *
	 * @see #random_value
	 */
	void mutate( double ***matrix );


	/**
	 * Mutate the port range
	 */
	pair <unsigned short , unsigned short> mutate_port_range( unsigned short start , unsigned short end , int popsize);

private:

	/**
	 * Return a random value within the range: min_val <= x <= max_val
	 */
	double random_value( void );

};

#endif /* GENETICSMATRIX_HPP_ */
