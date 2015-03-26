#/bin/sh
rm .crash
rm .fastboot
rm tar.log
tar czvf syslog-arhiv-`date "+%d-%m-%Y-%H-%M-%S"`.tgz syslog.2* syslog-tail*
rm syslog.2*
rm syslog-tail*
