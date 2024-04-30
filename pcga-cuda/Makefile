all:
	g++ graph_gen.cpp -std=c++11 -o graph_gen.out
	nvcc pgca.cu -std=c++11 -o pgca.out
	javac Visuals.java

run:
	./graph_gen.out graph.in
	./pgca.out graph.in coloring.in
	java Visuals graph.in coloring.in

clean_up:
	rm -f *.out
	rm -f *.class
clean_all:
	rm -f *.in
	rm -f *.out
	rm -f *.class