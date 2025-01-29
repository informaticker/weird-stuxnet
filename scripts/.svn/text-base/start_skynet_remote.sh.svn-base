#!/bin/sh
#
# Author: Fabian Schläpfer
# Date:   July 8th 2010
#
# This script starts skynet using the "run.pl" script on all remote machines specified in the for loop
#
# WARNING: pubkey authentication must be set up in order for this script to work
#

# start on all machines
for ip in `seq 101 108`; do

	echo "Processing IP 192.168.0.$ip ..."

	# start it
	ssh -lroot "192.168.0.$ip" "nohup /root/eco/branches/skynet/run.pl 1>stdout_runpl 2>stderr_runpl &"

	echo "... done"

done
