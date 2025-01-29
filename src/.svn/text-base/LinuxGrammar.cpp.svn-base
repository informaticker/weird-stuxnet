#include "LinuxGrammar.hpp"
#include "nn/nn.h"

int update_file_list_file_select( const struct direct *entry );

using namespace std;

//list of all valid command names. 10 per line.
const char * LinuxGrammar::command_names[LAST_COMMAND] = { "basename", "bc", "cal", "cat", "cd", "cp", "cut", "date", "diff", "dig",
		"dirname", "dmesg", "du", "echo", "env", "export", "file", "find", "grep", "groups",
		"head", "tail", "history", "hostname", "id", "ifconfig", "ln -s -T", "locate", "ls", "lsof -b",
		"mkdir -p", "mv -f", "ping", "ps", "pwd", "scp", "rm -f", "sed", "seq", "sort", "tee", "touch",
		"uname", "uniq", "users", "wc","who","hping3 -q"};

//1 <- information
//2 <- IO
//4 <- non-IO enviroment modificaiton
//8 <- copy
//16 <- manipulation
//32 <- web access and related

const unsigned long LinuxGrammar::command_types[LAST_COMMAND] = {1, 1, 1, 2, 4, 8, 16, 1, 16, 1,
		1, 1, 1, 2, 1, 4, 1, 1, 16, 1,
		16, 16, 1, 1, 1, 1|4, 4, 1, 1, 1,
		4, 4, 32, 1, 1, 32|8, 4, 16, 16, 2, 4, 4,
		1, 16, 1, 1|16, 1, 32};


const char *LinuxGrammar::command_flags[LAST_COMMAND] = {
		"",/*basename*/         "",/*bc*/               "jym",/*cal*/                   "AbeEnstTv",/*cat*/
		"",/*cd*/               "fr",/*cp*/             "bfd",/*cut, spec. case*/       "",/*date*/
		"abBiqrswy",/*diff*/    "",/*dig*/              "",/*dirname*/                  "",/*dmesg*/
		"b",/*du*/              "nEe",/*echo*/          "",/*env*/                      "p",/*export*/
		"",/*file*/             "1234",/*find,v.spec.*/ "BACrvo",/*grep, spec.*/        "",/*groups*/
		"n", /*head, spec*/     "1",/*tail, v. spec*/   "",/*history*/ "",/*hostname*/ "gGnru",/*id*/
		"a",/*ifconfig*/        "",/*ln*/               "",/*locate*/                   "aAbBcCdDfGhHiKlmnNopqQrRsStTuUvwxX1", /*ls*/
		"a",/*lsof, req. -b*/   "",/*mkdir, req. -p*/   "",/*mv, req. -f*/              "bCf",/*ping, special, and -C is req.*/
		"aAdegNrTx",/*ps*/      "",/*pwd*/              "rp",/*scp*/                    "dr",/*rm. Yay for rm -rf.*/
		"",/*sed is not fully defined yet...*/          "",/*seq*/                      "nr",/*sort*/
		"a",/*tee*/             "",/*touch*/            "amnprsv",/*uname*/             "", /*uniq*/
		"",/*users*/            "wlc",/*wc*/            "",/*who*/                      "fnVaMLFSRPAUXY"/*hping, spec.*/ };

// Maybe use masks rather than enums to make this look cleaner
const command_io LinuxGrammar::command_ios[LAST_COMMAND] = {
		OUTPUT,/*basename*/          IP_OP,/*bc*/    OUTPUT,/*cal*/       IP_OPT_OP,/*cat*/
		NOTHING,/*cd*/                	NOTHING,/*cp*/     IP_OPT_OP,/*cut*/    OUTPUT,/*date*/
		OUTPUT,/*diff*/    	  OUTPUT,/*dig*/     OUTPUT,/*dirname*/   OUTPUT,/*dmesg*/
		OUTPUT,/*du*/                OUTPUT,/*echo*/       OUTPUT,/*env*/       OUTPUT,/*export*/
		OUTPUT,/*file*/              OUTPUT,/*find*/       IP_OPT_OP,/*grep*/ OUTPUT,/*groups*/
		IP_OPT_OP, /*head*/        IP_OPT_OP,/*tail*/  OUTPUT,/*history*/   OUTPUT,/*hostname*/   OUTPUT,/*id*/
		OUTPUT,/*ifconfig*/          OUTPUT,/*ln*/         OUTPUT,/*locate*/    OUTPUT, /*ls*/
		OUTPUT,/*lsof*/              NOTHING,/*mkdir*/      NOTHING,/*mv*/             OUTPUT,/*ping*/
		OUTPUT,/*ps*/                OUTPUT,/*pwd*/        NOTHING,/*scp*/             NOTHING,/*rm*/
		IP_OPT_OP,/*sed*/          OUTPUT,/*seq*/        IP_OPT_OP,/*sort*/
		IP_OP,/*tee*/          NOTHING,/*touch*/            OUTPUT,/*uname*/     IP_OPT_OP, /*uniq*/
		OUTPUT,/*users*/             IP_OPT_OP,/*wc*/    OUTPUT,/*who*/       IP_OP/*hping*/ };

//0 == fname
//1 == fname if no stdin provided.
//2 == directory (for cd, this may also be . or ..)
//3 == word, fname, or directory.
//4 == fname or dir
//5 == IP or URL
//6 == path to a file
//7 == particular integer
//8 == "a=9"
//9 == "words"
//a == word
//b == scp special format
//c == particular integer, range 1 to 12.
//d == path to a directory
//e == path to either a file or directory
const char *LinuxGrammar::required_clargs[LAST_COMMAND] = {
		"0", "", "", "1", "2","43", //basename, bc, cal, cat, cd, cp.
		"1", "", "44", "5", "e", "", //cut, date, diff, dig, dirname, dmesg
		"0", "", "", "", "0", "d", //du, echo, env, export, file, find
		"91", "", "1", "1", "", "", //ended at hostname
		"", "", "4a", "3", "4", //ended at ls
		"", "a", "43", "5", "", //ended at ps
		"", "b", "4", "", "7", "0", // ended at sort.
		"", "a", "", "1", "", "1", "", //ended at who.
		"5" // hping
};

const char *LinuxGrammar::optional_clargs[LAST_COMMAND] = {
		"", "", "7c", "", "", "", // ended at cp
		"", "", "" , "", "", "", //ended at dmesg
		"", "", "", "8", "", "", //ended at find
		"", "", "", "", "", "", //ended at hostname
		"", "", "", "", "", //ended at ls
		"", "", "", "", "", //ended at ps
		"", "", "", "", "77", "", //ended at sort
		"a", "", "", "", "", "", "", //ended at who
		"" // hping

};

const char *LinuxGrammar::url_suffixes[] = { // UPDATE num_tlds !! // 15
		// gTLDs
		"asia", "biz", "com", "edu", "gov", "info", "mobi", "name", "net", "org", "travel",
		// countries
		"ch", "ca", "de", "fr"
};

LinuxGrammar::LinuxGrammar(unsigned long opsize, int nbuf, double threash, OS2NN * b){

	this->nnop_size = opsize;
	this->num_bufs = nbuf;
	this->threashold = threash;
	this->bufs = b;

	this->mbuf = NULL;
	this->mbuf_orig = NULL;
	this->mbuf_length = 0;
	this->mbuf_length_orig = 0;
	this->mbuf_pos = 0;
	this->mbuf_pos_orig = 0;

	buf_init( BUFSIZ );

	num_files = 0;
	num_dirs = 0;

	// KEEP THIS UP TO DATE
	num_tlds = 15;

	// initialize random seed
	srand48( time(NULL) );

	for( unsigned int i=0; i<MAX_NUM_FILE_LIST; i++ ) file_list[i] = NULL;
	for( unsigned int i=0; i<MAX_NUM_DIR_LIST; i++ ) dir_list[i] = NULL;
}

LinuxGrammar::~LinuxGrammar() {
	if( DEBUG_LEVEL > 2 ) cout << "in LinuxGrammar::~LinuxGrammar()" << endl;

	for( int i=0; i<num_files; i++ ) {
		free( file_list[i] );
		file_list[i] = NULL;
	}
	for( int i=0; i<num_dirs; i++ ) {
		free( dir_list[i] );
		dir_list[i] = NULL;
	}

	free(mbuf_orig);
}

//fills buf with a command line, or the string "WROTE BUFFER", if a buffer was written to instead.
//returns -1 if the command line became too large for the buffer, a value representing the kind of command
//that was executed if execution was successful, or 0 otherwise.
int LinuxGrammar::S( char *dstbuf, double* nnop ){
	where_in_nnop = 1;
	buf_clear();

	this->accumulate = 0;

	if( DEBUG_LEVEL > 2 ) cout << "+++ LinuxGrammar::S() -> Building command string." << endl;

	// update file and directory list in current directory
	update_file_dir_list();

	int x = cmd(nnop, 0,0,0);

	if( DEBUG_LEVEL > 2 ) cout << "--- in LinuxGrammar::S() -> Done building command string (" << x << "): '" << mbuf << "'" << endl;

	dstbuf[0] = '\0';
	strcat(dstbuf, (char *)mbuf);

	if( DEBUG_LEVEL > 3 ) cout << "LG::S() returning" << endl;

	// empty commands for the fail
	if( strlen(mbuf) == 0 ) x = -1;

	if(x == -1) return x;
	else return accumulate; //maybe? Order preserving, but captures no command information at all. Maybe 2 decimal places, and use cname?
	// only 9 calls max. Not many. Could store in an array, and tack onto the end at write-time? Expensive, possible.
	//why not use the raw strings themselvs then? <-- expensive. This encoding also makes NWA meaningless.
	//Capture list of raw output values? Same problem. Meaningless without context. Some will be linearized.

}

int LinuxGrammar::clargs( double *nnop, command c, int ip_used, int op_used, int err_used){
	char * tmp = new char[BUFSIZ];
	memset( tmp, 0x00, BUFSIZ );

	strcat(tmp, required_clargs[c]);
	strcat(tmp, optional_clargs[c]);

	if( DEBUG_LEVEL > 2 ) cout << "+++++++++ in LinuxGrammmar::clargs() -> command: '" << command_names[c] << "' options: '" << tmp << "'" << endl;

	for(unsigned int i = 0; i < strlen(tmp); i++){
		int r = 0;
		if(i < strlen(required_clargs[c]) || next_nnop(nnop) > threashold) {
			switch(tmp[i]){
			case '0':	r = fname(nnop);
			break;

			case '1':	r = ip_used ? 0 : fname(nnop);
			break;

			case '2':	r = directory(nnop);
			break;

			case '3': r = (drand48()>0.3) ? da_word(nnop) : (drand48()>0.5 ? fname(nnop) : directory(nnop));
			break;

			case '4':	r = drand48()>0.5 ? fname(nnop) : directory(nnop);
			break;

			case '5': r = drand48()>0.5 ? ip_address(nnop) : URL(nnop);
			break;

			case '6': r = path(nnop, 'f');
			break;

			case '7': r = number(nnop, 1);
			break;

			case '8': {
				r = 0;
				if(da_word(nnop)<0) { r = -1; break; }
				mbuf[strlen(mbuf)-1]= '='; // DIRECT ACCESS OF mbuf IS DEPRECATED -> write a method to manipulate certain positions... e.g. index -1 = second-last char
				if(buf_append("\"",1)) { r = -1; break; }
				if(gen_words(nnop)<0) { r = -1; break; }
				if(buf_append("\" ",2)) { r = -1; break; }
				break;
			}
			case '9': {
				r = 0;
				if(buf_append("\"",1)) { r = -1; break; }
				if(gen_words(nnop)<0) { r = -1; break; }
				if(buf_append("\" ",2)) { r = -1; break; }
				break;
			}
			case 'a': r = da_word(nnop);
			break;

			case 'b': { //scp special case.
				r = 0;
				if(drand48()>0.5){
					if(drand48() > 0.5) {
						if(ip_address(nnop)<0) { r = -1; break; }
					} else {
						if(URL(nnop)<0) { r = -1; break; }
					}
					mbuf[strlen(mbuf)-1] = ':';
					if(da_word(nnop)<0) { r = -1; break; }
					if(drand48() > 0.5) {
						if(fname(nnop)<0) { r = -1; break; }
					} else {
						if(da_word(nnop)<0) { r = -1; break; }
					}
				} else {
					if(drand48() > 0.5) {
						if(fname(nnop)<0) { r = -1; break; }
					} else {
						if(directory(nnop)<0) { r = -1; break; }
					}

					if(drand48() > 0.5) {
						if(ip_address(nnop)<0) { r = -1; break; }
					} else {
						if(URL(nnop)<0) { r = -1; break; }
					}
					mbuf[strlen(mbuf)-1] = ':';
					if(da_word(nnop)<0) { r = -1; break; }
				}
				break;
			}

			case 'c': {
				r = 0;
				char tb[3] = "";
				sprintf(tb, "%d", (int)(drand48()*12 + 1));
				if(buf_append(tb,strlen(tb))) { r = -1; break; }

				break;
			}

			case 'd': {
				if( path(nnop, 'd') < 0 ) { r = -1; break; }

				break;
			}

			case 'e' : {
				if( path(nnop, 'e') < 0 ) { r = -1; break; }

				break;
			}

			} // switch

		} // if

		if(r == -1){
			delete [] tmp;
			return -1;
		}
	}

	delete [] tmp;

	return 0;
}

double LinuxGrammar::next_nnop(double *nnop) {
	return nnop[where_in_nnop++%nnop_size];
}

//Non-terminal Symbols

// get the contents of a buffer or a call to words
int LinuxGrammar::iptype(double *nnop){

	char * bt = new char[BUFSIZ];
	memset( bt, 0x00, BUFSIZ );

	if(next_nnop(nnop) > threashold) {
		delete [] bt;
		return(gen_words(nnop));
	}

	bufs->fetch_buffer((unsigned long)linearize(next_nnop(nnop)), bt);

	if( strlen(bt) > 0 ) {
		if(buf_append(bt,strlen(bt))){
			delete [] bt;
			return -1;
		}

	}

	delete[] bt;
	return 0;
}

//a non-terminal which selects several terminals of particular types using the op vector.
int LinuxGrammar::gen_words(double *nnop){
	int x = (int)linearize(next_nnop(nnop));
	int safe = 0;
	switch(x%7){
	case 0:	safe = random_word(nnop); break;
	case 1:	safe = da_word(nnop); break;
	case 2: safe = random_number(nnop); break;
	case 3: safe = number(nnop, 1); break;
	case 4: safe = ip_address(nnop); break;
	case 5: safe = URL(nnop); break;
	case 6: safe = getchar(nnop);
	}
	if(safe != 0){
		return -1;
	}
	if(next_nnop(nnop) >= threashold && drand48() > 0.1)
		return gen_words(nnop);
	else
		return 0;
}

int LinuxGrammar::cmd(double *nnop, int ip_used, int op_used, int err_used){

	if( DEBUG_LEVEL > 2 ) cout << "in LinuxGrammar::cmd()" << endl;

	unsigned int x = (unsigned int)linearize(next_nnop(nnop)); // no negative values but overflow (wraps to zero) might occur (if returned value > (2*INT_MAX)+2 -> 0)

	//to prevent infinitely long command strings from forming.
	if(drand48() < 0.01)
		x = 8;

	if(x%9 == 0){ // cmd1 | cmd2
		if(cmd(nnop, ip_used, 1, 0))
			return -1;
		if(buf_append( " | ", 3 )) // might remove that leading space since we always (right?) append a trailing one.
			return -1;
		if(cmd(nnop, 1, op_used, err_used))
			return -1;
	}
	if(x %9 == 1){ // if input not used yet (echo doesn't read from stdin) -> echo "[word,IP,url,buffer...]" | cmd1
		if(ip_used == 0){
			// temporarily use new buffer 'tb' since we're not sure
			// if we're actually gonna close that echo ".

			char *tb = new char[BUFSIZ];
			memset( tb, 0x00, BUFSIZ );

			const char *tmpstr1 = "echo \"";
			const char *tmpstr2 = "\" | ";

			// temporarily set tb to be our main buffer so we can get the data from iptype()
			buf_set( tb, BUFSIZ );
			if( buf_append( tmpstr1, strlen(tmpstr1) ) ) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			if(iptype(nnop)) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			if( buf_append(tmpstr2, strlen(tmpstr2)) ) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			// reset main buffer
			buf_set( NULL, 0 );

			// apply partial command string
			if( buf_append(tb, strlen(tb)) ) {
				delete [] tb;
				return -1;
			}

			// append command after pipe (to receive echo data)
			if(cmd(nnop, 1, op_used, err_used)) {
				delete [] tb;
				return -1;
			}

			accumulate = accumulate | command_types[ECHO];

			delete [] tb;
		}
		else {
			x = 8; //otherwise, treat this as a terminal command.
		}
	}

	if( x % 9 == 2 || x%9 == 4 || x%9 == 5 || x%9 == 6){ // redirect STDOUT or STDERR to file (existing or new WORD)
		if((op_used == 0 && x% 9 < 5) || (err_used == 0 && x%9 > 4)){
			if(cmd(nnop,ip_used, (x%9 > 4?op_used:1), (x%9 < 5?err_used:1)))
				return -1;
			if(x%9==5 || x%9 == 6) {
				if( buf_append(" 2", 2) )
					return -1;
			} else {
				if( buf_append(" ", 1) )
					return -1;
			}
			if( x%9==2 || x%9==5 ) {
				if( buf_append( "> ", 2) )
					return -1;
			} else {
				if( buf_append( ">> ", 3) )
					return -1;
			}
			if(next_nnop(nnop) >= threashold){
				if(da_word(nnop))
					return -1;
			}
			else
				if(fname(nnop))
					return -1;

			accumulate = accumulate | command_types[CAT];
		}
		else
			x = 8;
	}
	if(x %9 == 3){ //... need rhs input??? Tricky. Replace with cat.
		if(ip_used == 0){ // if input not used yet, cat file contents to command
			// temporarily use new buffer 'tb' since we're not sure
			// if we're actually gonna use that cat.

			char *tb = new char[BUFSIZ];
			memset( tb, 0x00, BUFSIZ );

			buf_set( tb, BUFSIZ );

			if( buf_append("cat ", 4) ) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			if( fname(nnop) ) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			if( buf_append(" | ", 3) ) {
				buf_set( NULL, 0 );
				delete [] tb;
				return -1;
			}

			buf_set( NULL, 0 );

			if( buf_append(tb, strlen(tb)) ) {
				delete [] tb;
				return -1;
			}

			if(cmd(nnop, 1, op_used, err_used)) {
				delete [] tb;
				return -1;
			}

			accumulate = accumulate | command_types[CAT];

			delete [] tb;
		}
		else
			x = 8;
	}
	if(x%9==7){ // if error not redirected yet, redirect error to STDOUT
		if(err_used == 0){
			if(cmd(nnop, ip_used, op_used, 1))
				return -1;

			const char *tmpstr2 = " 2>&1 ";
			if( buf_append(tmpstr2, strlen(tmpstr2)) )
				return -1;
		}
		else
			x = 8;
	}
	if(x%9==8){ // generate a command name and append to "buf"

		command which = cname(nnop, ip_used, op_used, err_used);

		if(which == SPECIAL_COMMAND)
			return -1;

		if(flags(nnop, which, ip_used, op_used, err_used))
			return -1;

		if(clargs(nnop, which, ip_used, op_used, err_used)){
			return -1;
		}

		accumulate = accumulate | command_types[which];
	}

	return 0;
}

//Terminal Symbols

int LinuxGrammar::random_word( double *nnop ) {
	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::random_word()" << endl;

	char *p = exec_and_return_line( "cat /usr/share/dict/words 2>/dev/null", -1 );
	if( p== NULL || buf_append( p, strlen(p) ) ) {
		delete [] p;
		return -1;
	}
	delete [] p;

	if( buf_append( " ", 1 ) )
		return -1;

	return 0;
}

int LinuxGrammar::da_word(double *nnop) {
	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::da_word()" << endl;

	int choice = 1000 * linearize( next_nnop( nnop ) );

	char *p = exec_and_return_line( "cat /usr/share/dict/words 2>/dev/null", choice );
	if(p == NULL || buf_append( p, strlen(p) ) ) {
		delete [] p;
		return -1;
	}
	delete [] p;

	if( buf_append( " ", 1 ) )
		return -1;

	return 0;
}

double LinuxGrammar::linearize(double y){

	if( y == 0 ) return 0;

	double x = sqrt(-2*GAUSS_DEVIATION*GAUSS_DEVIATION*log(y/GAUSS_HEIGHT));

	if( isinf(x) ) {
		cerr << "LinuxGrammar::linearize() -> x = " << x << ", y = " << y << endl;
	}

	if(x < 0){
		std::cerr << "OVERFLOW IN GAUSSIAN LINEARIZATION!!!" << std::endl;
		x = -x;
	}

	return x;
}


//random number from 0 to 1
int LinuxGrammar::random_number(double *nnop){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	sprintf(tb, "%lf ", drand48());

	if( buf_append(tb,strlen(tb)) ){
		delete [] tb;
		return -1;
	}

	delete [] tb;

	return 0;
}

//random number from bottom to top.
int LinuxGrammar::random_number_range(  double *nnop, double top, double bottom, int ints_only){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	double x = drand48()*(top - bottom) + bottom;

	if(ints_only)
		sprintf(tb, "%ld ", (long)x);
	else
		sprintf(tb, "%lf ", x);

	if( buf_append(tb,strlen(tb)) ){
		delete [] tb;
		return -1;
	}

	delete [] tb;

	return 0;
}

// particular number specified by the op vector.
int LinuxGrammar::number(  double *nnop, int ints_only) {
	char *tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	double x = linearize(next_nnop(nnop));

	if(ints_only)
		sprintf(tb, "%ld ", (long)x);
	else
		sprintf(tb, "%lf ", x);

	if( buf_append(tb,strlen(tb)) ){
		delete [] tb;
		return -1;
	}

	delete [] tb;

	return 0;
}

//random IPV4 address. Make IPV6?
int LinuxGrammar::ip_address(  double *nnop){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	sprintf(tb, "%d.%d.%d.%d ", (int)(drand48()*256), (int)(drand48()*256), (int)(drand48()*256), (int)(drand48()*256));

	if( buf_append(tb,strlen(tb)) ){
		delete [] tb;
		return -1;
	}

	delete [] tb;

	return 0;
}

//Random URL. Could be better, with better dict.
int LinuxGrammar::URL(  double *nnop){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	// SUBDOMAIN / HOSTNAME
	if( next_nnop(nnop) >= threashold ) { // specific word
		// TODO: check if this word is valid in an URL
		if( next_nnop( nnop ) >= threashold ) {
			if( da_word( nnop ) ) {
				delete [] tb;
				return -1;
			}
		} else {
			if( random_word( nnop ) ) {
				delete [] tb;
				return -1;
			}
		}

		mbuf_pos--;

	} else {
		if( buf_append("www",3) ){
			delete [] tb;
			return -1;
		}
	}

	if( buf_append(".",1) ){
		delete [] tb;
		mbuf_pos++;
		return -1;
	}

	// DOMAIN
	// TODO: check if this word is valid in an URL
	if( next_nnop( nnop ) >= threashold ) {
		if( da_word( nnop ) ) {
			delete [] tb;
			return -1;
		}
	} else {
		if( random_word( nnop ) ) {
			delete [] tb;
			return -1;
		}
	}

	mbuf_pos--;
	if( buf_append(".",1) ){
		delete [] tb;
		mbuf_pos++;
		return -1;
	}

	// TLD
	if( next_nnop(nnop) >= threashold ) { // specific word
		const char *url = url_suffixes[(int)linearize(next_nnop(nnop))%num_tlds];

		if( buf_append(url,strlen(url)) ){
			delete [] tb;
			return -1;
		}

	} else {
		const char *url = url_suffixes[(int)(drand48()*num_tlds)];

		if( buf_append(url,strlen(url)) ){
			delete [] tb;
			return -1;
		}
	}

	// done
	if( buf_append(" ",1) ){
		delete [] tb;
		return -1;
	}

	delete [] tb;

	return 0;
}

// particular char specified by the op vector.
int LinuxGrammar::getchar(double *nnop){
	char tb[3] = "";

	int x = linearize(next_nnop(nnop));
	char c = (char)( (x%(128-32)) + 32 ); //do not allow the bots to put the terminal into non-standard modes.

	sprintf(tb, (drand48()>0.25?"%c":"%c "), c);

	if( buf_append(tb,strlen(tb)) )
		return -1;

	return 0;
}


int LinuxGrammar::buff_num(double *nnop){
	return (int)(linearize(next_nnop(nnop)))%num_bufs;
}

int LinuxGrammar::write_mode(double *nnop){
	return (long)(linearize(next_nnop(nnop)))%2;
}


command LinuxGrammar::cname(double *nnop, int ip_used, int op_used, int err_used){ // if op_used, >asds
	command c;

	// get requested command
	c = (command)((unsigned int)linearize(next_nnop(nnop)) % (int)LAST_COMMAND);

	// check that this command can work with the existing set of streams.
	// -- if need input is not given or input is given where none is accepted
	// ---- note: it's safe to allow head and tail to be used without stdin because in clargs() they will be given files to read from
	if( (ip_used && command_ios[c]%2 == 0 && command_ios[c]/IP_OPT == 0) //if input is given, but cannot be used.
			|| (!ip_used && command_ios[c]%2 == 1) // input is not given, but required.
			|| (op_used && (command_ios[c]/2)%2 == 0) //if output must be provided, but is not,
	){ //note that it's okay to provide output if op_used is false. It will end up in the buffer.
		if( DEBUG_LEVEL > 3 ) cerr << "LinuxGrammar::cname(): command ERR: cmd:'" << command_names[c] << ", buffer: " << mbuf << endl;
		return SPECIAL_COMMAND;
	} else {
		if( DEBUG_LEVEL > 3 ) cerr << "LinuxGrammar::cname(): command OK: cmd:'" << command_names[c] << ", buffer: " << mbuf << endl;
	}

	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	sprintf(tb, "%s ", command_names[c]);

	if( buf_append(tb,strlen(tb)) ){
		delete [] tb;
		return SPECIAL_COMMAND; // -1
	}

	delete [] tb;

	return c;
}

int LinuxGrammar::flags(double *nnop, command c, int ip_used, int op_used, int err_used){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	//handle the aweful special cases first.
	if(c == CUT){
		int start = (int)linearize(next_nnop(nnop));
		int end = (int)linearize(next_nnop(nnop)) + start;
		sprintf(tb, (next_nnop(nnop)>threashold?"-f %d-%d ":"-b %d %d "), start, end);
		if(next_nnop(nnop) >= threashold)
			strcat(tb, "-d' ' ");
	}
	else if(c == FIND){
		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );
		if(next_nnop(nnop) >= threashold){
			sprintf(tbb, "-size %d ", (int)(linearize(next_nnop(nnop))));
			strcat(tb, tbb);
		}
		if(next_nnop(nnop) >= threashold){
			sprintf(tbb, "-ctime %d ", (int)(linearize(next_nnop(nnop))));
			strcat(tb, tbb);
		}
		if(next_nnop(nnop) >= threashold){
			strcat(tb, "-iname *" );

			if( da_word( nnop ) ) {
				delete [] tb;
				return -1;
			}

			strcat(tb, "* ");

			strcat(tb, tbb);
		}
		if(next_nnop(nnop) >= threashold){
			const char * tmpstr = "bcdpflsD";
			sprintf(tbb, "-type %c ", tmpstr[(int)(linearize(next_nnop(nnop)))%strlen(tmpstr)] );
			strcat(tb, tbb);
		}
		delete[] tbb;
	}
	else if(c == GREP){
		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );

		const char * tmpstr = "BACrvo";
		for(unsigned int i =0; i<strlen(tmpstr); i++){
			if(next_nnop(nnop) >= threashold){
				sprintf(tbb, (i<=2?"-%c %d ":"-%c "), tmpstr[i], (i<=2?(int)(linearize(next_nnop(nnop))):0));
				strcat(tb,tbb);
			}
		}
		delete[] tbb;
	}
	else if(c == HEAD || c == TAIL){
		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );

		if(next_nnop(nnop) >= threashold){
			sprintf(tbb, (c==TAIL?"+n %d ":"-n %d "), (int)(linearize(next_nnop(nnop))));
			strcat(tb, tbb);
		}
		delete[] tbb;
	}
	else if(c == PING){
		const char *tmpstr = "Cbf";

		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );

		for(unsigned int i =0; i<strlen(tmpstr); i++){
			if(next_nnop(nnop) >= threashold || i == 0){
				sprintf(tbb, (i==0?"-%c %d ":"-%c "), tmpstr[i], (i==0?(int)(linearize(next_nnop(nnop))):0));
				strcat(tb,tbb);
			}
		}
		delete[] tbb;
	}
	else if(c == HPING){ // optional flags: nVaMLFSRPAUXY
		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );

		// packet count
		sprintf( tbb, "-c %d ", (int)linearize(next_nnop(nnop)) );
		strcat( tb, tbb );

		// mode
		int mode = (int)linearize(next_nnop(nnop));
		switch( mode ) {
		case 1: { // RAW IP
			strcat( tb, "-0 " );
			break;
		}
		case 2: { // ICMP
			strcat( tb, "-1 " );
			break;
		}
		case 3: { // UDP
			strcat( tb, "-2 " );
			break;
		}
		case 4: { // scan
			strcat( tb, "-8 " );
			break;
		}
		case 5: { // listen
			strcat( tb, "-9 " );
			break;
		}
		default: { // default mode
			break;
		}
		}

		// random source address?
		if( next_nnop(nnop) >= threashold ) strcat( tb, "--rand-source " );

		// TTL
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-t %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// source port
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-s %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// destination port
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-p %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// TCP window size
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-w %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// TCP seqnum
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-M %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// TCP ack
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-L %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}


		// data size
		if( next_nnop(nnop) >= threashold ) {
			sprintf( tbb, "-d %d ", (int)linearize(next_nnop(nnop)) );
			strcat( tb, tbb );
		}

		// payload from file
		if( next_nnop(nnop) >= threashold ) {

			strcat( tb, "-E " );

			// create and set temporary buffer
			char ttb[1024];
			buf_set( ttb, 1024 );

			// write filename to temporary buffer
			if( fname(nnop) ) { // appends filename plus a space to tb using nnop information
				delete [] tb;
				return -1;
			}

			// restore original buffer and append filename to our "tb" buffer
			buf_set( NULL, 0 );
			strcat( tb, tbb );

		}

		// dump in hex or printable characters
		if( next_nnop(nnop) >= threashold ) {
			// which one
			if( next_nnop(nnop) >= threashold ) strcat( tb, "-J " );
			else strcat( tb, "-j " );
		}

		// traceroute mode
		if( next_nnop(nnop) >= threashold ) strcat( tb, "-T " );

		delete[] tbb;
	}
	//finally, deal with the normal cases.
	else{
		char * tbb = new char[BUFSIZ];
		memset( tbb, 0x00, BUFSIZ );

		const char *tmpstr = command_flags[c];
		for(unsigned int i =0; i<strlen(tmpstr); i++){
			if(next_nnop(nnop) >= threashold){
				sprintf(tbb, "-%c ", tmpstr[i]);
				strcat(tb,tbb);
			}
		}
		delete[] tbb;
	}

	// Done!!!
	if( strlen(tb) > 0 ) {
		if( buf_append(tb,strlen(tb)) ){
			delete[] tb;
			return -1;
		}

	} else {
		if( DEBUG_LEVEL > 3 ) {
			cout << "+++++++++ LinuxGrammar::flags(): Done, but not generated any flags! BUT THATS OK!" << endl;
		}
	}

	delete[] tb;

	return 0;
}

// decide whether or not to include an entry as a file in the current directory ("ls-filter")
int update_file_list_file_select( const struct direct *entry ) {
	/*if( (strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0) ) {
		return 0;
	} else {
		return 1;
	}*/
	return 1;
}

// get files and directories in current directory and store them in {file,dir}_list
int LinuxGrammar::update_file_dir_list( void ) {
	int count, i;
	struct direct **files;
	unsigned int file_list_idx = 0;
	unsigned int dir_list_idx = 0;

	// fetch data
	count = scandir( cwd, &files, &(update_file_list_file_select), alphasort );

	if (count > 0) {
		for( i=0; i<count; ++i ) {
			if( (i<MAX_NUM_FILE_LIST)&&(i<MAX_NUM_DIR_LIST) ) {
				if( files[i]->d_type == DT_REG ) {
					// remove old string
					if( file_list[file_list_idx] != NULL ) delete [] file_list[file_list_idx];
					// store new data
					file_list[file_list_idx++] = strdup(files[i]->d_name);
				} else if( files[i]->d_type == DT_DIR ) {
					// remove old string
					if( dir_list[dir_list_idx] != NULL ) delete [] dir_list[dir_list_idx];
					// store new data
					dir_list[dir_list_idx++] = strdup(files[i]->d_name);
				}
			}

			free(files[i]);
		}

		num_files = file_list_idx;
		num_dirs = dir_list_idx;
		free(files);

	} else {
		if(DEBUG_LEVEL > 2)
			printf( "The current working directory '%s' is empty.\n", cwd );

		return -1;
	}

	return 0;
}

/*
 *
 * TODO: implement calling the 'update_file_dir_list' method before using fname/dir data
 *
 */

//get a filename, as specified by the op vector.
int LinuxGrammar::fname(double *nnop){
	char * tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	// return dictionary by default if there are no files in the current directory
	if( num_files < 1 ) {
		if( DEBUG_LEVEL > 2 ) cerr << "LinuxGrammar::fname(): Returning -1 since no files are available!" << endl;
		delete[] tb;
		return -1;
	}

	if( next_nnop(nnop) >= threashold ) { // specific directory
		unsigned int choice = (unsigned int)linearize( next_nnop( nnop ) );
		if( DEBUG_LEVEL > 3 ) cout << "LinuxGrammar::fname(): choice != 0, selecting specific element idx: " << (choice-1) % num_files << ": " << file_list[(choice-1)%num_files] << endl;
		sprintf( tb, "\"%s\" ", file_list[(choice-1)%num_files] );
	} else {
		if( DEBUG_LEVEL > 3 ) cout << "LinuxGrammar::fname(): choice == 0" << endl;
		if( DEBUG_LEVEL > 3 ) cout << "LinuxGrammar::fname(): randomly selecting element from file_list array (num_files = " << num_files << ")" << endl;
		sprintf( tb, "\"%s\" ", file_list[(int)(drand48())*num_files] );
	}

	if( buf_append(tb,strlen(tb)) ){
		delete[] tb;
		return -1;
	}

	delete[] tb;

	return 0;
}

int LinuxGrammar::directory(double *nnop){
	char *tb = new char[BUFSIZ];
	memset( tb, 0x00, BUFSIZ );

	if( num_dirs < 1 ) {
		if( buf_append(".. ",3) ){
			delete[] tb;
			return -1 ;
		}

		delete[] tb;

		return 0;
	}

	if( next_nnop( nnop ) >= threashold ) { // specific directory
		int choice = (int)linearize( next_nnop( nnop ) );
		sprintf( tb, "\"%s\" ", dir_list[(choice-1)%num_dirs] );
	} else { // random directory
		sprintf( tb, "\"%s\" ", dir_list[(int)(drand48())*num_dirs] );
	}

	if( buf_append(tb,strlen(tb)) ){
		delete[] tb;
		return -1;
	}

	delete[] tb;

	return 0;
}

// execute a command and return the n-th line (random if n<0)
// WARNING: if cmd is an invalid bash command, this function might hang because bash is expecting more input!
// WARNING: this method may execute the command given twice!
char *LinuxGrammar::exec_and_return_line( const char *cmd, int n ) {
	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::exec_and_return_line()" << endl;

	FILE *fp = NULL;

	char *line = new char[BUFSIZ];
	memset( line, 0x00, BUFSIZ );

	int nmax = 0;

	// sanity check
	if( cmd == NULL ) return NULL;

	// get number of lines
	fp = popen( cmd, "r" );
	if( fp == NULL ) {
		return NULL;
	}
	nmax = 0;
	while( fgets( line, BUFSIZ, fp ) != NULL ) nmax++;
	pclose( fp );

	if( nmax == 0 ) {
		return NULL;
	}

	// random line?
	if( n < 0 ) n = rand() % nmax;
	else n = n % nmax;

	// build command string
	const char *fmt = "%s | head -n %d | tail -n 1";
	int len = strlen(fmt) + strlen(cmd) + sizeof(n);
	char *c = new char[len];
	snprintf( c, len, fmt, cmd, n );

	fp = popen( c, "r" );
	if( fp == NULL ) {
		return NULL;
	}
	fgets( line, BUFSIZ, fp );
	pclose( fp );

	delete [] c;

	// remove newline
	line[strlen(line)-1] = '\0';

	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::exec_and_return_line() - done" << endl;
	return line;
}

int LinuxGrammar::path(double *nnop, char what ) {
	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::path()" << endl;

	int choice = 0;
	if( next_nnop( nnop ) >= threashold ) {
		choice = linearize( next_nnop( nnop ) );
	} else {
		choice = -1;
	}

	if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::path() : choice : " << choice << endl;


	const char *fmt1 = "find \"%s\" -type %c 2>/dev/null";
	const char *fmt2 = "find \"%s\" 2>/dev/null";
	int cmd_len = strlen(fmt1) + strlen(this->cwd);
	char *cmd = new char[cmd_len];
	char *p = NULL;

	if( what == 'f' || what == 'd' ) {
		snprintf( cmd, cmd_len, fmt1, cwd, what );
		if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::path() : cmd : " << cmd << endl;
		p = exec_and_return_line( cmd, choice );
	} else if( what == 'e' ) {
		snprintf( cmd, cmd_len, fmt2, cwd );
		if( DEBUG_LEVEL > 2 ) cout << "In LinuxGrammar::path() : cmd : " << cmd << endl;
		p = exec_and_return_line( cmd, choice );
	} else {
		delete [] cmd;
		delete [] p;
		return -1;
	}

	if(p == NULL){
		delete [] cmd;
		delete [] p;
		return -1;
	}

	if( buf_append( p, strlen(p) ) ) {
		delete [] cmd;
		delete [] p;
		return -1;
	}

	delete [] cmd;
	delete [] p;

	return 0;
}

int LinuxGrammar::buf_init( unsigned long size ) {

	if( DEBUG_LEVEL > 5 ) {
		cout << "-----------------------------------------------------------------------------------------" << endl;
		cout << "-+-+-+-+-+ buf_init()" << endl;
		cout << "-----------------------------------------------------------------------------------------" << endl;
	}

	// check if buffer has already been initialized
	if( mbuf_orig != NULL ) {
		if( DEBUG_LEVEL > 0 ) cerr << "LinuxGrammar::buf_init(): mbuf != NULL!" << endl;
		return -1;
	}

	// initialized buffer
	mbuf_length_orig = size;
	mbuf_orig = (char *)malloc( mbuf_length_orig );
	if( mbuf_orig == NULL ) {
		perror("ERROR: malloc!");
		cerr << "LinuxGrammar::buf_init(): buffer could not be initialized!" << endl;
		return -1;
	}

	// set active
	mbuf = mbuf_orig;
	mbuf_length = mbuf_length_orig;

	return 0;
}

int LinuxGrammar::buf_clear() {

	if( DEBUG_LEVEL > 5 ) {
		cout << "-----------------------------------------------------------------------------------------" << endl;
		cout << "-+-+-+-+-+ buf_clear()" << endl;
		cout << "-----------------------------------------------------------------------------------------" << endl;
	}

	if( mbuf == NULL ) {
		if( DEBUG_LEVEL > 2 ) cerr << "LinuxGrammar::buf_clear(): mbuf == NULL!" << endl;
		return -1;
	}

	if( mbuf_length == 0 ) {
		if( DEBUG_LEVEL > 2 ) cerr << "LinuxGrammar::buf_clear(): mbuf_length == 0!" << endl;
		return -1;
	}

	memset( mbuf, 0x00, mbuf_length );
	mbuf_pos = 0;

	return 0;
}

void LinuxGrammar::buf_set( char *tb, unsigned long size ) {

	if( DEBUG_LEVEL > 5 ) {
		cout << "-----------------------------------------------------------------------------------------" << endl;
		cout << "-+-+-+-+-+ buf_set() tb=" << &tb << ", size=" << size << endl;
		cout << "-----------------------------------------------------------------------------------------" << endl;
	}

	if( tb == NULL ) { // reset main buffer
		mbuf = mbuf_orig;

	} else { // use temporary buffer
		// store current position to restore it later
		mbuf_pos_orig = mbuf_pos;

		// use given buffer
		mbuf = tb;
		mbuf_length = size;
		mbuf_pos = 0;
	}
}

int LinuxGrammar::buf_append( const char *data, unsigned long size ) {

	if( DEBUG_LEVEL > 5 ) {
		cout << "-----------------------------------------------------------------------------------------" << endl;
		cout << "-+-+-+-+-+ buf_append() size=" << size <<  "data='" << data << "'" << endl;
		cout << "-----------------------------------------------------------------------------------------" << endl;
	}

	if( data == NULL || size == 0 ) {
		if(DEBUG_LEVEL > 1) cerr << "LinuxGrammar::buf_append(): Invalid arguments supplied! (data=" << (void *)data << ", size=" << size << ")" << endl;
		return -1;
	}

	if( size > (mbuf_length - mbuf_pos) ) {
		if(DEBUG_LEVEL > 1) cerr << "LinuxGrammar::buf_append(): Insufficent space!" << endl;
		return -1;
	}

	// check for unprintable characters (DEBUGGING!!)
	if( DEBUG_LEVEL > 2) {
		bool info = false;
		for( unsigned int i=0; i<size; i++ ) {
			int a = (int)(data[i]);
			if( a < 32 || a > 127 ) {
				if( ! info ) {
					info = true;
					if( DEBUG_LEVEL > 2 ) cerr << "-> Error checking data: '" << data << "'" << endl;
				}
				if( DEBUG_LEVEL > 2 ) cerr << "buf_append(): data[" << i << "]: '" << data[i] << "' (" << a << ") seems to be unprintable!" << endl;
			}
		}
	}

	// append data to buffer
	memcpy( mbuf + mbuf_pos, data, size );

	// increment internal pointer
	mbuf_pos += size;

	return 0;
}
