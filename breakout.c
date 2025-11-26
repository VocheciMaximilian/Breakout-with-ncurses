#include <ncurses.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

struct bricks{
    int x, y;
    int width, height;
    int hp;
    char symbol;
};

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

char pickChar(int hp){
    switch(hp){
        case 3:
            return '#';
        case 2:
            return '$';
        case 1:
            return '@';
        default:
            return ' ';   
    }
}
void drawBrick(WINDOW *win, struct bricks x){
    for(int i=0; i<x.height; i++)
        for(int j=0; j<x.width; j++){
            x.symbol=pickChar(x.hp);
            mvwaddch(win, x.y+i , x.x+j, x.symbol);
        }
}

void drawPaddle(WINDOW *win){
    for(int i=0; i<paddle.width; i++)
        mvwaddch(win, paddle.y , paddle.x+i, '=');
}

void drawBall(WINDOW *win){
    mvwaddch(win, ball.y, ball.x, 'O');
}

void freeMem(struct bricks **bricks, int bricksRows){
    for(int i=0; i<bricksRows; i++)
        free(bricks[i]);
    free(bricks);
}

void moveBall(struct bricks **bricks, int bricksRows, int bricksCollumns, int *numberOfBricks){
    ball.x+=ball.dx;
    ball.y+=ball.dy;
    if(ball.x<=0 || ball.x>=window.cols-1)
        ball.dx*=-1;
    if(ball.y<=0 || ball.y>=window.rows-1)
        ball.dy*=-1;
    if(ball.y==paddle.y-1 && ball.x>=paddle.x && ball.x<=paddle.x+paddle.width ){
        ball.dy*=-1;
    } else if( (ball.x==paddle.x || ball.x==paddle.x+paddle.width) && ball.y>=paddle.y && ball.y<=paddle.y+1){
        ball.dx*=-1;
    }
    for(int i = 0; i < bricksRows; i++)
        for(int j = 0; j < bricksCollumns; j++)
            if(bricks[i][j].hp>0){
                bool wasHit=false;
                if(ball.y==bricks[i][j].y && ball.x>=bricks[i][j].x && ball.x<=bricks[i][j].x+bricks[i][j].width){
                    ball.dy*=-1;
                    wasHit=true;
                } else if(!wasHit && ball.y==bricks[i][j].y+bricks[i][j].height && ball.x>=bricks[i][j].x && ball.x<=bricks[i][j].x+bricks[i][j].width){
                    ball.dy*=-1;
                    wasHit=true;
                } else if(!wasHit && ball.x==bricks[i][j].x && ball.y>=bricks[i][j].y && ball.y<=bricks[i][j].y+bricks[i][j].height){
                    ball.dx*=-1;
                    wasHit=true;
                } else if(!wasHit && ball.x==bricks[i][j].x+bricks[i][j].width && ball.y>=bricks[i][j].y && ball.y<=bricks[i][j].y+bricks[i][j].height){
                    ball.dx*=-1;
                    wasHit=true;
                }
                if(wasHit){
                    if(bricks[i][j].hp>0) bricks[i][j].hp--;
                    else if((*numberOfBricks)>0) (*numberOfBricks)--;
                    else printf("GAME OVER");
                }
            }
}

void handleInput(WINDOW *win){
    int ch=wgetch(win);
    if(ch == KEY_LEFT && paddle.x>0){
        paddle.x-=1;
        return;
    }
    if(ch == KEY_RIGHT && paddle.x+paddle.width<window.cols-1){
        paddle.x+=1;
        return;
    }
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
    ball.x=(paddle.x+paddle.width)/2; ball.y=paddle.y-1; ball.dx=1; ball.dy=1;
    struct bricks baseBrick;
    baseBrick.width=5;
    baseBrick.height=2;
    baseBrick.hp=3;
    int padding=2;
    int bricksCollumns=(int)floor(window.cols/(baseBrick.width+padding));
    int bricksRows=(int)floor((window.rows*0.5)/(baseBrick.height+padding));
    int numberOfBricks=bricksCollumns*bricksRows;

    printf("Number of bricks on a collumn: %d\n", bricksCollumns);
    printf("Number of brciks on a row: %d\n", bricksRows);

    struct bricks **bricks;
    if( (bricks=malloc(bricksRows*sizeof(*bricks)))==NULL ){
        errno=1;
        freeMem(bricks, bricksRows);
        perror("Alocare mem **bricks");
        exit(1);
    }
    for(int i=0; i<bricksRows; i++){
        if( (bricks[i]=malloc(bricksCollumns*sizeof(struct bricks)))==NULL){
                errno=2;
                freeMem(bricks, bricksRows);
                perror("Alocare mem *bricks");
                exit(1);
        }
        for(int j=0; j<bricksCollumns; j++){
            bricks[i][j].width=baseBrick.width;
            bricks[i][j].height=baseBrick.height;
            bricks[i][j].x=j*(padding+baseBrick.width);
            bricks[i][j].y=padding+i*(padding+baseBrick.height);
            bricks[i][j].hp=baseBrick.hp;
        }
    }
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
        for(int i=0; i<bricksRows; i++)
            for(int j=0; j<bricksCollumns; j++){
                drawBrick(win, bricks[i][j]);
            }
        drawPaddle(win);
        drawBall(win);
        moveBall(bricks, bricksRows, bricksCollumns, &numberOfBricks);
        handleInput(win);
        napms(50);
        wrefresh(win);
    }

    delwin(win);
    endwin();
}