#include <iostream>
#include <queue>
#include <tuple>
#include <ncurses.h>
#include <string>
#include <thread>
#include <getopt.h>
#include <unistd.h>
using namespace std;

void emptyInput() {
	int ch,prev=ERR;
	while((ch=getch())!=ERR) prev=ch;
	if (prev!=ERR) ungetch(prev);
}

int main(int argc, char* argv[]) {
	int fps = 10;

	int opt;
	while((opt = getopt(argc, argv, "f:")) != -1)  {
		switch(opt) {  
			case 'f': {
				int val = atoi(optarg);
				if (val)
					fps=val;
				else {
					cout<<"bruh"<<'\n';
					exit(0);
				}
				break;  
			}
			case ':':  
			case '?':  
				exit(0);
				break;  
		}  
	}  

	initscr();
	cbreak();
	timeout(0);
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
	clear();
	srand(time(NULL));

	int maxY=LINES-1, maxX=COLS-1;
	int dotY = rand() % maxY, dotX = rand() % maxX;
	int midY = maxY/2, midX = maxX/2;
	int y=midY, x=midX;
	int lastY, lastX;
	int points = 0;
	int prevIn = KEY_LEFT;
	queue<pair<int,int>> tail;

	mvaddch(dotY, dotX, '*');

	int in;
	bool quit=0;
	while (!quit) {
		int currChar = mvinch(y,x);
		if (y>maxY || x>maxX || y<0 || x<0 || currChar == 'X') {
			quit=1;
			break;
		}

		if (y==dotY && x==dotX) {
			points++;
			dotY = rand() % maxY;
			dotX = rand() % maxX;
		} else if (!tail.empty()) {
			tie(lastY,lastX)=tail.front();
			tail.pop();
			mvaddch(lastY, lastX, ' ');
		}

		mvaddch(dotY, dotX, '*');
		mvaddch(y,x,'X');
		mvprintw(maxY, 0, "points: %d", points);
		refresh();

		tail.push({y,x});

		in=getch();
		emptyInput();
		this_thread::sleep_for(chrono::microseconds(1000000/fps));

		if (in == ERR)
			in=prevIn;
		switch (in) {
			case KEY_LEFT:
				if (tail.size()>1 && prevIn == KEY_RIGHT) {
					in=prevIn;
					x++;
				} else 
					x--;
				break;
			case KEY_RIGHT:
				if (tail.size()>1 && prevIn == KEY_LEFT) {
					in=prevIn;
					x--;
				} else
					x++;
				break;
			case KEY_UP:
				if (tail.size()>1 && prevIn == KEY_DOWN) {
					in=prevIn;
					y++;
				} else
					y--;
				break;
			case KEY_DOWN:
				if (tail.size()>1 && prevIn == KEY_UP) {
					in=prevIn;
					y--;
				} else
					y++;
				break;
			case 'q':
				quit=1;
				break;
			case KEY_RESIZE:
				clear();
				refresh();
				break;
		}
		prevIn=in;
	}
	endwin();
	printf("Your score: %d\n", points);

	return 0;
}
