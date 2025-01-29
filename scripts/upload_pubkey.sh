#!/bin/sh
#
# Author: Fabian Schläpfer
# Date:   July 8th 2010
#
# This script makes sure that our pubkey is on every host's .ssh/authorized_keys in the for-loop.
#
# WARNING: In order for this script to work, the id_rsa.pub file must be available using HTTP (var/www/...)
#

echo "Starting script..."

for ip in `seq 101 108`; do
	echo "Processing IP 192.168.0.$ip ..."
	sshpass -pgggggg ssh -o StrictHostKeyChecking=no -lroot "192.168.0.$ip" "if [ \`grep \"root@debianjump\" .ssh/authorized_keys 2>&1 >/dev/null\` ]; then wget http://192.168.0.1/id_rsa.pub -O /root/jump.pub; cat /root/jump.pub >> /root/.ssh/authorized_keys; fi"
done

echo "... done!"
