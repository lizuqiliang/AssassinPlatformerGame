#include<allegro.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<math.h>
#define scrx 800
#define scry 600
#define tile_x 32
#define tile_y 32
#define mapheight 30
#define mapwidth 50
#define ennum 10
#define gm_time 180
#define WHITE makecol(255, 255, 255)

using namespace std;

volatile long speed_counter = 0;
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter);

struct map
{
     int stage[mapheight][mapwidth];
     int num;
     void load();
};

struct player
{
    void setup(int,int);
    bool lose(){return hp <= 0;}
    bool dead;
    bool gm(){return gm_counter <= gm_time;}
    bool hidden;
    bool direction;
    bool movement;
    int gm_counter;
    int x;
    int y;
    float xspeed;
    float yspeed;
    int hp;
};

struct enemy
{
    void setup(int,int, int, int);
    bool dead(){return hp <= 0;}
    bool direction;
    bool movement;
    bool hurt;
    float xspeed;
    float yspeed;
    bool kill;
    bool detect;
    int x;
    int y;
    int angle;
    int hp;
};

bool encollide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool collide(int l,int t,int w, int h, int map[][mapwidth]);
bool wincollide(int l,int t,int w, int h, int map[][mapwidth]);
int keyrel(int k);
void gameover();
void gamewin();
void start(int &num);

int main(int argc, char *argv[]){
    
    allegro_init();
    install_keyboard();
    install_mouse(); 
    install_timer();
    set_color_depth(desktop_color_depth());
    install_sound (MIDI_AUTODETECT, MIDI_AUTODETECT , 0); //Needed to play Music
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, scrx, scry, 0, 0);
    LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));
    COLOR_MAP global_light_table;
    srand(time(NULL));
    set_trans_blender(0, 0, 0, 0);
    
    //BITMAP--------------------------------------------------------------------------------------------------
    
    BITMAP *pleft = load_bitmap("sprites/player/pleft.bmp", NULL);
    BITMAP *pright = load_bitmap("sprites/player/pright.bmp", NULL);
    BITMAP *phidden = load_bitmap("sprites/player/phidden.bmp", NULL);
    BITMAP *enright = load_bitmap("sprites/enemy/enright.bmp", NULL);
    BITMAP *enleft = load_bitmap("sprites/enemy/enleft.bmp", NULL);
    BITMAP *enrightdead = load_bitmap("sprites/enemy/enrightdead.bmp", NULL);
    BITMAP *enleftdead = load_bitmap("sprites/enemy/enleftdead.bmp", NULL);
    BITMAP *door = load_bitmap("sprites/map/door.bmp", NULL);
    BITMAP *end = load_bitmap("sprites/map/end.bmp", NULL);
    BITMAP *pic = load_bitmap("sprites/map/pic.bmp", NULL);
    BITMAP *rock = load_bitmap("sprites/map/rock1.bmp", NULL);
    BITMAP *bg = load_bitmap("sprites/map/bg1.bmp", NULL);
    
    BITMAP *buffer = create_bitmap(scrx,scry);
    BITMAP *background = create_bitmap(32*mapwidth,32*mapheight);
    
    //VARIABLES-------------------------------------------------------------------------------------------------
    map map;
    player p;
    enemy en[ennum];
    
    int walk_counter;
    int ani_counter;
    int ani;
    int walk_ani;
    int tilt=0;
    int mapscroll_x;
    int mapscroll_y;
    int jump_num;
    int kill_counter = 0;
    int d1 = 1;
    int flash_counter = 0;
    int startinit = 1;
    int angle;
    bool kill;
    bool dash;
    bool atk;
    bool keyhold = false;
    bool slow;
    bool tackle=false;
    
    int colorchange[4];
    colorchange[0] = 300;
    colorchange[1] = 300;
    colorchange[2] = 300;
    colorchange[3] = 300;
    
    p.hp = 4;
    p.direction = 1;
    p.gm_counter = 500;
    map.num = 0;
    
    bool initialize = true;
    
    while(!key[KEY_ESC])
    {
        if(startinit == 1)
            start(startinit);
        if(initialize == true)
        {
            angle = 0;
            if(map.num == 0)
            {
                p.setup(66,322);
                en[0].setup(483,310,1,0);
                en[1].setup(526,250,0,0);
                en[2].setup(617,320,0,1);
                en[3].setup(587,320,0,1);
                en[4].setup(711,320,1,1);
                en[5].setup(687,320,0,1);
                en[6].setup(647,320,1,1);
                en[7].setup(687,320,0,1);
                en[8].setup(647,320,1,1);
                en[9].setup(687,320,0,1);
                en[10].setup(647,320,1,1);
            }
            map.load();
            draw_gouraud_sprite(background, bg, 0, 0,250,250,0,0);
            for (int i=0;i<mapheight;i++)
            {
                for (int j=0;j<mapwidth;j++)
                { 
                    if (map.stage[i][j] == 1)
                        draw_gouraud_sprite(background, rock, j*tile_x, i*tile_y,200,200,0,0);
                    else if (map.stage[i][j] == 2)
                        draw_gouraud_sprite(background, door, j*tile_x, i*tile_y,200,200,200,0);
                    else if (map.stage[i][j] == 3)
                        draw_gouraud_sprite(background, pic, j*tile_x, i*tile_y,200,200,0,0);
                    else if (map.stage[i][j] == 4)
                        draw_gouraud_sprite(background, end, j*tile_x, i*tile_y,200,200,0,0);
                }
            }
            initialize = false;
        }
        while(speed_counter > 0)
        {
            mapscroll_x = p.x + pright->w/2 - scrx/2;
            mapscroll_y = p.y + pright->h/2 - scry/2;
            if(mapscroll_x <= 0)
                mapscroll_x = 0;
            if(mapscroll_x >= mapwidth*tile_x-scrx)
                mapscroll_x = mapwidth*tile_x-scrx;
            if(mapscroll_y <= 0)
                mapscroll_y = 0;
            if(mapscroll_y >= mapheight*tile_y-scry)
                mapscroll_y = mapheight*tile_y-scry;
            angle = 0;
           
            if(int(p.xspeed) > 0)
                p.direction = 1;
            else if(int(p.xspeed) < 0)
                p.direction = 0;
            
            if(collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                jump_num = 0;
            if(!p.lose())
            {
                if(key[KEY_X] && tackle == true && p.xspeed <1 && int(p.xspeed) == 0 && collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                {
                    kill = true;
                }
           
                if(kill == true)
                {
                    for(int i=0;i<ennum;i++)
                    {
                        if(en[i].kill==true)
                        {
                            en[i].hurt = true;
                            en[i].hp--;
                            if(en[i].dead())
                            {
                                en[i].y=p.y+4;
                                tackle = false;
                                en[i].kill = false;
                                kill = false;
                            }
                        }
                    }
                }
                
                if(p.gm())
                    p.hidden = true;
                if(!key[KEY_DOWN] && !p.gm())
                    p.hidden = false;
                printf("%d\n", tilt);
                if(dash == false && tackle == false)
                {
              		if(key[KEY_DOWN]) 
            		{
                        p.hidden = true;
            		}
                    if(key[KEY_UP] && jump_num < 2 && keyhold == false && dash == false)
            		{   
                        p.yspeed = -8;
                        jump_num++;
                        keyhold = true;
            		} 
            		if(key[KEY_LEFT]) 
            		{
                        if(p.xspeed>=-6)
                            p.xspeed-=0.75;
            		}
                	if(key[KEY_RIGHT])
            		{
                        if(p.xspeed<=6)
                            p.xspeed+=0.75;
            		}
            		if(key[KEY_Z] && tilt == 0)
            		{
                        p.yspeed = -7;
                        if(p.direction == 1)
                            p.xspeed = 22;
                        else if(p.direction == 0)
                            p.xspeed = -22;
                        dash = true;
                    }
                    if(!key[KEY_UP])
                        keyhold = false;
                }
      		    //MOVEMENTS---------------------------------------------------------------------------------------------------------------------------------------
        		for(int i = 0;i<abs(int(p.yspeed));i++)
    		    {
                    if(p.yspeed>=0)
                    {
                        if(!collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                            p.y++;
                        else
                            break;
                    }
                    if(p.yspeed<=0)
                    {
                        if(!collide(p.x,p.y-1,pright->w, pright->h,map.stage))
                            p.y--;
                        else
                        {
                            p.yspeed = 0;
                            break;
                        }
                    }
                }
                
                for(int i = 0;i<abs(int(p.xspeed));i++)
    		    {
                    if(p.xspeed>=0)
                    {
                        if(!collide(p.x+1,p.y,pright->w, pright->h,map.stage))
                        {
                            p.x++;
                        }
                        else break;
                    }
                    if(p.xspeed<=0)
                    {
                        if(!collide(p.x-1,p.y,pright->w, pright->h,map.stage))
                        {
                            p.x--;
                        }
                        else break;
                    }
                }
                
                //DASH------------------------------------------------------------------------------------------------------------------------------------------
                if(dash == true && collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                {
                    if(int(p.xspeed) == 0)
                        dash = false;
                    else
                    {
                        if(p.xspeed > 0)
                            p.xspeed--;
                        else if (p.xspeed < 0)
                            p.xspeed++;
                    }
                }
                for(int i = 0;i<ennum;i++)
                {
                    if(!en[i].dead())
                    {
                        if(dash == true && tackle == false &&  encollide(p.x, p.y, pright->w, pright->h, en[i].x, en[i].y, enright->w, enright->w))
                        {
                            en[i].kill = true;
                            tackle = true;
                        }
                    }
                }
                if (tackle == true || dash == true)
                {
                    if(p.direction == 1 && tilt <= 62)
                    {
                        tilt +=2;
                    }
                    else if(p.direction == 0 && tilt >= -62)
                    {
                        tilt -=2;
                    }
                }
                else
                {
                    for(int i =0;i<3;i++)
                    {
                        if(tilt >0)
                            tilt --;
                        else if(tilt <0)
                            tilt ++;
                        else break;
                    }
                }
                angle = itofix(tilt);
    
                //MOVEMENT COLLISION/GRAVITY/WALL------------------------------------------------------------------------------------------------------------------------------
        		if(collide(p.x+1,p.y,pright->w, pright->h,map.stage) && p.xspeed > 0)
                    p.xspeed = 0;
                else if(collide(p.x-1,p.y,pright->w, pright->h,map.stage) && p.xspeed < 0)
                    p.xspeed = 0;
        		
        		if(!collide(p.x,p.y+1,pright->w, pright->h,map.stage))
        		    p.yspeed+=0.5;
    		    else
    		        p.yspeed = 0;
          
          		if(p.xspeed > 0)
        		    p.xspeed -=0.15;
    		    else if (p.xspeed < 0)
    		        p.xspeed +=0.15;
    		        
		        if(wincollide(p.x,p.y,pright->w, pright->h,map.stage))
		        {
    		        initialize = true;
    	            map.num++;
                }
    		    //ENEMY COLLISION--------------------------------------------------------------------------------------------------------------------------------
    		    for(int i = 0;i<ennum;i++)
    		    {
                    if(!en[i].dead())
                    {
                        if(encollide(p.x, p.y, pright->w, pright->h, en[i].x, en[i].y, enright->w, enright->h) && p.gm() == false && dash == false && en[i].kill == false && (tilt == 64 || tilt == -64 || tilt == 0))
                        {
                            p.dead = true;
                            p.hp--;
                            for(int j = 0;j<ennum;j++)
                            {
                                if (en[j].kill == true)
                                {
                                    en[j].kill = false;
                                    en[j].hurt = false;
                                    tackle = false;
                                    kill = false;
                                }
                            }
                            if(!p.lose())
                            {
                                p.gm_counter = 0;
                                if(p.direction == 1)
                                    p.xspeed=-10;
                                else
                                    p.xspeed=10;
                                p.yspeed=-5;
                            }
                            else 
                            {
                                return 0;
                            }
                        }  
                    }
                }
            }
		        
            //ENEMY SHTUUFFFF-------------------------------------------------------------------------------------------------------------------------------------
    		
    		
    		for(int i = 0;i<ennum;i++)
    		{
                if(!en[i].dead())
                {
                    if (en[i].kill == false)
                    {    
                        if(p.gm_counter>gm_time)
                        {
                            if(en[i].direction == 0)
                            {
                                for(int j = 0;j<500;j++)
                                {
                                    if(!collide(en[i].x-j,en[i].y,enright->w, enright->h,map.stage))
                                        continue;
                                    else 
                                    {
                                        d1=j;
                                        break;
                                    }
                                }
                                for(int j = 0;j<d1;j++)
                                {
                                    if(!encollide(en[i].x-j,en[i].y,enright->w,enright->h, p.x, p.y, pright->w, pright->h))
                                        continue;
                                    else
                                    {
                                        en[i].detect = true;   
                                        en[i].movement = true;  
                                    } 
                                }
                            }
                            
                            if(en[i].direction == 1)
                            {
                                for(int j = 0;j<500;j++)
                                {
                                    if(!collide(en[i].x+j,en[i].y,enright->w, enright->h,map.stage))
                                        continue;
                                    else 
                                    {
                                        d1=j;
                                        break;
                                    }
                                }
                                for(int j = 0;j<d1;j++)
                                {
                                    if(!encollide(en[i].x+j,en[i].y,enright->w,enright->h, p.x, p.y, pright->w, pright->h))
                                        continue;
                                    else
                                    {
                                        en[i].detect = true;    
                                        en[i].movement = true;  
                                    }       
                                }
                            }
                            
                            if (en[i].detect == true)
                            {
                                if(p.x+pright->w<en[i].x)
                                {
                                    en[i].direction = 0;
                                    en[i].xspeed=-4;
                                }
                                else if(p.x>en[i].x+enright->w)
                                {
                                    en[i].direction = 1;
                                    en[i].xspeed=4;
                                }
                            }
                        }
                        else
                            en[i].detect = false;
                        if(en[i].detect == false && en[i].movement == true)
                        {
                             if (en[i].direction == 1)
                             {
                                 if(!collide(en[i].x+1,en[i].y,enright->w, enright->h,map.stage))
                                     en[i].xspeed = 1;
                                 else if(collide(en[i].x+1,en[i].y,enright->w, enright->h,map.stage))
                                     en[i].direction = 0;
                             }
                             else if (en[i].direction == 0)
                             {
                                 if(!collide(en[i].x-1,en[i].y,enright->w, enright->h,map.stage))
                                     en[i].xspeed = -1;
                                 else if(collide(en[i].x-1,en[i].y,enright->w, enright->h,map.stage))
                                     en[i].direction = 1;
                             }
                        }
                        for(int j = 0;j<abs(int(en[i].yspeed));j++)
            		    {
                            if(en[i].yspeed>=0)
                            {
                                if(!collide(en[i].x,en[i].y+1,enright->w, enright->h,map.stage))
                                    en[i].y++;
                                else
                                    break;
                            }
                            if(en[i].yspeed<=0)
                            {
                                if(!collide(en[i].x,en[i].y-1,enright->w, enright->h,map.stage))
                                    en[i].y--;
                                else
                                {
                                    en[i].yspeed = 0;
                                    break;
                                }
                            }
                        }
                        
                        for(int j = 0;j<abs(int(en[i].xspeed));j++)
            		    {
                            if(en[i].xspeed>=0)
                            {
                                if(!collide(en[i].x+1,en[i].y,enright->w, enright->h,map.stage))
                                {
                                    en[i].x++;
                                }
                                else break;
                            }
                            if(en[i].xspeed<=0)
                            {
                                if(!collide(en[i].x-1,en[i].y,enright->w, enright->h,map.stage))
                                {
                                    en[i].x--;
                                }
                                else break;
                            }
                        }
                  
                		if(!collide(en[i].x,en[i].y+1,enright->w, enright->h,map.stage))
                		    en[i].yspeed+=0.5;
            		    else
            		        en[i].yspeed = 0;
                    }
                    else if(en[i].kill == true)
                    {
                        en[i].x=p.x;
                        en[i].y=p.y+12;
                        en[i].angle = angle;
                    }
                }
            }
            if(int(p.xspeed) != 0)
                walk_counter ++;
            kill_counter++;
            flash_counter++;
            p.gm_counter ++;
  		    speed_counter--;
        }
        if(walk_counter > 5)
            walk_counter = 0;
        if(kill_counter > 1)
            kill_counter = 0;
        if(flash_counter >8)
            flash_counter = 0;
            
        if(int(p.xspeed) != 0)
            walk_ani = int(floor(double(walk_counter/3)))*2;
        else
            walk_ani = 0;
        
        draw_gouraud_sprite(buffer, background, 0-mapscroll_x, 0-mapscroll_y,colorchange[0],colorchange[1],colorchange[2],colorchange[3]);
        for(int i = 0;i<ennum;i++)
        {
            if(!en[i].dead())
            {
                if(en[i].kill == true)
                {
                    if(en[i].hurt == true)
                    {
                        if(kill_counter == 0)
                        {
                            en[i].x += rand()%20-10;
                            en[i].y += rand()%20-10;
                        }
                        else
                        {
                            en[i].x = p.x;
                            en[i].y = p.y+12;
                        }    
                    
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                        line(buffer,en[i].x+(rand() % 50-25)-mapscroll_x,en[i].y-+(rand() % 50-25)-mapscroll_y,en[i].x+enright->w+(rand() % 50-25)-mapscroll_x,en[i].y+enright->h+(rand() % 50-25)-mapscroll_y,makecol(255,0,0));
                    }
                }
                if (en[i].direction == 1)
                    rotate_sprite(buffer,enright,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                else
                    rotate_sprite(buffer,enleft,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                
                if(en[i].hp <= 0)
                    en[i].x=p.x-4;
            }
            else
            {
                if (en[i].direction == 1)
                    rotate_sprite(buffer,enrightdead,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                else
                    rotate_sprite(buffer,enleftdead,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
            }

            if(p.hidden == false && p.gm_counter>gm_time)
            {
                if(p.direction == 1)
                {
                    rotate_sprite(buffer,pright,p.x-mapscroll_x,p.y-mapscroll_y-walk_ani,angle);
                }
                else
                {
                    rotate_sprite(buffer,pleft,p.x-mapscroll_x,p.y-mapscroll_y-walk_ani,angle);
                }
            }
            else if(p.hidden == true && p.gm_counter > gm_time)
                rotate_sprite(buffer, phidden, p.x-mapscroll_x,p.y-mapscroll_y-walk_ani,angle);
                
            if(p.gm_counter <= gm_time)
            {
                if(flash_counter <4)
                    rotate_sprite(buffer, phidden, p.x-mapscroll_x,p.y-mapscroll_y,angle);
            }
        }
        blit(buffer, screen, 0, 0, 0, 0, scrx, scry); // Blit the buffer
		clear(buffer); // Clear the buffer

    }
}END_OF_MAIN();

int keyrel(int k)
{
    static int initialized = 0;
    static int keyp[KEY_MAX];
 
    if(!initialized)
    {
        int i;
        for(i = 0; i < KEY_MAX; i++) keyp[i] = 0;
        initialized = 1;
    }
 
    if(key[k] && !keyp[k])
    {
        keyp[k] = 1;
        return false;
    }
    else if(!key[k] && keyp[k])
    {
        keyp[k] = 0;
        return true;
    }
    return false;
}

bool encollide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    //the code is taken from allegro tutorial and made into a function
    int image1_bb_left = x1;
    int image1_bb_top = y1;
    int image1_bb_right = (image1_bb_left + w1);
    int image1_bb_bottom = (image1_bb_top + h1);
    int image2_bb_left = x2;
    int image2_bb_top = y2;
    int image2_bb_right = (image2_bb_left + w2);
    int image2_bb_bottom = (image2_bb_top + h2);
    if(image1_bb_bottom < image2_bb_top)
        return false;
    else if(image1_bb_top > image2_bb_bottom)
        return false;
    else if(image1_bb_right < image2_bb_left)
        return false;
    else if(image1_bb_left > image2_bb_right)
        return false;
    return(true);
}
END_OF_FUNCTION(collide)

bool collide(int l,int t,int w, int h, int map[][mapwidth])
{  
    int r = (l + w);
    int b = (t + h);
    int px = l/32;
    int py = t/32;
    int px2 = r/32;
    int py2 = b/32;
    if(map[py][px]==1||map[py2][px]==1||map[py][px2]==1||map[py2][px2]==1 || r == 32*mapwidth || l == 0)
        return true;
    return false;
}

bool wincollide(int l,int t,int w, int h, int map[][mapwidth])
{  
    int r = (l + w);
    int b = (t + h);
    int px = l/32;
    int py = t/32;
    int px2 = r/32;
    int py2 = b/32;
    if(map[py][px]==2||map[py2][px]==2||map[py][px2]==2||map[py2][px2]==2 || map[py][px]==4||map[py2][px]==4||map[py][px2]==4||map[py2][px2]==4)
        return true;
    return false;
}

void start(int &num)
{
     BITMAP *buffer3 = create_bitmap(scrx,scry); 
     BITMAP *menu2 = load_bitmap("sprites/map/gamestart.bmp", NULL);
     while(!key[KEY_ENTER])
     {
         while(speed_counter>0)
         {
             speed_counter --;
         }
         draw_sprite(buffer3,menu2,0,0);
         blit(buffer3,screen,0,0,0,0,scrx,scry);
         clear_bitmap(buffer3);
     }
     num = 0;
}

void map::load()
{
    FILE *fptr;
    char name[30];
    sprintf(name,"data/map%d.txt",num);
    fptr = fopen(name, "r"); 
    if (fptr == NULL){ //Error checking                     
        set_gfx_mode(GFX_TEXT,0,0,0,0);
        allegro_message("Could not Open Level!");
        exit(EXIT_FAILURE);
    }   
    else
    { //read from file
        for(int i=0;i<mapheight;i++)
        {
            for(int j=0;j<mapwidth;j++)
            {
                fscanf(fptr,"%d",&stage[i][j]);
            }
        }
    }
    fclose(fptr); 
}
void player::setup(int x1, int y1)
{
     x = x1;
     y = y1;
}

void enemy::setup(int x1,int y1, int d1,int m1)
{
    x=x1;
    y=y1;
    direction = d1;
    hp=25;
    movement = m1;
}
