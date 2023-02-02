/**includes**/
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#define CTRL_KEY(k)((k) & 0x1f)
struct editorConfig{
struct termios original_termios;
};
struct editorConfig E;
void die(const char *s){
  write(STDOUT_FILENO, "\x1b[2J",4);
  write(STDOUT_FILENO,"\x1b[H",3);

  perror(s);
  exit(1);
}

void disableRawMode(){
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH, &E.original_termios)==-1){
    die("tcsetattr");
  }
}
void enableRawMode(){
  //struct termios raw;

  if(tcgetattr(STDIN_FILENO,&E.original_termios)==-1) die("tcgetattr");
  atexit(disableRawMode);
  struct termios raw=E.original_termios;
  raw.c_iflag &=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON);
  raw.c_oflag &=~(OPOST);
  raw.c_cflag &=~(CS8);
  raw.c_lflag &=~(ECHO|ICANON|IEXTEN|ISIG);
  raw.c_cc [VMIN]=0;
  raw.c_cc [VTIME]=1;
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH,&raw)==-1) die("tcsetattr");

}
/**OUTPUT**/
char editorReadKey(){
  int nread;
  char c;
  while ((nread=read(STDIN_FILENO,&c,1))!=1) {
   if(nread== -1 && errno!=EAGAIN)die("read");
  }
  return c;
}
void editorProcessKeyPress(){
  char c=editorReadKey();

  switch (c) {
  case CTRL_KEY('q'):
      write(STDOUT_FILENO,"\x1b[2J", 4);
      write(STDOUT_FILENO,"\x1b[H",3);
      exit(0);
      break;
  }
}

void editorDrawRows(){
  int y;
  for(y=0;y<24;y++){
   write(STDOUT_FILENO,"~\r\n",3);
  }
}
void editorRefreshScreen(){
  write(STDOUT_FILENO,"\x1b[2J",4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  editorDrawRows();
  write(STDOUT_FILENO,"\x1b[H",3);
}

int main(){
  enableRawMode();
  while(1){
   /*char c='\0';
    if(read(STDIN_FILENO,&c,1)==-1 && errno!=EAGAIN ) die("read");
    
    if(iscntrl(c)){
      printf("%d\r\n",c);
    }
    else{
      printf("%d ('%c')\r\n",c,c);
    }
    if(c==CTRL_KEY('q')) break;*/
    editorReadKey();
    editorProcessKeyPress();
  }
	return 0;

}
