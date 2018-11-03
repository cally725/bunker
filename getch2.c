#include <stdio.h>
#include <ncurses.h>

int main(){
  char r;
  initscr();
  nodelay( stdscr, true );  
 
  for(;;){
    r = getch();
    if (r >= 0x20 && r <= 0x7F){
      printf("\n %c,%x", r,(int)r);
      if (toupper(r) == 'Q') {
        endwin();
        puts("\n");
        return 0;
      }
    }
  }
}
