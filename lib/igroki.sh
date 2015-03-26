#/bin/sh
cat plrs/players.lst | awk '{print $1}' > system.txt
