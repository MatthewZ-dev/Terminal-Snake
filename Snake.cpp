#include <iostream>
#include <deque>
#include <thread>

//#include "rawinput.h"

#include <unistd.h>
#include <termios.h>

class Term{
    private:
        struct termios old_tio, new_tio;
	bool safety = true;
    public:
        void init(){
            if(safety){	
                tcgetattr(STDIN_FILENO, &old_tio);
                new_tio=old_tio;
                new_tio.c_lflag &=(~ICANON & ~ECHO);
                tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
                safety = false;
            }
        }

        void kill(){
            tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
        }
};

//x,y , x,y , x,y , x,y
std::deque<int> body; int iBody = 1;
bool food = true;
void mov(int x, int y){
	body.push_front(y);// -> [y]
	body.push_front(x);// -> [x y]
	if(food){
		food = false;
		iBody++;
	}else{
		body.pop_back();
		body.pop_back();
	}
}

//bool wait(){ return true; }

int main(){
	Term term;
	term.init(); //super important
	//kill is even more important

	//board is inverted for convinience porpouses
	// 0 # # #   \	3 # # #
	// 1 # # # ===  2 # # #
	// 2 # # #   /  1 # # #
	// * 0 1 2	0 1 2 3

	char screen[40][40]; //this sucks but i didnt learn a better way :P
	for(int y = 39; y >= 0; y--){ for(int x = 0; x < 40; x++){
			screen[x][y] = '`';
	}	}
	
	unsigned char c = 'd';
	int buff; //buff = pos; pos++; screen[buff] = '#';
	int xPos = 20, yPos = 20;
	int xApple, yApple; 
	bool foodOnScreen = false;
	do{
		//std::thread timer (wait, 800);
		if(c=getchar()){

			switch(c){
				case 'w': mov(xPos,yPos); screen[xPos][yPos] = '#'; yPos++; break;
				case 's': mov(xPos,yPos); screen[xPos][yPos] = '#'; yPos--; break;
				case 'a': mov(xPos,yPos); screen[xPos][yPos] = '#'; xPos--; break;
				case 'd': mov(xPos,yPos); screen[xPos][yPos] = '#'; xPos++; break;
			}

			if(0 > xPos || xPos >= 40 || 0 > yPos || yPos >= 40){term.kill();exit(1);}		
			
			if(!foodOnScreen){xApple = std::rand()%40; yApple = std::rand()%40; foodOnScreen = true;}
			
			screen[xApple][yApple] = '*';

			if(screen[xPos][yPos] == '#'){term.kill(); exit(1);}
			else if(screen[xPos][yPos] == '*'){screen[xPos][yPos] = '@'; foodOnScreen = false; food = true;}
			else screen[xPos][yPos] = 'O';
			
			for(int i = 0; i < iBody; i++){
				screen[body[i]][body[i+1]] = '#';
			}

			system("clear");
			for(int y = 39; y >= 0; y--){
				for(int x = 0; x < 40; x++){
					std::cout << screen[x][y] << " ";
				}
				std::cout << std::endl;
			}
			for(int i = 0; i < iBody; i++){
				std::cout << "x: " << body[i] << " y: " << body[i+1] << std::endl;
			}
			//cleanup
			for(int y = 39; y >= 0; y--){ for(int x = 0; x < 40; x++){
					screen[x][y] = '`';
			}	}
		}
	}while(c != 'q');

	term.kill();
	return 0;
}
