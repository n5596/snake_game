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
        vector<vector<int> > tail_pos;

	int tail_len;

	Game(int w, int h){
	    width = w;
	    height = h;
	}

	void setup(){
	    /*
		This method sets the game up and initializes the game variables. The snake is initially at the center of the grid and is tail-less.
	    */
	    game_over = false;
	    direction = 'q';
	    score = 0;
            pos_x = width/2;
	    pos_y = width/2;
	    
	    tail_len = 0;
	    reset_fruit();
	}

	void reset_fruit(){
	    /* Resets the fruit to a random location in the grid. */
	    fruit_x = rand()%width;
	    fruit_y = rand()%height;
	}

	void draw_grid(){
	    /* Draws the grid including the snake and the fruit. */
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
		
   		    vector<int> curr_pos{j, i};
		    if (std::find(tail_pos.begin(), tail_pos.end(), curr_pos) != tail_pos.end()){
			cout << "o";
			continue;
		    }

		    if (j == pos_x && i == pos_y)
			cout << "O";
		    else if (j == fruit_x && i == fruit_y)
			cout << "F";
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

	    cout << "Score: " << score << "\n";
	}

	void check_for_input(){
	    /* Checks to see if input has been provided. The ASWD (lowercase) keys are used to denote left, down, up and right, respectively. If the user presses x, the game stops and any other key is ignored. */
	    if (kbhit()){
		char pressed_key = getchar();

		if (pressed_key == 'a' or pressed_key == 'd' or pressed_key == 'w' or pressed_key == 's')
		    direction = pressed_key;
		else if (pressed_key == 'x')
		    game_over = true;
	    }
	}

	void move_tail(){
	    /* Moves the tail according to user input. */
	    tail_pos.push_back(vector<int>{pos_x, pos_y});

	    while (tail_pos.size() > tail_len){
		tail_pos.erase(tail_pos.begin());
	    }
	}

	void move(){
	    /* Moves the snake according to the input provided (ASWD). If the snake touches the walls, the player loses whereas if the snake eats the food, it grows larger and the score increments by 10. */
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
