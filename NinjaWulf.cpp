//Headers
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

//Define constants
#define scrx 800
#define scry 600
#define tile_x 32
#define tile_y 32
#define mapheight 30
#define mapwidth 50
#define ennum 10
#define gm_time 300
#define enspeed 4
#define WHITE makecol(255, 255, 255)

//Speed Counter
volatile long speed_counter = 0;\
void increment_speed_counter(){
	speed_counter++;
}END_OF_FUNCTION(increment_speed_counter);

//Global variables for initialization and enemy speed increase
bool initialize = true;
int enspd = 0;

//Map loading class
struct map
{
     int stage[mapheight][mapwidth];
     int num;
     void load();
};

//Player Structure
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

//Enemy Strucutre
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

//General Game Functions
bool encollide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
bool collide(int l,int t,int w, int h, int map[][mapwidth]);
bool wincollide(int l,int t,int w, int h, int map[][mapwidth]);
int keyrel(int k);
void gameover();
void gamewin();
void start(int &num);

//Main
int main(int argc, char *argv[]){
    
    //Initializing Allegro
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
    //Map variables
    map map;
    player p;
    enemy en[ennum];
    
    //General Variables
    int walk_counter;
    int ani_counter;
    int ani;
    int walk_ani;
    int tilt;
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
    //Setting the colour variables for the draw function
    int colorchange[4];
    colorchange[0] = 300;
    colorchange[1] = 300;
    colorchange[2] = 300;
    colorchange[3] = 300;
    //Player Variables
    p.hp = 4;
    p.direction = 1;
    p.gm_counter = 500;
    map.num = 0;

    while(!key[KEY_ESC])
    {
        //Initializing each level of the game
        if(startinit == 1)
            start(startinit);
        //Level initializing sequence
        if(initialize == true)
        {
            //Character angle
            angle = 0;
            for(int i=0;i<ennum;i++){
                en[i].hp=25;
            }
            //Map number 1
            if(map.num == 0)
            {
                //Player Setup (xpos,ypos)
                p.setup(66,322);
                //Enemy setup (xpos,ypos,facing direction, moving or not)
                en[0].setup(483,50,1,0);
                en[1].setup(526,50,0,0);
                en[2].setup(617,50,0,1);
                en[3].setup(587,50,0,1);
                en[4].setup(711,320,1,1);
                en[5].setup(687,320,0,1);
                en[6].setup(647,320,1,1);
                en[7].setup(687,320,0,1);
                en[8].setup(647,320,1,1);
                en[9].setup(687,320,0,1);
                en[10].setup(647,320,1,1);
            }
            //Map number 2
            else if(map.num == 1)
            {
                p.setup(60,322);
                en[0].setup(800,320,1,1);
                en[1].setup(711,250,1,1);
                en[2].setup(800,320,0,1);
                en[3].setup(800,320,0,1);
                en[4].setup(800,320,1,1);
                en[5].setup(800,320,0,1);
                en[6].setup(800,320,1,1);
                en[7].setup(687,320,0,1);
                en[8].setup(647,320,1,1);
                en[9].setup(687,320,0,1);
                en[10].setup(647,320,1,1);
            }
            //Map number 3
            else if(map.num == 2)
            {
                p.setup(60,322);
                en[0].setup(711,320,1,1);
                en[1].setup(711,250,1,1);
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
            
            //loading map
            map.load();
            //drawing background
            draw_gouraud_sprite(background, bg, 0, 0,250,250,0,0);
            //Setting up map tiles
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
            //Map scrolling for exploring the map
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
            //Changing player direction when he moves left and right
            if(int(p.xspeed) > 0)
                p.direction = 1;
            else if(int(p.xspeed) < 0)
                p.direction = 0;
            
            //player/map_tile collision
            if(collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                jump_num = 0;
            // If you're still alive
            if(!p.lose())
            {
                //kill skill to kill the enemy
                if(key[KEY_X] && tackle == true && p.xspeed <1 && int(p.xspeed) == 0 && collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                {
                    kill = true;
                }
                //if enemy is being killed, this happens
                if(kill == true)
                {
                    for(int i=0;i<ennum;i++)
                    {
                        if(en[i].kill==true)
                        {
                            //hurts enemy
                            en[i].hurt = true;
                            en[i].hp--;
                            //if enemy is dead, this happens
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
                //godmode
                if(p.gm())
                    p.hidden = true;
                //not hidden if not pressing down
                if(!key[KEY_DOWN] && !p.gm())
                    p.hidden = false;
                printf("%d\n", tilt);
                
                if(dash == false && tackle == false)
                {
                   
            		//jump
                    if(key[KEY_UP] && jump_num < 2 && keyhold == false && dash == false)
            		{   
                        p.yspeed = -8;
                        jump_num++;
                        keyhold = true;
            		} 
            		//moves left
            		if(key[KEY_LEFT]) 
            		{
                        if(p.xspeed>=-6)
                            p.xspeed-=0.75;
            		}
            		//moves right
                	if(key[KEY_RIGHT])
            		{
                        if(p.xspeed<=6)
                            p.xspeed+=0.75;
            		}
            		//tackles enemies
            		if(key[KEY_Z] && tilt == 0)
            		{
                        p.yspeed = -7;
                        if(p.direction == 1)
                            p.xspeed = 22;
                        else if(p.direction == 0)
                            p.xspeed = -22;
                        //Dash mode
                        dash = true;
                    }
                    //prevents flying if you hold up
                    if(!key[KEY_UP])
                        keyhold = false;
                }
      		    //MOVEMENTS---------------------------------------------------------------------------------------------------------------------------------------
        		for(int i = 0;i<abs(int(p.yspeed));i++)
    		    {
                    //vertical movement collision
                    if(p.yspeed>=0)
                    {
                        if(!collide(p.x,p.y+1,pright->w, pright->h,map.stage))
                            p.y++;
                        else
                            break;
                    }
                    //more vertical movement collision
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
                
                //Horizontal Movement Collision
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
                    //Gound friction when the player falls downz
                    else
                    {
                        if(p.xspeed > 0)
                            p.xspeed--;
                        else if (p.xspeed < 0)
                            p.xspeed++;
                    }
                }
                //Enemy functions
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
                //when an enemy is tackled
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
                    //player tilting stuff
                    for(int i =0;i<3;i++)
                    {
                        if(tilt >0)
                            tilt --;
                        else if(tilt <0)
                            tilt ++;
                        else break;
                    }
                }
                //change the angle from an integer to a fixed
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
                        if(p.hidden == false){
                            if(tackle == false){
                                //enemy collision with player
                                if(encollide(p.x, p.y, pright->w, pright->h, en[i].x, en[i].y, enright->w, enright->h) && p.gm() == false && dash == false && en[i].kill == false && (tilt == 64 || tilt == -64 || tilt == 0))
                                {
                                    p.dead = true;
                                    p.hp--;
                                    if (p.hp <=0){ //Error checking                     
                                        set_gfx_mode(GFX_TEXT,0,0,0,0);
                                        allegro_message("You Lose!");
                                        exit(EXIT_FAILURE);
                                    }
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
                                        p.xspeed=-10;
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
                                    //enemy collision with wall
                                    if(!collide(en[i].x-j,en[i].y,enright->w, enright->h,map.stage))
                                        continue;
                                    else 
                                    {
                                        d1=j;
                                        break;
                                    }
                                }
                                //enemy detection function
                                for(int j = 0;j<d1;j++)
                                {
                                    if(!encollide(en[i].x-j,en[i].y,enright->w,enright->h, p.x, p.y, pright->w, pright->h) || p.hidden == true)
                                        continue;
                                    else
                                    {
                                        en[i].detect = true;   
                                        en[i].movement = true;  
                                    } 
                                }
                            }
                            //enemy collision with wall
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
                                //enemy detection 
                                for(int j = 0;j<d1;j++)
                                {
                                    if(!encollide(en[i].x+j,en[i].y,enright->w,enright->h, p.x, p.y, pright->w, pright->h) || p.hidden == true)
                                        continue;
                                    else
                                    {
                                        en[i].detect = true;    
                                        en[i].movement = true;  
                                    }       
                                }
                            }
                            //hidden mode
                            if (p.hidden == true)
                                en[i].detect = false;
                            
                            //what enemy does when he detects player
                            if (en[i].detect == true)
                            {
                                if(p.x+pright->w<en[i].x)
                                {
                                    en[i].direction = 0;
                                    en[i].xspeed=-enspeed-enspd;
                                }
                                else if(p.x>en[i].x+enright->w)
                                {
                                    en[i].direction = 1;
                                    en[i].xspeed=enspeed+enspd;
                                }
                            }
                        }
                        else
                            en[i].detect = false;
                        //when enemy isn't detecting the player
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
                        //enemy ground collision
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
                        //more enemy wall collision
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
            		    //en[i].detect = false;
                    }
                    else if(en[i].kill == true)
                    {
                        en[i].x=p.x;
                        en[i].y=p.y+12;
                        en[i].angle = angle;
                    }
                }
            }
            //counters
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
        //walking animation
        if(int(p.xspeed) != 0)
            walk_ani = int(floor(double(walk_counter/3)))*2;
        else
            walk_ani = 0;
        //drawing background
        draw_gouraud_sprite(buffer, background, 0-mapscroll_x, 0-mapscroll_y,colorchange[0],colorchange[1],colorchange[2],colorchange[3]);
        //this is what happens when you kill the enemy
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
                        //draw red lines to show the stabbing animation
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
                //draw enemies
                if (en[i].direction == 1)
                    rotate_sprite(buffer,enright,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                else
                    rotate_sprite(buffer,enleft,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                //enemy position when he dies
                if(en[i].hp <= 0)
                    en[i].x=p.x-4;
            }
            //draw dead enemies
            else
            {
                if (en[i].direction == 1)
                    rotate_sprite(buffer,enrightdead,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
                else
                    rotate_sprite(buffer,enleftdead,en[i].x-mapscroll_x,en[i].y-mapscroll_y,en[i].angle);
            }
            //draw player when is invincible
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
            //invincibility time
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
//key release function to detect when a key is released
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
//enemy collisionm function with bounty box
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

//map collision between entities and map tiles
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
//door collision when you win
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
//startmenu
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

//map loading function
void map::load()
{
    FILE *fptr;
    char name[30];
    sprintf(name,"data/map%d.txt",num);
    fptr = fopen(name, "r"); 
    if (fptr == NULL){ //Error checking                     
        set_gfx_mode(GFX_TEXT,0,0,0,0);
        allegro_message("You Win!");
        exit(EXIT_FAILURE);
    }   
    else
    { //read from file
        initialize = true;
        enspd+=2;
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
//setting up the player
void player::setup(int x1, int y1)
{
     x = x1;
     y = y1;
}
//setting up the enemy
void enemy::setup(int x1,int y1, int d1,int m1)
{
    x=x1;
    y=y1;
    direction = d1;
    hp=25;
    movement = m1;
}

