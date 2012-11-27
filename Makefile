#
# Projekt: IMS
# Autor:   xvysko10@stud.fit.vutbr.cz
# Pouziti­:
#   - preklad:      make
#   - ladit: 	    make debug
#   - zabalit:      make pack
#   - vycistit:     make clean
#   - vycistit vse: make clean-all
#

NAME=uohs

CC=g++                               
CFLAGS=-std=c++98 -lsimlib -lm    # parametry prekladace -g

ALLFILES=uohs.cc Makefile neco.pdf     # obsah projektu

FILES=uohs.o
ALL: $(FILES)
	$(CC) $(CFLAGS) -o $(NAME) $(FILES)
	

.PHONY: debug-s debug-c pack clean clean-exe clean-all

	
debug: $(NAME)
	export XEDITOR=gvim;ddd $(NAME)

pack:
	tar cvf xvysko10.tar $(ALLFILES)
	#tar cvzf xvysko10.tar.gz $(ALLFILES) #gzip
	#zip $(NAME).zip $(ALLFILES)

clean:
	rm -f *~ *.bak $(NAME)
  
clean-exe:
	rm -f $(NAME)
	

clean-all: clean-exe clean

