grafor: grafor.c
	gcc -ggdb3 grafor.c -o grafor
#	gcc -ggdb3 grafor.c -lncurses -o script.cgi
map:	grafor
	cat ../../lib/world/wld/*.wld > rooms.lst
	./grafor > index.html
	cp index.html /var/www/mud.kharkov.org/map
