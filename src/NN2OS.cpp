#include "NN2OS.hpp"
#include <cmath>
#include <iostream>
using namespace std;

//Yay for C++ linking... Just a re-declaration. Very silly.
NN2OS * NN2OS::active;

//used to avoid signal competition
bool dont_set_value;

/**
 * Used to determine whether to
 * override the signal handler when initializing the constructor, since doing that
 * more than once could produce race conditions.
 */
static bool first_cmd = true;

NN2OS::NN2OS(const char *cwd, double op_max, double op_min, unsigned long op_size, OS2NN *interface, Individual *whoami){
	this->max_op = op_max;
	this->min_op = op_min;
	this->op_size = op_size;
	this->whoami = whoami;

	lg_cwd = strdup( cwd );

	signal(SIG_SUCCESS, sig_buffer_ready);
	signal(SIG_FAIL, sig_buffer_ready);
	signal(SIGPIPE, SIG_IGN);

	int rb = 0;
	do {
		if(rb) sleep(1);
		rb = init_bash();
	} while(rb);

	double threashold = (max_op - min_op)/2 + min_op;

	// load LinuxGrammar
	this->interface = interface;
	this->threashold = threashold;

	dont_set_value = false;

	lg = new LinuxGrammar( op_size, interface->max_nb, threashold, interface );
	strcpy( lg->cwd, lg_cwd );
}

NN2OS::~NN2OS(){
	if(getpid() == parent_process){
		close(w_pipe[1]);
		close(r_pipe[0]);
		int kr = kill(-child_process, 9); //send kill signal to the shell process group
		if (kr < 0) {
			whoami->world->print(ERROR,"N2OS::~NN2OS(): There was an error sending the kill signal to the bash process",strerror(errno));
		} else {
			int status;
			wait(&status);
		}
	}
	delete lg;

}

void NN2OS::sig_buffer_ready(int sig){

	NN2OS* fake_this = active; //...interrupt table will only accept static function address.
	// this is a hacky way to support multiple instantiations.
	// it is NOT THREAD SAFE!!!!
	// DO NOT ALLOW MULTIPLE INSTANCES IN THE SAME HWP TO ACCESS THIS FUNCTION AT ONCE!
	// IMPLIMENT A SEMAPHORE IF NEEDED

	if( DEBUG_LEVEL > 2 ) cout << "In signal handler w/signal #" << sig << endl;

	if(sig == SIG_SUCCESS){
		signal(SIG_SUCCESS, sig_buffer_ready);
	}

	if(sig == SIG_FAIL){
		signal(SIG_FAIL, sig_buffer_ready);
	}

	sem_post(&(fake_this->command_done));

	if(dont_set_value){
		if( DEBUG_LEVEL > 2 ) cout << "sem_timedwait already timed out or result from pwd, exiting signal handler..." << endl;

	} else {

		if( DEBUG_LEVEL > 2 ) cout << "first signal, storing return value" << endl;

		if( DEBUG_LEVEL > 3 ) cout << "SIG_SUCCESS: " << SIG_SUCCESS << endl;
		if( DEBUG_LEVEL > 3 ) cout << "SIG_FAIL: " << SIG_FAIL << endl;
		if( DEBUG_LEVEL > 3 ) cout << "SIG_NO_REPO: " << SIG_NO_REPO << endl;

		fake_this->rval_cmd = sig;

	}

	if( DEBUG_LEVEL > 2 ) cout << "Exiting from signal handler..." << endl;
}

int NN2OS::interpret(double *nnop, OS2NN *interface){

	//assumes the first 3 elements define mode bits
	double threashold = (max_op - min_op)/2 + min_op;

	this->last_cmd_active = 0;

	if( DEBUG_LEVEL > 2 ) cout << "NN2OS::interpret(): Parsing neural network output..." << endl;

	int a = lg->S(in_cmdbuf, nnop);

	//idle?
	if( nnop[0] < threashold ) {
		if( DEBUG_LEVEL > 2 ) cout << "+++ MODE: Idle | nnop[0]=" << nnop[0] << endl;
		rval_cmd = 0;
		if(a == -1)
			this->last_cmd_active = 128;
		else
			this->last_cmd_active = 64;
		return 0;
	}

	if(DEBUG_LEVEL > 2) cout << "a = " << a << endl;

	if( a == -1 ) { // failed to build command string

		if( DEBUG_LEVEL > 2 ) cout << "+++ MODE: Malformed string" << endl;

		rval_cmd = SIG_NO_REPO;
		last_cmd_active = 128;
		return rval_cmd;

	} else if( a > 0 ) { // successfully built command string

		if( DEBUG_LEVEL > 2 ) cout << "+++ MODE: Command execution" << endl;

		// execute in_cmdbuf
		exec_command(0);

		rval_cmd = SIG_SUCCESS;

		if( DEBUG_LEVEL > 2 ) cout << "Executed Command" << endl;

		last_cmd_active = a;

		if(rval_cmd == SIG_NO_REPO ) {
			last_cmd_active += 128;
			return SIG_NO_REPO;
		}

		if( rval_cmd == SIG_FAIL){
			last_cmd_active += 64;
		}
	}

	// write output to buffer
	//TODO: generalize this?
	int op_buf = 0;
	if(nnop[1] >= threashold)
		op_buf += 8;
	if(nnop[2] >= threashold)
		op_buf += 4;
	if(nnop[3] >= threashold)
		op_buf += 2;
	if(nnop[4] >= threashold)
		op_buf += 1;

	if( DEBUG_LEVEL > 2 ) cout << "Located op Buffer: " << op_buf << endl;

	if(nnop[5] <= threashold) //else, don't write at all
		interface->set_buffer(op_buf, out_cmdbuf);

	update_inputs(nnop, interface);

	return rval_cmd;
}

//called if the alarm timer expires
// sets the command return value to the special SIG_NO_REPO
// indicating that this individual should not produce any offspring
// and that the individual may not execute any more commands.
// Kills the child process, closes the pipes, and re-initializes bash.
void NN2OS::malformed_command(){

	close(w_pipe[1]);
	close(r_pipe[0]);
	int kr = kill(-child_process, 9); //send kill signal to the shell process group
	if (kr < 0) {
		whoami->world->print(ERROR,"NN2OS::malformed_command(): There was an error sending the kill signal to the bash process",strerror(errno));
	} else {
		int status;
		wait(&status);
	}

	if( DEBUG_LEVEL > 2 ) cout << "Ind #" << whoami->id << ": received SIGALRM while processing command: '" << in_cmdbuf << "' !" << endl;

	int rb = 0;
	do {
		if(rb) sleep(1);
		rb = init_bash();
	} while(rb);

	rval_cmd = SIG_NO_REPO;
}

int NN2OS::init_bash() {

	if (first_cmd) {
		first_cmd = false;

		if( sem_init( &(this->command_done), 1, 0 ) < 0 ) {
			whoami->world->print(ERROR, "Error initializing semaphore for a bash shell!");
			return -1;
		}

		int cmd_done = -1;
		sem_getvalue( &(this->command_done), &cmd_done );
		if( DEBUG_LEVEL > 2 ) cout << "Inizialized semaphore command_done to: " << cmd_done << endl;

	} else {
		// close the pipes
		close( w_pipe[0] );
		close( r_pipe[1] );
	}

	if( pipe(r_pipe) < 0 || pipe(w_pipe) < 0 ) {
		whoami->world->print(ERROR,"Error creating pipes for a bash shell!");
		return -1;
	}

	// keep trying to fork into a bash shell until successful (may be running low on memory)
	bool first_try = true;
	do {
		if( ! first_try ) {
			whoami->world->print(ERROR,"NN2OS::init_bash()",strerror(errno));

			sleep(1);
		} else {
			first_try = false;
		}
		child_process = vfork();
	} while( child_process < 0 );

	if( child_process == 0 ) { // child

		// close parent streams
		close( w_pipe[1] );
		close( r_pipe[0] );

		// replace stdin / stdout
		dup2( w_pipe[0], 0 );
		dup2( r_pipe[1], 1 );
		dup2( r_pipe[1], 2 );

		// put the bash process in a seperate process group so we can use kill(0,9) to kill it including its child processes
		int sr = setpgid(0,0);
		if (sr < 0) {
			whoami->world->print(ERROR,"NN2OS::init_bash(): There was an error calling setpgid(), continuing anyway",strerror(errno));
		}

		// exec bash
		execl( "/bin/bash", "/bin/bash", (char *)0 );

	} else { // parent

		this->parent_process = getpid();

		if( DEBUG_LEVEL > 2 ) cout << "The process number is: " << this->parent_process << endl;

		// close child streams
		close( w_pipe[0] );
		close( r_pipe[1] );

		// set pipe nonblocking
		int flags = 0;

		flags = fcntl(w_pipe[1], F_GETFL, 0);
		fcntl(w_pipe[1], F_SETFD, flags | O_NONBLOCK);

		flags = fcntl(r_pipe[0], F_GETFL, 0);
		fcntl(r_pipe[0], F_SETFD, flags | O_NONBLOCK);

		// make the bash cd to the actual cwd
		const char *fmt = "cd \"%s\"; echo \"done\"\n";
		int cd_to_cwd_len = strlen(fmt) + strlen(lg_cwd) + 1;
		char *cd_to_cwd = new char[cd_to_cwd_len];

		// build command string
		snprintf( cd_to_cwd, cd_to_cwd_len, fmt, lg_cwd );

		// tell bash to do it
		int wr = write( w_pipe[1], cd_to_cwd, strlen(cd_to_cwd)+1 );
		if (wr < 0) {
			whoami->world->print(ERROR,"There was an error writing the cd command to the bash process! Error: ",strerror(errno));
		}

		// read a maximum of BUFSIZ bytes so we can continue (even though we don't care about the data)
		int read_bytes = -1;
		while( ( read_bytes = read( r_pipe[0], out_cmdbuf, BUFSIZ ) ) == BUFSIZ );
		if (read_bytes < 0) {
			whoami->world->print(ERROR,"There was an error reading from the bash process after cd'ing it! Error: ",strerror(errno));
		} else {
			out_cmdbuf[read_bytes] = '\0';
		}

		if( DEBUG_LEVEL > 2 ) cout << "out_cmdbuf " << this->out_cmdbuf << endl;

		// clear the output buffer (we dont need the data)
		memset(out_cmdbuf, 0x00, BUFSIZ);

	}

	return 0;
}

int NN2OS::exec_command(int loud){ //execute the command in in_cmdbuf, store the command output in out_cmdbuf, store the return value in rval_cmd
	char *full_cmd = new char[BUFSIZ*2];
	memset(full_cmd, '\0', BUFSIZ*2);

	this->parent_process = getpid();

	struct timespec ts;
	int s;

	/**
	 *
	 * Execute the indivdual's command first
	 *
	 */
	snprintf(full_cmd, BUFSIZ*2 - 1, "if %s; then echo \"done\"; kill -%d %d; else echo \"done\"; kill -%d %d; fi;\n", in_cmdbuf, SIG_SUCCESS, parent_process, SIG_FAIL, parent_process);
	full_cmd[BUFSIZ*2-1] = '\0';

	char command_data [BUFSIZ*2] = "";
	sprintf(command_data,"%s#%s",in_cmdbuf,lg->cwd);
	whoami->world->print(COMMAND,command_data);


	//note: This implicitly executes the command in the child process shell. rval_cmd will be set when the child sends
	//SIG_FAIL or SIG_SUCCESS back to the parent.
	dont_set_value = false;

	//////////////Critical Section//////////////
	sem_init( &command_done, 1, 0 );
	this->active = this; //set this interface as the active one. This is NOT THREAD SAFE!

	if (DEBUG_LEVEL > 2 && loud) {
		cout << "In critical section, writing to pipe...";
		cout.flush();
	}

	int wr = write( w_pipe[1], full_cmd, strlen(full_cmd)+1 );
	if (wr < 0) {
		whoami->world->print(ERROR,"There was an error writing the command to the pipe! Error: ",strerror(errno));
	}

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg = "Wrote to pipe, about to wait for interrupt\n";
		write(1, msg, strlen(msg));
	}

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		whoami->world->print(ERROR,"clock_gettime 2",strerror(errno));

	ts.tv_sec += 2;

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg = "calling sem_timedwait...\n";
		write(1, msg, strlen(msg));
	}

	s = sem_timedwait(&command_done, &ts);

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg1 = "sem_timedwait done\n";
		write(1, msg1, strlen(msg1));
	}

	if (s == -1 && errno == EINTR) {
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait interrupted by a signal" << endl;
	} else if (s ==  -1 && errno == ETIMEDOUT) {
		dont_set_value = true;
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait timed out..." << endl;
		malformed_command();
		delete [] full_cmd;
		return rval_cmd;
	} else {
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait didn't return -1!" << endl;
	}

	if( DEBUG_LEVEL > 2 ) cout << "\tExiting critical section" << endl;

	/**
	 *
	 * Read the output
	 *
	 */
	memset(out_cmdbuf, 0x00, BUFSIZ);

	if( DEBUG_LEVEL > 2 ) cout << "Preparing to read output..." << endl;

	int read_bytes = -1;
	if(rval_cmd != SIG_NO_REPO){
		bool stop_reading = false;
		// we can only store BUFSIZ bytes so we only store the last block of the output
		do {
			if( DEBUG_LEVEL > 4 ) cout << "Reading from pipe..." << endl;

			read_bytes = read(r_pipe[0], out_cmdbuf, (BUFSIZ-1));

			if( DEBUG_LEVEL > 4 ) cout << "Read from pipe. read_bytes=" << read_bytes << endl;
			if( DEBUG_LEVEL > 4 ) cout << "The last 5 bytes are: '" << &(out_cmdbuf[read_bytes-1-4]) << "'" << endl;

			// check if our "done" was printed at the end
			if (!strncmp(&(out_cmdbuf[read_bytes-1-4]), "done\n",5)) {
				if( DEBUG_LEVEL > 4 ) cout << "*** It's the end" << endl;
				stop_reading = true;
			}  else {
				if( DEBUG_LEVEL > 4 ) cout << "*** It's NOT the end" << endl;
			}

		} while(read_bytes == (BUFSIZ-1) && !stop_reading);

		if (read_bytes < 0) {
			whoami->world->print(ERROR,"There was an error writing the command to the pipe! Error: ",strerror(errno));
		} else {
			out_cmdbuf[read_bytes] = '\0';
		}

	}

	if( DEBUG_LEVEL > 2 ) cout << "Done reading output" << endl;

	if (DEBUG_LEVEL > 2 ) cout << "Output before trimming (" << strlen(out_cmdbuf) << ")" << endl;

	if( rval_cmd == SIG_NO_REPO ) {
		delete [] full_cmd;
		return rval_cmd;
	}

	for(unsigned int i=0; i<strlen(out_cmdbuf); i++)
		if(out_cmdbuf[i] == '\n')
			out_cmdbuf[i] = ' ';

	if (DEBUG_LEVEL > 2 ) cout << "Output after trimming (" << strlen(out_cmdbuf) << ")" << endl;


	/**
	 *
	 * Get the individuals cwd
	 *
	 */
	char *cwd_buf = new char[BUFSIZ];

	// clear buffers
	memset(cwd_buf, '\0', BUFSIZ);
	memset(full_cmd, '\0', BUFSIZ*2);

	sprintf(full_cmd, "echo \"blah \"; pwd; reset; kill -%d %d\n", SIG_SUCCESS, parent_process );

	if (DEBUG_LEVEL > 2 ) cout << "--- cwd --->  Trying '" << full_cmd << "'..." << endl;
	if (DEBUG_LEVEL > 2 ) cout.flush();

	//note: This implicitly executes the command in the child process shell. rval_cmd will be set when the child sends
	//SIG_FAIL or SIG_SUCCESS back to the parent.

	dont_set_value = true;

	//////////////Critical Section//////////////
	sem_init( &command_done, 1, 0 );

	if (DEBUG_LEVEL > 2 ) if( DEBUG_LEVEL > 2 && loud ) cout << endl <<"\t--- cwd ---> In critical section...";
	if (DEBUG_LEVEL > 2 ) cout.flush();
	this->active = this; //set this interface as the active one. This is NOT THREAD SAFE!

	wr = write( w_pipe[1], full_cmd, strlen(full_cmd)+1 );
	if (wr < 0) {
		whoami->world->print(ERROR,"There was an error writing the command to the pipe during fetching of the CWD! Error: ",strerror(errno));
	}

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg = "--- cwd ---> Wrote to pipe\n";
		write(1, msg, strlen(msg));
	}

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg = "\t\t--- cwd ---> About to wait for interrupt.\n";
		write(1, msg, strlen(msg));
	}

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		whoami->world->print(ERROR,"clock_gettime",strerror(errno));

	ts.tv_sec += 2;


	s = sem_timedwait(&command_done, &ts);

	if (DEBUG_LEVEL > 2 && loud) {
		const char *msg1 = "sem_timedwait done\n";
		write(1, msg1, strlen(msg1));
	}

	if (s == -1 && errno == EINTR) {
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait interrupted by a signal" << endl;
	} else if (s ==  -1 && errno == ETIMEDOUT) {
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait timed out..." << endl;
		malformed_command();
		delete [] full_cmd;
		delete [] cwd_buf;
		return rval_cmd;
	} else {
		if (DEBUG_LEVEL > 2 ) cout << "sem_timedwait didn't return -1!" << endl;
	}


	if( DEBUG_LEVEL > 2 && loud ) cout << "--- cwd ---> Done wait..." << endl;
	if( DEBUG_LEVEL > 2 && loud ) cout << "\t--- cwd ---> Exiting critical section" << endl;

	//won't block, since blah gets written.

	if( DEBUG_LEVEL > 2 && loud ) cout << "reading from pipe..." << endl;

	read_bytes = read(r_pipe[0], cwd_buf, BUFSIZ);

	if( read_bytes < 1 ) {
		if(loud)  whoami->world->print(ERROR, "Error reading PWD result from pipe");
	} else {
		if(DEBUG_LEVEL > 2 && loud) cout << "Read PWD, parsing result..." << endl;
		char *tmp = strtok(cwd_buf, " "); //cut off the "blah" bit.
		tmp = strtok(NULL, "\n"); //now point at the path. Note that the full path is preserved, even if there are spaces in it.
		if(strlen(tmp) == 0){
			if(loud) whoami->world->print(ERROR, "NN2OS::exec_command: FAILED to update cwd! No path returned by pwd!");
		} else {

			// update local cwd
			free(lg_cwd);
			lg_cwd = strdup(tmp);

			// update LG's cwd
			strcpy( lg->cwd, lg_cwd );

		}
	}

	if( DEBUG_LEVEL > 2 && loud ) cout << "NN2OS::exec_command: done, returning..." << endl;

	delete [] cwd_buf;
	delete [] full_cmd;

	return 0;
}

void NN2OS::update_inputs(double* nnop, OS2NN* interface){
	unsigned long which_buf = 0;
	double threashold = (max_op - min_op)/2 + min_op;

	if( DEBUG_LEVEL > 2 ) cout << "Locating new buffer" << endl;
	// buffer
	int counter=0;
	for(int factor = 8 ; factor > 0; factor /= 2, counter++)
		if(nnop[counter] >= threashold)
			which_buf += factor;
	if( DEBUG_LEVEL > 2 ) cout << "Chose buffer: " << which_buf << endl;

	interface->which_buf_next = which_buf;
	if( DEBUG_LEVEL > 3 ) cout << "Made it" <<endl;

	// word
	if(nnop[counter++] >= threashold){ // specific word
		if( DEBUG_LEVEL > 2 ) cout << "Locating token with direct access." << endl;
		unsigned long x = (int)(sqrt(-8*log(nnop[counter++]/2)))%BUFSIZ;
		if( DEBUG_LEVEL > 2 ) cout << "Selected token: " << x << endl;
		interface->which_word_next = x;
	}
	else{ // advance or reset
		if( DEBUG_LEVEL > 2 ) cout << "Locating token sequentially" << endl;
		if(nnop[counter++] >= threashold)
			interface->which_word_next++;
		else
			interface->which_word_next = 0;
	}

	// offset
	if(nnop[counter++] >= threashold){ // specific offset
		if( DEBUG_LEVEL > 2 ) cout << "Locating offset with direct access."<<endl;

		unsigned long x = (int)(sqrt(-8*log(nnop[counter++]/2)))%BUFSIZ;
		if( DEBUG_LEVEL > 2 ) cout << "Selected offset: " << x << endl;
		interface->which_offset_next = x;
	}
	else{ // advance or reset
		if( DEBUG_LEVEL > 2 ) cout << "Locating offset sequentially" << endl;
		if(nnop[counter++] >= threashold)
			interface->which_offset_next++;
		else
			interface->which_offset_next = 0;
	}

}

int NN2OS::get_last_cmd_number(){
	return this->last_cmd_active;
}
