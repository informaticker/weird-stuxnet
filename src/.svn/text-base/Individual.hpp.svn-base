#ifndef INDIVIDUAL_HPP_
#define INDIVIDUAL_HPP_

#include <list>
#include <iostream>
#include <cstdio>
#include <cmath>
#include<sys/time.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdlib.h>
#include <unistd.h>

#include "World.hpp"
#include "Brain.hpp"
#include "NN2OS.hpp"
#include "OS2NN.hpp"
#include "nn/nn.h"

class NN2OS;
class World;

using namespace std;

/**
 * This class represents a single individual of a population.
 * It is connected to the World, Population and Settings in order to work properly.
 * It also has a Brain that decides which action to take using a neural network.
 * The two classes NN2OS and OS2NN serve as interfaces to and from the System this program is running on.
 * The phenotype of an individual is measured in this class as well.
 * In this version of the program, there are no sensors implemented.
 *
 * @see Brain
 * @see NN2OS
 * @see OS2NN
 * @see Population
 *
 * @author Fabian Schläpfer
 * @date March 24th 2010
 */

class Individual {

private:



	/**
	 * This individual's instance of OS2NN. It stores temporary buffers the individual can write to and read from.
	 *
	 * @see OS2NN
	 */
	OS2NN *os2nn;

	/**
	 * Get free memory
	 *
	 * @return	-1 if error reading memory information, otherwise free memory
	 */
	long get_memfree();

	/**
	 * Get the depth of the current working directory of the individuals bash process, used as an input to the ANN
	 *
	 * @param	cwd	A null terminated string containing the current working directory.
	 *
	 * @return	The depth of the current working directory or -1 on error.
	 */
	int cwd_depth( const char *cwd );



public:

	/**
	 * A counter that specifies the age of this individual, used as an input to the ANN
	 */
	unsigned long age;

	/**
	 * This individual's instance of NN2OS. It interprets the output of this individual's
	 * neural network and executes commands accordingly.
	 *
	 * @see NN2OS
	 */
	NN2OS *nn2os;

	/**
	 * A pointer to the world this individual's population lives in.
	 */
	World *world;

	/**
	 * The individual's brain. It uses information about its environment and decides which actions to take.
	 *
	 * @see Brain
	 */
	Brain *brain;

	/**
	 * This individual's "identification number".
	 */
	unsigned long id;

	/**
	 * The lifespan of this individual.
	 */
	unsigned long lifespan;

	/**
	 * The phenotype of this individual.
	 * It contains information about its behaviour which is used to determine it's novelty during fitness evaluation.
	 * It consists of a list of commands executed, represented by integer numbers.
	 */
	vector<int> phenotype; // list of commands executed (represented as integer numbers)

	/**
	 * This integer is needed to keep track of the writing location of the phenotype.
	 */
	unsigned int phenotype_index;

	/**
	 * The inputs to the neural network, represented as a vector of double values.
	 */
	vector<double> inputs;

	/**
	 * The fitness of this individual.
	 */
	double fitness;

	/**
	 * A counter to keep track of failed commands.
	 * This value is used for fitness calculation.
	 */
	unsigned int kill_me;

	/**
	 * Path to the binary program of this process.
	 */
	const char *path_to_bin;

	/**
	 * Path to the file where the genome is stored.
	 */
	const char *path_to_datafile;

	/**
	 * Store the current memory usage
	 */
	long free_memory;
	double free_memory_percent;

	/**
	 * The constructor simply stores the pointers to the World and the Population for future use.
	 * The Brain as well as the NN2OS and the OS2NN objects are initialized here.
	 *
	 * @param w				A pointer to the world
	 * @param pop			A pointer to the population
	 * @param struc			An array representing the structure of a neural network.
	 * 						It is passed to the brain's constructor.
	 * @param num_layers	The number of layers (size of struc array) in the neural network.
	 * 						It is passed to the brain's constructor.
	 * @param lifespan		The number of times the individual is run before optimization is attempted (commands per generation).
	 *
	 * @see World
	 * @see Population
	 * @see Brain
	 */
	Individual( World *w, unsigned int *struc, unsigned int num_layers, unsigned long lifespan, const char *path_to_bin , const char *path_to_darafile, const char *startdir );

	/**
	 * The destructor frees memory that is not used anymore.
	 */
	~Individual();

	/**
	 * This method resets the individuals values that might be set from previous use.
	 * It is usually called after a population has been evaluated to reset the fitness etc.
	 */
	void reset( void );

	/**
	 * This method feeds the brain with input and uses its output to determine what to do.
	 * It also measured the behavior and stores it in phenotype.
	 *
	 * @see Brain
	 */
	void run( void );

	/**
	 * This method returns an ASCII string representation of the two phenotypes a and b, following each other.
	 * The resulting string is used to compute the compression distance between two individuals.
	 *
	 * @param a The first phenotype, represented as a vector of integers
	 * @param b The second phenotype, represented as a vector of integers
	 *
	 * @return A null-terminated string of characters representing the two phenotypes following each other.
	 */
	static char * phen_to_string(vector<int> &a, vector<int> &b);

	static void print_phenotype(vector<int> &v);

	/**
	 * Get the non-localhost LAN ip address of the host (e.g. 192.168.X.X, not 127.0.0.1)
	 *
	 * @param	addr	A pointer to an array of four integers. This is where the ip address will be stored.
	 *
	 * @return True on success, False on failure.
	 */
	bool get_ip_address( int *addr[] );

	/**
	 * This method gets the ANN started.
	 *
	 * @param s 	A (Brain::num_inputs)-characters long ASCII string. Every character is converted to double and then
	 * 				fed as inputs to the brain.
	 *
	 * @see Brain
	 * @see #think
	 */
	void initial_thinking(const char *s);

};

#endif /* INDIVIDUAL_HPP_ */
