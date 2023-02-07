WORLD	=	/home/www/mud/lib/world

generator: generator.c config.h

install: generator
	    ./generator
	    cp *.mob $(WORLD)/mob
	    cp *.obj $(WORLD)/obj
	    cp *.zon $(WORLD)/zon
	    cp *.wld $(WORLD)/wld
	    cp *.trg $(WORLD)/trg
	    cp *.shp $(WORLD)/shp
