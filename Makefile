game:
	g++ -std=c++17 Game.cpp Level.cpp Decision.cpp Player.cpp Enemy.cpp GameObject.cpp InputManager.cpp ResourceManager.cpp main.cpp -g -o ./build/game -I include -L lib -l SDL2-2.0.0 -l SDL2_image-2.0.0 -l SDL2_ttf-2.0.0
clean:
	rm -f ./build/game