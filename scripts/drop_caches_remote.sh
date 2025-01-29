#!/bin/sh

while :
do
	for ip in 101 102 104 106 107
	do
		ssh -lroot "192.168.0.$ip" "echo 1 >/proc/sys/vm/drop_caches"
	done

	for ip in 103 105 108
	do
		ssh -lroot "192.168.0.$ip" "echo 2 >/proc/sys/vm/drop_caches"
	done
	sleep 60
done
