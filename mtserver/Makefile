all:
	make -f Makefile.client
	make -f Makefile.server

coverage:
	make -f Makefile.server val_cov
	
docs:
	make -f Makefile.server html
	
clean:
	make -f Makefile.client clean
	make -f Makefile.server clean

clear_all:
	make -f Makefile.client clear_all
	make -f Makefile.server clear_all

	