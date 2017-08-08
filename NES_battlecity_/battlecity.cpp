#include <graphics.h>
#include <conio.h> 
#include <stdio.h>
#include <math.h>
#pragma comment(lib,"winmm.lib")
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>
#define Q 5
#define N 27 
#define N_ofbullet 4
#define ALLOWONFIELD 4 
#define USERLIVE 5
#define N_of_prop 4
#define DELAYTIME 22
int stageno=0;
#define VELOCITY 3


struct position
{
    int x;
    int y;
    int direction;
    int launch;
};

struct item
{
    char path[16][40];
    int x;
    int y;
    int direction;
    int launch;
    int no;
    int team;
    int live;
    int effect;
    int bv;
    
};
int max(int a,int b)
{
    if(a>b)return a;
    else return b;
}
void setbomb(item *explode,int x,int y)
{
    explode->launch=0;
    explode->x=x;
    explode->y=y;
}

void setstar(item *star,int x,int y)
{
    star->launch=0;
    star->x=x;
    star->y=y;
}
void setappear(item *appear,int x,int y)
{
    appear->launch=0;
    appear->x=x-13;
    appear->y=y-13;
}
void setshield(item *shield,int x,int y)
{
    shield->launch=0;
    shield->x=x;
    shield->y=y;
}


    



int page=1;
int launcha[2]={1,1};
char map[26+2][26+2]={0};
char body[448][448]={0};
void *brickbitmap;
void *grassbitmap;
void *steelbitmap;
void  *seabitmap0;
void  *seabitmap1; 
void  *enemymarkbitmap; 
void  *flagbitmap; 
void  *livebitmap; 


char stage[11][10]={"map00.txt",
                    "map01.txt",
                    "map02.txt",
                    "map03.txt",
                    "map04.txt",
                    "map05.txt",
                    "map06.txt",
                    "map07.txt",
                    "map08.txt",
                    "map09.txt",
                    "map10.txt"};
  
    
void readmap()
{
    FILE *fp;
    fp=fopen(stage[stageno],"r");
    //initialized
    for(int j=(0+1);j<(26+1);j++)
    {
        for(int k=(0+1);k<(26+1);k++)
        {
            fscanf(fp," %d",&map[j][k]);  
        }
    }
    fclose(fp);
}



void setactaulbarrier(int y,int x,int type)
{
    for(int j=(16*y);j<(16*y+16);j++)
    {
        for(int k=(16*x);k<(16*x+16);k++)
        {
            body[j][k]=type;
        }
    }
}

int keyboard(position bullet[N_ofbullet])//按下哪顆鍵 
{    
    
    
    if(bullet[0].launch==0)
        if(GetKeyState(90)<0)
            {PlaySound("launch.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);return 4;}
    if(stageno!=-2&&bullet[1].launch==0)
        if(GetKeyState(88)<0)
            {PlaySound("launch.wav", NULL,SND_FILENAME|SND_ASYNC);return 5;}    
    if(stageno!=-2&&bullet[2].launch==0)
        if(GetKeyState(67)<0)
            {PlaySound("launch.wav", NULL,SND_FILENAME|SND_ASYNC);return 6;}  
    if(stageno!=-2&&bullet[3].launch==0)
        if(GetKeyState(86)<0)
            {PlaySound("launch.wav", NULL,SND_FILENAME|SND_ASYNC);return 7;}
    if(GetKeyState(VK_UP)<0)
        {PlaySound("walk.wav", NULL,SND_NOSTOP|SND_FILENAME|SND_ASYNC);return 0;}
    if(GetKeyState(VK_DOWN)<0)
        {PlaySound("walk.wav", NULL,SND_NOSTOP|SND_FILENAME|SND_ASYNC);return 1;}
    if(GetKeyState(VK_LEFT)<0)
        {PlaySound("walk.wav", NULL,SND_NOSTOP|SND_FILENAME|SND_ASYNC);return 2;}
    if(GetKeyState(VK_RIGHT)<0)
        {PlaySound("walk.wav", NULL,SND_NOSTOP|SND_FILENAME|SND_ASYNC);return 3;}
    return 8;
}

int edgetrigger(int*last,int duration)
{
    if(time(NULL)%duration==0&&time(NULL)!=*last)
    {
        *last=time(NULL);    
        return 1;  
    }
    else
        return 0;
}





void animation(item tank[N],position bullet[N][N_ofbullet],item explode[N],item star[N],int onfield,item appear[N],item* shield,item *prop)
{
    setactivepage(page);
    cleardevice();
    //敵人剩幾隻 
    int ii=0,picx=448,picy=32,remainenemy=N-1;
    for(int i=1;i<N;i++)
    {
        if(tank[i].no==0)
            remainenemy--;
    }
    
    
    while(ii<remainenemy)
    {   
        putimage(picx,picy,enemymarkbitmap,0);
        ii++;
        picx+=16;
        if(picx>=480)
        {
            picx=448;
            picy+=16;//換行 
        }
    }
    //命 &關卡標示 
    char tmp[3];
    sprintf(tmp,"%d",stageno);
    putimage(448,352,flagbitmap,0);
    if(stageno==-2)outtextxy(464,372,"final");
    else outtextxy(464,372,tmp);
    sprintf(tmp,"%d",tank[0].live);
    putimage(448,272,livebitmap,0);outtextxy(466,288,tmp);

            
    
    //坦克車 
    setfillstyle(5,14);
    for(int i=0;i<onfield;i++)
    {
        if(tank[i].no==0);//0就是死了 
        else
        {
            readimagefile(tank[i].path[(tank[i].bv==7?8:0)+page+(tank[i].direction)*2],tank[i].x-26,tank[i].y-26,tank[i].x,tank[i].y);
            if(tank[i].live==1&&tank[i].effect!=0)
                readimagefile("santamask.gif",tank[i].x-26,tank[i].y-26,tank[i].x,tank[i].y);
        }
    }
    //磚塊//草棚//鋼牆//海 
    
    for(int k=(0+1);k<(26+1);k++)
    {
        for(int j=(0+1);j<(26+1);j++)
        {    
            if(map[j][k]==1)
                putimage(16*k,16*j,brickbitmap,0);
            if(map[j][k]==2)
                putimage(16*k,16*j,grassbitmap,1);
            if(map[j][k]==3)
                putimage(16*k,16*j,steelbitmap,0);
            if(map[j][k]==4&&time(NULL)%2==0)
                putimage(16*k,16*j,seabitmap0,0);
            else if(map[j][k]==4&&time(NULL)%2==1)
                putimage(16*k,16*j,seabitmap1,0);
        }
    }
    //子彈 
    for(int i=0;i<onfield;i++)
    {
        if(tank[i].no==0);//0就是死了 
        else
        {
            for(int j=0;j<N_ofbullet;j++)
            {
                if(bullet[i][j].launch==0)
                    fillellipse(tank[i].x-13,tank[i].y-13,3,3);
                else
                    fillellipse(bullet[i][j].x,bullet[i][j].y,3,3);
            }
        }
    }
    //道具
    for(int i=0;i<N_of_prop;i++)
    {
        if(prop[i].launch==1)
        {
            readimagefile(prop[i].path[0],prop[i].x,prop[i].y,prop[i].x+32,prop[i].y+32); 
            //printf("有進來吧\n");
        }   
    }
    
    if(shield[0].launch==0&&tank[0].effect==9)
    { 
        if(shield[0].direction>32767)shield[0].direction-=32767;
        shield[0].direction++;
        readimagefile(shield[0].path[shield[0].direction%2],shield[0].x-13,shield[0].y-13,shield[0].x+13,shield[0].y+13);
        
    }
    
    
    //老鷹 
    if(tank[0].no==50)readimagefile("eagle.gif",192+16,384+16,224+16,416+16); 
    else
    {
        readimagefile("eaglecrash.gif",192+16,384+16,224+16,416+16);
        readimagefile("gameover.gif",100,144,348,304);
    } 
    //爆炸 
    for(int i=0;i<N;i++)
    {
        if((explode[i].x<435&&explode[i].y<435)&&explode[i].launch<4)
        {
            readimagefile(explode[i].path[explode[i].launch],explode[i].x-15,explode[i].y-15,explode[i].x+15,explode[i].y+15);
            explode[i].launch++;
        }
        if(explode[i].launch==1&&tank[i].no==50)PlaySound("explode.wav", NULL,SND_FILENAME|SND_ASYNC);
        //else PlaySound("hit.wav", NULL,SND_FILENAME|SND_ASYNC);
    }    
    //瞄準 
    for(int i=0;i<N;i++)
    {
        if(star[i].launch<7)
        {
            readimagefile(star[i].path[star[i].launch],star[i].x-26,star[i].y-26,star[i].x+26,star[i].y+26);
            star[i].launch++;
        }
    }
    //剛出現 
    for(int i=0;i<N;i++)
    {
        if(appear[i].launch<11)
        {
            readimagefile(appear[i].path[appear[i].launch],appear[i].x-26,appear[i].y-26,appear[i].x+26,appear[i].y+26);
            appear[i].launch++;
        }
    }
        
    rectangle(16,16,432,432);
    
    setvisualpage(page);
    page=!page;
    delay(DELAYTIME);
      
}
//////////////////////////////////////////////////////////////
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM//
int staytimes[N]={0};//,4,5,5
int alreadytimes[N]={0};//,5,4,4
int comctrl1(item *tank,int randtimes,item target)
{
    //random
    if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
    {
        staytimes[(tank->no)-50]=randtimes;
        alreadytimes[(tank->no)-50]=0;
        return rand()%Q;
             }
    else
    {
        alreadytimes[(tank->no)-50]++;
        return (*tank).direction;
    }
    

}
//接近時 方向random 
int comctrl2(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    int distance[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)
    dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)
    dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)
    dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)
    dirc[3]=1;
    
    if(dirc[0]==1)distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    else distance[0]=500000;
    if(dirc[1]==1)distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    else distance[1]=500000;
    if(dirc[2]==1)distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[2]=500000;
    if(dirc[3]==1)distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[3]=500000;
    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            
            choice=i;
            minimum=distance[i];
        }
    }
    if(minimum>800) return choice;
    return rand()%Q;
}
int comctrl3(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    int distance[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)
    dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)
    dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)
    dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)
    dirc[3]=1;
    
    if(dirc[0]==1)distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    else distance[0]=500000;
    if(dirc[1]==1)distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    else distance[1]=500000;
    if(dirc[2]==1)distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[2]=500000;
    if(dirc[3]==1)distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[3]=500000;
    int minimum=500000,max=0;
    int choice=(*tank).direction;
    int choiceM=(*tank).direction;
    
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    for(int i=0;i<4;i++)
    {
        if(distance[i]>max)
        {
            choiceM=i;
            max=distance[i];
        }
    }
    //if(body[mousex()][mousey()]==50)printf("%d %d %d\n",mousex(),mousey(),body[mousex()][mousey()]);
                //printf("%d %d %d %d ",dirc[0],dirc[1],dirc[2],dirc[3]);
                //printf("%d %d %d %d",distance[0],distance[1],distance[2],distance[3]);
                //printf("(%d %d\n",choiceM,choice);
    if(minimum>2500&&minimum<20000)
    {
        if((((target.x-15)<(*tank).x&&(*tank).x<(target.x+15)))||((target.y-15)<(*tank).y&&(*tank).y<(target.y+15)))
        {
            if(choiceM==0)(*tank).direction=0;
            if(choiceM==1)(*tank).direction=1;
            if(choiceM==2)(*tank).direction=2;
            if(choiceM==3)(*tank).direction=3;
            return 4;
        }
        else 
            return choice;
    } 
    else
    {
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}


int comctrl4(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    int distance[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)
    dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)
    dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)
    dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)
    dirc[3]=1;
    
    if(dirc[0]==1)distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    else distance[0]=500000;
    if(dirc[1]==1)distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    else distance[1]=500000;
    if(dirc[2]==1)distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[2]=500000;
    if(dirc[3]==1)distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[3]=500000;
    int minimum=500000,max=0;
    int choice=(*tank).direction;
    int choiceM=(*tank).direction;
    
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    for(int i=0;i<4;i++)
    {
        if(distance[i]>max)
        {
            choiceM=i;
            max=distance[i];
        }
    }
    //if(body[mousex()][mousey()]==50)printf("%d %d %d\n",mousex(),mousey(),body[mousex()][mousey()]);
                //printf("%d %d %d %d ",dirc[0],dirc[1],dirc[2],dirc[3]);
                //printf("%d %d %d %d",distance[0],distance[1],distance[2],distance[3]);
                //printf("(%d,%d (%d,%d\n",target.x,target.y,(*tank).x,(*tank).y);
    if(minimum>2500&&minimum<25000)
    {
        if((((target.x-10)<(*tank).x&&(*tank).x<(target.x+10)))||((target.y-10)<(*tank).y&&(*tank).y<(target.y+10)))
        {
            if(choiceM==0)(*tank).direction=1;
            if(choiceM==1)(*tank).direction=0;
            if(choiceM==2)(*tank).direction=3;
            if(choiceM==3)(*tank).direction=2;
            return 4;
        }
        else
        {
            if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
            {
                staytimes[(tank->no)-50]=randtimes;
                alreadytimes[(tank->no)-50]=0;
                return (rand()%Q);
                }
            else
            {
                alreadytimes[(tank->no)-50]++;
                return (*tank).direction;
            }
        }
    } 
    else
    {
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}



int comctrl5(item *tank,int randtimes,item target)
{
        int distance[4]={0};
        int max=0,choiceM=0;
        distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
        distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
        distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
        distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
        for(int i=0;i<4;i++)
        {
            if(distance[i]>max)
            {
                choiceM=i;
                max=distance[i];
            }
        }
        if((((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y))*(target.y-((*tank).y))))<6000)
        {   
            return choiceM;
        }
        else
        {
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
            {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}


int comctrl6(item *tank,int randtimes,item target)
{

    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
       
    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }

    if(minimum>2500&&minimum<30000)
    {
        if(((target.y-15)<(*tank).y&&(*tank).y<(target.y+15)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            return 4;
        }
        else if((((target.x-15)<(*tank).x&&(*tank).x<(target.x+15))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            return 4;
        }
        else 
            return choice;
    } 
    else
    {
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}
int lastdistance[N][4];
int comctrl7(item *tank,int randtimes,item target)
{

    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
        //printf("%d %d %d %d\n",distance[0],distance[1],distance[2],distance[3]);

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
   //if(body[mousex()][mousey()]==50)printf("%d %d %d\n",mousex(),mousey(),body[mousex()][mousey()]);
    if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
    {
 
        return 4;
    }
    if(minimum>2500&&minimum<20000)
    {
        if(((target.y-15)<(*tank).y&&(*tank).y<(target.y+15)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            return 4;
        }
        else if((((target.x-15)<(*tank).x&&(*tank).x<(target.x+15))))
        {
            
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            return 4;
        }
        else 
        {
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            return choice;
        }
    } 
    else
    {
     if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}
int comctrl8(item *tank,int randtimes,item target)
{
int dirc[4]={0};
    int distance[4]={0};
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)
    dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)
    dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)
    dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)
    dirc[3]=1;
    
    if(dirc[0]==1)distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    else distance[0]=500000;
    if(dirc[1]==1)distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    else distance[1]=500000;
    if(dirc[2]==1)distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[2]=500000;
    if(dirc[3]==1)distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    else distance[3]=500000;   //  printf("%d %d %d %d\n",distance[0],distance[1],distance[2],distance[3]);

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    //if(body[mousex()][mousey()]==50)printf("%d %d %d\n",mousex(),mousey(),body[mousex()][mousey()]);
    if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
    {
        return 4;
    }
    if(minimum>2500&&minimum<20000)
    {
        if(((target.y-15)<(*tank).y&&(*tank).y<(target.y+15)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            return 4;
        }
        else if((((target.x-15)<(*tank).x&&(*tank).x<(target.x+15))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            return 4;
        }
        else 
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            return choice;
    } 
    else
    {
     if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=(randtimes);
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}
int getaround[N]={0};
int comctrl9(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)dirc[3]=1;
   
    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    if(getaround[(*tank).no-50]!=0)
    {
        if(dirc[0]==0&&getaround[(*tank).no-50]!=12)
        {
            getaround[(*tank).no-50]=12;
            return (2+rand()%2);
        }
        else if(dirc[0]==0&&getaround[(*tank).no-50]==12)
        {
            //printf("卡在0");
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
                //printf("%d %d %d %d))%d %d %d %d\n",lastdistance[0],lastdistance[1],lastdistance[2],lastdistance[3],distance[0],distance[1],distance[2],distance[3]);
 
    
    //printf("                           %d %d %d %d %d\n",dirc[0],dirc[1],dirc[2],dirc[3],getaround[(*tank).no-50]);

            return (*tank).direction;
        }
        else if(dirc[0]==1&&getaround[(*tank).no-50]==12)
        {
            getaround[(*tank).no-50]=0;
            return 0;            
        }
        
        
        if(dirc[1]==0&&getaround[(*tank).no-50]!=22)
        {
            getaround[(*tank).no-50]=22;
            return (2+rand()%2);
        }
        else if(dirc[1]==0&&getaround[(*tank).no-50]==22)
        {//printf("卡在1");
            return (*tank).direction;
        }
        else if(dirc[1]==1&&getaround[(*tank).no-50]==22)
        {
            getaround[(*tank).no-50]=0;
            return 1;            
        }
        
        
        if(dirc[2]==0&&getaround[(*tank).no-50]!=32)
        {
            getaround[(*tank).no-50]=32;
            return (rand()%2);
        }
        else if(dirc[2]==0&&getaround[(*tank).no-50]==32)
        {//printf("卡在2");
            return (*tank).direction;
        }
        else if(dirc[2]==1&&getaround[(*tank).no-50]==32)
        {
            getaround[(*tank).no-50]=0;
            return 2;            
        }
        if(dirc[3]==0&&getaround[(*tank).no-50]!=42)
        {
            getaround[(*tank).no-50]=42;
            return (rand()%2);
        }
        else if(dirc[3]==0&&getaround[(*tank).no-50]==42)
        {//printf("卡在3");
            return (*tank).direction;
        }
        else if(dirc[3]==1&&getaround[(*tank).no-50]==42)
        {
            getaround[(*tank).no-50]=0;      
            return 3;      
        }
    }
    else if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
    {
        if((dirc[0]==0)||(dirc[1]==0)||(dirc[2]==0)||(dirc[3]==0))getaround[(*tank).no-50]=1;
        //printf("卡在4 %d",getaround[(*tank).no-50]);
        return 4;
    }
    if(minimum>2500&&minimum<20000)
    {
        
        //printf(" MMMMMMMMMMMMMMMMMM 進來亂數控制");
        if(((target.y-15)<(*tank).y&&(*tank).y<(target.y+15)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            //printf("卡在5");
            return 4;
        }
        else if((((target.x-15)<(*tank).x&&(*tank).x<(target.x+15))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("卡在6");
            return 4;
        }
        else 
        {
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("卡在7");
            return choice;
        }
    } 
    else
    {
     if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            
            return (*tank).direction;
        }
    }
}
int comctrl10(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)dirc[3]=1;
   
    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    
    if(minimum>2500&&minimum<35000)
    {
        if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
            return rand()%Q;
        else
        {
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            return choice;
        }
    } 
    else
    {
        if(((target.y-8)<(*tank).y&&(*tank).y<(target.y+8)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            //printf("卡在1\n");
            return 4;
        }
        else if((((target.x-8)<(*tank).x&&(*tank).x<(target.x+8))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            //printf("卡在2\n");
            return 4;
        }
        else if(alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            //printf("卡在3\n");
            return rand()%Q;
            }
        else
        {
            alreadytimes[(tank->no)-50]++;
            //printf("卡在4\n");
            return (*tank).direction;
        }
    }
}


int comctrl11(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)dirc[3]=1;
   
    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    if(minimum>2500&&minimum<20000)
    {        
        //printf("ooooooo\n");
        if(((target.y-8)<(*tank).y&&(*tank).y<(target.y+8)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            return 4;
        }
        else if((((target.x-8)<(*tank).x&&(*tank).x<(target.x+8))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            return 4;
        }
        else 
            return choice;
    } 
    else
    {
        if(minimum>1000&&((target.y-8)<(*tank).y&&(*tank).y<(target.y+8)))
        {
            if(target.x<(*tank).x)(*tank).direction=2;
            if(target.x>(*tank).x)(*tank).direction=3;
            return 4;
        }
        else if(minimum>1000&&(((target.x-8)<(*tank).x&&(*tank).x<(target.x+8))))
        {
            if(target.y<(*tank).y)(*tank).direction=0;
            if(target.y>(*tank).y)(*tank).direction=1;
            return 4;
        }
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            return rand()%(Q-1);
        }
        else
        {
            alreadytimes[(tank->no)-50]++;
            return (*tank).direction;
        }
    }
}
void tankbarrier(item);
int comctrl12(item *tank,int randtimes,item target)
{
    int dirc[4]={0};
    
    if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)dirc[0]=1;
    if(!(((*tank).y+1)>432)&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)dirc[1]=1;
    if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-22]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)dirc[2]=1;
    if(!(((*tank).x+2)>432)&&body[(*tank).x+3][(*tank).y-22]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)dirc[3]=1;
   
    int distance[4]={0};
    distance[0]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y-3))*(target.y-((*tank).y-3)));
    distance[1]=((target.x-((*tank).x))*(target.x-((*tank).x))+(target.y-((*tank).y+3))*(target.y-((*tank).y+3)));
    distance[2]=((target.x-((*tank).x-3))*(target.x-((*tank).x-3))+(target.y-((*tank).y))*(target.y-((*tank).y)));
    distance[3]=((target.x-((*tank).x+3))*(target.x-((*tank).x+3))+(target.y-((*tank).y))*(target.y-((*tank).y)));

    int minimum=500000;
    int choice=(*tank).direction;
    for(int i=0;i<4;i++)
    {
        if(distance[i]<minimum)
        {
            choice=i;
            minimum=distance[i];
        }
    }
    if(getaround[(*tank).no-50]!=0)
    {   //////////////////////////////////////////////

        if(dirc[1]==0&&getaround[(*tank).no-50]!=22)
        {
            getaround[(*tank).no-50]=22;
            //printf("4\n");
            return (rand()%4);
        }
        else if(dirc[1]==0&&getaround[(*tank).no-50]==22)
        {
            if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
                {
                    getaround[(*tank).no-50]=1;//printf("5\n");
                }
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("6\n");
            return (*tank).direction;
        }
        else if(dirc[1]==1&&getaround[(*tank).no-50]==22)
        {
            
            
            if((*tank).direction==3){(*tank).x+=2;tankbarrier((*tank));}
            else if((*tank).direction==2){(*tank).x-=2;tankbarrier((*tank));}
            (*tank).direction=1;(*tank).y+=2;tankbarrier((*tank));(*tank).y+=2;tankbarrier((*tank));
            
            getaround[(*tank).no-50]=0;
            //printf("7\n");
            return 1;            
        }
        //////////////////////////////////////////////                
        if(dirc[0]==0&&getaround[(*tank).no-50]!=12)
        {
            getaround[(*tank).no-50]=12;
            //printf("0\n");
            return (rand()%4);
        }
        else if(dirc[0]==0&&getaround[(*tank).no-50]==12)
        {
            if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
                {
                    getaround[(*tank).no-50]=1;
                }
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("2\n");
            return (*tank).direction;
        }
        else if(dirc[0]==1&&getaround[(*tank).no-50]==12)
        {
            
            
            if((*tank).direction==3){(*tank).x+=2;tankbarrier((*tank));}
            else if((*tank).direction==2){(*tank).x-=2;tankbarrier((*tank));}
            (*tank).direction=0;(*tank).y-=2;tankbarrier((*tank));(*tank).y-=2;tankbarrier((*tank));
           
            getaround[(*tank).no-50]=0;
            //printf("3\n");
            return 0;            
        }
//////////////////////////////////////////////
        if(dirc[2]==0&&getaround[(*tank).no-50]!=32)
        {
            getaround[(*tank).no-50]=32;
            //printf("8\n");
            return (rand()%4);
        }
        else if(dirc[2]==0&&getaround[(*tank).no-50]==32)
        {
            if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
                {
                    getaround[(*tank).no-50]=1;
                    //printf("9\n");
                }
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("10\n");
            return (*tank).direction;
        }
        else if(dirc[2]==1&&getaround[(*tank).no-50]==32)
        {
            
            if((*tank).direction==1){(*tank).y+=2;tankbarrier((*tank));}
            else if((*tank).direction==0){(*tank).y-=2;tankbarrier((*tank));}
            (*tank).direction=2;(*tank).x-=2;tankbarrier((*tank));(*tank).x-=2;tankbarrier((*tank));
            
            getaround[(*tank).no-50]=0;
            //printf("11\n");
            return 2;            
        }
        //////////////////////////////////////////////
        if(dirc[3]==0&&getaround[(*tank).no-50]!=42)
        {
            getaround[(*tank).no-50]=42;
            //printf("12\n");
            return (rand()%4);
        }
        else if(dirc[3]==0&&getaround[(*tank).no-50]==42)
        {
            if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
                {
                    getaround[(*tank).no-50]=1;//printf("13\n");
                }
            lastdistance[(*tank).no-50][0]=distance[0];
            lastdistance[(*tank).no-50][1]=distance[1];
            lastdistance[(*tank).no-50][2]=distance[2];
            lastdistance[(*tank).no-50][3]=distance[3];
            //printf("14\n");
            return (*tank).direction;
        }
        else if(dirc[3]==1&&getaround[(*tank).no-50]==42)
        {
            
            if((*tank).direction==1){(*tank).y+=2;tankbarrier((*tank));}
            else if((*tank).direction==0){(*tank).y-=2;tankbarrier((*tank));}
            (*tank).direction=3;(*tank).x+=2;tankbarrier((*tank));(*tank).x+=2;tankbarrier((*tank));
            
            getaround[(*tank).no-50]=0;      
            //printf("15\n");
            return 3;      
        }
    }
    else if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
    {
        if((dirc[0]==0)||(dirc[1]==0)||(dirc[2]==0)||(dirc[3]==0))
        {
            getaround[(*tank).no-50]=1;
        }
        //printf("16\n");
        return rand()%4;
    }
    
    if(((target.y-8)<(*tank).y&&(*tank).y<(target.y+8))&&minimum<20000)
    {
        if(target.x<(*tank).x&&dirc[2]==1){(*tank).direction=2;/*printf("17\n");*/(*tank).x-=1;tankbarrier((*tank));return 4;}
        if(target.x>(*tank).x&&dirc[3]==1){(*tank).direction=3;/*printf("18\n");**/(*tank).x+=1;tankbarrier((*tank));return 4;}
        return 0;
    }
    else if((((target.x-8)<(*tank).x&&(*tank).x<(target.x+8)))&&minimum<20000)
    {
            
        if(target.y<(*tank).y&&dirc[0]==1){(*tank).direction=0;/*printf("19\n");*/(*tank).y-=1;tankbarrier((*tank));return 4;}
        if(target.y>(*tank).y&&dirc[1]==1){(*tank).direction=1;/*printf("20\n");*/(*tank).y+=1;tankbarrier((*tank));return 4;}
        return 2;
    }
        
    if(minimum>2500&&minimum<35000&&getaround[(*tank).no-50]!=1)
    {
        if(lastdistance[(*tank).no-50][0]==distance[0]&&lastdistance[(*tank).no-50][1]==distance[1]&&lastdistance[(*tank).no-50][2]==distance[2]&&lastdistance[(*tank).no-50][3]==distance[3])
        {
            //printf("          %d %d %d %d %d %d %d %d %d %d %d %d %d\n",getaround[(*tank).no-50] ,dirc[0],dirc[1],dirc[2],dirc[3],lastdistance[(*tank).no-50][0],distance[0],lastdistance[(*tank).no-50][1],distance[1],lastdistance[(*tank).no-50][2],distance[2],lastdistance[(*tank).no-50][3],distance[3]);
            getaround[(*tank).no-50]=1;
        }
        lastdistance[(*tank).no-50][0]=distance[0];
        lastdistance[(*tank).no-50][1]=distance[1];
        lastdistance[(*tank).no-50][2]=distance[2];
        lastdistance[(*tank).no-50][3]=distance[3];
        //printf("21\n");
        return choice;
    } 
    else
    {
        if(minimum>1500&&((target.y-8)<(*tank).y&&(*tank).y<(target.y+8)))
        {
            
            if(target.x<(*tank).x&&dirc[2]==1){(*tank).direction=2;/*printf("22\n");(*tank).x-=1;*/tankbarrier((*tank));return 4;}
            if(target.x>(*tank).x&&dirc[3]==1){(*tank).direction=3;/*printf("23\n");(*tank).x+=1;*/tankbarrier((*tank));return 4;}
            //printf("24\n");
            return 1;
        }
        else if(minimum>1500&&(((target.x-8)<(*tank).x&&(*tank).x<(target.x+8))))
        {
            
            if(target.y<(*tank).y&&dirc[0]==1){(*tank).direction=0;(*tank).y-=1;tankbarrier((*tank));return 4;}
            if(target.y>(*tank).y&&dirc[1]==1){(*tank).direction=1;(*tank).y+=1;tankbarrier((*tank));return 4;}
            //printf("27\n");
            return 3;
        }
        if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
        {
            staytimes[(tank->no)-50]=randtimes;
            alreadytimes[(tank->no)-50]=0;
            //printf("28\n");
            return rand()%(Q);
        }
        else
        {
            alreadytimes[(tank->no)-50]++;
            //printf("          %d %d %d %d %d %d %d %d %d %d %d %d %d ",getaround[(*tank).no-50] ,dirc[0],dirc[1],dirc[2],dirc[3],lastdistance[(*tank).no-50][0],distance[0],lastdistance[(*tank).no-50][1],distance[1],lastdistance[(*tank).no-50][2],distance[2],lastdistance[(*tank).no-50][3],distance[3]);
            
            //printf("29 alreadytimes :%d\n",alreadytimes[(tank->no)-50]);
            return (*tank).direction;
        }
    }
}
int comctrl13(item *tank,int randtimes,item target)
{
    //random
    if(((target.y-8)<(*tank).y&&(*tank).y<(target.y+8)))
        {
            
            if(target.x<(*tank).x){(*tank).direction=2;(*tank).x-=3;tankbarrier((*tank));}
            if(target.x>(*tank).x){(*tank).direction=3;(*tank).x+=3;tankbarrier((*tank));}
            if(((target.y-80)<(*tank).y&&(*tank).y<(target.y+80))&&(((target.x-80)<(*tank).x&&(*tank).x<(target.x+80))))
                return 4;//printf("24\n");
                return 1;
        }
        else if((((target.x-8)<(*tank).x&&(*tank).x<(target.x+8))))
        {
            
            if(target.y<(*tank).y){(*tank).direction=0;(*tank).y-=3;tankbarrier((*tank));}
            if(target.y>(*tank).y){(*tank).direction=1;(*tank).y+=3;tankbarrier((*tank));}
            if(((target.y-80)<(*tank).y&&(*tank).y<(target.y+80))&&(((target.x-80)<(*tank).x&&(*tank).x<(target.x+80))))
                return 4;//printf("27\n");
                return 3;
        }
    if( alreadytimes[(tank->no)-50]==staytimes[(tank->no)-50]||alreadytimes[(tank->no)-50]>staytimes[(tank->no)-50])
    {
        staytimes[(tank->no)-50]=randtimes;
        alreadytimes[(tank->no)-50]=0;
        return rand()%(Q);
             }
    else
    {
        alreadytimes[(tank->no)-50]++;
        return (*tank).direction;
    }
    

}

//////////////////////////////////////////////////////////////
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM//

void bulletbarrier(position bullet)
{    
    int y1,x1,x2,y2;
    if(bullet.launch==1)
    {   if(bullet.direction==0){y1=(bullet.y)-5;y2=(bullet.y-3);x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==1){y1=(bullet.y)+3;y2=(bullet.y+5);x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==2){y1=(bullet.y)-6;y2=(bullet.y+4);x1=(bullet.x)-5;x2=(bullet.x-2);}
        if(bullet.direction==3){y1=(bullet.y)-6;y2=(bullet.y+4);x1=(bullet.x)+2;x2=(bullet.x+5);}
        for(int j=y1;j<=y2;j++)
        {
            for(int k=x1;k<=x2;k++)
            {
                body[k][j]=11;
            }
        }
        if(bullet.direction==0){y1=y2+1;y2=y2+7;x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==1){y1=y1-6;y2=y1+6;x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==2){y1=(bullet.y)-6;y2=(bullet.y+4);x1=x2+1;x2=x1+7;}
        if(bullet.direction==3){y1=(bullet.y)-6;y2=(bullet.y+4);x1=x1-6;x2=x1+6;}
        for(int j=y1;j<=y2;j++)
        {
            for(int k=x1;k<=x2;k++)
            {
                body[k][j]=0;
            }
        }
    }
    else if(bullet.launch==0)
    {
        if(bullet.direction==0){y1=(bullet.y)+1;y2=(bullet.y+4);x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==1){y1=(bullet.y)-4;y2=(bullet.y-1);x1=(bullet.x)-5;x2=(bullet.x+5);}
        if(bullet.direction==2){y1=(bullet.y)-6;y2=(bullet.y+4);x1=(bullet.x)+1;x2=(bullet.x+5);}
        if(bullet.direction==3){y1=(bullet.y)-6;y2=(bullet.y+4);x1=(bullet.x)-4;x2=(bullet.x-1);}
        for(int j=y1;j<=y2;j++)
        {
            for(int k=x1;k<=x2;k++)
            {
                body[k][j]=0;
            }
        }
        
    }
}
float theta=0;
float radius=100;//都玩家專用
int trace=0;
int clockwise=0;
int lsttm;
item *nearest;
item *findnearest(item *tank,int onfield)
{
    int mindistance=340000;
    int no=0;
    for(int i=1;i<onfield;i++)
    {
        if(tank[i].no==0)continue;
        if(((tank[i].x-tank[0].x)*(tank[i].x-tank[0].x)+(tank[i].y-tank[0].y)*(tank[i].y-tank[0].y))<mindistance)
        {
            no=i;
            mindistance=(tank[i].x-tank[0].x)*(tank[i].x-tank[0].x)+(tank[i].y-tank[0].y)*(tank[i].y-tank[0].y);
        }
    }
    return (no+tank);
    
}

void itemctrl(item *tank,int direction,position bullet[N_ofbullet], item *explode, item *star,int onfield,item *shield,item *prop)
{
    if(direction==0)
    {
        tank->direction=0;
        if(bullet[0].launch==0){bullet[0].direction=0;}
    /*if(bullet[1].launch==0)*/{bullet[1].direction=0;}
        if(bullet[2].launch==0){bullet[2].direction=0;}
        if(bullet[3].launch==0){bullet[3].direction=0;}
        
        if(!(((*tank).y-30)<16)&&body[(*tank).x-26][(*tank).y-27]==0&&body[(*tank).x][(*tank).y-27]==0&&body[(*tank).x-13][(*tank).y-27]==0)
        {
            if((*tank).no==50)
            {
                (*tank).y-=VELOCITY;
            }
            else(*tank).y-=(VELOCITY-1);
        }  
    }   
    if(direction==1)
    { 
        tank->direction=1;
        if(bullet[0].launch==0){bullet[0].direction=1;}//子彈未發射時都跟著 
    /*if(bullet[1].launch==0)*/{bullet[1].direction=1;}
        if(bullet[2].launch==0){bullet[2].direction=1;}
        if(bullet[3].launch==0){bullet[3].direction=1;}

        if(!(((*tank).y+1)>(416+16))&&body[(*tank).x-26][(*tank).y+3]==0&&body[(*tank).x][(*tank).y+3]==0&&body[(*tank).x-13][(*tank).y+3]==0)
        {
            if((*tank).no==50)
            {
                (*tank).y+=VELOCITY;
            }
            else(*tank).y+=(VELOCITY-1);
        }
    }
    if(direction==2)
    {
        tank->direction=2;
        if(bullet[0].launch==0){bullet[0].direction=2;}
    /*if(bullet[1].launch==0)*/{bullet[1].direction=2;}
        if(bullet[2].launch==0){bullet[2].direction=2;}
        if(bullet[3].launch==0){bullet[3].direction=2;}

        if(!(((*tank).x-33)<16)&&body[(*tank).x-29][(*tank).y-24]==0&&body[(*tank).x-29][(*tank).y]==0&&body[(*tank).x-29][(*tank).y-13]==0)
        {
            if((*tank).no==50)
            {
                (*tank).x-=VELOCITY;
            }
            else(*tank).x-=(VELOCITY-1);
        }
    }
    if(direction==3)
    {
        tank->direction=3;
        if(bullet[0].launch==0){bullet[0].direction=3;}
    /*if(bullet[1].launch==0)*/{bullet[1].direction=3;}
        if(bullet[2].launch==0){bullet[2].direction=3;}
        if(bullet[3].launch==0){bullet[3].direction=3;}

        if(!(((*tank).x+2)>(416+16))&&body[(*tank).x+3][(*tank).y-24]==0&&body[(*tank).x+2][(*tank).y]==0&&body[(*tank).x+2][(*tank).y-13]==0)
        {
            if((*tank).no==50)
            {
                (*tank).x+=VELOCITY;
            }
            else(*tank).x+=(VELOCITY-1);
        }
    } 
    //子彈部分
    
  
        //printf("%d %d %d %d\n",bullet[0].launch,bullet[0].y,bullet[1].launch,bullet[1].y);
        //(*tank).launch=2-(bullet[0].launch+bullet[1].launch); 
        if(direction==4)bullet[0].launch=1;
        if(direction==5)bullet[1].launch=1;
        if(direction==6)bullet[2].launch=1;
        if(direction==7)bullet[3].launch=1;
        //
        //printf("                    %d %d\n",bullet[3].launch,trace);
    for(int i=0;i<N_ofbullet;i++)//不停地運算這些顆子彈的位置 
    {          
        if(bullet[i].launch==1)//是發射中，就進來 
        {
            if(tank[0].no==50&&i==2)//特殊功能 
            {
                bullet[i].y=(int)((*(tank)).y-13+25*sin(theta));
                bullet[i].x=(int)((*(tank)).x-13+25*cos(theta));
                theta+=0.7;
                if(theta>6.28)theta-=6.28;
            }            
            else if(tank[0].no==50&&i==3)//特殊功能 子彈會轉彎 
            {
                if(trace==0)
                {
                    nearest=findnearest(tank,onfield);
                    setstar(star,nearest->x-13,nearest->y-13);
                    if((bullet[i].direction==1||bullet[i].direction==0)&&bullet[i].y>((*(nearest)).y-20)&&bullet[i].y<((*(nearest)).y-6))
                    {   trace=1;
                        
                        if(bullet[i].direction==0)
                        {
                            
                            if((*(nearest)).x-bullet[i].x>0){theta=3.14;clockwise=1;radius=abs(nearest->x-13-bullet[i].x);}//printf("算出R%d  %d %d \n",nearest->x-bullet[i].x,nearest->x,bullet[i].x);}//
                            if((*(nearest)).x-bullet[i].x<0){theta=6.28;clockwise=0;radius=abs(nearest->x-13-bullet[i].x);}//
                        }                    
                        if(bullet[i].direction==1)
                        {
                            if((*(nearest)).x-bullet[i].x>0){theta=3.14;clockwise=0;radius=abs(nearest->x-13-bullet[i].x);}
                            if((*(nearest)).x-bullet[i].x<0){theta=6.28;clockwise=1;radius=abs(nearest->x-13-bullet[i].x);}
                        }
                    }
                    else if((bullet[i].direction==2||bullet[i].direction==3)&&bullet[i].x>((*(nearest)).x-20)&&bullet[i].x<((*(nearest)).x-6))
                    {   trace=1;
                        
                        if(bullet[i].direction==2)
                        {
                            if((*(nearest)).y-bullet[i].y>0){theta=4.71;clockwise=0;radius=abs(nearest->y-13-bullet[i].y);}
                            if((*(nearest)).y-bullet[i].y<0){theta=1.57;clockwise=1;radius=abs(nearest->y-13-bullet[i].y);}
                        }                    
                        if(bullet[i].direction==3)
                        {
                            if((*(nearest)).y-bullet[i].y>0){theta=4.71;clockwise=1;radius=abs(nearest->y-13-bullet[i].y);}
                            if((*(nearest)).y-bullet[i].y<0){theta=1.57;clockwise=0;radius=abs(nearest->y-13-bullet[i].y);}
                        }
                    }
                    //沒找到時直線前進 
                    //printf("不只找一次吧%d %d\n",bullet[3].x,bullet[3].y);
                    if(bullet[i].direction==0)bullet[i].y=bullet[i].y-tank[0].bv;//printf("值線%d %d \n",bullet[i].x,bullet[i].y);}
                    if(bullet[i].direction==1)bullet[i].y=bullet[i].y+tank[0].bv;
                    if(bullet[i].direction==2)bullet[i].x=bullet[i].x-tank[0].bv;
                    if(bullet[i].direction==3)bullet[i].x=bullet[i].x+tank[0].bv;
                                

                }
                if(trace==1)//找到才能進來 
                {
                    if(clockwise==1){theta+=0.12;if(theta>6.28)theta-=6.28;}
                    if(clockwise==0){theta-=0.12;if(theta<0)theta+=6.28;}
                    bullet[i].y=(int)(nearest->y+radius*sin(theta)-13);
                    bullet[i].x=(int)(nearest->x+radius*cos(theta)-13);
                    //printf("繞圓%d %d R=%f\n",bullet[i].x,bullet[i].y,radius);
                    radius-=3;
                }


            }
            else//一般子彈路徑 
            {
                if(bullet[i].direction==0)bullet[i].y=bullet[i].y-tank[0].bv;
                if(bullet[i].direction==1)bullet[i].y=bullet[i].y+tank[0].bv;
                if(bullet[i].direction==2)bullet[i].x=bullet[i].x-tank[0].bv;
                if(bullet[i].direction==3)bullet[i].x=bullet[i].x+tank[0].bv;
            }
            
            
            
            
            //撞到的判斷 
            if(bullet[i].y>16&&bullet[i].y<(416+16)&&bullet[i].x>+16&&bullet[i].x<(416+16)&&map[bullet[i].y/16][bullet[i].x/16]==0&&body[bullet[i].x][bullet[i].y]==0);
            else if(map[bullet[i].y/16][bullet[i].x/16]==2||body[bullet[i].x][bullet[i].y]==2);//穿越草 
            else if(map[bullet[i].y/16][bullet[i].x/16]==4||body[bullet[i].x][bullet[i].y]==4);//穿越海 
            else
            {
                
                if(bullet[i].direction==0||bullet[i].direction==1)
                {
                    int tmp=max(body[(bullet[i].x-5)][bullet[i].y],body[(bullet[i].x+5)][bullet[i].y]);
                    setbomb(explode,bullet[i].x,bullet[i].y);   
                    if(map[bullet[i].y/16][bullet[i].x/16]==1||body[bullet[i].x][bullet[i].y]==1)
                    {
                        if(map[bullet[i].y/16][(bullet[i].x-5)/16]==1)
                        {
                            map[bullet[i].y/16][(bullet[i].x-5)/16]=0;setactaulbarrier((bullet[i].x-5)/16,bullet[i].y/16,0);
                        }
                        if(map[bullet[i].y/16][(bullet[i].x+5)/16]==1)
                        {
                            map[bullet[i].y/16][(bullet[i].x+5)/16]=0;setactaulbarrier((bullet[i].x+5)/16,bullet[i].y/16,0);
                        }
                    }
                    if(tank[0].bv==7&&(map[bullet[i].y/16][bullet[i].x/16]==3||body[bullet[i].x][bullet[i].y]==3))
                    {
                        if(map[bullet[i].y/16][(bullet[i].x-5)/16]==3)
                        {
                            map[bullet[i].y/16][(bullet[i].x-5)/16]=1;setactaulbarrier((bullet[i].x-5)/16,bullet[i].y/16,1);
                        }
                        if(map[bullet[i].y/16][(bullet[i].x+5)/16]==3)
                        {
                            map[bullet[i].y/16][(bullet[i].x+5)/16]=1;setactaulbarrier((bullet[i].x+5)/16,bullet[i].y/16,1);
                        }
                    }
                    
                    
                    
                    int offset=tmp-(*tank).no;
                    for(int i=(-(*tank).no+50);i<(50+N-(*tank).no);i++)
                    {
                        if(offset==i)
                        {                                              
                            if(tank[0].no==50&&tank[offset].live==1&&tank[offset].effect>0)
                            {
                                prop[tank[offset].effect-1].launch=1;
                                PlaySound("propappear.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                                prop[tank[offset].effect-1].x=(rand()%384)+16;
                                prop[tank[offset].effect-1].y=(rand()%384)+16;
                            }
                                                                 /*允許打老鷹 自殺*/
                            if(tank[offset].no==50&&tank[offset].effect==9);//防護罩狀態 
                            else
                            {
                                if((tank[offset].team^tank[0].team==1)||tank[0].no==50)
                                    tank[offset].live--;
                                if(tank[offset].live==-1)
                                    tank[offset].no=0;
                            }
                        }   
                    }
                }
                
                if(bullet[i].direction==2||bullet[i].direction==3)
                {
                    int tmp=max(body[(bullet[i].x-5)][bullet[i].y],body[(bullet[i].x+5)][bullet[i].y]);
                    setbomb(explode,bullet[i].x,bullet[i].y);

                    if(map[bullet[i].y/16][bullet[i].x/16]==1||body[bullet[i].x][bullet[i].y]==1)
                    {
                        if(map[(bullet[i].y+5)/16][((bullet[i].x)/16)]==1)
                        {
                            map[(bullet[i].y+5)/16][((bullet[i].x)/16)]=0;setactaulbarrier((bullet[i].x)/16,(bullet[i].y+5)/16,0);
                        }
                        if(map[(bullet[i].y-5)/16][((bullet[i].x)/16)]==1)
                        {
                            map[(bullet[i].y-5)/16][((bullet[i].x)/16)]=0;setactaulbarrier((bullet[i].x)/16,(bullet[i].y-5)/16,0);
                        }
                    }
                    if(tank[0].bv==7&&(map[bullet[i].y/16][bullet[i].x/16]==3||body[bullet[i].x][bullet[i].y]==3))
                    {
                        if(map[(bullet[i].y+5)/16][((bullet[i].x)/16)]==3)
                        {
                            map[(bullet[i].y+5)/16][((bullet[i].x)/16)]=1;setactaulbarrier((bullet[i].x)/16,(bullet[i].y+5)/16,1);
                        }
                        if(map[(bullet[i].y-5)/16][((bullet[i].x)/16)]==3)
                        {
                            map[(bullet[i].y-5)/16][((bullet[i].x)/16)]=1;setactaulbarrier((bullet[i].x)/16,(bullet[i].y-5)/16,1);
                        }
                    }





                    int offset=tmp-(*tank).no;
                    for(int i=(50-(*tank).no);i<((50+N-(*tank).no));i++)
                    {
                        if(offset==i)
                        {   
                            if(tank[0].no==50&&tank[offset].live==1&&tank[offset].effect>0)//打人 
                            {
                                prop[tank[offset].effect-1].launch=1;
                                PlaySound("propappear.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                                prop[tank[offset].effect-1].x=(rand()%384)+16;
                                prop[tank[offset].effect-1].y=(rand()%384)+16;
                            }
                            if(tank[offset].no==50&&tank[offset].effect==9);//防護罩狀態 被打 
                            else
                            {
                                if((tank[offset].team^tank[0].team==1)||tank[0].no==50)
                                    tank[offset].live--;
                                if(tank[offset].live==-1)
                                    tank[offset].no=0;
                            } 
                        }   
                    }
                }
                bullet[i].launch=0; 
                
                //if(tank[0].no==50)printf("                   沒打到 有經過嗎?\n");
                if(tank->no==50&&bullet[3].launch==0)trace=0;//只有編號50能存取trace變數 
                goto label1;//打到後,也回去跟著戰車 
            }
        }
        else //沒發射就一直跟著戰車 
        {
label1: 
            if(tank->direction==0){bullet[i].direction=0;bullet[i].x=(*tank).x-13;bullet[i].y=(*tank).y-24;}
            if(tank->direction==1){bullet[i].direction=1;bullet[i].x=(*tank).x-13;bullet[i].y=(*tank).y-2;}
            if(tank->direction==2){bullet[i].direction=2;bullet[i].x=(*tank).x-23;bullet[i].y=(*tank).y-12;}
            if(tank->direction==3){bullet[i].direction=3;bullet[i].x=(*tank).x-2;bullet[i].y=(*tank).y-12;}

        }
    } 
    //道具部分
    if(tank[0].no==50)
    {    //加血 
        if(prop[0].launch==1)
        {
            if((((prop[0].x-26)<(tank[0].x-26))&&((prop[0].x+26)>(tank[0].x-26)))&&(((prop[0].y-26)<(tank[0].y-26))&&((prop[0].y+26)>(tank[0].y-26))))
            {
                PlaySound("getprop.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                tank[0].live+=5;
                prop[0].launch=0;        
            }
        }
        //兵帽 
        if(prop[1].launch==1)
        {
            if((((prop[1].x-26)<(tank[0].x-26))&&((prop[1].x+26)>(tank[0].x-26)))&&(((prop[1].y-26)<(tank[0].y-26))&&((prop[1].y+26)>(tank[0].y-26))))
            {
                PlaySound("getprop.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                tank[0].effect=9;
                prop[1].launch=0;        
            }
        }
    
        if(tank[0].effect==9&&edgetrigger(&lsttm,50)==1)
        {
            tank[0].effect=0;     
            shield[0].launch=1;
        }
    
        else if(tank[0].effect==9)
        {
            tank[0].effect=9;
            setshield(shield,tank[0].x-13,tank[0].y-13);
        }
    //打爆鋼牆 
        if(prop[2].launch==1)
        {
            if((((prop[2].x-26)<(tank[0].x-26))&&((prop[2].x+26)>(tank[0].x-26)))&&(((prop[2].y-26)<(tank[0].y-26))&&((prop[2].y+26)>(tank[0].y-26))))
            {
                PlaySound("getprop.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                tank[0].bv=7;
                prop[2].launch=0;        
            }
        }
        //鏟子 
        if(prop[3].launch!=0)
        {
            if(prop[3].launch==2&&edgetrigger(&lsttm,40)==1 )
            {
                map[26][15]=1;map[25][15]=1;map[24][15]=1;map[24][14]=1;map[24][13]=1;map[24][12]=1;map[25][12]=1;map[26][12]=1;
                setactaulbarrier(12,26,1);
                setactaulbarrier(12,25,1);
                setactaulbarrier(12,24,1);
                setactaulbarrier(13,24,1);
                setactaulbarrier(14,24,1);
                setactaulbarrier(15,24,1);
                setactaulbarrier(15,25,1);
                setactaulbarrier(15,26,1);
                prop[3].launch=0; 
            }
            else if(prop[3].launch==1&&(((prop[3].x-26)<(tank[0].x-26))&&((prop[3].x+26)>(tank[0].x-26)))&&(((prop[3].y-26)<(tank[0].y-26))&&((prop[3].y+26)>(tank[0].y-26))))
            {
                PlaySound("getprop.wav",NULL,SND_SYNC|SND_FILENAME|SND_ASYNC);
                map[26][15]=3;map[25][15]=3;map[24][15]=3;map[24][14]=3;map[24][13]=3;map[24][12]=3;map[25][12]=3;map[26][12]=3;
                setactaulbarrier(12,26,3);
                setactaulbarrier(12,25,3);
                setactaulbarrier(12,24,3);
                setactaulbarrier(13,24,3);
                setactaulbarrier(14,24,3);
                setactaulbarrier(15,24,3);
                setactaulbarrier(15,25,3);
                setactaulbarrier(15,26,3);
                prop[3].launch=2; 
            }
        }
    }
}


void tankbarrier(item tank)
{
    int y1,x1,x2,y2;
    y1=(tank.y)-29;
    x1=(tank.x)-29;
    y2=(tank.y+3);
    x2=(tank.x+3);
    if(tank.no==0)//死了變0 
    {
        for(int j=y1;j<=y2;j++)
        {
            for(int k=x1;k<=x2;k++)
            {
            body[k][j]=0;
            }
        }
    }
    else{
    
       
    for(int j=(y1+4);j<=(y2-4);j++)
    {
        for(int k=(x1+4);k<=(x2-4);k++)
        {
            if(k<448&&j<448)body[k][j]=tank.no;
        }
    }
    
    if(tank.direction==0)//上 
    {
        for(int j=(y1+4);j<=(y1+6);j++)
        {
            for(int k=(x1+14);k<=(x2-14);k++)
            {   
                body[k][j]=0;
            }
        }
        for(int j=(y2-4);j<=(y2+2);j++)
        {
            for(int k=(x1+4);k<=(x2-4);k++)
            {   
                body[k][j]=0;
            }
        } 
    }
    if(tank.direction==1)//下 
    {
        for(int j=(y2-6);j<=(y2-4);j++)
        {
            for(int k=(x1+14);k<=(x2-14);k++)
            {
                body[k][j]=0;
            }
        }
        for(int j=(y1-1);j<=(y1+4);j++)
        {
            for(int k=(x1+4);k<=(x2-4);k++)
            {
                body[k][j]=0;
            }
        }
    }
    if(tank.direction==3)//右 
    {    
        for(int j=(y1+16);j<=(y2-14);j++)
        {
            for(int k=(x2-6);k<=(x2-4);k++)
            {
                body[k][j]=0;
            }
        }     
        for(int j=(y1+4);j<=(y2-4);j++)
        {
            for(int k=(x1-1);k<=(x1+3);k++)
            {
                body[k][j]=0;
            }
        } 
    }
    if(tank.direction==2)//左 
    {
        for(int j=(y1+16);j<=(y2-14);j++)
        {
            for(int k=(x1+4);k<=(x1+7);k++)
            {
                body[k][j]=0;
            }
        }
        for(int j=(y1+4);j<=(y2-4);j++)
        {
            for(int k=(x2-6);k<=(x2+1);k++)
            {
                body[k][j]=0;
            }
        }
    }
    //if(tank.direction==0){body[(tank).x-13][(tank).y-24]=4;}
    //if(tank.direction==1){body[(tank).x-13][(tank).y-2]=4;}
    //if(tank.direction==2){body[(tank).x-23][(tank).y-12]=4;}
    //if(tank.direction==3){body[(tank).x-2][(tank).y-12]=4;}

    
    }
    
}


int main()
{
    //宣告戰車 
    item tank={{"tank1_u.gif","tank2_u.gif",
                        "tank1_d.gif","tank2_d.gif",
                            "tank1_l.gif","tank2_l.gif",
                                "tank1_r.gif","tank2_r.gif",
                                 "tankbu1.gif","tankbu2.gif",
                                 "tankbd1.gif","tankbd2.gif",
                                 "tankbl1.gif","tankbl2.gif",
                                 "tankbr1.gif","tankbr2.gif",},250,100,0,1,50,1,USERLIVE,0,6};
                                    
                        
                                    
    item tanka[N];
    item tankb={{"atank1_u.gif","atank2_u.gif",
                        "atank1_d.gif","atank2_d.gif",
                            "atank1_l.gif","atank2_l.gif",
                                "atank1_r.gif","atank2_r.gif"
                                    },200,200,1,1,50,1,0,0,6};

                 
   
   
   
   
   

    
    //宣告子彈 
    position bullet={436,436,0,0};
    position bulleta[N][N_ofbullet];
    for(int i=0;i<N;i++)
        for(int j=0;j<N_ofbullet;j++)
            bulleta[i][j]=bullet;
    //宣告爆炸 
    item explodetemplate={{"exp0.gif","exp1.gif","exp2.gif","exp3.gif",},200,200,72,4};
    item explode[N];
    for(int i=0;i<N;i++)
        explode[i]=explodetemplate;    
    item startemplate={{"star0.gif","star1.gif","star2.gif","star3.gif","star4.gif","star5.gif","star6.gif","star7.gif"},200,200,72,4};
    item star[N];
    for(int i=0;i<N;i++)
        star[i]=startemplate;
    item appeartemplate={{"appear0.gif","appear1.gif","appear2.gif","appear3.gif","appear4.gif","appear5.gif","appear6.gif","appear7.gif","appear8.gif","appear9.gif","appear10.gif","appear11.gif"},200,200,72,4};
    item appear[N];
    for(int i=0;i<N;i++)
        appear[i]=appeartemplate;
    item shield={{"shield0.gif","shield1.gif"},200,200,0,4};
    item prop[4]={{"prop0.gif",0,0,0,0},{"prop1.gif",0,0,0,0},{"prop2.gif",0,0,0,0},{"prop3.gif",0,0,0,0}};


    int lastshowtime;
    initwindow(464+32,416+32);
    int choice=0;
    int (*comfunc[13])(item*,int,item)={comctrl1,comctrl2,comctrl3,comctrl4,comctrl5,comctrl6,comctrl7,comctrl8,comctrl9,comctrl10,comctrl11,comctrl12,comctrl13};

    
    readimagefile("image.gif",416+16,0+16,448+16,112+16);
    int size = imagesize(0,0,16,16);
    grassbitmap = malloc(size);
    steelbitmap = malloc(size);
    seabitmap0 = malloc(size);
    seabitmap1 = malloc(size);
    brickbitmap = malloc(size);
    enemymarkbitmap = malloc(size);
    flagbitmap= malloc(imagesize(0,0,32,32));
    livebitmap= malloc(imagesize(0,0,32,32));

    getimage(432,16,447,31,grassbitmap);
    getimage(448,16,463,31,steelbitmap);
    getimage(432,32,447,47,seabitmap0);
    getimage(448,32,463,47,seabitmap1);
    getimage(432,48,447,63,brickbitmap);
    getimage(448,48,463,63,enemymarkbitmap);
    getimage(432,64,464,95,flagbitmap);
    getimage(432,96,464,127,livebitmap);
    
    
    
head:
    //遊戲重新開始 
    tanka[0]=tank;
    stageno=0;
    while(1)
    {
        setfillstyle(1,BLACK);bar(416+16,0+16,449+16,113+16);
        readimagefile("cover.gif",60,86,436,206+16);
        if(GetKeyState(VK_UP)<0)choice=0;
        if(GetKeyState(VK_DOWN)<0)choice=1;
        if(GetKeyState(VK_DOWN)<0)choice=1;
        if(GetKeyState(27)<0)return 0;
        
        if(choice==0)readimagefile("choice0.gif",123,260+16,373,326+16);
        if(choice==1)readimagefile("choice1.gif",123,260+16,373,326+16);
        outtextxy(123,345,"control：z x c v ↑↓← →");
        outtextxy(123,360,"z：normal");
        outtextxy(123,375,"x：remotecontrol");
        outtextxy(123,390,"c：circular");
        outtextxy(123,405,"v：missile");

        if(GetKeyState(13)<0)
        {
            if (choice==0)break;
            if (choice==1)system("tankmapeditor.exe");
        }
        delay(80);
    }
    

nextstage:
    for(int i=0;i<447;i++)
        for(int j=0;j<447;j++)
            body[i][j]=0;
    int liveandonf=0;    
    int onfield=2;
    readmap();
    for(int i=1;i<N;i++)
    {
        
        tanka[i]=tankb;
        tanka[i].no=50+i;
        tanka[i].team=0;
        tanka[i].y=46;
        tanka[i].live=0;
        
        if(i%3==0)tanka[i].x=46;
        else if(i%3==1)tanka[i].x=239;
        else if(i%3==2)tanka[i].x=426;
    }
    //每關兩個道具 
    tanka[0].y=431;
    tanka[0].x=173;
    int tmp=1+rand()%10;
    tanka[tmp].live=1;
    tanka[tmp].effect=1+rand()%4; 
    tmp=11+rand()%10;
    tanka[tmp].live=1;
    tanka[tmp].effect=1+rand()%4; 
    tmp=21+rand()%5;
    tanka[tmp].live=1;
    tanka[tmp].effect=1+rand()%4; 
    
    animation(tanka,bulleta,explode,star,onfield,appear,&shield,prop);
        
PlaySound("beginning.wav", NULL,SND_FILENAME|SND_ASYNC);

    delay(2500);

    srand(time(NULL));
    while(1)
    {    
        for(int j=(0+1);j<(26+1);j++)
        {
            for(int k=(0+1);k<(26+1);k++)
            {

                if(map[j][k]==1)
                {
                    setactaulbarrier(k,j,1);
                }
                if(map[j][k]==2)
                {
                    setactaulbarrier(k,j,0);
                }
                if(map[j][k]==3)
                {
                    setactaulbarrier(k,j,3);
                }
                if(map[j][k]==4)
                {   
                    setactaulbarrier(k,j,4);
                }
            }
        }
        animation(tanka,bulleta,explode,star,onfield,appear,&shield,prop);
        // 老鷹本體 
        setactaulbarrier(12+1,25+1,50);setactaulbarrier(13+1,25+1,50);
        setactaulbarrier(12+1,24+1,50);setactaulbarrier(13+1,24+1,50);
  
        if(stageno==10)
        {     
            //printf("%d\n",stageno) ;   
            onfield=N-1-7;
            for(int i=1;i<N;i++)
            {
                tanka[i]=tankb;
                tanka[i].no=50+i;
                tanka[i].team=0;
                tanka[i].live=0;
                stageno=-2;//最後一關 
            }
        }
        //場上只能出現2隻 間隔3秒
        else 
        {
            if(liveandonf<ALLOWONFIELD&&onfield<N&&edgetrigger(&lastshowtime,3)==1)
            {    
                setappear(&appear[onfield],tanka[onfield].x,tanka[onfield].y);
                onfield++;
                
            }
        }
        
        liveandonf=0;
        for(int i=1;i<onfield;i++)
        {
            if(tanka[i].no!=0)liveandonf++;
            //printf("%d %d\n",stageno,liveandonf);
            
        }
        //printf("%d %d \n",liveandonf,onfield);
        if(stageno==-1)break;
        if(liveandonf==0&&onfield==N)//判斷贏 
        {
            stageno++;
            goto nextstage;
        } 
        
        if(tanka[0].no!=0)
        {
            itemctrl(&tanka[0],keyboard(bulleta[0]),bulleta[0],&explode[0],&star[0],onfield,&shield,prop);
            tankbarrier(tanka[0]);
        }
        for(int i=1;i<onfield;i++)
        {
            if(tanka[i].no!=0)
            {
                itemctrl(&tanka[i],(*comfunc[rand()%13])(&tanka[i],8+rand()%8,tanka[0]),bulleta[i],&explode[i],&star[i],onfield,&shield,prop);
            }
            tankbarrier(tanka[i]);
        }
        //輸了 
        if(tanka[0].no==0)
        {
            break;
        }



    }
    

    if(stageno==-1)
    {
        readimagefile("win.gif",0,0,480,432); 
        delay(3000);
        goto head;
    }
    else 
    {
        animation(tanka,bulleta,explode,star,onfield,appear,&shield,prop);
        PlaySound("explode.wav",NULL,SND_ASYNC|SND_FILENAME);
        delay(500);
        PlaySound("gameover.wav",NULL,SND_ASYNC|SND_FILENAME);
        delay(3000);
        goto head;
    }
    
    closegraph();
}
