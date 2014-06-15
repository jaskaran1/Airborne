all:
	g++ Airborne.cpp -lglut -lGL -lGLU -lalut -lopenal -o viewer
	g++ gui.cpp -lalut -lopenal -lglut -lGL -lGLU -o play
