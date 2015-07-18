all: 
	make  -C./simple || exit 1
	make  -C./controller ||exit 1

clean:
	make clean -C./simple  || exit 1
	make clean -C./controller ||exit 1
.PHONY: clean cleanall
