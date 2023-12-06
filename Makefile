all: mst

mst: mst.cc
	g++ -Wall mst.cc -o mst
run: mst
	./mst
clean:
	rm mst
