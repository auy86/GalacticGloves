#include <iostream>

#include "gunner.h"

using namespace std;

/*
class Gunner {
  public:
    int size;
    int* x;
    int* y;
    int R;
    int G;
    int B;
    int angle;

    Gunner();
    ~Gunner();
    void set_color(int, int, int);
    void set_x(int);
    void set_angle(int);
};
*/

Gunner::Gunner() {
  R = 200;
  G = 200;
  B = 200;
  angle = 0;
  size = 2;
  x0 = 15; //Gunner body
  y0 = 15;
  x1 = 15; //Gunner turret
  y1 = 14;
  //cout << "created new gunner at (" << x0 << "," << y0 << ") with color (" << R << "," << G << "," << B << ")\n\t" \
         // << "gunner angle is: " << angle << "\n";
}

Gunner::~Gunner() {
}

void Gunner::set_color(int R_new, int G_new, int B_new) {
  R = R_new;
  G = G_new;
  B = B_new;
 // cout << "Changed gunner color to: (" << R << "," << G << "," << B << ")\n";
}

//Update the position of the body of the Gunner
// move left if -1, move right if 1
void Gunner::set_x(int dir) {
  int new_x = x0 + dir;
  if(new_x >= 0 && new_x <= 31) {
    x0 = new_x;
    if(new_x == 0 && angle == -1) {
      angle = 0;
    }
    else if(new_x == 31 && angle == 1) {
      angle = 0;
    }
    x1 = new_x + angle;
  }
  //cout << "Moved gunner to (" << x0 << "," << y0 << ") turret is at (" << x1 << "," << y1 << ")\n";
}

//Update the angle to be to the left if -1, straight up if 0,
//and to the right if 1
void Gunner::set_angle(int new_angle) {
  //Check if new angle position is within bounds
  if(x0 == 0 && angle == -1) {
    angle = 0;
  }
  else if(x0 == 31 && angle == 1) {
    angle = 0;
  }
  else {
    angle = new_angle;
  }
  //Update coordinate of turret
  if(angle == -1) {
    x1 = x0 -1;
  }
  else if (angle == 1) {
    x1 = x0 + 1;
  }
  else {
    x1 = x0;
  }
  //cout << "Angle changed, gunner at(" << x0 << "," << y0 <<") turret is at (" << x1 << "," << y1 << ")\n";
}


/*
int main() {
    Gunner g;
    g.set_color(12,3,12);
    cout << "** move left **\n";
    g.set_x(-1);
    cout << "** point turret left **\n";
    g.set_angle(-1);
    cout << "** move right **\n";
    g.set_x(1);
    cout << "** point turret right **\n";
    g.set_angle(1);
    cout << "** point turret front **\n";
    g.set_angle(0);
    cout << "** move nowhere **\n";
    g.set_x(0);
    return 0;
}*/
