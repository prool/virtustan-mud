#/bin/sh
# Make distro
# Делаем дистрибутив
# usage: ./makedistro.sh
#
cd ..
rm z-distro.tgz
rm z-tmp.tar
rm -rf NEWROOT
mkdir NEWROOT
echo tar cf start
tar cf z-tmp.tar proolmud
echo tar cf finish
mv z-tmp.tar NEWROOT
cd NEWROOT
echo tar xf start
tar xf z-tmp.tar
echo tar xf finish
echo delete files start
rm z-tmp.tar
cd proolmud
rm bin/vmud2
rm bin/*.txt
rm lib/etc/board/*
rm lib/etc/mudmap.txt
rm lib/etc/plrmail
rm lib/system.txt
rm lib/script.txt
rm lib/script.sh
rm -rf lib/misc/1
rm lib/misc/maxusers.lst
echo>lib/misc/apr_name
echo>lib/misc/dis_name
echo>lib/misc/new_name
echo>lib/misc/bugs
echo>lib/misc/globaluid
echo>lib/misc/ideas
echo>lib/misc/typos
# rm -rf lib/misc/privilege*
# mv lib/misc/sample.privilege.lst lib/misc/privilege.lst
rm lib/etc/board.immort
mv lib/etc/smtp.xml.example lib/etc/smtp.xml
#
rm -rf lib/plralias/A-E/*
rm -rf lib/plralias/F-J/*
rm -rf lib/plralias/K-O/*
rm -rf lib/plralias/P-T/*
rm -rf lib/plralias/U-Z/*
rm -rf lib/plralias/ZZZ/*
#
rm -rf lib/plrobjs/A-E/*
rm -rf lib/plrobjs/F-J/*
rm -rf lib/plrobjs/K-O/*
rm -rf lib/plrobjs/P-T/*
rm -rf lib/plrobjs/U-Z/*
rm -rf lib/plrobjs/ZZZ/*
#
rm -rf lib/plrstuff/depot/*
rm -rf lib/plrstuff/house/*
rm lib/plrstuff/*
#
rm -rf lib/plrs/A-E/*
rm -rf lib/plrs/F-J/*
rm -rf lib/plrs/K-O/*
rm -rf lib/plrs/P-T/*
rm -rf lib/plrs/U-Z/*
rm -rf lib/plrs/ZZZ/*
rm lib/plrs/*
#
rm -rf log/*
rm .crash
rm .fastboot
rm autolog.txt
rm pause
rm syslog
rm syslog*
rm src/*.o
rm attic/vmud2
echo delete files finish
echo tar distro start
tar czf vmud2-distro.tgz * .hg
echo tar distro finish
mv vmud2-distro.tgz ../..
cd ../..
mv vmud2-distro.tgz vmud-distro-`date "+%d-%m-%Y"`.tgz
rm -rf NEWROOT
