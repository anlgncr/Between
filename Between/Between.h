#ifndef Between_h
#define Between_h

#define LINEAR 0
#define EASE_IN 1
#define EASE_OUT 2
#define EASE_IN_BOUNCE 3
#define EASE_OUT_BOUNCE 4
#define EASE_IN_BACK 5
#define EASE_OUT_BACK 6
#define EASE_IN_ELASTIC 7 
#define EASE_OUT_ELASTIC 8
#define EASE_IN_OUT 9
#define EASE_OUT_IN 10
#define EASE_OUT_IN_ELASTIC 11 
#define EASE_IN_OUT_ELASTIC 12

#define NUMBER_OF_FUNCTION 12
#define FOREVER 0
#define FINISHED 1

class Between{
	public:
		Between(uint16_t);
		struct Tween{
			int* variable;
			int init_value;
			int end_value;
			uint32_t total_time;
			bool repeat;
			bool reverse;
			uint8_t anim;
			uint16_t repeat_count;
			void (*onFinish)(void);
			void (*onUpdate)(void);
			void (*onRepeat)(void);
			bool isFinished;
			uint32_t init_time;
			bool period;
		};
		
		/*	Tweens dizisindeki indeksin bilgisini döndürür	*/
		uint16_t getIndex();
		
		Tween* create(int*, int, int, uint32_t, uint8_t);
		
		/*	Tweens dizisine, indeksin gösterdiği adrese tween ekler	*/
		bool add(Tween*);
		
		/*	Tweens dizisinden tween'i siler	*/
		bool remove(Tween*);
		
		/*	Tweens dizisindeki tweenleri zamana göre günceller*/
		bool update();
		
	private:
		/* 	Tween yapısının göstergesine gösterge. Her bir gösterge 2byte
			Tweenlerin adreslerini tutan tek boyutlu dizi	*/
		Tween** tweens; 
		
		/*	Maksimum kaç tane tween göstergesinin olacağını tutan değişken */
		uint16_t tween_length = 0;
		
		/*	Tweens dizisindeki tweenlerin son indeksini gösterir, 
			tweenler silindikçe	bu değer azalır eklendikçe artar	*/
		uint16_t last_index = 0;
		
		
		/*	Tweens dizisindeki tweenlerden çalışması bitenlerin veya silinenlerin 
			(NULL göstergelerin) oluşturduğu boşlukları kaldırır yeni tweenler için yer açar */
		void dispose();
		
		void repeat(uint16_t);
		void finish(uint16_t);
		
		/*	float parametreli ve float döndüren geçiş fonksiyonları için gösterge tanımlaması*/
		typedef float (Between::*tweenFunction)(float);
		tweenFunction tFunction;
		
		float easeCombination(tweenFunction, tweenFunction, float);
		float easeOutInElastic(float);
		float easeInOutElastic(float);
		float easeInBounce(float);
		float easeOutIn(float);
		float easeInOut(float);
		float easeOutElastic(float oran);
		float easeOutBack(float);
		float easeInBack(float);
		float easeIn(float);
		float easeOut(float);
		float easeInElastic(float);
		float easeOutBounce(float);
		float linear(float);
	
		/*Geçiş fonksiyonlarının adreslerini tutan dizi*/
		const tweenFunction tFunctions[13] = {
			&Between::linear
			,&Between::easeIn
			,&Between::easeOut
			,&Between::easeInBounce
			,&Between::easeOutBounce
			,&Between::easeInBack
			,&Between::easeOutBack
			,&Between::easeInElastic
			,&Between::easeOutElastic
			,&Between::easeInOut
			,&Between::easeOutIn
			,&Between::easeInOutElastic
			,&Between::easeOutInElastic
		};
};
#endif