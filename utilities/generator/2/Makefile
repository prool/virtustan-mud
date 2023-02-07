WORLD	=	/home/prool/MUD/lib/world

generator: generator.c config.h
	gcc generator.c -o generator

clean:
	rm generator

install: generator
	    ./generator
	    cp *.mob $(WORLD)/mob
	    cp *.obj $(WORLD)/obj
	    cp *.zon $(WORLD)/zon
	    cp *.wld $(WORLD)/wld
	    cp *.trg $(WORLD)/trg
