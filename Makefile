LIBS = \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_imgproc

all:	demo

demo:	demo.o grid_seams.o
		g++ demo.o grid_seams.o -o demo $(LIBS)

demo.o:	demo.cpp
		g++ -c demo.cpp

grid_seams.o:	grid_seams.cpp
				g++ -c grid_seams.cpp

clean:
	rm -f demo demo.o grid_seams.o
