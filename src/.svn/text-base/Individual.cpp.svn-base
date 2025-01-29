#include "Individual.hpp"

Individual::Individual( World *w, unsigned int *struc, unsigned int num_layers, unsigned long lifespan, const char *path_to_bin , const char *path_to_datafile, const char *startdir ) {

	if( struc[0] < 16 ) {
		w->print(ERROR, "Individual::Individual(): struc[0] < num_static_inputs, not enough input neurons...");
		exit(-1);
	}

	os2nn = new OS2NN( struc[0] - 16 - 1, 16 ); // "struc[0] - X" for static inputs, additional "- 1" because OS2NN writes to struc[word_size]

	char *starting_directory = strdup( startdir );
	if( strlen(startdir) == 0 ) { // no startdir given, generate one
		starting_directory = LinuxGrammar::exec_and_return_line("find / -type d 2>/dev/null", -1);

	} else { // check if the directory exists


		const char *fmt = "find %s -maxdepth 0 2>&1 >/dev/null";

		int cmdlen = strlen(fmt) + strlen(starting_directory);
		char *cmd = new char[cmdlen];
		sprintf( cmd, fmt, starting_directory );

		if (system(cmd)) { // if it doesn't exist fetch a random one
			delete [] starting_directory;
			starting_directory = LinuxGrammar::exec_and_return_line("find / -type d 2>/dev/null", -1);
		}

		delete [] cmd;
	}

	nn2os = new NN2OS( starting_directory, GAUSS_HEIGHT, 0, struc[num_layers-1], os2nn, this );

	free( starting_directory );

	this->world = w;
	this->id = world->runid; // stupid id concept
	this->path_to_bin = strdup( path_to_bin );
	this->path_to_datafile = strdup( path_to_datafile );

	// initialize ANN
	this->brain = new Brain( Dontcare, GAUSS, false, struc, num_layers );

	// set default flags & values
	fitness = 0;
	this->lifespan = lifespan;

	age = 0;

	// initialize / reset other stuff
	reset();

};

Individual::~Individual() {
	// free heap memory by calling their destructors
	delete brain;
	delete os2nn;
	delete nn2os;
}

void Individual::reset( void ) {
	phenotype_index = 0;
	phenotype.clear();
	phenotype.resize( lifespan );
	kill_me = 0;
}

void Individual::run( void ) {

	if( DEBUG_LEVEL > 2 ) cout << "+++ Running Ind. #" << id << endl;

	get_memfree();

	if( DEBUG_LEVEL > 4 ) cout << "got free memory" << endl;

	// age
	age++;

	if( DEBUG_LEVEL > 4 ) cout << "time stuff" << endl;

	// time information
	time_t unixtime = time(NULL);
	struct tm *timeinfo = localtime( &unixtime );

	int time_hour = timeinfo->tm_hour;
	int time_minute = timeinfo->tm_min;
	int time_second = timeinfo->tm_sec;

	if( DEBUG_LEVEL > 4 ) cout << "IP address" << endl;
	int *ipaddr = new int[4];
	for( unsigned int i=0; i<4; i++ ) ipaddr[i] = 0;
	get_ip_address( (int **)&ipaddr );


	if( DEBUG_LEVEL > 4 ) cout << "clearing inputs" << endl;

	// clear inputs
	inputs.clear();

	if( DEBUG_LEVEL > 4 ) cout << "resizing inputs" << endl;

	inputs.resize(brain->num_inputs);

	if( DEBUG_LEVEL > 2 ) cout << "Getting input..." << endl;


	/**
	 *
	 * Load static inputs
	 *
	 */
	// return value of last command executed
	inputs[0] = nn2os->last_rval;

	// available memory
	inputs[1] = free_memory_percent;

	// number of files and directories in cwd
	nn2os->lg->update_file_dir_list();
	inputs[2] = nn2os->lg->num_files;
	inputs[3] = nn2os->lg->num_dirs;

	// age information
	inputs[4] = age;
	inputs[5] = world->lifecycle;

	// time information
	inputs[6] = time_hour;
	inputs[7] = time_minute;
	inputs[8] = time_second;
	inputs[9] = unixtime;

	// cwd depth
	inputs[10] = cwd_depth( (const char *)(nn2os->lg->cwd) );

	//cpu usage
	inputs[11] = world->cpu;

	// ip address
	inputs[12] = ipaddr[0];
	inputs[13] = ipaddr[1];
	inputs[14] = ipaddr[2];
	inputs[15] = ipaddr[3];
	delete [] ipaddr;

	/**
	 *
	 * Load dynamic inputs
	 *
	 */
	// retrieve buffer
	os2nn->retrieve_input(os2nn->which_buf_next, os2nn->which_word_next, os2nn->which_offset_next, &inputs[16]);

	// check for nan
	for (unsigned int i=0; i<inputs.size(); i++) {
		if (isnan(inputs[i])) {
			world->print(ERROR,"input is NaN!");
		}
	}

	/**
	 *
	 * Apply inputs
	 *
	 */
	if( DEBUG_LEVEL > 2 ) cout << "Got input. Setting brain inputs." << endl;
	// set inputs
	brain->set_inputs( inputs );

	// DEBUG PRINT INPUTS
	if( DEBUG_LEVEL > 2 ) {
		for( unsigned int i=0; i<inputs.size(); i++ ) cout << "inputs[" << i << "] = " << inputs[i] << endl;
	}

	/**
	 *
	 * Think aka. calculate outputs
	 *
	 */
	if( DEBUG_LEVEL > 3 ) cout << "Thinking." << endl;
	// run neural network
	brain->think( 1 );
	if( DEBUG_LEVEL > 3 ) cout << "Done Thinking." << endl;

	// get output
	vector<double> output = brain->get_output();

	/* for testing purposes: randomize output neurons */
	//for (int i=0; i<output.size(); i++) output[i] = Util::randdouble(0,100);

	if( output.size() != nn2os->op_size) {
		world->print(ERROR, "Invalid brain output size: ");

		// BAD BEHAVIOR!
		phenotype[phenotype_index++] = 192;

	} else {

		// check if ANY of the outputs are NaN
		bool is_nan = false;
		for (unsigned int i=0; i<output.size(); i++) {
			if (isnan(output[i])) {
				is_nan = true;
				world->print(ERROR, "output is NaN!");
			}
		}

		if (is_nan) {
			world->print(ERROR, "-> One or more of the outputs are NaN! Checking weight matrix for errors...");

			brain->check_weights_for_nan();

			// BAD BEHAVIOR!
			phenotype[phenotype_index++] = 192;
			kill_me++;

		} else {

			// DEBUG PRINT OUTPUT
			if( DEBUG_LEVEL > 0 ) {
				for( unsigned int i=0; i<output.size(); i++ ) cout << "output[" << i << "] = " << output[i] << endl;
			}

			if( DEBUG_LEVEL > 3 ) cout << "Attempting command interpretation..." << endl;
			nn2os->last_rval = nn2os->interpret(&output[0], os2nn);
			if( DEBUG_LEVEL > 3 ) cout << "Interpreted command" << endl;

			if(nn2os->last_rval == SIG_NO_REPO)	kill_me++;

			// get phenotype data
			phenotype [phenotype_index++] = nn2os->get_last_cmd_number();

		}

	}

	// DONT USE UP ALL OF MY CPU
	// TODO: fix memory usage
	usleep( 10 * 1000 );
}

char *Individual::phen_to_string(vector<int> &v1, vector<int> &v2){
	char* opstr = (char*)malloc(sizeof(char)*(v1.size()*25 + v2.size()*25)); //make it bigger than we need...
	opstr[0] = '\0';
	char tmp[v1.size()];
	for(unsigned int i=0; i < v1.size(); i++){
		sprintf(tmp, "%c", v1[i] );
		strcat(opstr, tmp);
	}
	for(unsigned int i=0; i < v2.size(); i++){
		sprintf(tmp, "%c", v2[i] );
		strcat(opstr, tmp);
	}
	return opstr;
}

void Individual::print_phenotype(vector<int> &v){
	for(unsigned int i=0; i < v.size(); i++){
			cout << v[i] << " " ;
		}
	cout << endl;
}

long Individual::get_memfree() {
	char fbuf[1024];
	long memtotal = -1;
	long memfree = -1;
	long buffers = -1;
	long cached = -1;

	const char *fn = "/proc/meminfo";

	const char *ln_total = "MemTotal:";
	const char *ln_free = "MemFree:";
	const char *ln_buffers = "Buffers:";
	const char *ln_cached = "Cached:";

	FILE *f = fopen( fn, "r" );
	if( f == NULL ) {
		world->print(ERROR, "Individual::get_memfree(): Unable to open file" );
		memfree = 0;
	} else {
		while( fgets(fbuf, 1024, f) != NULL ) {

			if( ! strncmp( ln_free, fbuf, strlen(ln_free) ) ) {

				char *tb = strdup( fbuf );
				char *t = strtok( tb, " " );
				if( t != NULL ) {
					t = strtok( NULL, " " );
					memfree = atol( t );

					if( memfree < 0 ) {
						world->print(ERROR,  "Individual #::mem_free(): Error getting MemFree!");
						memfree = 0;
					}

				} else {
					world->print(ERROR,  "Error getting MemFree due to a strtok() error!");
				}
				free(tb);
			}

			if( ! strncmp( ln_total, fbuf, strlen(ln_total) ) ) {

				char *tb = strdup( fbuf );
				char *t = strtok( tb, " " );
				if( t != NULL ) {
					t = strtok( NULL, " " );
					memtotal = atol( t );

					if( memtotal < 0 ) {
						world->print(ERROR,  "Individual #::mem_free(): Error getting MemFree!");
						memtotal = 0;
					}

				} else {
					world->print(ERROR,  "Error getting MemFree due to a strtok() error!");
				}
				free(tb);
			}

			if( ! strncmp( ln_buffers, fbuf, strlen(ln_buffers) ) ) {

				char *tb = strdup( fbuf );
				char *t = strtok( tb, " " );
				if( t != NULL ) {
					t = strtok( NULL, " " );
					buffers = atol( t );

					if( buffers < 0 ) {
						world->print(ERROR,  "Individual #::mem_free(): Error getting MemFree!");
						buffers = 0;
					}

				} else {
					world->print(ERROR,  "Error getting MemFree due to a strtok() error!");
				}
				free(tb);
			}

			if( ! strncmp( ln_cached, fbuf, strlen(ln_cached) ) ) {

				char *tb = strdup( fbuf );
				char *t = strtok( tb, " " );
				if( t != NULL ) {
					t = strtok( NULL, " " );
					cached = atol( t );

					if( cached < 0 ) {
						world->print(ERROR,  "Individual #::mem_free(): Error getting MemFree!");
						cached = 0;
					}

				} else {
					world->print(ERROR,  "Error getting MemFree due to a strtok() error!");
				}
				free(tb);
				break;
			}

		}
		fclose( f );

		free_memory = memfree + buffers + cached;
		free_memory_percent = ((double)free_memory/memtotal)*100;

	}
	return free_memory;
}

void Individual::initial_thinking(const char *start_string){

	// sanity check
	if( strlen( start_string ) < brain->num_inputs ) {
		world->print(ERROR, "Individual::initial_thinking(): Too small start_string ");
		return;
	}

	// feed string as inputs to the neural network
	inputs.clear();
	for(unsigned int i = 0; i < brain->num_inputs; i++) {
		inputs.push_back((double)(start_string[i]));
	}
	brain->set_inputs( inputs );
	brain->think(10);
}

int Individual::cwd_depth( const char *cwd ) {

	// sanity check
	if( strlen(cwd) < 1 ) return -1;

	// count number of slashes
	int c = 0; // c=1 means they are located in /, to avoid zero-values in the input layer of the ANN
	for( unsigned int i=0; i<strlen(cwd); i++ ) {
		if( cwd[i] == '/' ) c++;
	}

	return c;
}

bool Individual::get_ip_address( int *addr[] ) {
	struct ifaddrs *ifaddr;
	struct ifaddrs *ifa;
	int family;
	int s;
	char host[NI_MAXHOST];

	if( addr == NULL ) {
		world->print(ERROR,  "addr == NULL, aborting..." );
		return false;
	}

	if (getifaddrs(&ifaddr) == -1) {
		world->print(ERROR, "getifaddrs");
		return false;
	}

	for( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next ) {
		family = ifa->ifa_addr->sa_family;

		if ( (family == AF_INET) && strcmp( ifa->ifa_name, "lo" ) ) {
			s = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST );

			if( s != 0 ) {
				world->print(ERROR, "getnameinfo() failed");
				return false;
			}

			// parse host for A.B.C.D and store to array int **addr
			const char *delim = ".";
			char *ptr = NULL;
			int i = 0;
			ptr = strtok( host, delim );
			while( ptr != NULL ) {
				(*addr)[i++] = atoi( ptr );
				ptr = strtok( NULL, delim );
			}
		}
	}

	freeifaddrs(ifaddr);


	return true;
}
