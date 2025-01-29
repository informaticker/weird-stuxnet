/*
 * File: GeneticsMatrix.cpp
 *
 * Project: eco
 *
 * * * * * * * * * * * * * * *
 *
 *  Created on: Mar 26, 2010
 *      Author: fabian
 *
 */

#include "GeneticsMatrix.hpp"


GeneticsMatrix::GeneticsMatrix( double mutation_rate, double mutation_rate_element, double crossover_rate, double min, double max, double factor ) {
	this->rows = 0;
	this->cols = 0;

	this->mutation_rate = mutation_rate;
	this->mutation_rate_element = mutation_rate_element;
	this->crossover_rate = crossover_rate;

	this->min_val = min;
	this->max_val = max;

	this->mfactor = factor;

}

void GeneticsMatrix::set_size( unsigned int cols, unsigned int rows ) {
	this->rows = rows;
	this->cols = cols;
}

double GeneticsMatrix::random_value( void ) {
	return Util::randdouble( min_val, max_val );
}

double **GeneticsMatrix::random_matrix( void ) {
	double **matrix;

	// allocate new matrix
	matrix = new double*[rows];
	for( unsigned int i=0; i<rows; i++ ) matrix[i] = new double[cols];

	// fill with random values -> Index 1 /= 100'000
	for( unsigned int i=0; i<rows; i++ ) {
		for( unsigned int j=0; j<cols; j++ ) {
			if( (rand() % 3) == 0 ){
				// unixtime? divide by 100k
				if(  j == 9 ) {
					matrix[i][j] = (double)(random_value()) / 100000;
				} else {
					matrix[i][j] = random_value();
				}
			}
			else{
				matrix[i][j] = 0;
			}
		}
	}

	return matrix;
}

void GeneticsMatrix::mutate( double ***matrix ) {

	// go through the entire matrix and check if we should mutate
	for( unsigned int i=0; i<rows; i++ ) {
		for( unsigned int j=0; j<cols; j++ ) {
			if( rand() < mutation_rate_element ) {
				int r = rand() % 3;

				// intentioally not keeping track of value bounds
				if( r == 0 ) (*matrix)[i][j] += Util::randdouble( min_val, max_val ) * mfactor; // mutate weight
				else if( r == 1 ) (*matrix)[i][j] *= -1; // switch positive / negative
				else if( r == 2 ) (*matrix)[i][j] = 0; // turn off connection
			}
		}
	}
}

pair <unsigned short , unsigned short> GeneticsMatrix::mutate_port_range( unsigned short start , unsigned short end , int popsize) {

	pair <unsigned short , unsigned short> p_orig(start,end);
	pair <unsigned short , unsigned short> p;
	int r = rand() % 2;

	if (r==0) start++;
	else if (r==1) start--;

	r = rand() % 2;

	if (r==0) end++;
	else if (r==1) end--;

	if ((end - start) >= popsize) {
		p.first = start;
		p.second = end;
	} else {
		p.first = p_orig.first;
		p.second = p_orig.second;
	}

	return p;
}

void GeneticsMatrix::crossover( double **mum, double **dad, double ***child1, double ***child2 ) {

	unsigned int cp_col = rand() % cols;
	unsigned int cp_row = rand() % rows;

	if( rand() < crossover_rate ) { // crossover?
		// crossover at crossover point
		for( unsigned int i=0; i<rows; i++ ) {
			for( unsigned int j=0; j<cols; j++ ) {
				if( i < cp_col && j < cp_row ) {
					(*child1)[i][j] = mum[i][j];
					(*child2)[i][j] = dad[i][j];
				} else {
					(*child1)[i][j] = dad[i][j];
					(*child2)[i][j] = mum[i][j];
				}
			}
		}
	} else {
		// no crossover, mum = child1, dad = child2
		for( unsigned int i=0; i<rows; i++ ) {
			for( unsigned int j=0; j<cols; j++ ) {
				(*child1)[i][j] = mum[i][j];
				(*child2)[i][j] = dad[i][j];
			}
		}
	}

	// mutations
	if( drand48() < mutation_rate ) mutate( child1 );
	if( drand48() < mutation_rate ) mutate( child2 );

}

void GeneticsMatrix::crossover( double **mum, double **dad, double ***child1 ) {

	unsigned int cp_col = rand() % cols;
	unsigned int cp_row = rand() % rows;

	if( rand() < crossover_rate ) { // crossover?
		// crossover at crossover point
		for( unsigned int i=0; i<rows; i++ ) {
			for( unsigned int j=0; j<cols; j++ ) {
				if( i < cp_col && j < cp_row ) {
					(*child1)[i][j] = mum[i][j];
				} else {
					(*child1)[i][j] = dad[i][j];
				}
			}
		}
	} else { // no crossover

		// select dad or mum
		double **parent = NULL;
		if( (rand() % 2) == 0 ) parent = mum;
		else parent = dad;

		for( unsigned int i=0; i<rows; i++ ) {
			for( unsigned int j=0; j<cols; j++ ) {
				(*child1)[i][j] = parent[i][j];
			}
		}
	}

	// mutations
	if( drand48() < mutation_rate ) mutate( child1 );

}

void GeneticsMatrix::dump( double **m ) {

	cout << "# Dumping matrix:" << endl;
	for( unsigned int j=0; j<cols; j++ ) {
		for( unsigned int i=0; i<rows; i++ ) {
			cout << m[i][j] << "\t";
		}
		cout << endl;
	}
	cout << endl;

}
