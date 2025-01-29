#ifndef GENOME_IO_H
#define GENOME_IO_H

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <new>

/**
 * The genomes and additional data required to build an individual are stored in a binary file that belongs to one specific individual
 * This class provides access to such a file.
 * One separate instance is to be used per individual.
 *
 * @author Fabian Schläpfer, October 4th 2010
 */
class Genome_IO {

	private:

		/**
		 * File descriptor to be used instead of opening a file
		 */
		std::fstream *fd;

		/**
		 * Path to the file where we all the data is stored.
		 */
		const char *filepath;

		/**
		 * Read/Write phenotype as well?
		 */
		bool phen_enabled;


	public:
	
		/*******************************************************************
		 * Member functions
		 ******************************************************************/

		/**
		 * This is the constructor for this class.
		 *
		 * @param	path	A null-terminated string containing the path to a genome-file which we should use.
		 * @see #set_file
		 */
		Genome_IO( const char *path = "" );
		~Genome_IO() {};

		/**
		 * This methods simply sets the filename according to the argument.
		 *
		 * @param	path	A null-terminated string containing the path to a genome-file which we should use.
		 */
		void set_file( const char *path );

		/**
		 * This methods simply sets the filedescriptor according to the argument.
		 *
		 * @param	fd	An open filedescriptor of type fstream to be used for reading/writing
		 */
		void set_filedescriptor( std::fstream *fd );

		/**
		 * This method parses the file associated with this object and returns the weight matrice of double values.
		 *
		 * @return	The neural network weights stored in the file.
		 */
		//double **get_weights( void );

		/**
		 *  This method stores the neural network weights supplied to the file.
		 *
		 *  @param	w		An array of double values representing the weights of the individual's neural network
		 *  @param	cols	Number of columns in matrix.
		 *  @param	rows	Number of rows in matrix.
		 */
		//void set_weights( double **w, unsigned int cols, unsigned int rows );

		/**
		 * This method reads all the data available from the file and updates its member variables accordingly.
		 *
		 * @return	True if success, False if there was an error.
		 */
		bool read_data( void );

		/**
		 * This method writes all the data available from its member variables to the file for permanent storage.
		 *
		 * @return	True if success, False if there was an error.
		 */
		bool write_data( void );

		/**
		 * enable/disable phenotype usage
		 */
		void set_phenotype( bool use );

		/*******************************************************************
		 * Member variables
		 ******************************************************************/

		/**
		 * This is a one-dimensional array of unsigned int values. It describes the structure of the neural network.
		 */
		std::vector<unsigned int> struc;

		/**
		 * This is a two-dimensional array of doubles values. It represents the weights of the neural network.
		 */
		std::vector< std::vector<double> > weights;

		/**
		 * Vector of ints containing a phenotype
		 */
		std::vector<int> phen;

		/**
		 * These two values describe the port range to be used for communication.
		 */
		unsigned short port_range_start, port_range_end;

};

#endif
