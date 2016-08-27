#!/bin/sh
#
# MUNIN plugin #2 for MUD additional statistics
# by prool
# <proolix@gmail.com>
# www.prool.kharkov.org    mud.kharkov.org
# GPL v.3
#

if [ "$1" = "config" ]; then
    echo 'graph_title Zerkalo mobs http://mud.kharkov.org'
    echo 'graph_vlabel digits'
    echo 'graph_noscale true'
    echo 'graph_category MUD'
    echo 'graph_info This graph shows various MUD digits';
    echo 'mobs.label Zerkalo mobs'
#    echo 'objs.label VMUD objs'
    exit 0
fi

date >> /var/log/munin/prool-munin.log
echo "munin mud plugin start" >> /var/log/munin/prool-munin.log
cat /var/www/zerkalo.kharkov.org/who.html | grep "Total mobs" | awk '{print "mobs.value " $3}'
# cat /var/www/mud.kharkov.org/mudstat/vmud2.html | grep "Total objects" | awk '{print "objs.value " $3}'
