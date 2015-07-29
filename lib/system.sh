#/bin/sh
echo > system.txt

date +%d/%m/%Y >> system.txt

w >> system.txt
echo >> system.txt

echo "&Wdf -h&n" >> system.txt
df -h >> system.txt
echo >> system.txt

echo "&Wfree&n" >> system.txt
free >> system.txt
echo >> system.txt

echo "&Wps aux | grep circle&n" >> system.txt
ps aux | grep circle >> system.txt
