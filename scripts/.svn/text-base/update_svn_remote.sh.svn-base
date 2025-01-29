#!/bin/sh
#
# Author: Fabian Schläpfer
# Date:   July 8th 2010
#
# This script simply puts the newest version of the project
# on all remote machines (default: ~)
#
# WARNING: pubkey authentication must be set up in order for this script to work
#

# path to the local instance of the project
repo="/root/svn/eco"

# put on all machines
for ip in `seq 101 108`; do

	echo "Processing IP 192.168.0.$ip ..."

	# copy it
	scp -r -q -B $repo "192.168.0.$ip":/root/ >/dev/null

	echo "... done"

done
