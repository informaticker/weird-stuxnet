#include "World.hpp"

/*
 * Usage: ./skynet <startdir> [<path_to_datafile>]
 *
 * NOTE: argv[0] is implicitly declared and corresponds to the path to the binary itself
 *
 */
int main( int argc, char **argv ) {
	World *w = new World();

	if( argc == 5 ) { // Port range, startdir and path to datafile is given (offspring)
		w->run_individual( argv[0], (unsigned short)atoi(argv[1]),(unsigned short) atoi(argv[2]) , argv[3] , argv[4] );
	} else if( argc == 4 ) { // Port range and datafile but no startdir present
		w->run_individual( argv[0], (unsigned short)atoi(argv[1]),(unsigned short) atoi(argv[2]) , argv[3] , "" );
	} else { // nothing is given -> random startdir (better than nothing)
		w->run_individual( argv[0], 0, 0, "", "" );
	}

	delete w;

	if (DEBUG_LEVEL > 0) cout << "Exits properly..." << endl;

	return 0;
}
