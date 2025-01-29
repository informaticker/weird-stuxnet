#ifndef NN2OS_H
#define NN2OS_H

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "OS2NN.hpp"
#include "LinuxGrammar.hpp"
#include "Individual.hpp"

class Individual;

#define SIG_SUCCESS SIGUSR1
#define SIG_FAIL SIGUSR2
#define SIG_NO_REPO SIGALRM

/**
	This class maps a vector of output values from a neural network to
		OS commands and/or writing to temporary buffers.
		A bash process is kept captive as a child of this module, and executes
		the commands in context. 
		The intent of making this module an object is to allow multiple shells
		to co-exist, in case it is later desirable to allow many individuals to
		execute their commands at once, rather than having an individual execute
		all its commands in sequence before allowing the next one to act. Thus,
		the context of previous commands can be easily preserved. 

		To use jsut one shell at a time, make sure to call the reset_context(idnum)
		command for each individual prior to evaluation of the individual $idnum.
	@author: John A. Doucette
 	@date: June 22/2010
 */
class NN2OS{

protected:

	/**
	 * The top level directory which all individuals start in. /home/bots by default.
	 */
	char *context_prefix; 

	/**
	 * The maximum value produced by the associated Individual as output.
	 */
	double max_op;

	/**
	 * The minimum value produced by the associated Individual as output.
	 */
	double min_op;

	/**
	 * Each NN2OS object starts an associated bash shell. This is it's process id.
	 */
	pid_t child_process;

	/**
	 * The process id of this program.
	 */
	pid_t parent_process;

	/**
	 * A pipe which maps to the stdin file handle for the child bash process.
	 * Writing to the pipe allows commands to be executed in bash.
	 */
	int w_pipe[2];

	/**
	 * A pipe which maps to the stdout & stderr file handles for the child bash process.
	 * Reading from the pipe allows the output of bash commands to be returned to the 
	 * associated Individual's buffer for future use.
	 */
	int r_pipe[2];

	/**
	 * A semaphore which is used by this object communicate with the child bash process
	 * to prevent a race condition on reading and writing to the communication pipes.
	 * When set, the parent process has finished writing to w_pipe, and is waiting for
	 * it's commands to be executed. When unset, the shell has finished executing, and
	 * any output has been completely written to r_pipe.
	 */
	sem_t command_done; 

	/**
	 * A static method used to override the system interrupt vector for this process.
	 * Only three signals are handled, the rest result in unplanned termination of the
	 * program. <br/><br/>
	 * SIGUSR1 & SIGUSR2 are used to unset the command_done semaphore, and to communicate
	 * the success or failure of the input command to the parent process.
	 * SIGALRM is passed if the shell takes too long to run a command, and may have broken.
	 * In this case, the bash process is killed and restarted with a loss of context.
	 * This object's pipes are reset, and a flag is set indicating that no data is present
	 * before unsetting the semaphore.<br/><br/>
	 * @param sig	The signal number.
	 */
	static void sig_buffer_ready(int sig);



	/**
	 * A buffer in which commands may be store by other methods or objects, which
	 * is written to the write pipe on a call to exec_command.
	 */
	char in_cmdbuf[BUFSIZ];

	/**
	 * A buffer in which the output of an executed command is stored after a call
	 * to exec_command. The contents of the buffer will be meaningless if SIGALRM
	 * was caught during execution, and the rval_cmd varriable's state should be
	 * checked before using this data.
	 */
	char out_cmdbuf[BUFSIZ];

	/**
	 * Stores the value of the last interrupt received by the system. 
	 */
	int rval_cmd;


	/**
	 * Executes the command string currently stored in in_cmdbuf, and stores the result in out_cmdbuf.
	 * <b>This method is <u>not</u> threadsafe.</b>
	 * @param loud	If true, extra output, including the contents of in_cmdbuf, will be printed during
	 *	execution.
	 * @return The value of the interrupt sent by the child process.
	 */
	int exec_command(int loud);

	/**
	 * Called when a SIGALRM is sent, indicating the child process failed to complete execution
	 * of a command in a timely manner. Triggers a reset of the child process, and the context
	 * of the individual.
	 */
	void malformed_command();

	/**
	 * The value of the bitwise-OR combined summary values for all command executed in the last
	 * command line.
	 */
	int last_cmd_active;

	/**
	 * Uses some of the output values of the associated Individual's output vector to
	 * Determine the contents of the input lines in the next iteration.
	 * @param nnop 	An array of doubles produced by the associated individual.
	 * @param interface	A collection of buffers used by the associated Individual to
	 * 			read input.	
	 */
	void update_inputs(double *nnop, OS2NN *interface);

	/**
	 * A pointer to the associated Individual object.
	 * @see Individual.cpp
	 * Used when the context is reset as the result of a SIGALRM being caught.
	 */
	Individual *whoami;


public:

	OS2NN *interface;
	double threashold;
	char *lg_cwd;

	/**
	 * This pointer is used by the interupt handler when a SIGALRM is caught.
	 * The pointer is set to "this" in exec_command before the process blocks
	 * on the command_done semaphore. If SIGALRM is caught, the handler passes
	 * control back to the object pointed at by active to resest it's shell
	 * and the context of the associated Individual before unsetting the
	 * semaphore. For obvious reasons, access to this varriable is <b>not</b>
	 * threadsafe.
	 */
	static NN2OS* active;

	/**
	 * @param cwd	The path of the current working directory (where the bash process will be launched) -> starting position for the individual
	 * @param oma	The maximum output value produced by the associated Individual.
	 * @param omi	The minimum output value produced by the associated Individual.
	 * @param ops	The size of the output vector produced by the associated Individual.
	 * @params interface 	A pointer to the collection of buffers belonging to the associated
	 *			Individual.
	 * @params whoami	A pointer to the associated Individual.
	 */
	NN2OS(const char *cwd, double oma, double omi, unsigned long ops, OS2NN *interface, Individual *whoami);
	~NN2OS();

	/**
	 * A pointer to a LinuxGrammar object which is used by this object ot generate valid commands.
	 * @see LinuxGrammar.cpp
	 */
	LinuxGrammar *lg;

	/**
	 * Interprets an output vector produced by the associated Individual as
	 * a command string using the stored LinuxGrammar object.
	 * Executs the command, and stores the result in one of interface's
	 * buffers.
	 * @param nnop	An array of doubles produced by the associated Individual
	 * @param interface A pointer to the collection of buffers belonging to the 
	 * 		Associated individual.	
	 * @return The value of the interupt sent from the child process during
	 *	execution, or 0 if no execution occoured (as in buffer writing).
	 * @see LinuxGrammar.cpp
	 */
	int interpret(double * nnop, OS2NN *interface);

	/**
	 * Accessor method for the summary values produced by LinuxGrammar for the
	 * last command string. Used by the associated Individual as phenotypic
	 * aleals.
	 * @return The summary value.
	 * @see LinuxGrammar.cpp
	 * @see Individual.cpp
	 */
	int get_last_cmd_number();

	/**
	 * Initializes the child bash process and constructs the communication pipes.
	 * @return 0 on success, -1 if something went wrong.
	 */
	int init_bash();

	/**
	 * The value of the last interupt sent by the child process to this instance of 
	 * NN2OS.
	 */
	int last_rval;

	/**
	 * The expected size of output produced by the associated individual.
	 */
	unsigned long op_size;

};


#endif
