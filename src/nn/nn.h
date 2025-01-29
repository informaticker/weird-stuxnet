/*      nn.h
*       Mischa Lehmann
*       ducksource@duckpond.ch
*       Version:1.0
*
*       neruon_net
*       Require:
*		- stdio.h
*		- stdlib.h
*		- sting.h
*		- time.h
*		- math.h
*		- stdarg.h
*		- string.h
*		- utils.h
* 		
*
*
*       Licence:
*       You're allowed to edit and publish my source in all of your free and open-source projects.
*       Please send me an e-mail if you'll implement this source in one of your commercial or proprietary projects.
*       Leave this Header untouched!
*
*       Warranty:
*       Warranty void if signet is broken
*       =================== / /===================
*       [       Warranty   / /   Signet          ]
*       ==================/ /=====================
*	!!NOWAY!!
*
*/

/*==============================*/
/*             TODO's           */
/*==============================*/
/*
*
*
*
*
*
*
*/

/*==============================*/
/*           includes           */
/*==============================*/
#ifndef _NN_H
	#define _NN_H 1

	/*all Libraries*/
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdarg.h> // variable argument-lenghts for functions
	#include <time.h>
	#include <math.h>
	#include "../utils/utils.h"
	#include "../inc_stdint.h" // include standard integer

/*==============================*/
/*  System dependend stuff      */
/*==============================*/
#ifndef CURSystem
	#ifdef __unix__ 
		/*Unix-Systes*/
		#define CURSystem printf("Hello Unix-user\n You're elite!\n");
	#elif __BORLANDC__ && __MSDOS__
		/*Borland c-compiler and MSDOS*/
		#define CURSystem printf("Hello BorlandC-complier or MSDOS user!\n");
	#elif __WIN32__ || _MSC_VER
		/*Windows 32-Syste or visual C*/
		#define CURSystem printf("Hello windows user!\n");
	#else
		#define CURSystem printf("This programm is not compatible with your System, sorry -> EXIT\n");return EXIT_FAILURE;
	#endif
#endif

/*==============================*/
/*enums declaration + definition*/
/*==============================*/

#define NN_VERSION 1			// the version of the neuron-net
#define NN_DEBUG 0			// debug mode?

#define FILE_DATA_FILEDS 9		// the amount of different data in the nn-file

#define BIAS_VAL 1			// the value for the bias neuron
#define SIGMA_RANGE 1 			// The range of the normal sigmoid function (from 0)
#define SIGMA_STRETCH_RANGE 2 		// The range of the stretched sigmoid function (around 0)

#define NN_WEIGHTS_RND_RANGE 4	// the weights random range (eg. 2 == -1<x<1)

#define APPLY_NETSTRUCT_AFTER_LEARNING 1 // apply the netstructure after learning
#define APPLY_NETSTRUCT_AFTER_SETMATRIX 1 // apply the netstructure after a setmatrix-call

#define GAUSS_CENTER 0		// Center of the Gauss-afunc
#define GAUSS_DEVIATION 1500 //700 //75	// Deviation of the Gauss-afunc
#define GAUSS_HEIGHT 100	// Height of the Gauss-afunc

typedef enum{
	FeedForward=1, 			// this is a usual feed forward net (only connect neurons with the layer above)
	Recurrent,			// this is a recurrent net
	ZeroOne,			// Weights only Zero or One (if weight <= 0 then 0 else 1)
	Dontcare,			// don't care about the netstructure (init all weights with a random value)
	Zero,				// init all weights with zero
}net_type;

typedef enum{
	Delta=1,			// delta learning
	Backpropagation, 		// backpropagation learning
}learn_type;

typedef enum{
	ABOVE_ZERO=1,			// activation function :=  if value > 1 then 1
	SIGMA,				// afunc := Sigmoid function
	SIGMA_STRETCH,			// afunc := Sigmaoid from -1 to 1
	GAUSS,			// afunc := Gauss (see GAUSS_DEFINES)
 	ONE_TO_ONE,			// not defined activation function
}activation_function;

/*==============================*/
/*     global declarations      */
/*==============================*/

typedef struct {
	unsigned int data_c;	// amount of data in the dataset
	double ***input_data;	// input data
	double ***output_data;	// output data
}data;

typedef struct{
	double bigerr_val;	// the (absolute) biggest error in from one dataset in a think-run
	unsigned int bigerr_i;	// the index of the dataset producing the biggest error
	double error; 		// the error-sum of the net in one think run
}statistics;

typedef struct {
	boolean init;		// is the net completely initialized

	boolean matrix_init;	// is there a matrix?
	double **matrix;	// the weight-matrix

	/*types*/
	learn_type ltype; 	// the learning function
	net_type ntype;		// the nettype
	activation_function afunc; // the used activision function

	/*thinking*/
	unsigned int age;	// the "age" (steps of thinking)
	unsigned int inputs_t;	// inputs threwen (if piped input should grow with the same speed as age does!)
	unsigned int curr_data;	// the current input data at the output_layer
	unsigned int curr_layer;// the current processing layer
	double *input;		// the input array
	double *output;		// the output array
	double *ll_output;	// the output of the last layer
	boolean i_o_init; 

	/*learning*/
	boolean learn;		// should the net learn?
	boolean batch;		// batch learning ? (learn after all input samples has threwen and propagated)
	boolean lmatrix_init;	// is there a learn_matrix
	double **learn_matrix;	// temporary learn matrix 
	boolean data_present;	// is there data for learning?
	data *ndata;		// datasets for learning something
	double lconst;		// learn_constant

	/*data input*/
	boolean shuffle; 	// should the net shuffle the data
	boolean i_o_map_init; 	// is there a input->output mapping?
	boolean *threwen;	// mark the input as threwen
	unsigned int *i_o_map;	// map input to output data (if data comes shuffled)
	boolean piped_input; 	// piped input (means if true == do threw the new input step by step)
	double **r_input;	// remember the input vector (set if piped_input == false) with a x size of layers-1 (layers-1 = do not save net-output_data)
	double **r_output;	// remember the output vector (set if piped_input == false) with a x size of layers-1 (layers-1 = do not save net-input_data)

	unsigned int layers; 	// layers in the net
	unsigned int *struc; 	// the structure of the net (array_size=layers)
			     	// struc[0] = size of input-array
	unsigned int ll_output_c;// struc[layers-1] = size of output-array
			     	// struc[0] + struc[1] + struc[2] + ....  + struc[layers-1] = amount of neurons
	unsigned int neurons; 	// (see above)
	unsigned int *ntol; 	// map neurons to a layer

	boolean bias; 		// is there a bias neuron?

	/*statistics*/
	boolean mk_stats;	// make statistics?
	statistics *stats;	// fill up the statistics struct
}net;

/*==============================*/
/*    function declarations     */
/*==============================*/
/*
*	nn_init_v(...): init a neural-network and give the netstrucutre as a variabel argument
*	Arguments:
*		- nn: The dataholding net-object
*		- afunc: The activating-function 
*		- ntype: The network type (Feed-forward,recurrent)
*		- ltype: The learnalgorithm
*		- bias: Is there a Bias?
*		- piped_input: Make piped input (used for time depending data)
*		- stats: Make stats?
*		- layers: Amount of Layers
*		-  ... : Specify the amount of neurons in each layer
*/
int nn_init_v(net *nn,activation_function afunc,net_type ntype,learn_type ltype,boolean bias,boolean piped_input,boolean stats, unsigned int layers, ...);

/*
*	nn_init(...): init a neural-network
*	Arguments:
*		- nn: The dataholding net-object
*		- afunc: The activating-function 
*		- ntype: The network type (Feed-forward,recurrent)
*		- ltype: The learnalgorithm
*		- bias: Is there a Bias?
*		- piped_input: Make piped input (used for time depending data)
*		- stats: Make stats?
*		- layers: Amount of Layers
*		- struc : Specify the amount of neurons in each layer
*/
int nn_init(net *nn,activation_function afunc,net_type ntype,learn_type ltype,boolean bias,boolean piped_input,boolean stats, unsigned int layers, unsigned int *struc);

/*
*	nn_kill(net *nn): kills a neural-network
*	Arguments:
*		- nn: The dataholding net-object
*/
int nn_kill(net *nn);

/*
*	nn_load_from_file(net *nn,char *filename): load a nn from a file
*	Arguments:
*		- nn: The dataholding net-object
*		- filename: Path to the file
*/
int nn_load_from_file(net *nn,char *filename);

/*
*	nn_save_to_file(net *nn,char *filename): save a nn to a file
*	Arguments:
*		- nn: The dataholding net-object
*		- filename: Path to the file
*/
int nn_save_to_file(net *nn,char *filename);

/*
*	nn_copy(net *dst,net *src): copy a neural network from source to destination
*	Arguments:
*		- dst: the destination
*		- src: the source
*/
int nn_copy(net *dst,net *src);

/*
*	nn_rnd_matrix(net *nn): Randomize the weight-matrix
*	Arguments:
*		- nn: The dataholding net-object
*/
int nn_rnd_matrix(net *nn);

/*
*	nn_set_matrix(net *nn,double *matrix_data): Set a new matrix
*	Arguments:
*		- nn: The dataholding net-object
*		- *matrix_data: the matrix data as an array
*/
int nn_set_matrix(net *nn,double *matrix_data);

/*
*	nn_set_matrix_2d(net *nn,double **matrix_data): Set a new matrix
*	Arguments:
*		- nn: The dataholding net-object
*		- **matrix_data: The matrix data 
*/
int nn_set_matrix_2d(net *nn,double **matrix_data);

/*
*	nn_set_neurons_v(net *nn,double *input,unsigned int size, ... ): Set neurons
*	to a input value
*	Arguments:
*		- nn: The dataholding net-object
*		- *input: From here came all the input-values
*		- size: size of the input-array
*		- ...: maps the inputs to a neuron of the net 
*/
int nn_set_neurons_v(net *nn,double *input,unsigned int size, ... );

/*
*	nn_set_neurons(net *nn,double *input,unsigned int size,unsigned int *pos): Set neurons
*	to a input value
*	Arguments:
*		- nn: The dataholding net-object
*		- *input: From here came all the input-values
*		- size: size of the input-array
*		- *pos: maps the inputs to a neuron of the net (if pos == NULL set the neurons one by one)
*/
int nn_set_neurons(net *nn,double *input,unsigned int size,unsigned int *pos);

/*
*	nn_get_output(net *nn,double **output): Get the output of the neurons 
*	at the last layer
*	Arguments:
*		- nn: The dataholding net-object
*		- **output: Pointer to the output-array
*/
int nn_get_output(net *nn,double **output);

/*
*	nn_think(net *nn,unsigned int steps,boolean learn): Forward process data & learn
*	Arguments:
*		- nn: The dataholding net-object
*		- steps: Amount of forward-propagation-steps
*		- learn: shall the net learn?
*/
int nn_think(net *nn,unsigned int steps,boolean learn);

/*
*	nn_set_learnmode(net *nn,boolean learn,double lconst,boolean batch): Toggles learning (B/O) and
*	set a learnconstant 
*	Arguments:
*		- nn: The dataholding net-object
*		- learn: Toggle learning (ON/OFF)
*		- lconst: Set the learnconstant
*		- batch: Set Batch = True Or Online learning = False
*/
int nn_set_learnmode(net *nn,boolean learn,double lconst,boolean batch);

/*
*	nn_set_datasets(net *nn,data *ndata,boolean shuffle): Set Datasets
*	Arguments:
*		- nn: The dataholding net-object
*		- ndata: The data holding stucture
*		- shuffle: shall the net shuffle the input?
*/
int nn_set_datasets(net *nn,data *ndata,boolean shuffle);

/*
*	nn_unset_datasets(net *nn): Unet Datasets
*	Arguments:
*		- nn: The dataholding net-object
*/
int nn_unset_datasets(net *nn);

/*dumpfunctions go away after testing*/
int nn_dump_matrix(net *nn);
int nn_dump_all(net *nn);

#endif
