#ifndef NWAlign_H
#define NWAlign_H

#include <vector>

using namespace std;

/**
 * 
 * NWAligner impliments Needleman-Wuncsh global sequence alignment,
 * which can be used to find the optimal alignment of two strings
 * for a given cost function.
 * The alignment system is non-symbolic in this case, and uses 
 * real-valued inputs. The cost for aligning two locations is the
 * distance between them, thus alignments between the same values
 * is free. 
 * 
 * @Author John Doucette
 * @Date June 11th 2010
 */
class NWAligner{
protected:
	
	/**
	 *
	 * The cost of inserting a gap in the alignment.
	 * Should be equal to the maximum difference between to "similair" aleals. 
	 */
	double gap_cost; 
	
	/**
	 * NWAlign returns the cost of the optimal alignment between v1 and v2.
	 * @param v1 	Avector of doubles containing at least one element.
         *		Unchanged during the execution of the method.
  	 * @param v2 	A vector of doubles containing at least one element. 
	 *		Unchanged during the execution of the method.
         * @return 	The cost of the minimum alignment between v1 and v2 interpreted
	 *		as sequences, given the gap cost of this instantation of the class.
	 */
	double NWAlign(vector<double> &v1, vector<double> &v2);
public:
	
	/**
	 * Constructs a new NWAligner with the given gap cost.
	 * @param gc 	A positive, non-zero double describing the cost of aligning an
	 * 		element of either sequence with a gap character. Note that this
	 *		will be used in the sequence in any case where the difference in
	 *		two values exceeds 2*gc, and as such, should be selected carefully. 
	 */
	NWAligner(double gc);
	
	/**
	 * Find the cost of the minimum alignment of two genomes of equal length.
	 * Not for use with genomes of varriable length, although with suitable padding 
         * during preprocessing, it is possible to get around this constraint.
 	 * @param d1 	A pointer to an array containing at least l1 double values.
	 *		Only the first l1 values will be used in alignment.
	 * @param d2	A pointer to an array containing at least l2 double values
	 *		Only the first l2 values will be used in alignment.
	 * @param l1	The size of the first array to be used in alignment, d1.
	 * @param l2	The size of the second array to be used in alignment, d2.
	 * @return 	The minimum cost of alignment for the two genomes, d1 and d2.
	 */
	double align_genomes(double *d1, double *d2, int l1, int l2);

	/**
	 * Find the cost of the minimum alignment of two phenotypes of varriable lengths.
	 * Phenotypes are vectors generated during the execution of an instance of the 
	 * Individual class.
	 * @see Individual.hpp
	 * @param b1	An array containing at least l1 double values. Only the first
	 *		l1 values will be used during Alignment.
	 * @param b2	An array containing at least l2 double values. Only the first
	 *		l2 values will be used during Alignment.
	 * @param l1	The size of the first array to be used in alignment, d1.
	 * @param l2	The size of the second array to be used in alignment, d2.
	 * @return 	The cost of the best possible alignment of d1 and d2.
	 */
	double align_behaviors(double b1[], double b2[], int l1, int l2);

	/**
	 * Align two one dimensional vectors of integers
	 * @param v1	A reference to a vector containing at least one element.
	 *		The vector should store only integers.
	 * @param v2	A reference to a vector containing at least one element.
	 *		The vector should store only integers.
	 * @return 	The cost of the best possible alignment of v1 and v2.
	 */
	double NWAlign(vector<int> &v1, vector<int> &v2);
	
	/**
	 * print the alignment matrix at any point during alignment.
	 * Useful for debugging the alignment methods, but usually not used.
 	 * @param D	A pointer to a l1xl2 matrix represented as a 1 dimensional
	 *		array of doubles.
	 * @param x	A reference to the vector used for alignment along the 
	 *		vertical axis of the matrix D.
  	 * @param y	A reference to the vector used of alignment along the 
	 * 		horozontal axis of the matrix D.
	 * @param l1	The length of x, and thus the vertical size of D.
	 * @param l2	The length of y, and thus the horozontal size of D.
	 */
	void print(double *D, vector<double> &x, vector<double> &y, int l1, int l2);
};

#endif
