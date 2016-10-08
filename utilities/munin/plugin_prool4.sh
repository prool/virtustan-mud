#!/bin/sh
#
# MUNIN plugin #3 for MUD additional statistics
# by prool
# <proolix@gmail.com>
# www.prool.kharkov.org    mud.kharkov.org
# GPL v.3
#

if [ "$1" = "config" ]; then
    echo 'graph_title Z) Prool MUD registered players http://mud.kharkov.org'
    echo 'graph_vlabel digits'
    echo 'graph_noscale true'
    echo 'graph_category MUD'
    echo 'graph_info This graph shows various MUD digits'
    echo 'plrs1.label VMUD registered players'
    echo 'plrs2.label Zerkalo registered players'
    echo 'plrs1.colour COLOUR0'
    echo 'plrs2.colour COLOUR7'
    exit 0
fi

date >> /var/log/munin/prool-munin.log
echo "munin mud plugin start" >> /var/log/munin/prool-munin.log
cat /var/www/mud.kharkov.org/mudstat/vmud2.html | grep "Total registered" | awk '{print "plrs1.value " $4}'
cat /var/www/zerkalo.kharkov.org/who.html | grep "Total registered" | awk '{print "plrs2.value " $4}'
