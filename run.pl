#!/usr/bin/perl
#
# Author: Fabian Schläpfer
# Date: Apr 09 2010
#
# This script belongs to the project "Eco" and is used as a start script.
#
# Work in progress...
#

my $num_runs = 0; # number of running runs
my $bin_path = "./skynet";
my $tasks_file="tasks";
my @tasks;

print "Starting up.....\n";

`for i in \`seq 1 500 \`
do
rm /home/bots/individual_\$i/genome 2>/dev/null
done`;

# read tasks from file
print "Reading tasks from file '$tasks_file'...\n";
open( FILE, $tasks_file ) or die "Unable to open tasks file: '$tasks': $!\n";
while( <FILE> ) {

	# ignore comments by erasing them
	s/#.*//;

	# skip blank lines
	next if /^(\s)*$/;
	
	# remove newline
	tr/\n//;

	# add to tasks array
	push( @tasks, $_ );
}

print "... done, now reading tasks.\n";

# run tasks
foreach( @tasks ) {
	my $line = $_;
	my @task = split( / /, $line );

	$num_runs++;

	print "Reading task $num_runs...\n";

	# check if there's enough data
	my $num = scalar @task;
	if( $num < 15 ) {
		die "Invalid line: Too few arguments: '$num'!\n";
	}

	my $idx_counter = 0;
	my $cmd = <<EOF;
		$bin_path 
		        --runname="$task[$idx_counter++]"
		        --popsize=$task[$idx_counter++]
		        --elitesize=$task[$idx_counter++]
		        --mutation=$task[$idx_counter++]
		        --mutation_element=$task[$idx_counter++]
		        --crossover=$task[$idx_counter++]
		        --mutation-factor=$task[$idx_counter++]
		        --fitness_type=$task[$idx_counter++]
		        --net_type=$task[$idx_counter++]
		        --iterations=$task[$idx_counter++]
		        --lifespan=$task[$idx_counter++]
        		--minval=$task[$idx_counter++]
        		--maxval=$task[$idx_counter++]
			--hidden_layers=$task[$idx_counter++]
			--breed_type=$task[$idx_counter++]
EOF

	# remove newlines and tabs
	$cmd =~ s/\n/ /g;
	$cmd =~ s/\t//g;

	# run two runs at the same time
	#$cmd = $cmd . " &";

	print "Starting experiment...\n";

	# start the program
	print "Cmd: '$cmd'\n";
	system( $cmd );

	if( $? == -1 ) {
		print "Failed to execute command: $!\n";
	} elsif( $? & 127 ) {
		printf "Program died with signal %d, %s coredump\n", ($? & 127), ($? & 128) ? 'with' : 'without';
	} else {
		printf "Program exited with value %d (%d)\n", $? >> 8, $?;
	}

	print "... done!\n";
}
