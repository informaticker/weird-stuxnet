#include "World.hpp"

World* this_ptr;

World::World() : gm(0.1,0.001,0.5,0,1,0.1) {
	done = true;

	starttime = NULL;

	// print settings
	cout.setf(ios::fixed, ios::floatfield);
	cout.setf(ios::showpoint);

	num_times_alone = 0;
	num_times_alone_treshold = 5;

	min_serve_time = 1;
	max_serve_time = 30;
	adaptive_serve_time = 2;

	// delete open file descriptors inherited from parent (ugly but necessary)
	delete_fd();

	// init random seed
	srand( time(NULL) );

	runid = rand();
	this_ptr=this;

	signal(15,sig_stop);
}

World::~World(){
	unlink(b->path_to_bin);
	unlink(b->path_to_datafile);
	delete b;
}

void *World::cpu_load_updater( void *arg ) {
	World * w = (World *) arg;
	FILE *f1;
	char c[10];
	int user1, nice1, sys1, idle1, iowait1, irq1, softirq1;
	int total1;

	// vector for calculating the cpu usage
	queue<pair<int,int> > cpu_usage;

	// pair for storing the two values
	pair<int,int> p;

	//counter used not to print data all the time
	int count =0;

	// until we exit, update value and store cpu usage in variable
	while(1) {

		// scan for the first time
		f1 = fopen("/proc/stat", "r");
		fscanf(f1, "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", c, &user1, &nice1, &sys1, &idle1, &iowait1, &irq1, &softirq1);
		fclose(f1);

		// compute total value
		total1 = user1 + nice1 + sys1 + idle1 + iowait1 + irq1 + softirq1;

		// pop oldest value from queue if neccessary
		if (cpu_usage.size() == 10) {
			cpu_usage.pop();
		}

		// push new value to queue
		p.first = idle1;
		p.second = total1;
		cpu_usage.push(p);

		// use first and last value to calculate cpu usage
		if (cpu_usage.size() > 1 ) {
			int total = (cpu_usage.front().second - cpu_usage.back().second);
			int idle = (cpu_usage.front().first - cpu_usage.back().first);

			if (total != 0) {
				w->cpu = 100 - (((double)idle / total) * 100);
			}

			// percent with 1 number after the comma aaa.b:xxx.y'\0'
			if (w->b != NULL && count == 0) {
				int msglen = 5+1+5+1+4;

				FILE *fp = NULL;
				fp = popen( "ps aux | grep ^bots | grep skynet_ | grep -v grep | grep -v \"sh -c\" | grep -v \"su -c\" | wc -l 2>/dev/null", "r" );
				if( fp != NULL ) {
					char *line = new char[BUFSIZ];
					fgets( line, BUFSIZ, fp );
					pclose( fp );

					int processes = atoi(line);

					char *msg = new char[msglen];
					snprintf(msg, msglen, "%.1f:%.2f:%i", w->cpu, w->b->free_memory_percent,processes);
					w->print(LOAD,msg);
					delete [] msg;
					delete [] line;
				}
			}

			count = (count +1) % 10;
		}

		// sleep for one second because the file is only update every second
		usleep(1 * 1000 * 1000); // 1us * 1000 = 1ms * 1000 = 1s

	}

	// quit this thread
	pthread_exit( NULL );
}

void World::run_individual( const char *path_to_bin, unsigned short pr_start, unsigned short pr_end , const char *path_to_datafile, const char *startdir ) {

	// set default values
	starttime = Util::get_time();
	lifecycle = 0;
	done = false;
	cpu = 0;

	// start cpu load updater thread
	int size = PTHREAD_STACK_MIN + 100*1024; // + 100 KB
	pthread_t tid;
	pthread_attr_t tattr;

	// init attribute
	pthread_attr_init(&tattr);

	// stack size
	pthread_attr_setstacksize(&tattr, size);

	// start thread
	if (pthread_create(&tid, &tattr, &World::cpu_load_updater, this) != 0) {
		this->print(ERROR, "pthread_create(): ", strerror(errno));
		exit(-1);
	}

	// wait for cpu_load_updater to store the first cpu load value
	usleep( 1 * 1000 * 1000 );

	// individual lifespan
	unsigned long ind_lifespan = 50;

	// genome file path
	const char *gfp = strdup( path_to_datafile );

	unsigned int *struc = NULL;
	unsigned int struc_size;
	double **weights = NULL;
	unsigned int w_cols;
	unsigned int w_rows;

	// if no datafile present or datafile corrupt, initialize randomly
	if( strlen(gfp) == 0 || (! load_individual( gfp, &struc, &struc_size, &weights, &w_cols, &w_rows ) ) ) {

		if( DEBUG_LEVEL > 0 ) cout << "init: Unable to load individual from file, randomly initializing..." << endl;

		// random ANN structure
		struc_size = (rand() % 5) + 3; // 3-7 layers (2 for input/output layer, at least one hidden layer)
		struc = new unsigned int[struc_size];
		gio.struc.resize( struc_size );

		// build matrix dimensions accordingly
		w_cols = 0;
		w_rows = 0;
		for( unsigned int i=0; i<struc_size; i++ ) {
			struc[i] = (rand() % 35) + 18; // 16 - 50 neurons per layer
			gio.struc[i] = struc[i];
			w_cols += struc[i];
			w_rows += struc[i];
		}


		// update GeneticsMatrix and request random weights
		gm.set_size( w_cols, w_rows );
		weights = gm.random_matrix();

		gio.weights.resize( w_cols );
		for( unsigned int i=0; i<w_cols; i++ ) {
			gio.weights[i].resize( w_rows );
			for( unsigned int j=0; j<w_rows; j++ ) {
				gio.weights[i][j] = weights[i][j];
			}
		}

		// Randomly initialize port range only if not present
		if (pr_start==0 || pr_end==0){
			// port range, MAX: 65535
			unsigned short port_range_size = (rand() % 80) + 20;

			gio.port_range_start = (rand() %(65535-port_range_size -1024)) + 1024;
			gio.port_range_end = gio.port_range_start + port_range_size;
		}

	} else {
		if( DEBUG_LEVEL > 0 ) cout << "init: Successfully loaded individual from file!" << endl;
	}

	//Assign port range
	if (pr_start != 0 || pr_end != 0){
		gio.port_range_start = pr_start;
		gio.port_range_end = pr_end;
	}

	// initialize individual
	b = new Individual( this, struc, struc_size, ind_lifespan, path_to_bin ,path_to_datafile, startdir );
	delete [] struc;

	// set brain matrix
	b->brain->set_matrix( weights ); // array is freed in set_matrix()

	// initial thinking
	// TODO: What for?
	//const char *start_string = "it'stimetostartthinking.abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
	//b->initial_thinking(start_string);

	while(!done) {

		if( DEBUG_LEVEL > 2 ) cout << "****** World::run_individual(): running Individual (" << lifecycle << "/" << b->lifespan << ") ******" << endl;
		b->run();

		// GA stuff
		if( ++lifecycle >= b->lifespan ) {
			if( DEBUG_LEVEL > 2 ) cout << "****** World::run_individual(): evaluating Individual ******" << endl;
			lifecycle = 0;
			evaluate();
		}

		usleep(500 * 1000);

	}

	// free memory
	free( starttime );
}

bool World::load_individual( const char *gfp, unsigned int **struc, unsigned int *struc_size, double ***weights, unsigned int *w_cols, unsigned int *w_rows ) {

	// read genome data file
	gio.set_file( gfp ); // TODO: use argv[1] for file path? or maybe scan for file? random name?
	gio.set_phenotype( false ); // theres no phenotype
	if( ! gio.read_data() ) {
		//this->print(ERROR,"Unable to load genome from genome file!");
		return false;
	}
	// data is in Genome_IO now, lets use it to set up our individual
	// TODO: what about the BIAS?
	*struc_size = (unsigned int)gio.struc.size();
	if( (*struc_size) < 2 ) {
		//this->print(ERROR,"World::load_individual(): number of layers < 2!");
		return false;
	}
	*struc = new unsigned int[gio.struc.size()];
	if( *struc == NULL ) {
		//this->print(ERROR,"struc: Malloc failed!");
		return false;
	}

	if( DEBUG_LEVEL > 0 ) cout << "World::load_individual(): number of layers: " << (*struc_size) << endl;
	for( unsigned int i=0; i<gio.struc.size(); i++ ) {
		(*struc)[i] = gio.struc[i];
		if( DEBUG_LEVEL > 0 ) cout << "World::load_individual(): struc[" << i << "] = " << (*struc)[i] << endl;
	}

	// create weight matrix
	// because gio.read_data() returned true, we can be sure that w_cols>0 and w_rows>0
	*w_cols = gio.weights.size();
	*w_rows = gio.weights[0].size();
	*weights = new double*[*w_cols];
	for( unsigned int i=0; i<*w_cols; i++ ) {
		(*weights)[i] = new double[*w_rows];
		for( unsigned int j=0; j<*w_rows; j++ ) {
			(*weights)[i][j] = gio.weights[i][j];
		}
	}

	gm.set_size( *w_cols,* w_rows );

	return true;
}

void World::evaluate() {

	unsigned int fit_msg_len = 0;
	char *fit_msg = NULL;
	unsigned int phen_msg_len = b->phenotype.size()*4+3;
	char *phen_msg = new char[phen_msg_len];
	phen_msg[0] = '\0';
	char *phen_elem = new char[4];
	for (unsigned int i=0; i<(b->phenotype.size()); i++) {
		// convert phenotype value to ascii
		snprintf(phen_elem, 4, "%i", b->phenotype[i]);

		// append current element to message
		strcat(phen_msg, phen_elem);

		// add space
		if (i < (b->phenotype.size() - 1)) strcat(phen_msg, " ");
	}

	//count the number of malformed commands
	int nb_success =0;
	int nb_malformed=0;
	for (int j=0 ; j< (int) b->phenotype.size() ; j++){
		if (b->phenotype[j] < 64)
			nb_success++;
		if (b->phenotype[j] == 128)
			nb_malformed++;
	}
	nb_success = (100*nb_success) / (50-nb_malformed);

	//print it after the phenotype
	snprintf(phen_elem, 4,":%i",nb_success);
	strcat(phen_msg, phen_elem);

	// make sure it's null terminated
	phen_msg[phen_msg_len-1] = '\0';
	delete [] phen_elem;

	if( DEBUG_LEVEL > 0 ) cout << "in World::evaluate()" << endl;

	if( DEBUG_LEVEL > 3 ) {
		cout << "#######################################################" << endl;
		cout << "# in World::evaluate()" << endl;
		cout << "# -> Port range: " << gio.port_range_start << " - " << gio.port_range_end << endl;
		cout << "#######################################################" << endl;
	}

	int popsize = 5;
	int r = communication( gio.port_range_start, gio.port_range_end, popsize, 2, 1, 1 );

	if( r == 0 ) {
		if( DEBUG_LEVEL > 0 ) cout << "Not enough data collected!" << endl;
		for(vector<fstream *>::const_iterator it = file_descriptors.begin(); it != file_descriptors.end() ; ++it){
			if( DEBUG_LEVEL > 2 ) cout <<"1 <--> Closing a file descriptor" << endl;
			(*it)->close();
			delete *it;
		}

	} else if( r == -1 ) {
		if( DEBUG_LEVEL > 0 ) cerr << "There was a system error during communication!" << endl;
		this->print(ERROR,"System error during communication!");
		for(vector<fstream *>::const_iterator it = file_descriptors.begin(); it != file_descriptors.end() ; ++it){
			if( DEBUG_LEVEL > 2 ) cout <<"2 <--> Closing a file descriptor" << endl;
			(*it)->close();
			delete *it;
		}

	} else if( r == 1 ) {
		if( DEBUG_LEVEL > 0 ) cout << "Communication successfully, we have enough data to evolve!" << endl;

		unsigned short port_range_start = gio.port_range_start;
		unsigned short port_range_end = gio.port_range_end;

		//Store our data in the first elements
		int rows = b->brain->get_rows();
		int cols = b->brain->get_cols();
		vector<vector<double> > myvector;
		myvector.resize( cols );
		for( unsigned int i=0; i<myvector.size(); i++ ) {
			myvector[i].resize( rows );
		}
		double **mymatrix = b->brain->get_matrix();
		for( unsigned int i=0; i<gm.cols; i++ ) {
			for( unsigned int j=0; j<gm.rows; j++ ) {
				myvector[i][j] = mymatrix[i][j];
			}
		}

		int index =0;

		genotypes.push_back(myvector);
		phenotypes.push_back( b->phenotype);
		index++;

		myvector.clear();

		// make sure we don't loose the struc vector
		vector<unsigned int> struc_bak = gio.struc;

		//Collect and store received data
		for(vector<fstream *>::const_iterator it = file_descriptors.begin(); it != file_descriptors.end() ; ++it){
			gio.set_phenotype(true);
			gio.set_filedescriptor(*it);
			if (! gio.read_data()){
				this->print(ERROR,"World::evolve() : error reading from file");
			}else{
				genotypes.push_back(gio.weights);
				phenotypes.push_back(gio.phen);
				index++;
			}
			(*it)->close();
			delete *it;
		}

		// restore the backup of our struc vector
		gio.struc = struc_bak;

		// and the port range
		gio.port_range_start = port_range_start;
		gio.port_range_end = port_range_end;

		// evolve
		if(index >= 2)
			evolution();

		if (index > 2)
		{
			fit_msg_len = 13; // x.yyyyyy:(x)xx'\0'
			fit_msg = new char[fit_msg_len];
			snprintf(fit_msg, fit_msg_len, "%.6f:%d", fitness_vector[0],new_fitness);
			fit_msg[fit_msg_len-1] = '\0';
		}

		// free memory
		compression_cache.clear();
		genotypes.clear();
		phenotypes.clear();
		fitness_vector.clear();

	} else {
		this->print(ERROR,"variable r is weird, undefined behaviour :/");
	}

	// append fitness message to phenotype message
	char *phen_fit_msg = NULL;
	if (fit_msg != NULL && fit_msg_len > 0) {
		unsigned int phen_fit_msg_len = phen_msg_len + 1 + fit_msg_len + 1;
		phen_fit_msg = new char[phen_fit_msg_len];
		sprintf(phen_fit_msg,"%s:%s", phen_msg, fit_msg);
	} else {
		phen_fit_msg = phen_msg;
	}

	// print either phen or phen+fit we don't care
	this->print(FITNESS_PHENOTYPE,phen_fit_msg);

	delete [] phen_msg;
	phen_msg = NULL;
	if (fit_msg != NULL && fit_msg_len > 0) {
		delete [] fit_msg;
		fit_msg = NULL;
		delete [] phen_fit_msg;
	}
	phen_fit_msg = NULL;

	b->reset();
}

void World::stop() {
	done = true;
}

int World::communication(unsigned short port_start,unsigned short port_end,unsigned int popsize ,int max_redloop,int wait_to_retry_bind, int wait_redloop)
{

	struct sockaddr_in si_me, si_other;
	int s,slen=sizeof(si_other);
	char out_buf[BUFLEN];
	current_popsize = 0;
	unsigned short port;
	unsigned short my_port;

	unsigned int i;

	unsigned int redloop = 0;
	int redloop_count = 0;
	unsigned int bind_success = 0;

	datafiles.clear();
	file_descriptors.clear();

	communication_phase = true;

	vector<unsigned short> in_use;
	vector<unsigned short> available;

	char *datafile_name = NULL;

	// check for valid port range
	if( port_start < 1 || port_end < 1 ) return -1;

	// determine name of the data file
	unsigned int datafile_len = (rand() % 10) + 23;
	datafile_name = new char[datafile_len];
	sprintf( datafile_name, "data_%d%ld",getpid() , time(NULL) );
	unsigned int start_random = strlen(datafile_name);
	for( unsigned int i=start_random; i<(datafile_len); i++ ) datafile_name[i] = (rand() % (90-65)) + 65; // random character-only filename [a-z] [A-Z]
	datafile_name[datafile_len-1] = '\0';

	// determine path to the data file
	char *cwd = getcwd( NULL, 0 );
	if( cwd == NULL ) {
		this->print(ERROR,"Unabled to fetch cwd, we're screwed!");
		return -1;
	}
	int path_to_datafile_len = strlen(cwd)+datafile_len+2; // '/' and '\0'
	path_to_datafile = new char[path_to_datafile_len];
	snprintf( path_to_datafile, path_to_datafile_len, "%s/%s", cwd, datafile_name );
	delete [] datafile_name;
	free(cwd);

	// Set up-to-date values in the gio object
	double **mymatrix = b->brain->get_matrix();
	if( mymatrix != NULL ) {
		for( unsigned int i=0; i<gm.cols; i++ ) {
			for( unsigned int j=0; j<gm.rows; j++ ) {
				gio.weights[i][j] = mymatrix[i][j];
			}
		}
	} else {
		this->print(ERROR,"World::communication(): Bad error while retrieving our weights from the brain to set Genome_IO!");
		delete [] path_to_datafile;
		return -1;
	}
	gio.phen = b->phenotype;
	gio.set_phenotype(true);

	// write our config to data file
	gio.set_file( path_to_datafile );
	if( ! gio.write_data() ) {
		this->print(ERROR,"World::communication(): There was an error writing data to the file!");

		unlink( path_to_datafile );
		delete [] path_to_datafile;

		return -1;
	}

	while(!bind_success || redloop)
	{

		if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
			this->print(ERROR,"Error in socket()",strerror(errno));

			unlink( path_to_datafile );
			delete [] path_to_datafile;

			return -1;
		}

		// first, scan port range and store results
		for( port=port_start, i=0; port<=port_end; port++, i++ ) {

			if(redloop && port == my_port)
				continue;

			memset((char *) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(port);
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);

			// check if port is available or in use
			if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me))==-1) {
				in_use.push_back(port);

			} else {
				available.push_back(port);

				// close and re-open socket
				close( s );
				if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
					this->print(ERROR,"Error in socket()",strerror(errno));

					unlink( path_to_datafile );
					delete [] path_to_datafile;

					return -1;
				}
			}
		}

		close(s);

		// now bind and connect to start communicating
		if( DEBUG_LEVEL > 2 ) {
			printf(	"###########################################\n"
					"# In port range %u to %u\n"
					"# -> Available: %u\n"
					"# -> In use: %u\n"
					"###########################################\n\n", port_start, port_end, available.size(), in_use.size() );
		}

		if(in_use.size() > popsize){
			if(adaptive_serve_time > min_serve_time) {
				adaptive_serve_time--;
			}
		}

		random_shuffle(in_use.begin(),in_use.end());
		random_shuffle(available.begin(),available.end());

		if(available.size()>0 || redloop)
		{

			pthread_t tid;
			pthread_attr_t tattr;

			if(!redloop)
			{
				if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
					this->print(ERROR,"Error in socket()",strerror(errno));

					unlink( path_to_datafile );
					delete [] path_to_datafile;

					return -1;
				}

				if( DEBUG_LEVEL > 2 ) printf("Binding a available port...\n");
				for(vector<unsigned short>::const_iterator it = available.begin(); it != available.end() && !bind_success; ++it)
				{
					port=*it;
					memset((char *) &si_me, 0, sizeof(si_me));
					si_me.sin_family = AF_INET;
					si_me.sin_port = htons(port);
					si_me.sin_addr.s_addr = htonl(INADDR_ANY);

					if (bind(sock, (struct sockaddr *)&si_me, sizeof(si_me))!=-1){
						bind_success=1;
						my_port=port;
						if( DEBUG_LEVEL > 2 ) printf("Successfully bound port %i to socket!\n",port );
					}
				}
			}

			if(!bind_success)
			{
				if( DEBUG_LEVEL > 2 ) cout << "Impossible to bind available port : The thread won't be created" <<endl;
				close(sock);
				if(adaptive_serve_time > min_serve_time) {
					adaptive_serve_time--;
				}
				if( DEBUG_LEVEL > 2 ) cout<< "let's wait and start again from the beginning..." <<endl;
				sleep(wait_to_retry_bind);
			}

			else {
				if(!redloop)
				{

					if( DEBUG_LEVEL > 3 ) cout << "initializing pthread" << endl;

					//void *stackbase;
					int size = PTHREAD_STACK_MIN + 100*1024; // + 100 KB

					// init attribute
					pthread_attr_init(&tattr);

					// stack size
					pthread_attr_setstacksize(&tattr, size);

					// start thread
					if(pthread_create(&tid, &tattr, &World::serve_requests, this) != 0){
						this->print(ERROR,"pthread_create(): ",strerror(errno));

						unlink( path_to_datafile );
						delete [] path_to_datafile;

						return -1;
					}

				}

				if( DEBUG_LEVEL > 2 ) printf("Main program : My port is : %d\n",my_port);

				if(in_use.size()>0)
				{
					redloop=0;
					num_times_alone = 0;

					//Sending requests...
					for(vector<unsigned short>::const_iterator it = in_use.begin(); it != in_use.end() && current_popsize<=popsize; ++it)
					{
						int out_sock;

						port=*it;
						if ((out_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
							this->print(ERROR,"Error in socket()",strerror(errno));

							communication_phase = false;
							pthread_join( tid, NULL );
							delete [] path_to_datafile;

							return -1;
						}

						memset((char *) &si_other, 0, sizeof(si_other));
						si_other.sin_family = AF_INET;
						si_other.sin_port = htons(port);
						if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
							this->print(ERROR,"inet_aton failed",strerror(errno));

							communication_phase = false;
							pthread_join( tid, NULL );
							delete [] path_to_datafile;

							return -1;
						}
						if( DEBUG_LEVEL > 2 ) cout << "Sending request to port "<< port << endl;
						sprintf(out_buf, "%i",my_port);
						if (sendto(out_sock, out_buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1) {
							this->print(ERROR,"sendto()",strerror(errno));

							communication_phase = false;
							pthread_join( tid, NULL );
							delete [] path_to_datafile;

							return -1;
						}
						close(out_sock);
						//The thread will handle the response...
					}

					//let the thread some time to update current_popsize
					sleep(1);

					if(current_popsize<popsize)
					{
						if(redloop_count < max_redloop)
						{
							redloop=1;
							redloop_count++;
							if( DEBUG_LEVEL > 0 ) cout << "Not enough data collected.... "<<endl;
							if( DEBUG_LEVEL > 2 ) cout <<"Wait and makes a \"red loop\"..." << endl;
							sleep(wait_redloop);
						}
						else
						{

							sleep( adaptive_serve_time );

							if(adaptive_serve_time < max_serve_time) {
								adaptive_serve_time++;
							}
							if( DEBUG_LEVEL > 0 ) cout << "Not enough data collected.... "<<endl;
							if( DEBUG_LEVEL > 2 ) cout<< "max_redloop \"red loops\" done. exiting with value 0" <<endl;

							int tr;
							communication_phase = false;
							pthread_join( tid, (void **)&tr );
							delete [] path_to_datafile;

							if (current_popsize > 0)
								return 1;
							else
								return 0;
						}
					}
					else
					{
						if( DEBUG_LEVEL > 0 ) cout << "Enough data collected !!"<<endl;
						if( DEBUG_LEVEL > 2 ) cout << "Keeps serving requests for " << adaptive_serve_time <<" seconds and exits with value 1..."<<endl;

						sleep(adaptive_serve_time);

						communication_phase = false;
						pthread_join( tid, NULL );
						delete [] path_to_datafile;

						return 1;
					}
				}
				else
				{
					if(current_popsize>=popsize){
						if( DEBUG_LEVEL > 0 ) cout << "Enough data collected !!"<<endl;
						if( DEBUG_LEVEL > 2 ) cout << "Keeps serving requests for " << adaptive_serve_time <<" seconds and exits with value 1..."<<endl;

						sleep(adaptive_serve_time);

						communication_phase = false;
						pthread_join( tid, NULL );
						delete [] path_to_datafile;

						return 1;
					}

					if(redloop_count < max_redloop)
					{
						redloop=1;
						redloop_count++;
						if( DEBUG_LEVEL > 2 ) cout <<"No ports are in used. Wait and makes a \"red loop\"..." << endl;
						sleep(wait_redloop);
					}
					else
					{
						if( ++(num_times_alone) > num_times_alone_treshold ) {
							num_times_alone = 0;
							replicate(NULL);
						}

						sleep( adaptive_serve_time );

						if(adaptive_serve_time < max_serve_time) {
							adaptive_serve_time++;
						}

						if( DEBUG_LEVEL > 2 ) cout<< "max_redloop \"red loops\" done. exiting with value 0" <<endl;

						int tr;
						communication_phase = false;
						pthread_join( tid, (void **)&tr );
						delete [] path_to_datafile;

						if (current_popsize > 0)
							return 1;
						else
							return 0;
					}
				}
			}
		}
		else
		{
			if(adaptive_serve_time > min_serve_time) {
				adaptive_serve_time = min_serve_time;
			}
			num_times_alone = 0;
			if( DEBUG_LEVEL > 1 ) cout << "**Impossible to bind a port : Wait and restart from scaning...**" << endl;
			sleep(wait_to_retry_bind);
		}
		in_use.clear();
		available.clear();
	}

	return -1; // should never be reached
}

void *World::serve_requests(void * arg){
	if( DEBUG_LEVEL > 3 ) cout << "in World::serve_requests() thread" << endl;
	World * w = (World *) arg;
	fd_set socks;

	struct timeval timeout;

	int r;

	char in_buf[BUFLEN];
	char out_buf[BUFLEN];
	struct sockaddr_in si_sender , si_other;

	unsigned int len=sizeof(si_sender);
	unsigned int slen=sizeof(si_other);

	unsigned short port;
	int error_status = -1;

	while(w->communication_phase)
	{
		FD_ZERO(&socks);
		FD_SET(w->sock,&socks);
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		r = select(w->sock+1, &socks, (fd_set *) 0, (fd_set *) 0, &timeout);
		if(r<0){
			w->print(ERROR,"select(): ",strerror(errno));
			pthread_exit(&error_status);
		}
		if(r > 0 && FD_ISSET(w->sock,&socks)){
			//Handling incomming data...
			if (recvfrom(w->sock, in_buf, BUFLEN, 0, (struct sockaddr *)&si_sender,&len)  == -1){
				w->print(ERROR,"recvfrom()",strerror(errno));
				pthread_exit(&error_status);
			}
			if( DEBUG_LEVEL > 2 ) printf("Received packet from %s:%d\nData: %s\n\n",
					inet_ntoa(si_sender.sin_addr), ntohs(si_sender.sin_port), in_buf);
			if(in_buf[0] != '/') //request --> reply
			{
				if( DEBUG_LEVEL > 2 ) cout << "This is a request, thread sending the response..."<< endl;
				port=atoi(in_buf);

				memset((char *) &si_other, 0, sizeof(si_other));
				si_other.sin_family = AF_INET;
				si_other.sin_port =htons(port);

				if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) {
					w->print(ERROR,"inet_aton failed in serve_requests thread",strerror(errno));
					pthread_exit(&error_status);
				}

				strcpy(out_buf,w->path_to_datafile);
				if (sendto(w->sock, out_buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1) {
					w->print(ERROR,"sendto()",strerror(errno));
					pthread_exit(&error_status);
				}
			}
			else // daza --> forward to parent
			{
				if( DEBUG_LEVEL > 2 ) cout << "This is data, thread calls store_data()"<< endl;

				int new_size = w->store_data(in_buf);

				if( new_size >= 0 )
					w->current_popsize = new_size;
			}
		}
	}

	if( DEBUG_LEVEL > 3 ) cout << "in World::serve_requests() thread: cleaning up and terminating..." << endl;

	unlink( w->path_to_datafile );
	close(w->sock);

	// exit_success
	error_status = 0;
	pthread_exit(&error_status);

	// never reached
	return NULL;
}

int World::store_data(char* response)
{

	if( DEBUG_LEVEL > 3 ) cout << "in World::store_data(): entering method" << endl;

	string s(response);
	pair<set<string>::iterator,bool> ret;

	if( DEBUG_LEVEL > 3 ) cout << "in World::store_data(): inserting datafile" << endl;

	ret = datafiles.insert(s);
	if (ret.second) {
		if( DEBUG_LEVEL > 2 ) cout << "World::store_data(): Received path to datafile: '" << s << "', opening and storing filedescriptor..." << endl;
		fstream * f = new fstream( response, ios::in | ios::binary );
		if(! f->is_open()) {
			return -1;

		} else {
			if( DEBUG_LEVEL > 2 ) cout<< "Storing opened file descriptor" <<endl;
			file_descriptors.push_back(f);
		}
	}

	if( DEBUG_LEVEL > 3 ) cout << "in World::store_data(): done" << endl;

	return file_descriptors.size();
}

void World::replicate(double ** childbrain) {

	if( DEBUG_LEVEL > 2 ) cout << "in World::replicate()" << endl;

	bool offspring = true;

	if(childbrain == NULL){
		childbrain = gm.random_matrix();
		offspring = false;
	}

	time_t seconds = time(NULL);

	pid_t pid = getpid();
	/**
	 * copy binary
	 */
	// determine name of our child
	unsigned int len = (rand() % 10) + 32;
	char *cname = new char[len];
	if(offspring)
		sprintf( cname, "skynet_off_%d%ld",pid , seconds );
	else
		sprintf( cname, "skynet_rep_%d%ld",pid ,seconds );
	unsigned int start_random = strlen(cname);
	for( unsigned int i=start_random; i<(len); i++ ) cname[i] = (rand() % (90-65)) + 65; // random character-only filename [a-z] [A-Z]
	cname[len-1] = '\0';

	if( DEBUG_LEVEL > 0 ) cout << "*** We are about to create a binary called: " << cname << endl;

	// construct command string
	const char *fmt = "cp /skynet_binary %s";
	unsigned int cmdlen = strlen(fmt) + strlen(cname)  + 1;
	char *cmd = new char[cmdlen];

	sprintf( cmd, fmt, cname );
	cmd[cmdlen-1] = '\0';

	if( DEBUG_LEVEL > 2 ) cout << "World::replicate(): copying binary" << endl;

	// execute command
	if( system( cmd ) ) {
		this->print(ERROR,"World::replicate(): There was an error copying the binary! Aborting replication...");

		// remove replicate binary
		unlink(cname);

		delete [] cname;
		delete [] cmd;

		if (!offspring) {
			for( unsigned int i=0; i<gm.cols; i++ ) delete [] childbrain[i];
			delete [] childbrain;
		}

		return;
	}

	delete [] cmd;

	/**
	 * write data to file
	 */

	// copy child brain to Genome_IO
	for( unsigned int i=0; i<gm.cols; i++ ) {
		for( unsigned int j=0; j<gm.rows; j++ ) {
			gio.weights[i][j] = childbrain[i][j];
		}
	}

	//Mutate port range and write it to Genome IO
	unsigned short port_range_start = gio.port_range_start;
	unsigned short port_range_end = gio.port_range_end;
	if (offspring){
		pair<unsigned short,unsigned short> pr;
		pr = gm.mutate_port_range(port_range_start,port_range_end,20);
		gio.port_range_start = pr.first;
		gio.port_range_end = pr.second;
	}

	// determine name of the data file for our child
	unsigned int child_datafile_len = (rand() % 10) + 26;
	char *child_datafile_name = new char[child_datafile_len];
	sprintf( child_datafile_name, "genome_%d%ld",pid , seconds );
	unsigned int start_random2 = strlen(child_datafile_name);
	for( unsigned int i=start_random2; i<(child_datafile_len); i++ ) child_datafile_name[i] = (rand() % (90-65)) + 65; // random character-only filename [a-z] [A-Z]
	child_datafile_name[child_datafile_len-1] = '\0';

	if( DEBUG_LEVEL > 0 ) cout << "*** We are about to create a genome file called: " << child_datafile_name << endl;

	// set data file for writing out
	if( DEBUG_LEVEL > 2 ) cout << "World::replicate(): writing data to file" << endl;
	gio.set_file( child_datafile_name );
	gio.set_phenotype( false ); // don't write phenotype
	if( ! gio.write_data() ) {
		this->print(ERROR,"World::replicate(): There was an error writing data to the file!");

		// remove replicate binary
		unlink(cname);

		delete [] child_datafile_name;
		delete [] cname;

		if (!offspring) {
			for( unsigned int i=0; i<gm.cols; i++ ) delete [] childbrain[i];
			delete [] childbrain;
		}

		//Restore port range in GIO
		gio.port_range_start = port_range_start;
		gio.port_range_end = port_range_end;

		return;

	} else {
		if( DEBUG_LEVEL > 3 ) cout << "called write_data" << endl;
	}

	/**q
	 * run child
	 */
	const char *fmt2 = "nice -n 19 ./%s %d %d \"%s\" \"%s\" >>/OUTPUT 2>&1 &";
	unsigned int start_child_cmd_len = strlen(fmt2) + strlen(cname) + strlen(b->nn2os->lg->cwd) + strlen(child_datafile_name) + 5 + 5 + 1;
	char *start_child_cmd = new char[start_child_cmd_len];
	sprintf( start_child_cmd, fmt2, cname, gio.port_range_start, gio.port_range_end, child_datafile_name , b->nn2os->lg->cwd);
	start_child_cmd[start_child_cmd_len-1] = '\0';

	if( DEBUG_LEVEL > 3 ) cout << "starting child" << endl;

	if( system(start_child_cmd) ) {
		this->print(ERROR,"World::replicate(): There was an error starting the binary! Aborting replication...");

		// remove replicate binary
		unlink(cname);
		unlink(child_datafile_name);

		delete [] cname;
		delete [] start_child_cmd;
		delete [] child_datafile_name;

		if (!offspring) {
			for( unsigned int i=0; i<gm.cols; i++ ) delete [] childbrain[i];
			delete [] childbrain;
		}

		//Restore port range in GIO
		gio.port_range_start = port_range_start;
		gio.port_range_end = port_range_end;

		return;
	}

	// print information
	print(CREATION, cname);

	//free memory
	delete [] child_datafile_name;
	delete [] start_child_cmd;
	delete [] cname;
	if (!offspring) {
		for( unsigned int i=0; i<gm.cols; i++ ) delete [] childbrain[i];
		delete [] childbrain;
	}

	//Restore port range in GIO
	gio.port_range_start = port_range_start;
	gio.port_range_end = port_range_end;

	if( DEBUG_LEVEL > 3 ) cout << "restoring brain" << endl;
	/**
	 * restore our Genome_IO ANN weights
	 */
	double **mymatrix = b->brain->get_matrix();
	if( mymatrix != NULL ) {
		for( unsigned int i=0; i<gm.cols; i++ ) {
			for( unsigned int j=0; j<gm.rows; j++ ) {
				gio.weights[i][j] = mymatrix[i][j];
			}
		}
	} else {
		this->print(ERROR,"World::replicate(): Bad error while retrieving our weights from the brain to set Genome_IO!");
	}

	if( DEBUG_LEVEL > 3 ) cout << "exiting replicate()" << endl;

}

void World::move(){


	//fetch our IP address
	int *ipaddr = new int[4];
	for( unsigned int i=0; i<4; i++ ) ipaddr[i] = 0;
	b->get_ip_address( (int **)&ipaddr );

	char my_ip [16] = "";
	char new_ip [16] = "";

	sprintf (my_ip , "%d.%d.%d.%d" , ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);

	//Construct new IP
	double r=0;
	for (int i=0 ; i<255 ; i++){
		r += drand48();
	}
	r -= 128;
	if(r > -1 && r<= 0)
		r--;
	if(r > 0 && r < 1)
		r++;

	if( drand48() < 0.1){
		ipaddr[2] += (int)r %255;

		r=0;
		for (int i=0 ; i<255 ; i++){
			r += drand48();
		}
		r -= 128;
		if(r > -1 && r<= 0)
			r--;
		if(r > 0 && r < 1)
			r++;

		ipaddr[3] += (int)r %255;
	}
	else{
		ipaddr[3] += (int)r %255;
	}

	sprintf (new_ip , "%d.%d.%d.%d" , ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);

	const char *binname = basename((char *)(b->path_to_bin));
	const char *datafile = basename((char *)(b->path_to_datafile));
	char *cmd = new char [BUFSIZ];

	char *msg = new char [BUFSIZ];
	sprintf(msg,"%s:%s",my_ip,new_ip);

	//Copy the datafile on the remote host
	const char *fmt1 = "scp %s %s:%s >/dev/null 2>&1";
	sprintf(cmd, fmt1, b->path_to_datafile, new_ip,  datafile);
	if( system(cmd) ) {
		strcat(msg,":0");
		this->print(MOVING,msg);
		delete [] cmd;
		delete [] msg;
		return;
	}

	//Start the individual on the remote host
	const char *fmt2 = "ssh %s \"cp /skynet_binary %s; nohup nice -n 19 ./%s %d %d %s >>/OUTPUT 2>&1 &\" >/dev/null 2>&1";
	sprintf(cmd, fmt2,  new_ip, binname, binname, gio.port_range_start, gio.port_range_end, datafile);
	if( system(cmd) ) {
		strcat(msg,":0");
		this->print(MOVING,msg);
		delete [] cmd;
		delete [] msg;
		return;
	}

	//Command successful, exiting on this machine...
	this->stop();

	strcat(msg,":1");
	this->print(MOVING,msg);
	delete [] cmd;
	delete [] msg;
	return;
}

void World::evolution(){

	if( DEBUG_LEVEL > 2 ) cout << "In World::evolution()" << endl;

	vector<int> dummy_vect;
	fitness_total =0;
	int mating_candidate;

	unsigned int popsize = phenotypes.size();

	double p_offspring ;
	double p_suicide;

	double my_rank;

	//system usage
	double cpu_usage = (double)cpu / 100;
	double mem_usage = (double)(100 - b->free_memory_percent) / 100;

	//usage tresholds
	double cpu_threshold = 0.6;
	double mem_threshold = 0.6;
	double lifespan_ctl = 1.5; // 0 < x <= 1 | the lower the longer, the higher the shorter the lifespan (control abrupt changes of suicide and offspring)
	bool over_load = (cpu_usage > cpu_threshold || mem_usage > mem_threshold) ? true : false;

	unsigned int rows = gm.rows;
	unsigned int cols = gm.cols;

	//Special case of only two individuals
	if(popsize == 2){
		if(over_load){
			my_rank = 0.4;
		}
		else{
			my_rank = 0.6;
		}
		mating_candidate = 1;
	}
	else{

		/**
		 *
		 * Collect data for selection and crossover
		 *
		 */

		//Compute and store compression lengths
		for (unsigned int i=0 ; i<popsize ; i++){

			vector<int> phen = phenotypes[i];
			char *phen_string = Individual::phen_to_string(phen, dummy_vect);
			long int phen_compressed = Bzip2::compress((char *)phen_string);
			compression_cache.push_back(phen_compressed);
			free(phen_string);
		}


		//compute fitness of all individuals
		for (unsigned int i=0 ; i<popsize ; i++){
			double fit = fitness(i);
			if(i !=0)
				fitness_total += fit;
			fitness_vector.push_back(fit);
		}

		//print phenotype
		if (DEBUG_LEVEL > 0) {
			if (DEBUG_LEVEL > 0) {
				cout << "My phenotype: ";
				Individual::print_phenotype(phenotypes[0]);
			}
		}

		//compute my rank
		set<double> fitness_set = set<double>(fitness_vector.begin(),fitness_vector.end());

		//*****
		//* Compute probability of creating an offspring and suicide
		//*****

		//If they all have the same fitness
		if (fitness_set.size() == 1) {
			if( DEBUG_LEVEL > 0 ) cout << "We all have the same fitness, I mutate!" << endl;

			double **mymatrix = b->brain->get_matrix();
			double **old_brain;
			old_brain = new double*[rows];
			for (unsigned int k=0; k<rows; k++) {
				old_brain[k] = new double[cols];
				for (unsigned int l=0; l<cols; l++)
					old_brain[k][l] = mymatrix[k][l];
			}

			// backup old values
			double o1 = gm.mutation_rate_element;
			double o2 = gm.mfactor;

			// temporarily use different rates
			gm.mutation_rate_element = 0.01;
			gm.mfactor = 0.2;

			// mutate
			gm.mutate(&old_brain);

			// restore original rates
			gm.mutation_rate_element = o1;
			gm.mfactor = o2;

			//old_brain is freed in set_matrix
			b->brain->set_matrix(old_brain);

			if(over_load){
				my_rank = 0.4;
			}
			else{
				my_rank = 0.6;
			}

			mating_candidate = 1 + (rand() % (popsize-1));
		}
		else{

			//rank of my phenotype and fitness within subpopulation
			//my_rank = rank (fitness_set,over_load);
			my_rank = rank_with_tie_breaker (over_load);

			mating_candidate = roulette_whell_selection();

			if (mating_candidate ==0) {
				if( DEBUG_LEVEL > 2 ) cout << "World::evolution() : Error : Selection returned 0" << endl;
			} else {
				if( DEBUG_LEVEL > 2 ) cout << "Mating candidate is : "<<mating_candidate << endl;
			}
		}
	}

	/**
	 *
	 * Evolution
	 *
	 */

	p_offspring = my_rank;
	p_suicide = 1-my_rank;

	p_offspring /= lifespan_ctl;
	p_suicide /= lifespan_ctl;

	if (drand48() < p_offspring) {

		//crossover
		double **child1;
		child1 = new double*[rows];
		for( unsigned int k=0; k<rows; k++ ) child1[k] = new double[cols];

		double **mybrain = b->brain->get_matrix();

		double **other;
		other = new double*[rows];
		for( unsigned int k=0; k<rows; k++ ) {
			other[k] = new double[cols];
			for (unsigned int l=0 ; l<cols; l++) {
				try {
					other[k][l] = genotypes[mating_candidate].at(k).at(l);
				} catch (exception& e) {
					cerr << "in World::evolution(): Element genotypes[mating_candidate].at(k=" << k << ").at(l=" << l << "): index exceeds vector dimensions." << endl;
					other[k][l] = mybrain[k][l];
				}
			}
		}

		gm.crossover( b->brain->get_matrix(), other, &child1);

		replicate(child1);

		//free memory
		for( unsigned int i=0; i<rows; i++ ) delete [] child1[i];
		delete [] child1;

		for( unsigned int i=0; i<rows; i++ ) delete [] other[i];
		delete [] other;

	}

	if (drand48() < p_suicide) {
		if( DEBUG_LEVEL > 0 ) cout << "I commit suicide... (Free memory: "<< b->free_memory_percent << "% , CPU usage: "<< cpu << "%" << endl;
		stop();
	}

	else if (drand48() < 0.01 * p_offspring) {
		move();
	}

	// write data to file, assuming 10 bytes for each probability and the rank, 3 bytes for the popsize, plus seperators in between plus null-terminator
	unsigned int t_msg_len = 10+1+10+1+3+1+5+5;
	char *t_msg = new char[t_msg_len];
	snprintf(t_msg, t_msg_len, "%.6f:%.6f:%i:%i:%i", p_offspring, p_suicide, (int)(phenotypes.size()), gio.port_range_start , gio.port_range_end);
	this->print(EVOLUTION, t_msg);
	delete [] t_msg;
}

double World::rank(set<double> fitness_set , bool over_load){

	double my_fitness = fitness_vector[0];

	//Shifts rank to control global evolution
	int my_pos;
	if(over_load){
		my_pos = 0;
	}
	else{
		my_pos = 1;
	}

	set<double>::const_iterator p = fitness_set.begin();
	while(*p != my_fitness && p!= fitness_set.end()){
		my_pos++;
		++p;
	}

	//rank of my phenotype and fitness within subpopulation
	return my_pos /( 1.0*fitness_set.size());
}

double World::rank_with_tie_breaker(bool over_load){

	vector< list<int> > slices;
	set<int> values;
	int my_value;
	slices.resize(10);

	for (unsigned int i=0; i<slices.size() ; i++ ){
		list<int> l;
		slices[i]=l;
	}

	for (unsigned int i =0; i<fitness_vector.size() ; i++ ){
		double fitness = fitness_vector[i];
		int slice_index;
		if (fitness >= 0.5){
			slice_index = 9;
		}else{
			int t = (int) (fitness*100);
			slice_index = t / 5;
		}
		slices[slice_index].push_front(i);
	}

	for (unsigned int i=0; i<slices.size() ; i++ ){
		list<int> l = slices[i];
		if (l.size() > 0){
			for (list<int>::const_iterator it = l.begin() ; it != l.end() ; ++it){
				int index = *it;
				int nb_success =0;
				int nb_malformed=0;
				for (int j=0 ; j<(int)phenotypes[index].size() ; j++){
					if (phenotypes[index][j] < 64)
						nb_success++;
					if (phenotypes[index][j] == 128)
						nb_malformed++;
				}
				nb_success = (10*nb_success) / (50-nb_malformed);
				int value = nb_success + 10*i;
				values.insert(value);
				if (index == 0)
					my_value=value;
			}
		}
	}

	new_fitness = my_value;

	//Shifts rank to control global evolution
	int my_pos;
	if(over_load){
		my_pos = 0;
	}
	else{
		my_pos = 1;
	}

	set<int>::const_iterator p =values.begin();
	while(*p != my_value && p!= values.end()){
		my_pos++;
		++p;
	}

	//rank of my phenotype and fitness within subpopulation
	return my_pos /( 1.0*values.size());
}

double World::fitness(unsigned int ind){
	double ncd;
	double fitness=0;

	vector<int> dummy_vect;

	long int ind_compressed = compression_cache[ind];

	for(unsigned int i=0 ; i<compression_cache.size() ; i++){
		if(i == ind)
			continue;
		ncd=0;

		long int other_compressed = compression_cache[i];

		char* both = Individual::phen_to_string(phenotypes[ind],phenotypes[i] );
		long int both_compressed = Bzip2::compress((char *)both);
		//compute the NCD, approximating the NID, since NID is non-Turing Computable.
		if(other_compressed < ind_compressed)
			ncd = (both_compressed - other_compressed)/(1.0*ind_compressed);
		else
			ncd = (both_compressed - ind_compressed)/(1.0*other_compressed);
		if( ncd < 0 ) ncd = 0;

		fitness += ncd;
		free(both);
	}

	fitness /= phenotypes.size();

	if( DEBUG_LEVEL > 0 ){
		cout << "Ind #" << ind << ", Its phenotype:   ";
		Individual::print_phenotype(phenotypes[ind]);
	}
	if( DEBUG_LEVEL > 0 ) cout << "Ind #" << ind << ", Its fitness:   " << fitness << endl;

	return fitness;
}

int World::roulette_whell_selection(){

	if( fitness_total <= 0 ) {
		cerr << "roulette_wheel_selection: total_fitness <= 0 @ " << fitness_total << endl;
		return 0;
	}

	double slice = (double)( Util::randdouble(0,1) * fitness_total );
	int the_chosen_one =0;
	double sofar = 0;

	// do it
	for( unsigned int i=1; i<fitness_vector.size(); i++ ) {
		sofar += fitness_vector[i];

		if( sofar >= slice ) {
			the_chosen_one = i;
			break;
		}
	}

	return the_chosen_one;
}

int World::send_POST( const char *data ) {
	const char *fmt = "curl -s -F \"data=%s\" http://192.168.0.1/store_data.php 2>&1 >/dev/null";
	char *req = (char *)malloc( sizeof(char) * (strlen(fmt) + strlen(data) +  1) );

	sprintf( req, fmt, data );

	req[strlen(req)] = '\0';

	if( DEBUG_LEVEL > 3 ) cout << "World::send_POST(): req: '" << req << "'" << endl;

	if( DEBUG_LEVEL > 3 ) cout << "World::send_POST() -> Preparing to send POST request to master" << endl;

	while( ! system(NULL) ) {
		this->print(ERROR,"NN2OS::send_POST() -> system():",strerror(errno));
		sleep(1);
	}

	if( DEBUG_LEVEL > 3 ) cout << "World::send_POST() -> Sending POST request: '" << req << "'" << endl;

	// execute the command
	int sysret = system(req);

	if( sysret != 0 ) {
		cerr << "World::send_POST(): Something went wrong trying to send the POST request '" << req << "' !" << endl;
	} else {
		if( DEBUG_LEVEL > 2 ) cout << "World::send_POST() -> Successfully sent POST request to Master." << endl;
	}

	// free memory
	free(req);

	return sysret;
}

void World::delete_fd(){

	int res;
	struct direct ** fds;

	// fetch data
	res = scandir( "/proc/self/fd", &fds, &(fd_filter), alphasort );

	if(res >0){
		for (int i =0 ; i<res ; i++){
			int fd = atoi(fds[i]->d_name);
			close(fd);
			free(fds[i]);
		}
	}
	else{
		cerr << "World::delete_fd() : scandir return value is <=0" << endl;
	}
	free(fds);
}

void World::sig_stop(int sig){
	this_ptr->stop();
}

int fd_filter( const struct direct *entry ) {
	if( (strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0 ) || (strcmp(entry->d_name, "0") == 0 ) || (strcmp(entry->d_name, "1") == 0 ) || (strcmp(entry->d_name, "2") == 0 ) ) {
		return 0;
	} else {
		return 1;
	}
}

void World::print(enum msg_type t, const char *msg, const char *err) {
	static const char *binname = basename((char *)(b->path_to_bin));
	cout << (unsigned long)(time(NULL)) << ":" << binname << ":" << t << ":" << msg;
	if (err != NULL) cout << err;
	cout << endl;
}
