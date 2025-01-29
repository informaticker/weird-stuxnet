#ifndef WORLD_HPP_
#define WORLD_HPP_

#include <cstdio>  		// standard I/O functions
#include <iostream>  		// standard I/O functions
#include <cstdlib>		// malloc(), free() etc.
#include <cstring>  		// strtok(), strcmp(), strstr() (string functions)
#include <ctime>
#include <list> // list stuff
#include <map>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <queue>
#include <iostream>
#include <csignal>
#include <csetjmp>
#include <libgen.h>


#include <cfloat>
#include <cmath>

#include <algorithm> // find()
#include <set>

#include <pthread.h>
#include <fcntl.h>

// communication
#define BUFLEN 512

using namespace std;

#include "Util.hpp"
#include "Individual.hpp"
#include "Genome_IO.hpp"
#include "GeneticsMatrix.hpp"
#include "Bzip2.hpp"

class Population;
class Individual;

enum msg_type {
	FITNESS_PHENOTYPE=0,
	EVOLUTION,
	LOAD,
	COMMAND,
	ERROR,
	MOVING,
	CREATION
};

/**
 * This class holds all the magic together.
 * It sets up the individual and keeps it running. Also, it handles the printing of information.
 *
 * @author Fabian Schläpfer
 * @date March 24th 2010
 */

class World {

public:

	/**
	 * The individual
	 */
	Individual *b;

	/**
	 * Interface to data file in order to build individual according to the file contents
	 */
	Genome_IO gio;

	/**
	 * GeneticsMatrix provides matrix manipulation methods
	 */
	GeneticsMatrix gm;

	/**
	 * A randomly generated run-ID to identify log-data we send to the master
	 */
	unsigned long runid;

	/**
	 * A null-terminated string containing the start time of the application.
	 * Used to name datafiles uniquely.
	 */
	char *starttime;

	/**
	 * This variable is used to track the number of steps a population has been alive.
	 */
	unsigned long lifecycle;

	/**
	 * The constructor initializes everything and gets the program running.
	 */
	World();

	~World();

	/**
	 * This method loads an individual by using the file pointed to by the gfp argument.
	 *
	 * @param	gfp			A constant string containing the path to the individual data file
	 * @param	struc		A pointer to an array of unsigned integers, to be filled with data by this method
	 * @param 	struc_size	The number of elements written to the struc array
	 * @param 	weights		A pointer to a two-dimensional array of double values, to be filled with data by this method.
	 * @param	w_cols		The number of columns written to the weights matrix
	 * @param	w_rows		The number of rows written to the weights matrix
	 *
	 * @return	true on success, false on failure
	 */
	bool load_individual( const char *gfp, unsigned int **struc, unsigned int *struc_size, double ***weights, unsigned int *w_cols, unsigned int *w_rows );

	/**
	 * Run the individual associated with this program.
	 * This is the mainloop of the application.
	 */
	void run_individual( const char *path_to_bin, unsigned short pr_start, unsigned short pr_end , const char *path_to_datafile, const char *startdir );

	/**
	 * This method simply stops the mainloop by setting done to true.
	 */
	void stop();

	/**
	 * This method does the evolution.
	 * It is called periodically.
	 */
	void evaluate();

	/**
	 * This method stores data we receive from other individuals during the evolution communication process.
	 *
	 *
	 */
	int store_data(char* response);

	/**
	 * Main evolution method. Handles the communication required to acquire phenotypes and genomes from other individuals.
	 */
	int communication(unsigned short port_start,unsigned short port_end,unsigned int popsize ,int max_redloop,int wait_to_retry_bind, int wait_redloop);

	/**
	 * CPU load stuff
	 */
	static void *cpu_load_updater( void *arg );

	// variables
	double cpu;

	void print(enum msg_type t, const char *msg, const char *err=NULL);


private:

	/**
	 * Used to store data received from other individuals.
	 */
	set<string> datafiles;
	vector<fstream *> file_descriptors;

	/**
	 * This boolean is checked to determine whether to continue running the mainloop or to stop.
	 */
	bool done;

	/*
	 * Counters
	 */
	unsigned long num_times_alone;
	unsigned long num_times_alone_treshold;

	/**
	 * Time we wait to serve requests
	 */
	unsigned int adaptive_serve_time;
	unsigned int min_serve_time;
	unsigned int max_serve_time;

	/**
	 * Compression history. Used to compute fitness
	 */
	vector<long int> compression_cache;

	/**
	 * Variables shared with the thread in the communication phase
	 */
	unsigned int current_popsize;
	bool communication_phase;
	char *path_to_datafile;
	int sock;

	/**
	 * To strore dara collected in the communication phase
	 */
	vector<vector<vector<double> > > genotypes;
	vector<vector<int> > phenotypes ;
	vector<double> fitness_vector;
	double fitness_total;
	int new_fitness;

	static void *serve_requests(void * arg);

	void replicate(double ** brain);

	void evolution();

	double rank(set<double> fitness_set , bool over_load);

	double rank_with_tie_breaker(bool over_load);

	double fitness (unsigned int ind);

	int roulette_whell_selection();

	void delete_fd();

	static void sig_stop(int sig);

	int send_POST( const char *data );

	void move();

};

int fd_filter( const struct direct *entry );

#endif /* WORLD_HPP_ */
