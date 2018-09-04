#include <Between.h>
#define INIT_VALUE 0
#define END_VALUE 10
#define TOTAL_TIME 1000 //ms

Between animation(1);
Between::Tween *my_tween;
int my_var;

void onFinished(){Serial.println("My tween is finished");}
void onRepeated(){Serial.println("My tween is repeated");}
void onUpdated(){Serial.println(my_var);}

void setup() {
  Serial.begin(9600);
  my_tween = animation.create(&my_var, INIT_VALUE, END_VALUE, TOTAL_TIME, LINEAR);
  my_tween->onFinish = onFinished;
  my_tween->onUpdate = onUpdated;
  my_tween->onRepeat = onRepeated;
  my_tween->repeat = true;
  my_tween->repeat_count = 5;
}

void loop() {
    animation.update();
}
