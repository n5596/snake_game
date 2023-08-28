#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <algorithm>
#include <unistd.h>

using namespace std;

int kbhit(void)
{
  /* Got this code from here since conio.h is not available for Linux platforms: https://cboard.cprogramming.com/c-programming/63166-kbhit-linux.html */
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}

class Game{
    public:
	bool game_over;
	int width;
	int height;
	int pos_x, pos_y;
	int fruit_x, fruit_y;
	int score;
        char direction;
	int tail_x[100], tail_y[100];
	int tail_len;

	Game(int w, int h){
	    width = w;
	    height = h;
	}

	void setup(){
	    /*
		This method sets the game up and initializes the game variables. The snake is initially at the center of the grid and fruit position is randomly generated.
	    */
	    game_over = false;
	    direction = 'q';
	    score = 0;
            pos_x = width/2;
	    pos_y = width/2;
	    
	    for (int i = 0; i < 100; i++){
		tail_x[i], tail_y[i] = -1, -1;
	    }

	    tail_len = 0;
	    reset_fruit();
	}

	void reset_fruit(){
	    fruit_x = rand()%width;
	    fruit_y = rand()%height;
	}

	void draw_grid(){
	    system("clear");
	    int i, j, k;
	    bool found;

	    for (i = 0; i < width+2; i++)
		cout << "#";	
	    cout << "\n";

	    for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
                    found = false;
		    if (j == 0)
			cout << "#";
		
		    for (k = 0; k < tail_len; k++){
			if (tail_x[k] == j && tail_y[k] == i){
			    cout << "o";
			    found = true;
			    break;
			}
		    }
 
		    if (found)
			continue;

		    if (j == pos_x && i == pos_y)
			cout << "O";
		    else if (j == fruit_x && i == fruit_y)
			cout << "$";
		    else
		        cout << " ";
		    if (j == width-1)
			cout << "#";
		}
		cout << "\n";	
	    }

	    for (i = 0; i < width+2; i++)
		cout << "#";
	    cout << "\n";
	    //direction = 'q'; // resetting so it doesn't keep travelling in the same direction

	    cout << "Score: " << score << "\n";
	}

	void check_for_input(){
	    if (kbhit()){
		char pressed_key = getchar();

		if (pressed_key == 'a' or pressed_key == 'd' or pressed_key == 'w' or pressed_key == 's')
		    direction = pressed_key;
		else if (pressed_key == 'x')
		    game_over = true;
	    }
	}
	
	void print_tail(){
            cout << "printing tail" << endl;
	    for(int i = 0; i < tail_len; i++){
		cout << tail_x[i] << "   " << tail_y[i] << endl;
	    }
	}
	void move_tail(){
	    int prev_x, prev_y, prev_xx, prev_yy;
	    prev_x = tail_x[0];
	    prev_y = tail_y[0];
	    tail_x[0] = pos_x;
            tail_y[0] = pos_y;

	    for (int i = 1; i < tail_len; i++){
		prev_xx = tail_x[i];
                prev_yy = tail_y[i];
		tail_x[i] = prev_x;
	        tail_y[i] = prev_y;
		prev_x = prev_xx;
		prev_y = prev_yy;
	    }
	}

	void move(){
	    move_tail();
	    if (direction == 'a') // LEFT
		pos_x = pos_x-1; 
	    else if (direction == 'd') // RIGHT
	  	pos_x = pos_x+1;
	    else if (direction =='w')  // UP
		pos_y = pos_y-1;
	    else if (direction == 's') // DOWN
		pos_y = pos_y+1;

	    if (pos_x < 0 || pos_y < 0 || pos_x >= width || pos_y >= height)
		game_over = true;

	    if (pos_x == fruit_x && pos_y == fruit_y){
		score += 10;
	        tail_len += 1;
		reset_fruit();
	    }
	}
};

int main(){
    Game game(20, 20);
    game.setup();
    while (game.game_over == false){

	game.draw_grid();
	game.check_for_input();
	game.move();
	usleep(100000);
    }
    return 0;
}
