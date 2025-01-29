#ifndef BRAIN_HPP_
#define BRAIN_HPP_

#include <vector>
#include <iostream>
#include <cmath>

#include "nn/nn.h"

using namespace std;

/*
 * TODO: move members to private where possible and use getters / setters!
 */


/**
 *
 * This class is a wrapper for the library "nn".
 *
 * It allows us to initialize a neural network of arbitrary size, set inputs
 * and fetch output.
 * It is also possible to directly get and set the weights matrix directly.
 *
 * @author Fabian Schläpfer
 * @date March 26th 2010
 * @see nn/nn.h
 *
 */

class Brain {

	public:

		/**
		 * The constructor initializes the neural network using the information
		 * given as arguments
		 *
		 * @param ntype 	Type of neural network
		 * @param afunc 	Activation function of all neurons in the network
		 * @param bias 		Whether or not to use a bias input neuron
		 * @param struc 	An array where each element specifies the number of
		 * 					neurons in that layer (the first element
		 * 					corresponds to the input layer,
		 * 					the last one to the output layer)
		 *
		 * @see nn/nn.h
		 */
		Brain( net_type ntype, activation_function afunc, bool bias, unsigned int *struc, unsigned int num_layers );

		/**
		 * Set the weights matrix. The number of rows and cols of this
		 * matrix must correspond to the number given by get_cols()
		 * and get_rows() of this class.
		 * The matrix is deleted in this method.
		 *
		 * @param matrix	The new matrix represented by a two-dimensional array of doubles of size rows*cols.
		 */
		void set_matrix( double **matrix );

		/**
		 * Get the weights matrix represented by a two-dimenstional array of doubles.
		 * The number of rows and cols of this matrix will correspond to the
		 * values returned by get_cols() and get_rows() of this class.
		 *
		 * @return A two-dimensional array of size rows*cols, representing the weights matrix.
		 */
		double **get_matrix( void );

		/**
		 * Set the inputs for the neural network..
		 *
		 * @param inputs	The inputs of the neural network, represented as a vector of doubles
		 * 					of the size of the number of neurons in the output layer.
		 */
		void set_inputs( vector<double> &inputs );

		/**
		 * Get the output of the neural network, represented by a vector of
		 * doubles of the size of the number of neurons in the output layer.
		 *
		 * @return A vector of doubles representing the output neurons' values.
		 */
		vector<double> get_output( void );

		/**
		 * Get the number of columns of the weights matrix.
		 *
		 * @return The number of columns of the weights matrix
		 */
		double get_cols( void );

		/**
		 * Get the number of rows of the weights matrix.
		 *
		 * @return The number of rows of the weights matrix
		 */
		double get_rows( void );


		/**
		 * Dumps the neural network to stdout by calling nn_dump_all().
		 */
		void dump( void );

		/**
		 * Propagate data through the neural net, one layer at a time, "steps" times.
		 *
		 * @param steps		The number of steps to propagate forward.
		 */
		void think( unsigned int steps );

		void check_weights_for_nan( void );

		/**
		 * A pointer to the actual neural network of the "nn" library.
		 */
		net *nn;

		/**
		 * Number of inputs of the neural network.
		 */
		unsigned int num_inputs;

		/**
		 * Number of outputs of the neural network.
		 */
		unsigned int num_outputs;

		/**
		 * Number of layers in the neural network.
		 */
		unsigned int num_layers;

	private:

		/**
		 * Number of columns in the weights matrix.
		 *
		 * @see #get_cols
		 */
		unsigned int cols;

		/**
		 * Number of rows in the weights matrix.
		 *
		 * @see #get_rows
		 */
		unsigned int rows;

		/**
		 * A vector of doubles representing the input values.
		 *
		 * @see #get_inputs
		 * @see #set_inputs
		 */
		vector<double> inputs;

		/**
		 * A vector of doubles representing the output values.
		 *
		 * @see #get_output
		 * @see #set_output
		 */
		vector<double> output;

};


#endif /* BRAIN_HPP_ */
