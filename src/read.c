#include <config.h>
#include <gnome.h>

#include <sys/types.h>

#include "gwanderer.h"
#include "graphics.h"

char buffer[80];

int rscreen(num,maxmoves)
int *maxmoves, num;
{
  int  y,numr;
  FILE *fp;
  char name[100];
  char (*row_ptr)[ROWLEN+1] = screen;
  
  if(!edit_mode)
    sprintf(name,"%s/screen.%d",SCREENPATH,num);
  else {
    if(!edit_screen)
      sprintf(name,"./screen");
    else
      sprintf(name,"%s",edit_screen);
  }
  fp = fopen(name,"r");
  if (fp == NULL) {
    if(edit_mode)
      sprintf(buffer,"Cannot find file %s.",name);
    else
      sprintf(buffer,"File for screen %d unavailable.",num);
    inform_me(buffer,0);
  } else {
    for(y = 0;y<NOOFROWS;y++) {
      fgets(*row_ptr,ROWLEN + 2,fp);
      numr = strlen( *row_ptr ) - 1;
      while(numr < ROWLEN) (*row_ptr)[numr++] = ' ';
      row_ptr++;
    };
    fgets(screen_name,60,fp);
    screen_name[61] = '\0';
    screen_name[strlen(screen_name)-1] = '\0';
    if(fscanf(fp,"%d",maxmoves) != 1)
      *maxmoves=0;
    fclose(fp);
  };
  return (fp == NULL);
}

int wscreen(num,maxmoves)
int maxmoves, num;
{
  int  y,x;
  FILE *fp;
  char name[100];
  char (*row_ptr)[ROWLEN+1] = screen;

  if(!edit_screen)
    sprintf(name,"./screen");
  else
    sprintf(name,"%s",edit_screen);
  fp = fopen(name,"w");
  if(fp == NULL) {
    sprintf(name,"/tmp/screen.%d",getpid());
    fp = fopen(name,"w");
    sprintf(buffer,"Written file is %s",name);
    inform_me(buffer,0);
  }
  if(fp == NULL) {
    inform_me("File for screen cannot be written.",0) ;
  } else {
    for(y = 0;y<NOOFROWS;y++) {
      for(x = 0;x<ROWLEN;x++)
	fputc(row_ptr[y][x],fp);
      fputc('\n',fp);
    };
    if( *screen_name == '\0' )
      fputc('#',fp);
    else
      fputs(screen_name,fp);
    fputc('\n',fp);
    if(maxmoves != 0)
      fprintf(fp,"%d\n",maxmoves);
    fclose(fp);
  };
  return (fp == NULL);
}
