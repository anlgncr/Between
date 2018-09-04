#include <Between.h>
#define INIT_VALUE 0
#define END_VALUE 10
#define TOTAL_TIME 1000 //ms
#define REPEAT true
#define REPEAT_COUNT 5
#define NO_REVERSE false

Between animation(1);
Between::Tween my_tween = {};
int my_var = 0;

void onFinished(){Serial.println("My tween is finished");}
void onRepeated(){Serial.println("My tween is repeated");}
void onUpdated(){Serial.println(my_var);}

void setup() {
  Serial.begin(9600);
  my_tween = {&my_var, INIT_VALUE, END_VALUE, 
              TOTAL_TIME, REPEAT, NO_REVERSE, LINEAR,
              REPEAT_COUNT, onFinished, onUpdated, onRepeated
  };
  animation.add(&my_tween);
}

void loop() {
    animation.update();
}