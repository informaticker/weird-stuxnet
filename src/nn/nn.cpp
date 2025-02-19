/*      nn.c
*       Mischa Lehmann
*       ducksource@duckpond.ch
*       Version:1.0
*
*	neuron_net
*       Require:
*		- see header
*
*
*	Licence:
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
*	TODO: see mk_statistics
*	TODO: Bias adds itself to netstructure
*	TODO: File saving ensure portability (e.g. int)
*
*
*
*
*/

/*==============================*/
/*           includes           */
/*==============================*/
#include "nn.h"
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

/*==============================*/
/*     global declarations      */
/*==============================*/

/*==============================*/
/*    function declarations     */
/*==============================*/

/*
*	nn_apply_type(net *nn): Applies the Network-Type to the Matrix (e.g. Feed-forward)
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_apply_type(net *nn);

/*
*	nn_map_layer(net *nn): Maps each Neuron to a layer in the Net (with the struc-Array)
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_map_layer(net *nn);

/*
*	n_count_neurons(net *nn): Count the neurons given in the struc-Array
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_count_neurons(net *nn);

/*
*	nn_mk_remember(net *nn): Init the remember-Matrix which stores the net-outputs and inputs
*	Arguments:
*		- nn
*/
static int nn_mk_remember(net *nn);

/*
*	nn_del_remember(net *nn): Kills (frees) the remember-Matrix
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_del_remember(net *nn);

/*
*	nn_malloc_matrix(net *nn,double ***matrix): Malloc a weight-matrix specified in the net-object
*	Arguments:
*		- nn: The dataholding net-object
*		- ***matrix: A pointer to the Matrix for malloc
*/
static int nn_malloc_matrix(net *nn,double ***matrix);

/*
*	nn_mk_matrix(net *nn): Sets up a weight-matrix
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_mk_matrix(net *nn);

/*
*	nn_del_matrix(net *nn): Delets the weight-matrix
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_del_matrix(net *nn);

/*
*	nn_map_layer(net *nn)
*	Arguments:
*		- nn: The dataholding net-object
*/


/*dumpfunctions back here*/

/*
*	nn_dump_matrix(net *nn): Dump the matrix of the net-object
*	Arguments:
*		- nn: The dataholding net-object
*/
//static int nn_dump_matrix(net *nn);

/*
*	nn_dump_all(net *nn): Dump the input-array the weight-matrix and the output-array
*	Arguments:
*		- nn: The dataholding net-object
*/
//static int nn_dump_all(net *nn);

/*
*	nn_data_input(net *nn): Input the next dataset (if there is data specified)
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_data_input(net *nn);

/*
*	nn_init_i_o_map(net *nn): Init the matrix which maps the output to an input
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_init_i_o_map(net *nn);

/*
*	mk_statistics(net *nn): Init the statistic-module
*	Arguments:
*		- nn: The dataholding net-object
*/
static int mk_statistics(net *nn);

/*
*	reset_statistics(net *nn): Reset all the statistics
*	Arguments:
*		- nn: The dataholding net-object
*/
static int reset_statistics(net *nn);

/*
*	get_curr_data(net *nn): Get the Input-data for the current output-data of the net
*	Arguments:
*		- nn: The dataholding net-object
*/
static int get_curr_data(net *nn);

/*
*	nn_activate(net *nn): Process activating-function for the output-Array
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_activate(net *nn);

/*
*	static int nn_ractivate(net *nn,double *input,double **output): Make 1'st derivative of the
*	activiating function
*	Arguments:
*		- nn: The dataholding net-object
*		- *input: A net-output
*		- **output: A pointer to the 1'st derivative of the afunc of the input 
*/
static int nn_ractivate(net *nn,double *input,double **output);

/*
*	nn_backpropagation(net *nn): Make backpropagation-learning
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_backpropagation(net *nn);

/*
*	nn_learn(net *nn): process the selected learnalgorithm
*	Arguments:
*		- nn: The dataholding net-object
*/
static int nn_learn(net *nn);

/*==============================*/
/*     function definitions     */
/*==============================*/

int nn_init_v(net *nn,activation_function afunc,net_type ntype,learn_type ltype,boolean bias,boolean piped_input,boolean stats, unsigned int layers, ...){
	unsigned int i;
	va_list argptr; // ptr to the list of arguments
	unsigned int *struc; // the structure

	if(layers < 2){
		printerr("nn_init_v(): you should have at least 2 layers! (output/input)",0);
		return -1;
	}

	struc = (unsigned int *)malloc(sizeof(unsigned int) * layers);

	/*readout arguments*/
	va_start(argptr,layers); // goto start of list
	for(i=0;i<layers;i++){
		struc[i] = (unsigned int)va_arg(argptr,unsigned int); // read argument by argument
	}
	va_end(argptr); // free arguments

	/*and init the net!*/
	if(nn_init(nn,afunc,ntype,ltype,bias,piped_input,stats,layers,struc)<0){
		printerr("nn_init_v(): nn_init-Failed!",0);
		return -1;	
	}

	free(struc);

	return 1;
}

int nn_init(net *nn,activation_function afunc,net_type ntype,learn_type ltype,boolean bias,boolean piped_input,boolean stats, unsigned int layers, unsigned int *struc){
	static boolean rand_init = false;

	if(layers < 2){
		printerr("nn_init(): you should have at least 2 layers! (output/input)",0);
		return -1;
	}

	 /* initialize random */
	if(rand_init == false){
		srand(time(NULL));
		rand_init = true;
	}

	/*sets*/
	nn->data_present = false;
	nn->i_o_map_init = false;
	nn->shuffle = false;
	nn->layers = layers;
	nn->ntype = ntype;
	nn->ltype = ltype;
	nn->afunc = afunc;
	nn->bias = bias;
	nn->piped_input = piped_input;
	nn->mk_stats = stats;
	nn->curr_data = 0;
	nn->curr_layer = 0;
	nn->inputs_t = 0;
	nn->batch = false;
	nn->lmatrix_init = false;
	nn->lconst = 0.0f;

	/*malloc space for the structure-array*/
	nn->struc = (unsigned int *)malloc(sizeof(unsigned int) * layers);
	memcpy((void*)nn->struc,(void*)struc,sizeof(unsigned int) * layers); // copy the given structure

	/*add the bias to the input layer*/
	if(nn->bias == true){
		nn->struc[0] += 1; // add bias neuron to the first(input)-layer
	}

	if(nn_count_neurons(nn)<0){ // count the neurons
		printerr("nn_init(): can't count neurons!",0);
		return -1;
	}

	//set the amount of output_neurons
	nn->ll_output_c = nn->struc[nn->layers-1]; 

	/*init the input and the outpur array*/
	nn->input = (double*)calloc(nn->neurons,sizeof(double));
	nn->output = (double*)calloc(nn->neurons,sizeof(double));

	/*init the remember-input-matrix*/
	if(nn->piped_input == false){
		if(nn_mk_remember(nn)<0){
			printerr("nn_init(): can't malloc the remember-the-input matrix",0);
			return -1;
		}
	}

	/*init matrix*/
	if(nn_mk_matrix(nn)<0){	// build the matrix!
		printerr("nn_init(): can't build the matrix!",0);	
	}

	if(nn_map_layer(nn)<0){ // and make the neuron -> layer mapping
		printerr("nn_init(): can't map neurons to layers!",0);
		return -1;
	}
	
	/*apply the net type*/
	if(nn_apply_type(nn)<0){
		printerr("nn_init(): can't apply the net type!",0);
		return -1;
	}

	/*if the statistics switch is on make statistics struct*/
	if(nn->mk_stats == true){
		nn->stats = (statistics*)malloc(sizeof(statistics));
		/*reset the statistics*/
		nn->stats->error = 0;
	}

	// initialized!
	nn->init = true;

	return 1;
}

int nn_kill(net *nn){

	if(nn->init == true){
		free(nn->input); // free the input-vector
		free(nn->output); // free the output-vector
		free(nn->struc); // free the structure
		free(nn->ntol); // free the neuron to layer mapping
	}

	/*free i->o Mapping*/
	if(nn->i_o_map_init == true){
		free(nn->i_o_map);
		free(nn->threwen);
		nn->i_o_map_init = false;
	}

	/*datafree*/
	nn->data_present = false;

	/*kill the remember the input-field*/
	if(nn->piped_input == false){
		if(nn_del_remember(nn)<0){
			printerr("nn_kill(): can't kill the remember the input_matrix",0);
			return -1;
		}
	}

	/*kill matrix*/
	if(nn->matrix_init == true){ // make sure that ther's no matrix!
		if(nn_del_matrix(nn)<0){
			printerr("nn_kill(): can't delet the matrix!",0);
			return -1;
		}
	}

	/*free statistics*/
	if(nn->mk_stats == true){
		free(nn->stats);
		nn->mk_stats = false;
	}

	/*free the learnmatrix*/
	if(nn->lmatrix_init == true){
		free(*(nn->learn_matrix));
		free(nn->learn_matrix);
		nn->lmatrix_init = false;
	}

	nn->neurons = 0;
	nn->layers = 0;
	nn->ll_output_c = 0;

	return 1;
}

int nn_load_from_file(net *nn,char *filename){
	unsigned int i;
	FILE *fd = fopen(filename,"rb");
	int version;

	if(fd == NULL){
		printerr("nn_load_from_file(): can't open the file",0);
		return -1;
	}
	
	rewind(fd);

	if(feof(fd)!=0 || ferror(fd)!=0){
		clearerr(fd);
	}

	//first read magic bytes for solving the NUXI-problem
	if((version = fgetc(fd)) == EOF){
		printerr("nn_load_from_file(): fgetc failed",0);		
		fclose(fd);
		return -1;
	}

	if(NN_VERSION != version){ // read version
		printerr("nn_load_from_file(): version of the file not compatible",0);
		fclose(fd);
		return -1;
	}

	/*read the data..*/
	for(i=0;i<FILE_DATA_FILEDS;i++){
		void *ptr;
		size_t size;

		//all the attributes
		activation_function afunc;
		net_type ntype;
		learn_type ltype;
		boolean bias;
		boolean piped_input;
		boolean stats;
		unsigned int layers;
		unsigned int *struc;

		switch (i){
			case 0:
				ptr = (void*)&afunc;
				size = sizeof(afunc); 
			break;
			case 1:
				ptr = (void*)&ntype;
				size = sizeof(ntype);
			break;
			case 2:
				ptr = (void*)&ltype;
				size = sizeof(ltype);
			break;
			case 3:
				ptr = (void*)&bias;
				size = sizeof(bias);
			break;
			case 4:
				ptr = (void*)&piped_input;
				size = sizeof(piped_input);
			break;
			case 5:
				ptr = (void*)&stats;
				size = sizeof(stats);
			break;
			case 6:
				ptr = (void*)&layers;
				size = sizeof(layers);
			break;
			case 7:
				size = layers * sizeof(*(struc));
				struc = (unsigned int*)malloc(size);

				ptr = (void*)struc;
			break;
			case 8:
				/*init the net*/
				if(nn_init(nn,afunc,ntype,ltype,false,piped_input,stats,layers,struc)<0){ // init without bias
					printerr("nn_load_from_file(): Can't init the net!",0);
					return -1;
				}
				free(struc);
				nn->bias = bias; // add the bias if there is one

				ptr = (void*)(*(nn->matrix));
				size = sizeof(*(*(nn->matrix)))*nn->neurons*nn->neurons;
			break;
			default:
				ptr = 0;
				size = 0;
				printerr("nn_load_from_file(): Out of range!",0);
			break;
		}

		if(util_read_from_file(fd,ptr,size,true)<0){
			printerr("nn_load_from_file(): Failed loading data from file!",0);
			fclose(fd);
			return -1;
		}
	}

	if(fclose(fd)==EOF){
		printerr("nn_load_from_file():fclose failed",4);
		return -1;
	}

	return 1;
}

int nn_save_to_file(net *nn,char *filename){
	unsigned int i;

	/* :::NOTE:::
	* The Fileformat looks like this:
	[VERSION(char)][ARUMENT_0(?)][ARUMENT_1(?)][...][STRUCTURE][MATRIX]

	==>ARGUMENTS:
		0: activation_function afunc
		1: net_type ntype
		2: learn_type ltype
		3: boolean bias
		4: boolean piped_input
		5: boolean stats
		6: unsigned int layers

	ALL THE DATA IS STORED IN BIGENDIAN!
	*/

	FILE *fd = fopen(filename,"wb");

	if(fd  == NULL){
		printerr("nn_save_to_file(): can't open the file",0);
		return -1;
	}
	
	fputc(NN_VERSION,fd); // write version
	
	/*save the data..*/
	for(i=0;i<FILE_DATA_FILEDS;i++){
		void *ptr;
		size_t size;
		switch (i){
			case 0:
				ptr = (void*)&(nn->afunc);
				size = sizeof(nn->afunc); 
			break;
			case 1:
				ptr = (void*)&(nn->ntype);
				size = sizeof(nn->ntype);
			break;
			case 2:
				ptr = (void*)&(nn->ltype);
				size = sizeof(nn->ltype);
			break;
			case 3:
				ptr = (void*)&(nn->bias);
				size = sizeof(nn->bias);
			break;
			case 4:
				ptr = (void*)&(nn->piped_input);
				size = sizeof(nn->piped_input);
			break;
			case 5:
				ptr = (void*)&(nn->mk_stats);
				size = sizeof(nn->mk_stats);
			break;
			case 6:
				ptr = (void*)&(nn->layers);
				size = sizeof(nn->layers);
			break;
			case 7:
				ptr = (void*)nn->struc;
				size = nn->layers * sizeof(*(nn->struc));
			break;
			case 8:
				ptr = (void*)(*(nn->matrix));
				size = sizeof(*(*(nn->matrix)))*nn->neurons*nn->neurons;
			break;
			default:
				ptr = NULL;
				size = 0;
				printerr("nn_save_to_file(): Out of range!",0);
			break;
		}

		if(util_write_to_file(fd,ptr,size,true)<0){
			printerr("nn_save_to_file(): Failed saving data to file!",0);
			fclose(fd);
			return -1;
		}
	}


	if(fflush(fd)==EOF){
		printerr("nn_save_to_file(): Failed flushing buffer to file...",4);
		return -1;
	}

	if(fclose(fd)!=0){
		printerr("nn_save_to_file(): Failed closing file...",4);
		return -1;
	}

	return 1;
}

int nn_copy(net *dst,net *src){
	
	// init the dst net with the data from src
	if(nn_init(dst,src->afunc,src->ntype,src->ltype,false,src->piped_input,src->mk_stats,src->layers,src->struc)<0){
		printerr("nn_copy(): init of source net failed",0);
		return -1;
	}

	// set bias afterwards (otherwise nn_init would add a bias)
	dst->bias = src->bias;

	// copy matrix
	if(nn_set_matrix(dst,*(src->matrix))<0){
		printerr("nn_copy(): matrix copy failed",0);
		return -1;
	}

	// if there is data append the data
	if(src->data_present == true){
		if(nn_set_datasets(dst,src->ndata,src->shuffle)<0){
			printerr("nn_copy(): setting data failed",0);
			return -1;
		}
	}

	return 1;
}

int nn_apply_type(net *nn){
	unsigned int i,j;
	double *bias_save;

	if(nn->matrix_init == false){
		printerr("nn_apply_type(): first init a matrix!",0);
		return -1;
	}
	/*save the bias weights (the bias is in all layers!)*/
	if(nn->bias == true){
		unsigned int i;
		bias_save = (double*)malloc(sizeof(double)*nn->neurons);

		for(i=0;i<nn->neurons;i++)
			bias_save[i] = nn->matrix[i][nn->struc[0]-1]; // save the connection to the bias 
	}

	/*loop through the net*/
	for(i=0;i<nn->neurons;i++){
		for(j=0;j<nn->neurons;j++){
			/*make decision based on the nettype*/
			//NOTE: i = from j = to
			switch(nn->ntype){
				case FeedForward: // feed forward structure!
					if((nn->ntol[i]+1) != nn->ntol[j]){
						/*just allow connections to neurons who are one layer higher than the current*/
						nn->matrix[j][i] = 0.0f;
					}
				break;
				case Recurrent:
					if(nn->ntol[i] >= nn->ntol[j]){ //backward connection
						/*make backward connections < [+/-]1 otherwise we'll have an oscillating net*/
						if(nn->matrix[j][i] > 1.0f){
							nn->matrix[j][i] = 1.0f;
						}else if(nn->matrix[j][i] < -1.0f){
							nn->matrix[j][i] = -1.0f;
						}
					}
				break;
				case ZeroOne:
					if(nn->matrix[j][i] <= 0.0f){
						nn->matrix[j][i] = 0.0f;
					}else{
						nn->matrix[j][i] = 1.0f;
					}
				break;
				case Dontcare: // don't care about the netstrucutre
				break;
				case Zero: // init with zero
					nn->matrix[j][i] = 0.0f;
				break;
				default: //not a known structure
					printerr("nn_apply_type(): unknown net-structure!",0);
					return -1;
				break;

			}
		}
	}

	/*restore the bias weights (the bias is in all layers!)*/
	if(nn->bias == true){
		unsigned int i;
		for(i=0;i<nn->neurons;i++)
			nn->matrix[i][nn->struc[0]-1] = bias_save[i]; // restore the connection to the bias 

		free(bias_save);
	}

	return 1;
}

static int nn_map_layer(net *nn){
	unsigned int i; // the neuron
	unsigned int layer; // the current layer
	unsigned int layer_size; // amount of neurons from start to the current layer

	nn->ntol = (unsigned int *)malloc(sizeof(unsigned int*)*nn->neurons);

	for(i=0;i<nn->neurons;i++){ // do this for every neuron
		/*define the layer*/
		layer_size = 0;
		layer = 0;
		do{ // is the neuron in the layer?
			if(layer<nn->layers){ // overflow protection
				layer_size += nn->struc[layer];
			}else{
				printerr("nn_map_layer(): Neuron does not fit into a layer : layer >= nn->layers",0);
				return -1;
			}
			layer++; // inc layer
		}while(layer_size < (i+1)); // is the neuron in the layer?
		nn->ntol[i] = (layer-1); // and set it!  (-1 cause of the inc in the loop)
	}

	return 1;
}

static int nn_count_neurons(net *nn){
	unsigned int i;
	if(nn->struc == NULL){
		printerr("nn_count_neurons(): nn->struc == NULL!",0);
		return -1;
	}

	/*count the neurons!*/
	nn->neurons = 0;
	for(i=0;i<nn->layers;i++){
		nn->neurons += nn->struc[i];
	}

	return 1;
	
}

static int nn_mk_remember(net *nn){
	unsigned int i;
	double *ri_data,*ro_data;
	double **ri_d1,**ro_d1;

	/*malloc space for the remember the input matrix*/
	ri_data = (double*)calloc(nn->neurons*(nn->layers-1),sizeof(double));
	ri_d1 = (double**)calloc((nn->layers-1),sizeof(double*));

	/*malloc space for the remember the output matrix*/
	ro_data = (double*)calloc(nn->neurons*(nn->layers-1),sizeof(double));
	ro_d1 = (double**)calloc((nn->layers-1),sizeof(double*));

	/*set the remember the input/output matrix*/
	for(i=0;i<(nn->layers-1);i++){
		ri_d1[i] = &(ri_data[(i*nn->neurons)]);
		ro_d1[i] = &(ro_data[(i*nn->neurons)]);
	}
	
	/*and set it*/
	nn->r_input = ri_d1;
	nn->r_output = ro_d1;

	return 1;
}

static int nn_del_remember(net *nn){
	if(nn->piped_input == true){
		printerr("nn_del_r_input(): Piped input! So there's no remember the input-matrix",0);
		return -1;
	}
	/*free the remember the input*/
	free(*(nn->r_input));
	free(nn->r_input);
	/*free the remember the output*/
	free(*(nn->r_output));
	free(nn->r_output);

	nn->piped_input = true;

	return 1;
}

static int nn_malloc_matrix(net *nn,double ***matrix){
	unsigned int i;
	double *matrix_data;
	double **matrix_d1;
	
	if(nn->neurons <= 0){
		printerr("nn_malloc_matrix(): nn->neurons <= 0 ",0);
		return -1;
	}

	/*malloc space for the whole matrix*/
	matrix_data = (double*)malloc(sizeof(double)*nn->neurons*nn->neurons);
	matrix_d1 = (double**)malloc(sizeof(double*)*nn->neurons);

	/*now set the pointers correct!*/
	for(i=0;i<nn->neurons;i++){
		matrix_d1[i] = &(matrix_data[(i*nn->neurons)]);
	}

	*matrix = matrix_d1;	

	return 1;
}

static int nn_mk_matrix(net *nn){
	double **matrix;

	/*malloc the matrix*/
	if(nn_malloc_matrix(nn,&matrix)<0){
		printerr("nn_mk_matrix(): Can't malloc the matrix",0);
		return -1;
	}

	/*set the matrix*/	
	nn->matrix = matrix;

	/*randomize matrix elements*/
	if(nn_rnd_matrix(nn)<0){
		printerr("nn_mk_matrix(): Can't set random matrix elements!",0);
		return -1;
	}
	nn->matrix_init = true;

	return 1;
}

int nn_rnd_matrix(net *nn){
	unsigned int i,j;
	/*init the matrix with random numbers*/
	for(i=0;i<nn->neurons;i++){
		for(j=0;j<nn->neurons;j++){
			double rand_val;
			do{
				rand_val = (double)(((double)NN_WEIGHTS_RND_RANGE*((double)rand()/RAND_MAX))-((double)NN_WEIGHTS_RND_RANGE/2)); // get a random number in the rand rage
			}while(rand_val == 0.0f); // the rnd number should be != 0 cause 0 is a closed connection
			nn->matrix[i][j] = rand_val; // and set the random val
			//nn->matrix[i][j] = 1.0f;
		}
	}

	return 1;
}

int nn_set_matrix(net *nn,double *matrix_data){
	if(nn->matrix_init == false){
		printerr("nn_set_matrix(): no matrix present!",0);
		return -1;
	}

	memcpy((void*)(*(nn->matrix)),(void*)matrix_data,sizeof(double)*nn->neurons*nn->neurons); // copy matrix data into the matrix

#if APPLY_NETSTRUCT_AFTER_SETMATRIX > 0
	if(nn_apply_type(nn)<0){
		printerr("nn_set_matrix(): applying type after setmatrix failed!",0);
		return -1;
	}
#endif

	return 1;
}

int nn_set_matrix_2d(net *nn,double **matrix_data){
	unsigned int i;
	if(nn->matrix_init == false){
		printerr("nn_set_matrix(): no matrix present!",0);
		return -1;
	}

	for(i=0;i<nn->neurons;i++){
		memcpy((void*)(nn->matrix[i]),(void*)(matrix_data[i]),sizeof(double)*nn->neurons); // copy matrix data into the matrix
	}

#if APPLY_NETSTRUCT_AFTER_SETMATRIX > 0
	if(nn_apply_type(nn)<0){
		printerr("nn_set_matrix(): applying type after setmatrix failed!",0);
		return -1;
	}
#endif

	return 1;
}

static int nn_del_matrix(net *nn){
	if(nn->matrix_init == false){
		printerr("nn_del_matrix(): no matrix present!",0);
		return -1;
	}
	
	free(*(nn->matrix)); // free the data-array
	free(nn->matrix); // free the data-seperator array

	nn->matrix_init = false;

	return 1;
}

//static int nn_dump_matrix(net *nn){
int nn_dump_matrix(net *nn){
	unsigned int i,j;
	if(nn->matrix_init == false){
		printerr("nn_dump_matrix(): no matrix present!",0);
		return -1;		
	}
	
	printf("MATRIX_DUMP\n");
	for(i=0;i<nn->neurons;i++){
		printf("==========");
	}
	printf("\n");


	for(i=0;i<nn->neurons;i++){
		for(j=0;j<nn->neurons;j++){
			printf("|%lf|",nn->matrix[i][j]);
		}
		printf("\n");
	}

	for(i=0;i<nn->neurons;i++){
		printf("==========");
	}
	printf("\n");

	return 1;
}

//static int nn_dump_all(net *nn){
int nn_dump_all(net *nn){
	unsigned int i,j;
	if(nn->matrix_init == false){
		printerr("nn_dump_all(): no matrix present!",0);
		return -1;		
	}
	
	printf("ALL_DUMP\n");

	printf("==========\t");
	for(i=0;i<nn->neurons;i++){
		printf("===========");
	}
	printf("\t==========\n");


	for(i=0;i<nn->neurons;i++){
		printf("|%lf|\t",nn->input[i]);
		for(j=0;j<nn->neurons;j++){
			printf("|%lf|",nn->matrix[i][j]);
		}
		printf("\t|%lf|\n",nn->output[i]);
	}

	printf("==========\t");
	for(i=0;i<nn->neurons;i++){
		printf("===========");
	}
	printf("\t==========\n");

	return 1;

}

int nn_set_neurons_v(net *nn,double *input,unsigned int size, ... ){
	unsigned int i;
	va_list argptr; // ptr to the list of arguments
	unsigned int *pos;

	if(size > nn->neurons){
		printerr("nn_set_neurons_v(): You can't set more input-vals than existing neurons",0);
		return -1;
	}
	
	pos = (unsigned int *)malloc(sizeof(unsigned int)*size);

	va_start(argptr,size); // goto start of list
	for(i=0;i<size;i++){
		pos[i] = (unsigned int)va_arg(argptr,unsigned int); // read argument by argument
	}
	va_end(argptr); // free arguments

	if(nn_set_neurons(nn,input,size,pos)<0){
		printerr("nn_set_neurons_v(): cant' set neurons!",0);
		return -1;
	}

	free(pos);

	return 1;
}

int nn_set_neurons(net *nn,double *input,unsigned int size,unsigned int *pos){
	unsigned int i;

	if(size > nn->neurons){
		printerr("nn_set_neurons(): You can't set more input-vals than existing neurons",0);
		return -1;
	}

	if(nn->piped_input == false){
		printerr("nn_set_neurons(): Set neurons does not work with non-Piped input",0);
		return -1;
	}

	if(pos == NULL){
		/*set the neurons one by one*/
		memcpy((void*)nn->input,(void*)input,size*sizeof(double));
	}else{
		/*set the neurons*/
		for(i=0;i<size;i++){
			nn->input[pos[i]] = input[i];
		}
	}

	return 1;
}

static int nn_data_input(net *nn){

	/*check if the net can thren new input*/
	if(nn->piped_input == true || // on piped input don't care - threw new input every age-step!
	(nn->piped_input == false && nn->curr_layer == 0) // or if don't pipe the input threw only if the last input has passed the whole net 
	){
		/*flush the remember vectors and the old input_vector*/
		if(nn->piped_input == false){
			memset((void*)*(nn->r_input),0.0f,(size_t)nn->neurons*(nn->layers-1)*sizeof(double)); // Flush - remember the input 
			memset((void*)*(nn->r_output),0.0f,(size_t)nn->neurons*(nn->layers-1)*sizeof(double)); // Flush - remember the output
			memset((void*)nn->input,0.0f,(size_t)nn->neurons*sizeof(double)); // old inputs flush
		}

		/*then threw input*/
		if(nn->data_present == true){ // is there data for the net?
			unsigned int i;
			unsigned int curr_data; // get the current dataset
			unsigned int data_amount = nn->struc[0]; // amount of data (amount of input neurons)

			if(nn->bias == true)
				data_amount--;

			// randomize data_input
			if(nn->shuffle == true){
				/*all inputs threwen*/
				if(nn->inputs_t == 0){
					for(i=0;i<nn->ndata->data_c;i++) // reset the threwen array!
						nn->threwen[i] = false;
				}

				/*get a random element*/
				curr_data =(unsigned int)rand() % nn->ndata->data_c;
				if(nn->threwen[curr_data] == true){ // is the data already threwen?
					unsigned int i,j;
					unsigned int mapping_size = nn->ndata->data_c - nn->inputs_t; // get amount of unthrewen inputs
					int *goto_mapping = (int*)calloc(mapping_size,sizeof(int));

					/*make an array with index stored index changes to not threwen data, then choose one of them*/
					for(i=0,j=0;i<nn->ndata->data_c;i++){ // search for not threwen inputs
						if(nn->threwen[i] == false){
							goto_mapping[j] = i - curr_data;
							j++;
						}
					}

					/*now get the new curr_data*/
					curr_data = goto_mapping[(unsigned int)rand() % mapping_size] + curr_data;
					
					free(goto_mapping);					
				}

				nn->i_o_map[nn->inputs_t] = curr_data ; // map the curr_data 
				nn->threwen[curr_data] = true;
			}else{
				curr_data = nn->inputs_t;
			}

			for(i=0;i<data_amount;i++){ // set data for each neuron in the input-layer
				nn->input[i] = (*nn->ndata->input_data)[curr_data][i]; // and set it to the input array
			}
#if NN_DEBUG > 0
			printf("(%u):%u\n",nn->inputs_t,curr_data);
#endif 
			nn->inputs_t = (nn->inputs_t+1) % nn->ndata->data_c; // increment inputs threwen!
		}
	}

	/*set the bias*/
	if(nn->bias == true)
		nn->input[nn->struc[0]-1] = BIAS_VAL; // set the bias as the last of the neurons in the first layer!

	/*remember the input if the inpus is not piped*/
	if(nn->piped_input == false){
		memcpy((void*)nn->r_input[nn->curr_layer],(void*)nn->input,sizeof(double)*nn->neurons); // remember the input
	}	

	return 1;
}

int nn_get_output(net *nn,double **output){
	(*output) = &(nn->output[(nn->neurons-nn->ll_output_c)]);

	return 1;
}

static int nn_init_i_o_map(net *nn){
	if(nn->data_present == false){
		printerr("nn_init_i_o_map(): There's no data_present!",0);
		return -1;
	}
	nn->i_o_map = (unsigned int *)calloc(nn->ndata->data_c,sizeof(unsigned int)); // malloc space for the i-o-map
	nn->threwen = (boolean *)malloc(sizeof(boolean)*nn->ndata->data_c); // malloc space for the threwen-mapping
	nn->i_o_map_init = true;
	
	return 1;
}

int nn_think(net *nn,unsigned int steps,boolean learn){
	if(nn->matrix_init == false){
		printerr("nn_think(): set weights first!",0);
		return -1;
	}

	/*set the learnmode*/
	if(nn_set_learnmode(nn,learn,nn->lconst,nn->batch)<0){ 
		printerr("nn_think(): can't set the learnmode!",0);
		return -1;
	}

	if(nn->mk_stats == true){
		/*reset the statistics*/
		if(reset_statistics(nn)<0){
			printerr("nn_think(): can't reset the statistics\n",0);
			return -1;
		}
	}

	/*think!*/
	nn->inputs_t = 0; // reset inputs threwen count
	for(nn->age=0;nn->age<steps;nn->age++){
		unsigned int i,j;

		/*set the current_variables*/
		if(nn->piped_input == false){
			nn->curr_layer = nn->age % (nn->layers-1); // get the current layer
		}else{
			nn->curr_layer = 0; // reset the current layer
		}

		/*set inputs*/
		nn_data_input(nn); // set the input array

		/*flush the output-array*/
		memset(nn->output,0.0f,sizeof(double)*nn->neurons);

		/*apply weight on input-vector = get output-vector*/
		for(i=0;i<nn->neurons;i++){
			for(j=0;j<nn->neurons;j++){
				//calculate every neuron	(weight * input)
				nn->output[i] += nn->matrix[i][j] * nn->input[j];
			}
		}
		
		/*remember the output*/
		if(nn->piped_input == false){
			memcpy((void*)nn->r_output[nn->curr_layer],(void*)nn->output,sizeof(double)*nn->neurons); // copy the input
		}

		nn_activate(nn); // activision function

#if NN_DEBUG > 0
		//nn_dump_matrix(nn);
		//nn_dump_all(nn);
#endif

		/*make next steps only if the input has completely forward propagated*/
		if(nn->piped_input == true ||
		(nn->curr_layer == (nn->layers-2))){ // is at the last layer?..

			/*get the input data for the current output_data*/
			if(nn->data_present == true)
				get_curr_data(nn); // map the output_data to the input_data
	
			nn_get_output(nn,&(nn->ll_output)); // get output for the last layer

			/*learning*/
			if(nn->learn == true)
				nn_learn(nn);

			if(nn->mk_stats == true) // make statistics?
				mk_statistics(nn);

		}

		/*set output of the last layer as new input*/
		memcpy(nn->input,nn->output,sizeof(double)*nn->neurons);
	}

	//nn_dump_all(nn);

	return 1;
}

static int mk_statistics(net *nn){
	if(nn->data_present == true){ // is there data (TODO:this could make trouble and cause segfaults if there is input data but no learndata)
		unsigned int i;
		double curr_err = 0.0f; // the error for this dataset
		
		/*compute the net error*/
		for(i=0;i<nn->ll_output_c;i++){
			double err = (*nn->ndata->output_data)[nn->curr_data][i]-nn->ll_output[i];
			if(err > 0.0f){
				nn->stats->error += err;
				curr_err += err;
			}else if(err < 0.0f){
				nn->stats->error -= err;
				curr_err -= err;
			}
#if NN_DEBUG > 0
			printf("ERR(cd:%u i:%u): %lf - %lf %lf\n",nn->curr_data,i,(*nn->ndata->output_data)[nn->curr_data][i],nn->ll_output[i],err);
#endif
		}

		/*compute the bigerr*/
		if(curr_err > nn->stats->bigerr_val){ // is the current error bigger than the old bigerr?
			nn->stats->bigerr_val = curr_err; // set this one as the biggest error ever!
			nn->stats->bigerr_i = nn->curr_data; // and remember the dataset
		}

#if NN_DEBUG > 0
		printf("TOTAL-ERR:%lf CURR-ERR:%lf\n",nn->stats->error,curr_err);
#endif
	}

	return 1;
}

static int reset_statistics(net *nn){
	if(nn->mk_stats == true){
		nn->stats->error = 0.0f; // reset the net error
		nn->stats->bigerr_val = 0.0f; // reset the bigerr_val
		nn->stats->bigerr_i = 0; // reset the biggest index
	}

	return 1;
}

static int get_curr_data(net *nn){
	unsigned int curr_data = nn->inputs_t; // get the current dataset

	/*if the net is in piped input mode reverse forwardpropagation*/
	if(nn->piped_input == true){
		/*get the curr_data at the input_layer*/
		while(curr_data < (nn->layers-2)){ // protect curr_data from overflow!
			curr_data += nn->ndata->data_c; // add the dataamount
		}
		curr_data -= (nn->layers-2); // layers-2 == Time to pass the net (input_layer/output_layer)
	}

	/*nn->inputs_t to index*/
	if(curr_data == 0)
		curr_data = nn->ndata->data_c;
	curr_data--; // -1
	
	if(nn->shuffle == true) // on shuffled data get the value out of the i->o-MAP
		curr_data = nn->i_o_map[curr_data];

	nn->curr_data = curr_data; // and set it

#if NN_DEBUG > 0
	printf("nn->curr_data %u\n",nn->curr_data);
#endif

	return 1;
}

static int nn_activate(net *nn){
	unsigned int i;
	/*make activition for each neuron*/
	for(i=0;i<nn->neurons;i++){
		switch (nn->afunc){
			case ABOVE_ZERO:
				if(nn->output[i]>0.0f){
					nn->output[i] = 1.0f;
				}else{
					nn->output[i] = 0.0f;
				}
			break;
			case SIGMA:
				nn->output[i] = SIGMA_RANGE*1/(1 + exp(-1*(nn->output[i]))); // sigmoid
			break;
			case SIGMA_STRETCH:{
				double sig = 1/(1 + exp(-1*nn->output[i])); // sigmoid
				nn->output[i] = (SIGMA_STRETCH_RANGE*sig)-(SIGMA_STRETCH_RANGE/2); // strech and align to 0
			}break;
			case GAUSS:{
				double inval = (nn->output[i]-GAUSS_CENTER);
				nn->output[i] = GAUSS_HEIGHT*exp(-1*((inval*inval)/(2*GAUSS_DEVIATION*GAUSS_DEVIATION)));
			}break;
	 		case ONE_TO_ONE:
				//do nothing!
			break;
			default:
				printerr("nn_activate(): unknown activation function!",0);
				return -1;
			break;
		}
	}

	return 1;
}

static int nn_ractivate(net *nn,double *input,double **output){
	unsigned int i;
	/*make derivatives for the given vector*/
	for(i=0;i<nn->neurons;i++){
		switch (nn->afunc){
			case ABOVE_ZERO:
				printerr("nn_ractivate(): ABOVE_ZERO is not defined!",0);
				return -1;
			break;
			case SIGMA:{
				double sig = 1/(1 + exp(-1*input[i])); // sigmoid
				(*output)[i] = SIGMA_RANGE*(sig*(1-sig));
			}break;
			case SIGMA_STRETCH:{
				double sig = 1/(1 + exp(-1*input[i])); // sigmoid
				(*output)[i] = SIGMA_STRETCH_RANGE*(sig*(1-sig)); // derv
			}break;
			case GAUSS:{
				double inval = (input[i]-GAUSS_CENTER);
				(*output)[i] = exp(-1*((inval*inval)/(2*GAUSS_DEVIATION*GAUSS_DEVIATION)))*-1;
			}break;
	 		case ONE_TO_ONE:
				//do nothing!
			break;
			default:
				printerr("nn_ractivate(): unknown activation function!",0);
				return -1;
			break;
		}
	}

	return 1;
}

static int nn_learn(net *nn){
	if(nn->data_present == false){
		printerr("nn_learn(): can't learn without learndata!",0);
		return -1;
	}

	switch(nn->ltype){
		case Delta:{
			unsigned int i,j;

			for(i=0;i<nn->ll_output_c;i++){ // do this for each output neuron
				unsigned out_n = i+(nn->neurons-nn->ll_output_c); // the current output neuron
				double error = (*nn->ndata->output_data)[nn->curr_data][i]-nn->ll_output[i]; // get the error
				//printf("EXPECTED:: %lf\tGOT:: %lf\tError:: %lf\n",(*nn->ndata->output_data)[curr_data][i],nn->output[out_n],error);
				if(error == 0.0f) // continue if the error is 0
					continue;

				for(j=0;j<nn->neurons;j++){ // and set new weights
					if(nn->learn_matrix[out_n][j] == 0.0f) // do not open closed connections
						continue;
					nn->learn_matrix[out_n][j] += nn->lconst * error * nn->input[j];
				}
			}	
		}break;

		case Backpropagation:{
			if(nn_backpropagation(nn)<0){
				printerr("nn_learn(): Backpropagation failed!",0);
				return -1;
			}
		}break;

		default:
			printerr("nn_learn(): unknown learntype!",0);
		break;
	}


	/*set the learn matrix as the new matrix*/
	if(nn->batch == false || // the not batchmode
	nn->inputs_t == (nn->ndata->data_c-1)){ // or all data is threwen

#if NN_DEBUG > 0
		printf("\n\nPREV!\n");
		nn_dump_matrix(nn);
#endif
		if(nn_set_matrix(nn,(*nn->learn_matrix))<0){
			printerr("nn_learn(): can't set the learn matrix as the new matrix!",0);
			return -1;
		}

#if NN_DEBUG > 0
		printf("AFTER:\n");
		nn_dump_matrix(nn);
#endif

#if APPLY_NETSTRUCT_AFTER_LEARNING > 0
		if(nn_apply_type(nn)<0){
			printerr("nn_learn(): failed applying the netstructure",0);
			return -1;
		}
#endif
	}
	
	return 1;
}

static int nn_backpropagation(net *nn){
	unsigned int i,j,l;
	double *deltas,*ndeltas,*derv;

	/*check if piped input is disabled*/
	if(nn->piped_input == true){
		printerr("nn_backpropagation(): Backpropagation does not work with piped_input",0);
		return -1;
	}

	/*make delta vectors*/
	deltas = (double*) calloc(nn->neurons,sizeof(double)); // deltas are the deltas of the last layer
	ndeltas = (double*) calloc(nn->neurons,sizeof(double)); // new deltas are the new deltas for the current layer

	/*make vector for the deriviatives*/
	derv = (double*) calloc(nn->neurons,sizeof(double));

	for(l=0;l<(nn->layers-1);l++){ // make backpropagation for each layer (exept the input->layer)
		unsigned int bb_layer = (nn->layers-1)-l; // the current computing layer

		memcpy((void*)deltas,(void*)ndeltas,sizeof(double)*nn->neurons); // set new deltas as deltas
		memset((void*)ndeltas,0.0f,sizeof(double)*nn->neurons); // set new_deltas to 0;

		/*get the derivatives*/
		if(nn_ractivate(nn,nn->r_output[bb_layer-1],&derv)<0){ // make reverse activison of the layer above the current layer
			printerr("nn_backpropagation(): reverse activision failed!",0);
			return -1;
		}

		/*calculate new deltas*/
		if(l == 0){ // at the output layer
			/*get the output_deltas*/
			for(i=0;i<nn->ll_output_c;i++){ // set error as deltas
				unsigned int d_index = i+(nn->neurons-nn->ll_output_c); // the delta index of the current output-neuron
				ndeltas[d_index] = ((*nn->ndata->output_data)[nn->curr_data][i]-nn->ll_output[i])*derv[d_index]; // calc the error
			}

		}else{ // all other layers
			for(i=0;i<nn->neurons;i++){ // summarize all the old deltas * weight from the neuron to the old neuron = new deltas
				for(j=0;j<nn->neurons;j++){  // do this for each neuron
					ndeltas[i] += deltas[j] * nn->matrix[j][i] * derv[i]; // compute the new deltas for each neuron
				}
			}

		}

#if NN_DEBUG > 0	
		/*Make some debug printing*/
		printf("===========(%u)===========\n",nn->curr_data); // the current processed dataset
		printf("routput(%u):\t",bb_layer);
		for(i=0;i<nn->neurons;i++){ // print the output_layer
			printf("|%lf|",nn->r_output[bb_layer-1][i]);
		}
		printf("\n");
		printf("rinput(%u):\t",bb_layer);
		for(i=0;i<nn->neurons;i++){ // and the input_layer
			printf("|%lf|",nn->r_input[bb_layer-1][i]);
		}
		printf("\n");
		printf("dervs(%u):\t",bb_layer);
		for(i=0;i<nn->neurons;i++){ // the derivatives
			printf("|%lf|",derv[i]);
		}
		printf("\n");	
		printf("deltas(%u):\t",bb_layer);
		for(i=0;i<nn->neurons;i++){ // and the deltas for this layer
			printf("|%lf|",ndeltas[i]);
		}
		printf("\n");
		printf("\n");
#endif
		//now set new weights
		for(i=0;i<nn->neurons;i++){ // connection from
			for(j=0;j<nn->neurons;j++){ // connection to
				/*set new weights by adding the weight delta to the old weights*/
				/*			 (the learnconstant) * (that what the neuron got as input) * (the delta for the neuron)*/
				//double d = (nn->matrix[i][j]-nn->learn_matrix[i][j]); // the old delta

				nn->learn_matrix[i][j] += nn->lconst*nn->r_input[bb_layer-1][j]*ndeltas[i] ; //calculate the new weights
				
				//nn->learn_matrix[i][j] += 0.7*d;
			}
		}
	}

	/*free's*/
	free(deltas); // free delta vector
	free(ndeltas); // and new_deltas
	free(derv); // and the derivativess

	return 1;
}

int nn_set_learnmode(net *nn,boolean learn,double lconst,boolean batch){
	if(nn->matrix_init == false){
		printerr("nn_set_learnmode(): Initialize a matrix first!",0);
		return -1;
	}
	if(learn == true && nn->data_present == false){
		printerr("nn_set_learnmode(): No data present, can't learn without!",0);
		return -1;
	}	

	/*sets*/
	nn->learn = learn; // shall the net learn?
	nn->batch = batch; // batch learning?
	nn->lconst = lconst; //	set learnconstant

	/*make the learn_matrix*/
	if(nn->learn == true && nn->lmatrix_init == false){ // no learn-matrix present but net is in learnmode
		if(nn_malloc_matrix(nn,&(nn->learn_matrix))<0){
			printerr("nn_set_learnmod(): can't init the learnmatrix!",0);
			return -1;
		}
		memcpy((void*)(*nn->learn_matrix),(void*)(*nn->matrix),sizeof(double)*nn->neurons*nn->neurons); // make a copy of the original matrix

		nn->lmatrix_init = true; // the learn matrix is now initialized
	}else if(nn->learn == false && nn->lmatrix_init == true){ // matrix present but net is not in learnmode
		free(*(nn->learn_matrix));
		free(nn->learn_matrix);

		nn->lmatrix_init = false; // the learn matrix is now uninitialized		
	}

	return 1;
}

int nn_set_datasets(net *nn,data *ndata,boolean shuffle){

	if(ndata->data_c <= 0){
		printerr("nn_set_datasets(): You cannot set a dataset containing <= 0 -data",0);
		return -1;
	}

	nn->ndata = ndata;
	nn->data_present = true;
	nn->shuffle = shuffle; // shall the data be shuffled befor threwing into the net
	
	/*malloc or free i_o_map*/
	if(nn->shuffle == true && nn->i_o_map_init == false){
		if(nn_init_i_o_map(nn)<0){
			printerr("nn_set_datasets(): Can't init INPUT->OUTPUT-MAP",0);
			return -1;
		}
	}else if(nn->shuffle == false && nn->i_o_map_init == true){
		free(nn->i_o_map);
		free(nn->threwen);
		nn->i_o_map_init = false;
	}

	return 1;
}

int nn_unset_datasets(net *nn){
	nn->ndata = NULL;
	nn->data_present = false;
	nn->shuffle = false;

	/*free the i-o-MAP*/
	if(nn->i_o_map_init == true){
		free(nn->i_o_map);
		free(nn->threwen);
		nn->i_o_map_init = false;
	}

	return 1;
}

