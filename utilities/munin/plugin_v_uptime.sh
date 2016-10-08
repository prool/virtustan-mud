#!/bin/sh
#
# MUNIN plugin for VMUD MUD uptime
# by prool
# <proolix@gmail.com>
# www.prool.kharkov.org    mud.kharkov.org
# GPL v.3
#

if [ "$1" = "config" ]; then
    echo 'graph_title ZZ) VMUD uptime'
    echo 'graph_vlabel days'
    echo 'graph_noscale true'
    echo 'graph_category MUD'
    echo 'graph_info VMUD uptime'
    echo 'vup.label VMUD uptime'
    echo 'vup.draw AREA'
    echo 'vup.colour COLOUR0'
    exit 0
fi

date >> /var/log/munin/prool-munin.log
echo "munin mud plugin start" >> /var/log/munin/prool-munin.log
cat /var/www/mud.kharkov.org/mudstat/vmud2.html | grep "MUD server uptime" | awk '{print "vup.value " $4}'
