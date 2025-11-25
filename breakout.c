#include <ncurses.h>
#include <stdlib.h>

struct window{
    int rows;
    int cols;
} window;

struct start{
    int row;
    int col;
} start;

struct paddle{
    int x, y;
    int width;
} paddle;


struct ball{
    int x, y;
    int dx,dy;

} ball;

void drawPaddle(WINDOW *win){
    for(int i=0; i<paddle.width; i++)
        mvwaddch(win, paddle.y , paddle.x+i, '=');
}

void drawBall(WINDOW *win){
    mvwaddch(win, ball.y, ball.x, 'O');
}

void moveBall(){
    ball.x+=ball.dx;
    ball.y+=ball.dy;
    if(ball.x<=0 || ball.x>=window.cols-1)
        ball.dx*=-1;
    if(ball.y<=0 || ball.y>=window.rows-1)
        ball.dy*=-1;
    if(ball.y>paddle.y-2 && ball.x>=paddle.x && ball.x<=paddle.x+paddle.width ){
        ball.dy*=-1;
    }
}

void handleInput(WINDOW *win){
    int ch=wgetch(win);
    if(ch == KEY_LEFT && paddle.x>0)
        paddle.x-=2;
    if(ch == KEY_RIGHT && paddle.x+paddle.width<window.cols-1)
        paddle.x+=2;
    if(ch=='q'){
        delwin(win);
        endwin();
        exit(0);
    }
}

int main(int argc, char ** argv)
{
    window.rows=30; window.cols=50;
    start.row=0; start.col=0;
    paddle.x=window.cols/2; paddle.y=window.rows-2; paddle.width=5;
    ball.x=0; ball.y=0; ball.dx=1; ball.dy=1;

    WINDOW *win;
    initscr();
    cbreak();

    win=newwin(window.rows, window.cols, start.row,start.col);
    keypad(win, TRUE);
    nodelay(win,TRUE);
    wrefresh(win);
    noecho();
    curs_set(0);

    while(1){
        werase(win);
        drawPaddle(win);
        drawBall(win);
        moveBall();
        handleInput(win);
        napms(50);
        wrefresh(win);
    }

    delwin(win);
    endwin();
}
