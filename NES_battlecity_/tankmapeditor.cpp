#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

char tankmap[26+2][26+2];
void *brickbitmap;
void *grassbitmap;
void *steelbitmap;
void  *seabitmap0;
void  *seabitmap1;
void brick(int ,int ,int ) ;
void paintmap()
{
for(int j=(0+1);j<(26+1);j++)
 {
  for(int k=(0+1);k<(26+1);k++)
  {
    if(tankmap[j][k]==1)
    {
     putimage(16*k,16*j,brickbitmap,0);
    }
    if(tankmap[j][k]==2)
    {
     putimage(16*k,16*j,grassbitmap,0);
    }
    if(tankmap[j][k]==3)
    {
     putimage(16*k,16*j,steelbitmap,0);
    }
    if(tankmap[j][k]==4)
    {
        putimage(16*k,16*j,seabitmap1,0);
    }
  }
 }
}
void brick(int x,int y,int type)
{
    
    if(type==1)
    {
        putimage(x,y,brickbitmap,0); 
    }    
    if(type==2)
    {
        putimage(x,y,grassbitmap,0); 
    }    
    if(type==3)
    {
        putimage(x,y,steelbitmap,0); 
    }    
    if(type==4)
    {
        if(time(NULL)%2==0)
            putimage(x,y,seabitmap0,0);
        else if(time(NULL)%2==1)
            putimage(x,y,seabitmap1,0);
    }
    if(type==0)
    {
        setfillstyle(1,BLACK);bar(x,y,x+16,y+16);
    }
}



void readmap()
{
    FILE *fp;
    fp=fopen("map00.txt","r");
    for(int j=(0+1);j<(26+1);j++)//initialized
    {
        for(int k=(0+1);k<(26+1);k++)
        {
            fscanf(fp," %d",&tankmap[j][k]);  
        }
    }
    //for(int j=0;j<26;j++){for(int k=0;k<26;k++){printf("%c ",tankmap[j][k]+11);}printf("\n");}
    fclose(fp);
}
void savemap()
{
    FILE *fp;
    fp=fopen("map00.txt","w");
    //for(int j=0;j<26;j++){for(int k=0;k<26;k++){printf("%c ",tankmap[j][k]+11);}printf("\n");}
    for(int j=(0+1);j<(26+1);j++)//initialized
    {
        for(int k=(0+1);k<(26+1);k++)
        {
            fprintf(fp,"%d ",tankmap[j][k]);  
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}
void detectspuareposition(int *x,int *y)
{
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        *(x)=mousex();
        *(y)=mousey();
    }
    if(ismouseclick(WM_LBUTTONUP))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        clearmouseclick(WM_LBUTTONUP);
    }
    (*(x))/=16;(*(x))*=16;
    (*(y))/=16;(*(y))*=16;;
    //printf("%d %d\n",*x,*y);
}



int main()
{
    
    initwindow(500+32,416+32);
    rectangle(0+16,0+16,416+16,416+16);
    int x=999+16;
    int y=999+16;
    readmap();
    
    readimagefile("image.gif",416+16,0+16,448+16,112+16);
    int size = imagesize(0+16,0+16,16+16,16+16);
    grassbitmap = malloc(size);
    steelbitmap = malloc(size);
    seabitmap0 = malloc(size);
    seabitmap1 = malloc(size);
    brickbitmap = malloc(size);
    
    getimage(416+16,0+16,431+16,15+16,grassbitmap);
    getimage(432+16,0+16,447+16,15+16,steelbitmap);
    getimage(416+16,16+16,431+16,31+16,seabitmap0);
    getimage(432+16,16+16,447+16,31+16,seabitmap1);
    getimage(416+16,32+16,431+16,47+16,brickbitmap);
    setfillstyle(1,BLACK);bar(416+16,0+16,449+16,49+16);
    paintmap();
    int type=1;    
    readimagefile("button.gif",416+16,0+16,432+16,16+16);outtextxy(432+16,0+16,"exit");  
    readimagefile("button.gif",416+16,16+16,432+16,32+16);outtextxy(432+16,16+16,"clear");
    readimagefile("button.gif",416+16,32+16,432+16,48+16);outtextxy(432+16,32+16,"save");
    readimagefile("button.gif",416+16,48+16,432+16,64+16);outtextxy(432+16,48+16,"¿j");
    readimagefile("button.gif",416+16,64+16,432+16,80+16);outtextxy(432+16,64+16,"eraser");
    readimagefile("button.gif",416+16,80+16,432+16,96+16);outtextxy(432+16,80+16,"¯ó");
    readimagefile("button.gif",416+16,96+16,432+16,112+16);outtextxy(432+16,96+16,"¿û");
    readimagefile("button.gif",416+16,112+16,432+16,128+16);outtextxy(432+16,112+16,"®ü");
    

    
    
    while(1)
    {
        detectspuareposition(&x,&y);
        delay(25);
        
        if(x<(416+16)&&x>=(0+16))
        {
         brick(x,y,type);
         tankmap[(y/16)][(x/16)]=type;
         //printf("%d,%d) %d %d\n",x/16,y/16,x,y);
        }        
        
        if(x==(416+16)&&y==(112+16))
        {
            type=4;
        }        
        if(x==(416+16)&&y==(96+16))
        {
            type=3;
        }        
        if(x==(416+16)&&y==(80+16))
        {
            type=2;
        }        
        
        if(x==(416+16)&&y==(64+16))
        {
            type=0;
        }
        if(x==(416+16)&&y==(48+16))
        {
            type=1;
        }
        
        
        if(x==(416+16)&&y==(32+16))
        {
            savemap();
        }
        if(x==(416+16)&&y==(16+16))
        {
            for(int j=(0+1);j<(26+1);j++)
                for(int k=(0+1);k<(26+1);k++)
                    tankmap[j][k]=0;
            savemap();
            setfillstyle(1,BLACK);bar(1+16,1+16,416+16,416+16);     
        }
        if(x==(416+16)&&y==(0+16))break;
    }
    savemap();

    
    
    closegraph();
    return 0;
}
