CXXFLAGS=-std=c++11 -c -Wall -pedantic -Wno-long-long -O0 -ggdb -lncurses
OBJECTS=CDefinitions.o CEnemy.o CGame.o CGameEntity.o CGameGraphics.o CGfx.o CTower.o main.o 
NAME=build

doc: ./Doxyfile
	doxygen ./Doxyfile

towerdefense: $(OBJECTS)
	g++ $(CXXGLAGS) $(OBJECTS) -lncurses -o "$(NAME)/$(NAME)"

main.o: src/main.cpp src/CDefinitions.h src/CGfx.h src/CGameGraphics.h \
 src/CGame.h src/CGameEntity.h src/CEnemy.h src/CTile.h src/CTower.h \
 src/CMainMenu.h
	g++ $(CXXFLAGS) src/main.cpp

CMainMenu.o: src/CMainMenu.h src/CGfx.h
	g++ $(CXXFLAGS) src/CMainMenu.h

CGfx.o: src/CGfx.cpp src/CGfx.h
	g++ $(CXXFLAGS) src/CGfx.cpp

CDefinitions.o: src/CDefinitions.cpp src/CDefinitions.h
	g++ $(CXXFLAGS) src/CDefinitions.cpp

CGameEntity.o: src/CGameEntity.cpp src/CGame.h src/CGameEntity.h \
 src/CEnemy.h src/CTile.h src/CTower.h src/CDefinitions.h
	g++ $(CXXFLAGS) src/CGameEntity.cpp

CGameGraphics.o: src/CGameGraphics.cpp src/CGameGraphics.h src/CGame.h \
 src/CGameEntity.h src/CEnemy.h src/CTile.h src/CTower.h \
 src/CDefinitions.h src/CGfx.h
	g++ $(CXXFLAGS) src/CGameGraphics.cpp


CEnemy.o: src/CEnemy.cpp src/CEnemy.h src/CGameEntity.h src/CTile.h \
 src/CGame.h src/CTower.h src/CDefinitions.h src/CGameGraphics.h
	g++ $(CXXFLAGS) src/CEnemy.cpp

CTower.o: src/CTower.cpp src/CTower.h src/CGameEntity.h src/CEnemy.h \
 src/CTile.h src/CGameGraphics.h src/CGame.h src/CDefinitions.h \
 src/CGfx.h
	g++ $(CXXFLAGS) src/CTower.cpp

CGame.o: src/CGame.cpp src/CGfx.h src/CGame.h src/CGameEntity.h \
 src/CEnemy.h src/CTile.h src/CTower.h src/CDefinitions.h \
 src/CGameGraphics.h
	g++ $(CXXFLAGS) src/CGame.cpp

CTile.o: src/CTile.h
	g++ $(CXXFLAGS) src/CTile.h


.PHONY : run
run :	towerdefense
	-./$(NAME)/$(NAME)

.PHONY : compile
compile : 
	-mkdir $(NAME)
	-cp -R ./data ./$(NAME)/data
	-make towerdefense 
	 
.PHONY : all
all : 
	-make compile
	-make doc
	

.PHONY : clean
clean :
	-rm -R ./doc
	-rm -R *.o
	-rm -R $(NAME)
