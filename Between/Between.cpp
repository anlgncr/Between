#include "arduino.h"
#include "Between.h"

/*
	This variable defines max size of the tween pool
	And it is necessary to use SRAM(2kb) efficiently. Addresses are 2 bytes in arduino
	length => The number of the tweens that you want to use
*/
Between::Between(uint16_t length){
	tween_length = length;
	tweens = (Tween**)calloc(tween_length, sizeof(Tween*));
}

Between::Tween* Between::create(int* variable, int init_value, int end_value, uint32_t total_time, uint8_t anim){
	if(last_index >= tween_length)
		return false;
	
	//If timer register overflows then do not add my_tween
	if((millis() + total_time) < total_time)
		return false;
	
	//If animation is out of borders constrain it
	anim = constrain(anim, 0, NUMBER_OF_FUNCTION);
	
	Tween* my_tween = (Tween*)malloc(sizeof(Tween));
	*my_tween = {
		variable, init_value, end_value, total_time, 
		false, false, anim, 0, NULL, NULL, NULL, false, millis(), false
	};
	
	*variable = init_value;
	tweens[last_index++] = my_tween;
	
	return my_tween;
}

/*
	This method adds the Tween object's address to the pool
	my_tween => The address of my Tween object
*/
bool Between::add(Tween* my_tween){
	if(last_index >= tween_length)
		return false;

	my_tween->init_time = millis();
	
	//If timer register overflows then do not add my_tween
	if((my_tween->init_time + my_tween->total_time) < my_tween->total_time)
		return false;
	
	//Check if your my_tween is already in the pool, if so do not add it
	for(uint16_t i=0; i<last_index; i++)
		if(tweens[i] == my_tween)
			return false;
	
	*(my_tween->variable) = my_tween->init_value;
	my_tween->isFinished = false;
	my_tween->period = false;
	
	//If animation is out of borders constrain it
	my_tween->anim = constrain(my_tween->anim, 0, NUMBER_OF_FUNCTION);
	
	//Add my_tween to the pool
	tweens[last_index++] = my_tween;
	
	return true;
}

/*
	This method removes my_tween from pool
	my_tween => The address of my Tween object
*/
bool Between::remove(Tween* my_tween){
	if(last_index <= 0)
		return false;
	
	for(uint16_t i=0; i<last_index; i++){
		if(tweens[i] == my_tween){
			tweens[i] = NULL;
			dispose();
			return true;
		}
	}
	return false;
}

/*
	This method reorders the pool it disposes null pointers
*/
void Between::dispose(){
	unsigned int current_index = 0;
	
	for(uint16_t i=0; i<tween_length; i++){
		if(tweens[i] != NULL){
			if(current_index != i){
				tweens[current_index] = tweens[i];
				tweens[i] = NULL;
			}
			current_index++;
		}
	}
	last_index = current_index;
}

/*
	This method updates the library so that all the functions can work
*/
bool Between::update(){
	if(last_index <= 0)
		return false;
	
	uint32_t elapsed_time = 0;
	for(uint16_t i=0; i<last_index; i++)
	{
		if(tweens[i] == NULL){
			dispose();
		}
		else{
			elapsed_time = millis() - tweens[i]->init_time;
			
			if(elapsed_time >= tweens[i]->total_time){
				elapsed_time = tweens[i]->total_time;
				*(tweens[i]->variable) = tweens[i]->end_value;
			
				if(*(tweens[i]->onUpdate))
					(*(tweens[i]->onUpdate))();
				
				if(tweens[i]->reverse)
				{
					int temp = tweens[i]->init_value;
					tweens[i]->init_value = tweens[i]->end_value;
					tweens[i]->end_value = temp;
					tweens[i]->init_time = millis();
					
					if(!tweens[i]->repeat)
						tweens[i]->reverse = false;	
					
					if(tweens[i]->period)
						repeat(i);
					
					tweens[i]->period = !tweens[i]->period;	
				}
				else if(tweens[i]->repeat){
					repeat(i);
				}
				else{
					finish(i);
				}
			}
			else{
				this->tFunction = tFunctions[tweens[i]->anim];
				int value = tweens[i]->end_value - tweens[i]->init_value;
				float ratio = (float)elapsed_time / (float)tweens[i]->total_time;
				*(tweens[i]->variable) = tweens[i]->init_value + round(value * (this->*tFunction)(ratio));
				
				if(*(tweens[i]->onUpdate))
					(*(tweens[i]->onUpdate))();
			}
		}
	}
	return true;
}

void Between::repeat(uint16_t i){
	if(tweens[i]->repeat_count != FINISHED){
		if(*(tweens[i]->onRepeat))
			(*(tweens[i]->onRepeat))();
	}
	
	if(tweens[i]->repeat_count > 1){
		tweens[i]->init_time = millis();
		tweens[i]->repeat_count--;
	}
	else if(tweens[i]->repeat_count == FOREVER){
		tweens[i]->init_time = millis();
	}
	else if(tweens[i]->repeat_count == FINISHED){
		finish(i);
	}
}

void Between::finish(uint16_t i){
	tweens[i]->isFinished = true;
					
	if(*(tweens[i]->onFinish))
		(*(tweens[i]->onFinish))();
	
	remove(tweens[i]);
}

unsigned int Between::getIndex(){
	return last_index;
}


float Between::linear(float ratio){
	return ratio;
}

float Between::easeIn(float ratio){
  return ratio * ratio * ratio;
}

float Between::easeOut(float ratio){
  float invRatio = ratio - 1;
  return invRatio * invRatio * invRatio +1; 
}

float Between::easeInElastic(float ratio){
	if(ratio == 1 || ratio ==0)
		return ratio;
	else{
		float p = 0.3;
		float s = p/4.0;
		float invRatio = ratio - 1;
		return -1.0 * pow(2.0, 10.0 * invRatio) * sin((invRatio-s)*(2.0* 3.14)/p);
	}
}

float Between::easeOutBounce(float ratio){
	float s= 7.5625;
	float p= 2.75;
	float l;
	if (ratio < (1.0/p)){
		l = s * pow(ratio, 2);
	}
	else{
		if (ratio < (2.0/p)){
			ratio -= 1.5/p;
			l = s * pow(ratio, 2) + 0.75;
		}
		else{
			if (ratio < 2.5/p){
				ratio -= 2.25/p;
				l = s * pow(ratio, 2) + 0.9375;
			}
			else{
				ratio -= 2.625/p;
				l =  s * pow(ratio, 2) + 0.984375;
			}
		}
	}
	return l;
}

float Between::easeInBack(float ratio){
	float s = 1.70158;
	return pow(ratio, 2) * ((s + 1.0)*ratio - s);
}
		
float Between::easeOutBack(float ratio){
	float invRatio = ratio - 1.0;            
	float s = 1.70158;
	return pow(invRatio, 2) * ((s + 1.0)*invRatio + s) + 1.0;
}

float Between::easeOutElastic(float ratio){
	if (ratio == 0 || ratio == 1) return ratio;
	else{
		float p = 0.3;
		float s = p/4.0;                
		return pow(2.0, -10.0*ratio) * sin((ratio-s)*(2.0*3.14)/p) + 1;                
	}            
}      	

float Between::easeInOut(float ratio){
	return easeCombination(&Between::easeIn, &Between::easeOut, ratio);
} 

float Between::easeOutIn(float ratio){
	return easeCombination(&Between::easeOut, &Between::easeIn, ratio);
} 

float Between::easeInBounce(float ratio){
	return 1.0 - easeOutBounce(1.0 - ratio);
}

float Between::easeInOutElastic(float ratio){
	return easeCombination(&Between::easeInElastic, &Between::easeOutElastic, ratio);
}   

float Between::easeOutInElastic(float ratio){
	return easeCombination(&Between::easeOutElastic, &Between::easeInElastic, ratio);
}

float Between::easeCombination(tweenFunction init_function, tweenFunction end_function, float ratio){
	if (ratio < 0.5) {
		return 0.5 * (this->*init_function)(ratio * 2.0);
	}
	else {
		return 0.5 * (this->*end_function)((ratio - 0.5) * 2.0) + 0.5;
	}	
}







