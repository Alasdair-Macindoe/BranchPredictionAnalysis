CC = g++
EXPERIMENT = experiment.cpp -O2 -o experiment.out -std=c++14

program:
	$(CC) $(EXPERIMENT) -DNDEBUG

debug:
	$(CC) $(EXPERIMENT)

tests:
	$(CC) tests.cpp

clean:
	rm *.out
