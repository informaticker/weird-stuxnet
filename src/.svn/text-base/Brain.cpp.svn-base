#include "Brain.hpp"

Brain::Brain( net_type ntype, activation_function afunc, bool bias, unsigned int *struc, unsigned int num_layers ) {

	this->num_layers = num_layers;
	this->rows = 0;
	this->cols = 0;

	this->num_inputs = struc[0];
	this->num_outputs = struc[num_layers-1];

	nn = new net;
	nn_init( nn, afunc, ntype, Delta, bias, true, false, num_layers, struc );

	// get matrix dimensions
	rows = get_rows();
	cols = get_cols();

}

void Brain::set_matrix( double **matrix_data ) {

	// set matrix
	nn_set_matrix_2d( nn, matrix_data );

	// set zeros
	//nn_apply_type( nn );

	// DEBUG PRINT WEIGHTS
//	if( DEBUG_LEVEL > 4 ) {
//		cout << "##########################" << endl;
//		cout << "# Neural network weights:" << endl;
//		cout << "##########################" << endl;
//		for( unsigned int i=0; i<rows; i++ ) {
//			for( unsigned int j=0; j<cols; j++ ) {
//				cout << matrix_data[i][j] << "\t";
//			}
//			cout << endl;
//		}
//	}

	// free the matrix
	for( unsigned int i=0; i<rows; i++ ) delete [] matrix_data[i];
	delete [] matrix_data;
}

void Brain::set_inputs( vector<double> &inputs ){

	double *arr = new double[inputs.size()];
	for( unsigned int i=0; i<inputs.size(); i++ ) {
		arr[i] = inputs[i];
	}
	
	unsigned int *pos = new unsigned int[inputs.size()];
	for( unsigned int i=0; i<inputs.size(); i++ ) pos[i] = i;

	nn_set_neurons( nn, arr, inputs.size(), pos );

	delete [] arr;
	delete [] pos;

}

double** Brain::get_matrix( void ) {
	return nn->matrix;
}

vector<double> Brain::get_output( void ){
	return output;
}

double Brain::get_cols( void ) {
	if( cols == 0 ) cols = nn->neurons;

	return cols;
}

double Brain::get_rows( void ) {
	if( rows == 0 ) rows = nn->neurons;

	return rows;
}

void Brain::dump( void ) {
	nn_dump_all( nn );
}

void Brain::think( unsigned int steps ) {

	if( DEBUG_LEVEL > 3 ) cout << "calling nn_think" << endl;

	// think
	nn_think( nn, steps, false );

	if( DEBUG_LEVEL > 3 ) cout << "clearing output" << endl;

	// clear output
	output.clear();

	if( DEBUG_LEVEL > 3 ) cout << "setting output 1" << endl;

	// get pointer to output array
	double *out;
	nn_get_output( nn, &out );


	if( DEBUG_LEVEL > 3 ) cout << "setting output 2" << endl;

	// and store it to the output vector vector
	for( unsigned int i=0; i<(nn->ll_output_c); i++ ) {
		output.push_back( out[i] );
	}

	if( DEBUG_LEVEL > 3 ) cout << "Brain::think done" << endl;

}

void Brain::check_weights_for_nan( void ) {
	for (unsigned int i=0; i<rows; i++) {
		for (unsigned int j=0; j<cols; j++) {
			if (isnan(nn->matrix[i][j])) {
				cerr << "nn->matrix[i=" << i << "][j=" << j << "] is NaN!" << endl;
			}
		}
	}
}
