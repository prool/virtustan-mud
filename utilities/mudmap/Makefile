grafor: grafor.c
	gcc -ggdb3 grafor.c -o grafor
#	gcc -ggdb3 grafor.c -lncurses -o script.cgi
map:	grafor
	cat ../../lib/world/wld/*.wld > rooms.lst
	./grafor 99 > zone99.html
	./grafor 210 > zone210.html
	./grafor 1 > zone1.html
	cp zone*.html /var/www/mud.kharkov.org/map
clean:
	rm grafor
