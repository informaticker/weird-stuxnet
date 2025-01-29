#include "Genome_IO.hpp"

using namespace std;

Genome_IO::Genome_IO( const char *path ) {
	set_file( path );
	fd = NULL;
	port_range_start = 0;
	port_range_end = 0;
	phen_enabled = false;
}

void Genome_IO::set_file( const char *path ) {
	filepath = path;
	fd = NULL;
	if( DEBUG_LEVEL > 1 ) cout << "Genome_IO::set_file(): Using filepath: '" << filepath << "'." << endl;
}

void Genome_IO::set_filedescriptor( fstream *fd ) {
	this->fd = fd;
	filepath = NULL;
	if( DEBUG_LEVEL > 1 ) cout << "Genome_IO::set_filedescriptor(): set filedescriptor to use." << endl;
}

bool Genome_IO::read_data( void ) {

	bool we_opened_the_filedescriptor = false;

	// check if we either have a valid filename or a valid file descriptor
	if( (fd == NULL || (!fd->is_open()) ) && (filepath == NULL || (strlen(filepath) < 0) ) ) {
		cerr << "Genome_IO::read_data(): Neither filepath nor file descriptor valid!" << endl;
		return false;
	}

	if( fd == NULL ) {

		// open file to read data
		we_opened_the_filedescriptor = true;

		fd = new fstream( filepath, ios::in | ios::binary );
		if( ! fd->is_open() ) {
			cerr << "Genome_IO::read_data(): Error opening file '" << filepath << "'!" << endl;
			return false;
		}
	}

	// struc size
	unsigned int struc_size;
	fd->read( reinterpret_cast<char *>(&struc_size), sizeof(struc_size) );
	if( fd->fail() ) {
		cerr << "fd->fail is true!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	// struc content
	struc.clear();
	try {
		struc.resize( struc_size );
	} catch( length_error& le ) {
		cerr << "Length error: " << le.what() << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	} catch( bad_alloc& ba ) {
		cerr << "Bad alloc: " << ba.what() << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	struc.resize( struc_size );
	for( unsigned int i=0; i<struc.size(); i++ ) {
		fd->read( reinterpret_cast<char *>(&(struc[i])), sizeof(struc[i]) );
		if( fd->fail() ) {
			cerr << "fd->fail is true!" << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		}
	}

	// weights size
	// WARNING: ****** Assuming all rows (sub-vectors) have the same length ********
	unsigned int w_cols;
	unsigned int w_rows;
	fd->read( reinterpret_cast<char *>(&w_cols), sizeof(w_cols) );
	if( fd->fail() ) {
		cerr << "fd->fail is true!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}
	fd->read( reinterpret_cast<char *>(&w_rows), sizeof(w_rows) );
	if( fd->fail() ) {
		cerr << "fd->fail is true!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	if( w_cols == 0 || w_rows == 0 ) {
		cerr << "Genome_IO::read_data(): w_cols == 0 || w_rows == 0" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	// weights content
	weights.clear();
	try {
		weights.resize( w_cols );
	} catch( length_error& le ) {
		cerr << "Length error: " << le.what() << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	} catch( bad_alloc& ba ) {
		cerr << "Bad alloc: " << ba.what() << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	for( unsigned int i=0; i<weights.size(); i++ ) {

		try {
			weights[i].resize( w_rows );
		} catch( length_error& le ) {
			cerr << "Length error: " << le.what() << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		} catch( bad_alloc& ba ) {
			cerr << "Bad alloc: " << ba.what() << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		}

		for( unsigned int j=0; j<weights[i].size(); j++ ) {
			fd->read( reinterpret_cast<char *>(&(weights[i][j])), sizeof(double) );
			if( fd->fail() ) {
				cerr << "fd->fail is true!" << endl;

				if (we_opened_the_filedescriptor) {
					fd->close();
					delete fd;
					fd = NULL;
				}

				return false;
			}
		}
	}

	// phenotype
	if( phen_enabled ) {
		// length
		unsigned long phen_size = 0;
		fd->read( reinterpret_cast<char *>(&phen_size), sizeof(unsigned long) );

		if( fd->fail() ) {
			cerr << "fd->fail is true!" << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		}

		// resize
		phen.clear();

		try {
			phen.resize( phen_size );
		} catch( length_error& le ) {
			cerr << "Length error: " << le.what() << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		} catch( bad_alloc& ba ) {
			cerr << "Bad alloc: " << ba.what() << endl;

			if (we_opened_the_filedescriptor) {
				fd->close();
				delete fd;
				fd = NULL;
			}

			return false;
		}

		// fill with data
		for( unsigned long i=0; i<phen_size; i++ ) {
			//cout <<i << endl;
			fd->read( reinterpret_cast<char *>(&(phen[i])), sizeof(phen[i]) );
			if( fd->fail() ) {
				cerr << "fd->fail is true!" << endl;

				if (we_opened_the_filedescriptor) {
					fd->close();
					delete fd;
					fd = NULL;
				}

				return false;
			}
		}
	}

	// port range
	fd->read( reinterpret_cast<char *>(&port_range_start), sizeof(port_range_start) );
	if( fd->fail() ) {
		cerr << "fd->fail is true!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	fd->read( reinterpret_cast<char *>(&port_range_end), sizeof(port_range_end) );
	if( fd->fail() ) {
		cerr << "fd->fail is true!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	// error checking
	if( fd->fail() ) {
		cerr << "Genome_IO::read_data(): There was an error while reading from the file '" << filepath << "'!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	if (we_opened_the_filedescriptor) {
		fd->close();
		delete fd;
		fd = NULL;
	}

	return true;
}

bool Genome_IO::write_data( void ) {

	bool we_opened_the_filedescriptor = false;

	// check if we either have a valid filename or a valid file descriptor
	if( (fd == NULL || (!fd->is_open()) ) && (filepath == NULL || (strlen(filepath) < 0) ) ) {
		cerr << "Genome_IO::write_data(): Neither filepath nor file descriptor valid!" << endl;
		return false;
	}

	// check if the required data is set
	if( struc.size() < 1 || weights.size() < 1 || (phen_enabled && phen.size() < 1) ) {
			cerr << "Genome_IO::write_data(): Data not set!" << endl;
			return false;
	}

	// check if the required data is set
	if( filepath == NULL || strlen(filepath) < 1 || struc.size() < 1 || weights.size() < 1 || (phen_enabled && phen.size() < 1) ) {
		cerr << "Genome_IO::write_data(): Data not set!" << endl;
		return false;
	}

	if( fd == NULL ) {
		if( DEBUG_LEVEL > 3 ) cout << "fd == NULL, opening file" << endl;

		we_opened_the_filedescriptor = true;

		// open file to write data
		fd = new fstream( filepath, ios::out | ios::trunc | ios::binary );
		if( fd == NULL ) {
			cerr << "fd == NULL, failed to instanciate fstream-object" << endl;
			return false;
		} else {
			if( DEBUG_LEVEL > 4 ) cout << "successfully initialized fstream-object" << endl;
		}
		if( ! fd->is_open() ) {
			cerr << "Genome_IO::write_data(): Error opening file '" << filepath << "'!" << endl;
			return false;
		}
	}


	// struc size
	unsigned int struc_size = (unsigned int)struc.size();
	if( DEBUG_LEVEL > 3 ) cout << "writing struc_size = " << struc_size << endl;
	fd->write( reinterpret_cast<char *>(&struc_size), sizeof(struc_size) );

	// struc content
	for( unsigned long i=0; i<struc.size(); i++ ) {
		if( DEBUG_LEVEL > 3 ) cout << "writing struc[i=" << i << "] = " << struc[i] << endl;
		fd->write( reinterpret_cast<char *>(&(struc[i])), sizeof(struc[i]) );
	}

	// weights size
	// ****** Assuming all rows (sub-vectors) have the same length ********
	unsigned int w_cols = weights.size();
	unsigned int w_rows = weights[0].size();
	if( DEBUG_LEVEL > 3 ) cout << "writing cols=" << w_cols << ", rows=" << w_rows << endl;
	fd->write( reinterpret_cast<char *>(&w_cols), sizeof(w_cols) );
	fd->write( reinterpret_cast<char *>(&w_rows), sizeof(w_rows) );

	// weights content
	if( DEBUG_LEVEL > 3 ) cout << "writing weights: start" << endl;
	for( unsigned int i=0; i<weights.size(); i++ ) {
		for( unsigned int j=0; j<weights[i].size(); j++ ) {
			fd->write( reinterpret_cast<char *>(&(weights[i][j])), sizeof(double) );
		}
	}
	if( DEBUG_LEVEL > 3 ) cout << "writing weights: done" << endl;

	// phenotype
	if( phen_enabled ) {

		if( DEBUG_LEVEL > 3 ) cout << "phen enabled" << struc_size << endl;

		// length
		unsigned long phen_size = (unsigned long)phen.size();
		if( DEBUG_LEVEL > 3 ) cout << "phen size " << phen_size << endl;
		fd->write( reinterpret_cast<char *>(&phen_size), sizeof(unsigned long) );

		// data
		for( unsigned int i=0; i<phen.size(); i++ ) {
			fd->write( reinterpret_cast<char *>(&(phen[i])), sizeof(phen[i]) );
		}
	}

	// port range

	if( DEBUG_LEVEL > 3 ) cout << "port_range_start: " << port_range_start << ", port_range_end: " << port_range_end << endl;

	fd->write( reinterpret_cast<char *>(&port_range_start), sizeof(port_range_start) );
	fd->write( reinterpret_cast<char *>(&port_range_end), sizeof(port_range_end) );

	// error checking
	if( fd->bad() ) {
		cerr << "Genome_IO::write_data(): There was an error while writing to the file '" << filepath << "'!" << endl;

		if (we_opened_the_filedescriptor) {
			fd->close();
			delete fd;
			fd = NULL;
		}

		return false;
	}

	if (we_opened_the_filedescriptor) {
		fd->close();
		delete fd;
		fd = NULL;
	}

	if( DEBUG_LEVEL > 3 ) cout << "done (success)" << struc_size << endl;

	return true;
}

void Genome_IO::set_phenotype( bool use ) {
	this->phen_enabled = use;
	if( DEBUG_LEVEL > 1 ) {
		cout << "Genome_IO::set_phenotype(): " << boolalpha << use << endl;
		cout << noboolalpha;
	}
}
