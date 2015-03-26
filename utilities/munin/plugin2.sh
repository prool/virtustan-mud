#!/bin/sh
#
# MUNIN plugin for MUD statistics
# by prool
# <proolix@gmail.com>
# www.prool.kharkov.org    mud.kharkov.org
# GPL v.3
#

if [ "$1" = "config" ]; then
    echo 'graph_title Virtustan MUD and Zerkalo MUD http://mud.kharkov.org'
    echo 'graph_vlabel players'
    echo 'graph_noscale true'
    echo 'graph_category MUD'
    echo 'graph_info This graph shows amount of VMUD players';
    echo 'vmud-players.label vmud-players'
    echo 'vmud-players.draw AREA'
    echo 'vmud-imms.label vmud-imms'
    echo 'vmud-imms.draw AREA'
    echo 'zerkalo-players.label zerkalo-players'
    echo 'zerkalo-players.draw AREA'
    echo 'zerkalo-imms.label zerkalo-imms'
    echo 'zerkalo-imms.draw AREA'
    exit 0
fi

cat /home/prool/web/mud.kharkov.org/public_html/mudstat/vmud2.html | grep "Total:" | awk '{print "vmud-players.value " $5}'
cat /home/prool/web/mud.kharkov.org/public_html/mudstat/vmud2.html | grep "Total:" | awk '{print "vmud-imms.value " $3}'
cat /home/prool/web/zerkalo.kharkov.org/public_html/who.html | grep "Total online:" | awk '{print "zerkalo-players.value " $6}'
cat /home/prool/web/zerkalo.kharkov.org/public_html/who.html | grep "Total online:" | awk '{print "zerkalo-imms.value " $4}'
