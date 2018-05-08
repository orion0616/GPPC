OP= -O3 -std=c++11

all: main.o Entry.o Timer.o ScenarioLoader.o
	g++ $(OP) main.o Entry.o Timer.o ScenarioLoader.o
main.o: main.cpp
	g++ $(OP) -c main.cpp
Entry.o: Entry.cpp Entry.h
	g++ $(OP) -c Entry.cpp
Timer.o: Timer.cpp
	g++ $(OP) -c Timer.cpp
Scenario.o: ScenarioLoader.cpp
	g++ $(OP) -c ScenarioLoader.cpp
clean:
	rm a.out *.o
