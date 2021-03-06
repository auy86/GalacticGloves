#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <list>
#include <unistd.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <string.h>
#include <errno.h>
#include <curses.h>
#include <ctime>

#include "rpi-rgb-led-matrix-master/include/led-matrix.h"
#include "bullet.h"
#include "gunner.h"
#include "monster.h"
#include "draw.h"
#include "text-example.cc"

using namespace std;
using std::list;
using rgb_matrix::Canvas;

int MAX_SECONDS = 30;

char get_arduino_command(int fd)
{
  int char_in;
  char arduino_command;
  
  /*while(true) {
    char_in = serialGetchar(fd);
    if(char_in != 10) break;
  }*/
  arduino_command = char(serialGetchar(fd)); //char(char_in);
  return arduino_command;
}

bool Collides(Monster m, Bullet b) {
  if (m.color != b.color) return false;
  int* mx = m.xpos;
  int* my = m.ypos;
  int bx = b.x;
  int by = b.y;
  for (int i = 0; i < m.size; i++) {
    if (mx[i] == bx && my[i] == by) return true;
  }
  return false;
}

int main(int argc, char *argv[]) {

  // random seed
  srand(time(NULL));


  //TEMPORARY: keyboard input
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);

  //Setup Arduino-RPi interface
  int fd; //Run dmesg and search for Arduino to find port
  if ((fd = serialOpen ("/dev/ttyACM0", 9600)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: u%s\n",
             strerror (errno)) ;
    return 1 ;
  }

  //wiringpi gpio interface setup
  //a - physical pin 31 - wpi pin 22
  //s - physical pin 33 - wpi pin 23
  //d - physical pin 35 - wpi pin 24
  //f - physical pin 37 - wpi pin 24

  wiringPiSetup () ;
  int pin_a = 22; 
  int pin_b = 23; 
  int pin_c = 24; 
  int pin_d = 25; 

  pinMode(pin_a, INPUT);
  pinMode(pin_b, INPUT);
  pullUpDnControl(pin_b, PUD_OFF); 
  pinMode(pin_c, INPUT);
  pullUpDnControl(pin_c, PUD_OFF); 
  pinMode(pin_d, INPUT);
  pullUpDnControl(pin_d, PUD_OFF); 
  

  //game code
  bool gameover = false;
  int gameCounter = 0;
  int score = 0;

  //monster colors: r,g,b,amber
  int monsterR [4] = {255, 0,   0,   255};
  int monsterG [4] = {0,   255, 0,   200};
  int monsterB [4] = {0,   0,   255, 0};

  //bullet colors: magenta, light green, cyan, yellow
  int bulletR [4] = {255,   50,     0,   255};
  int bulletG [4] = {0,    255,   255,   100};
  int bulletB [4] = {100,   50,   255,   0};

  std::list<Monster> monsters;
  std::list<Bullet> bullets;
  
  Canvas *canvas = setupLED(argc, argv);
  
  int x = 3;
  int y = 2;
  Gunner gunner = Gunner();

  while(!gameover && gameCounter < MAX_SECONDS*25) {
    char arduino_cmd = 0;
    int digital_cmd = 0;

  wiringPiSetup () ;

  int a;
  int b; 
  int c; 
  int d; 
  //a = digitalRead(pin_a); 
  //b = digitalRead(pin_b); 
  //c = digitalRead(pin_c); 
  //d = digitalRead(pin_d); 
   
 // cout << "a" << a << " b " <<  b << " c" << c << " d " << d << "\n"; 
   digital_cmd = 0; 
    if(digitalRead(22) == LOW) digital_cmd = 97; 
    if(digitalRead(23) == LOW) digital_cmd = 115;
    if(digitalRead(24) == LOW) digital_cmd = 100; 
    if(digitalRead(25) == LOW) digital_cmd = 102; 
    //else digital_cmd = 0; 
    cout << digital_cmd << "\n"; 
 
    //arduino_cmd =  get_arduino_command(fd);
    //cout << arduino_cmd << "\n";

    // Keeps track of game loops
    gameCounter++;
   
    int ch = getch();
    switch (ch) {
      case KEY_LEFT: {
        gunner.set_x(-1);
        break;
      }
      case KEY_RIGHT: {
        gunner.set_x(1);
        break;
      }
      case 49: { // 1
        gunner.set_angle(-1);
        break;
      }
      case 50: { // 2
        gunner.set_angle(0);
        break;
      }
      case 51: { // 3
        gunner.set_angle(1);
        break;
      }
    }

    switch(digital_cmd) {
      case 0: break;
      case 97: { // a - red
        Bullet newBullet = Bullet(bulletR[0], bulletG[0], bulletB[0], 0,  gunner.x1, gunner.y1,
                                  gunner.angle, -1);
        bullets.push_front(newBullet);
        break;
      }
      case 115: { // s - green
        Bullet newBullet = Bullet(bulletR[1], bulletG[1], bulletB[1], 1, gunner.x1, gunner.y1,
                                  gunner.angle, -1);
        bullets.push_front(newBullet);
        break;
      }
      case 100: { // d - blue
        Bullet newBullet = Bullet(bulletR[2], bulletG[2], bulletB[2], 2,  gunner.x1, gunner.y1,
                                  gunner.angle, -1);
        bullets.push_front(newBullet);
        break;
      }
      case 102: { // f - yellow
        Bullet newBullet = Bullet(bulletR[3], bulletG[3], bulletB[3], 3,  gunner.x1, gunner.y1,
                                  gunner.angle, -1);
        bullets.push_front(newBullet);
        break;
      }
    }

    int difficulty;
    if (score > 60) difficulty = 20;
    else difficulty = 80 - score;

    if((rand()%difficulty+1) == 7) {
      int rand_col = rand()%4;
      int s = 4;
      int *x = new int[s];
      int *y = new int[s];
      x[0] = rand()%31;
      y[0] = rand()%13;
      x[1] = x[0];
      y[1] = y[0] + 1;
      x[2] = x[0] + 1;
      y[2] = y[0];
      x[3] = x[0] + 1;
      y[3] = y[0] + 1;

      Monster newMonster = Monster(4, x, y, monsterR[rand_col],
                                   monsterG[rand_col], monsterB[rand_col], rand_col, (rand()%2+1)*2-3);
      //Append monster to monster list
      monsters.push_front(newMonster);
    }

    
    if (gameCounter % 10 == 0) {
      //cout << "MOVING ALL BULLETS\n";
      //Move all Bullets & remove all bullets that are out of range
      for(std::list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); it++) {
        it->move();  //Move bullet
      }

      //Remove if out of range
      bullets.remove_if([](Bullet bullet){ return (bullet.get_x() < 0 || bullet.get_x() >= 35 || 
                                                   bullet.get_y() < 0 || bullet.get_y() >=16); });
    }
    
    //cout << "MOVING ALL MONSTERS\n";
    //Move all monsters
    for(std::list<Monster>::iterator it = monsters.begin(); it != monsters.end(); it++) {
      if (gameCounter % (100 / it->speed) == 0) it->move();
      //it->find();
      if (!it->alive) score = score - 3;
    }

    //if(bullet hits monster, kill monster)
    for(std::list<Bullet>::iterator b = bullets.begin(); b != bullets.end(); b++) {
      for(std::list<Monster>::iterator m = monsters.begin(); m != monsters.end(); m++) {
        if (Collides(*m,*b)) {
          m->die();
          b->size = 0;
          score++;
        }
      }
    }
    monsters.remove_if([](Monster m){ return !(m.alive); });
    bullets.remove_if([](Bullet b){ return b.size == 0 ;});

    //Move gunner
    //Create new bullets

    //cout << "DRAW\n";

    DrawOnCanvas(canvas, monsters, gunner, bullets); // draws monster, bullets, gunner
    

    usleep(40000); // 25 FPS
  }

  char buffer [50];
  sprintf(buffer, "%d", score);
  const char* c = buffer;

  endwin(); // TEMPORARY
  displayText(argc, argv, canvas, c);
  usleep(5000000);
  clearLED(canvas);

  cout << "\nSCORE: " << score << "\n\n";
  return 0;
}
