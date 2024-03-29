CXX=g++
CPPFLAGS=-Wall
SOURCE=mst.cc
TARGET=mst

all: $(TARGET)

$(TARGET): $(SOURCE)

run: $(TARGET)
	./mst graphs/graph_v100_e150.txt > v100_output.txt
	./mst graphs/graph_v200_e300.txt > v200_output.txt
	./mst graphs/graph_v400_e1200.txt > v400_output.txt
	./mst graphs/graph_v800_e3200.txt > v800_output.txt
	./mst graphs/graph_v1600_e6400.txt > v1600_output.txt
clean:
	rm $(TARGET) *_output.txt
