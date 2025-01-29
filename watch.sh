#!/bin/sh

echo "Number of skynet processes: "
ps -e | grep skynet | wc -l

echo "Omgoimg find / "
ps -e | grep find | wc -l

echo "Number of listening processes: "
netstat -plu | grep skynet | wc -l 2>/dev/null

echo "Number of Binaries: "
ls | grep skynet | wc -l

cat /proc/meminfo | grep MemFree

echo "DATA FROM OUTPUT FILES:"
echo "Total number of individuals: "
ls /root/output | wc -l

echo "Still alive (no suicide): "
grep -c "I commit suicide" /root/output/* | grep ":0" | wc -l

echo "Number of suicides: "
grep "I commit suicide" /root/output/* | wc -l

echo "Created by replication: "
ls /root/output |grep skynet_rep | wc -l

echo "Created by evolution: "
ls /root/output |grep skynet_off | wc -l



