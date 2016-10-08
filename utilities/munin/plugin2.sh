#!/bin/sh
#
# MUNIN plugin for MUD statistics
# by prool
# <proolix@gmail.com>
# www.prool.kharkov.org    mud.kharkov.org
# GPL v.3
#

if [ "$1" = "config" ]; then
    echo 'graph_title Virtustan and Zerkalo MUD http://mud.kharkov.org'
    echo 'graph_vlabel players'
    echo 'graph_noscale true'
    echo 'graph_category MUD'
    echo 'graph_info This graph shows amount of VMUD players'
    echo 'vmud-players.label VMUD players'
    echo 'vmud-players.draw AREASTACK'
    echo 'vmud-imms.label VMUD immortals'
    echo 'vmud-imms.draw AREASTACK'
    echo 'zerkalo-players.label Zerkalo MUD players'
    echo 'zerkalo-players.draw AREASTACK'
    echo 'zerkalo-imms.label Zerkalo MUD immortals'
    echo 'zerkalo-imms.draw AREASTACK'
    echo 'vmud-players.colour COLOUR0'
    echo 'vmud-imms.colour COLOUR17'
    echo 'zerkalo-players.colour COLOUR7'
    echo 'zerkalo-imms.colour COLOUR3'
    exit 0
fi

date >> /var/log/munin/prool-munin.log
echo "munin mud plugin start" >> ~prool/munin.log
cat /var/www/mud.kharkov.org/mudstat/vmud2.html | grep "Total:" | awk '{print "vmud-players.value " $5}'
cat /var/www/mud.kharkov.org/mudstat/vmud2.html | grep "Total:" | awk '{print "vmud-imms.value " $3}'
cat /var/www/zerkalo.kharkov.org/who.html | grep "Total online:" | awk '{print "zerkalo-players.value " $6}'
cat /var/www/zerkalo.kharkov.org/who.html | grep "Total online:" | awk '{print "zerkalo-imms.value " $4}'
