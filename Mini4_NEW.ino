#include <Arduino.h>

#define LED_R_COM 22
#define LED_G_COM 23
#define LED_B_COM 24
const int button1Pin = 3;     // ボタン1
const int button2Pin = 2;     // ボタン2
const int ledPin5 = 12;       // LED5_ONOFFモーターと無関係
const int ledPin6 = 13;       // LED6_ONOFFモーターと無関係
//const int dcMotorPin0 = 9;   // DCモーター1のpin１
//const int dcMotorPin1 = 10;  // DCモーター1のpin２
const int dcMotorPin0 = 6;   // DCモーター2のpin１
const int dcMotorPin1 = 11;  // DCモーター2のpin２
uint8_t g_sw1 = 0;	// ボタン1押下SW
uint8_t g_sw2 = 0;	// ボタン2押下SW
// HC-SR04 距離計用の変数
int Trig = 12;
int Echo = 9;
unsigned long time_start = 0;
unsigned long time_total = 0;
unsigned long toggle_count = 0;
unsigned long DurationAverage;  // 応答時間　us
unsigned long DurationAverageHalf;  // 応答時間片道 us
float Distance;  // 距離
unsigned int SpeedOfSound;  // 音速　m/s
float temp_c = 0;  // 摂氏値( ℃ )

void setup()  { 
	// LEDピンを出力
	pinMode(ledPin5, OUTPUT);
	pinMode(ledPin6, OUTPUT);
    led5_off();
    led6_off();
    
	// LEDの色を出力
    pinMode(LED_R_COM, OUTPUT);
    pinMode(LED_G_COM, OUTPUT);
    pinMode(LED_B_COM, OUTPUT);
    led_red_off();
    led_green_off();
    led_blue_off();

    // ボタンを入力として初期化
    pinMode(button1Pin, INPUT_PULLUP);
    pinMode(button2Pin, INPUT_PULLUP);

    // DCモーターピンを出力として初期化
    pinMode(dcMotorPin0, OUTPUT);
    pinMode(dcMotorPin1, OUTPUT);
    // DCモーターを停止（両方に同じ値を入れると停止）
    analogWrite(dcMotorPin0, 0);
    analogWrite(dcMotorPin1, 0);
  
    // SR04
    pinMode( Trig,OUTPUT );
    pinMode( Echo,INPUT );
    digitalWrite( Trig, LOW );
    
    Serial.begin(9600);
}

int i=0;
// -------------------------------------------------------------
void loop()  {
    float temp;       
    char msg[256];
    unsigned long time_now = 0;
	
// スタートボタン状態を読み込む
	if(g_sw1==0){
		led_red_on();
		led6_on();
		// ボタンが押されるのを待つ
		while(digitalRead(button1Pin)==1)      ;
		led_red_off();
		led6_off();
		g_sw1=1;
		time_start=millis();
		delay( 2000 );	//押されてから一秒後にスタート
//        motor_on();	// スタート
		led_red_on();
		led6_on();
	}

	// 時間 経過時間=time_total(1000msの動いていない時間含む)
	time_now = millis();
	time_total = time_now-time_start;
//	Serial.println( time_total );

	// センサー距離を計測
	get_Duration_1times();
	// 温度センサー
//	get_temperature();  // temp_c セット
	
	// 距離の計算:取得できない場合は前の値を採用
        if ( DurationAverage > 0 )
	{
		i++;
                temp_c = 22.0;    // 気温
		SpeedOfSound = 331.5 + 0.6*temp_c; // 音速 ultrasonic speed is 340m/s = 34000cm/s = 0.034cm/us
//                Serial.println( SpeedOfSound );
                DurationAverageHalf = DurationAverage/2;		
//                Serial.println( DurationAverageHalf );
                Distance = (DurationAverageHalf * SpeedOfSound * 100) / 1000000;
                Serial.println( Distance );               
//        	sprintf( msg,"%d %d us(DurationAverage), %lu m/s(SpeedOfSound),%f cm(Distance)",i, DurationAverage,SpeedOfSound,Distance);
        	sprintf( msg,"%d %d us(DurationAverage),%f cm(Distance)",i, DurationAverage,Distance);
                Serial.println( msg );
	}else{
                Serial.println( "距離取得失敗" );
        }


	// 測定と計算終了
//	led_blue_off();  led_green_off();  led_red_off();

	// *****時間と距離でモーターコントロールここから*****
	// 場合わけでLEDを点灯
	if(time_total<2500){
//		motor_on();		// フルここまでは時間だけ
	}else if(time_total<2800){
//		motor_on12();		// フルここまでは時間だけ
	}else if(time_total<3000){
//		motor_on();		// フルここまでは時間だけ
//	}else if(time_total<6000){
	}else if(time_total<20000){	// 開始から10000ms未満
		// 距離を確認する。
		if ( Distance < (2+1) ){	// 2cm以下は微妙、車体先頭から2cm以上離して取付。実質1cm以下
			led_blue_on();
//	motor_break();
		}else if ( Distance < 4 ){
		}else if ( Distance < 5 ){
		}else if ( Distance < 6 ){
		}else if ( Distance < 7 ){	// 実質5cm以下
          		led_all_off();
			led_red_on();	motor_free();
		}else if ( Distance < 8 ){
		}else if ( Distance < 9 ){
		}else if ( Distance < 10 ){
		}else if ( Distance < 11 ){
		}else if ( Distance < 12 ){ 	// 実質10cm以下
			led_blue_on();	led_green_on();
//	motor_on12();
		}else if ( Distance < 20 ){
		}else if ( Distance < 50 ){
		}else if ( Distance < 100 ){
          		led_all_off();
			led_green_on();
//	motor_on12();
                        delay(1);
		}else{
			// それ以外の距離（遠い）
          		led_all_off();
			led_blue_on();
//			motor_on();
                        delay(10);
		}
	}else{
		// 時間オーバー停止
//		led_all_off();
		motor_free();
		while(1)	;
	}
	// *****時間と距離でモーターコントロールここまで*****
  
	// ストップボタン
	if(g_sw2==0){
        // ボタンが押されたら終了:LED OFF モーターOFF
		if(digitalRead(button2Pin)==0){
			led_all_off();
			motor_free();
			while(1)	;
                }
    	}

}
// -------------------------------------------------------


// LEDカラー（ON/OFFは制御しない）
void led_red_on(){	digitalWrite(LED_R_COM, HIGH);	}
void led_green_on(){	digitalWrite(LED_G_COM, HIGH);	}
void led_blue_on(){	digitalWrite(LED_B_COM, HIGH);	}
void led_red_off(){	digitalWrite(LED_R_COM, LOW);	}
void led_green_off(){	digitalWrite(LED_G_COM, LOW);	}
void led_blue_off(){	digitalWrite(LED_B_COM, LOW);	}
// LED ON/OFF
void led5_on(){	digitalWrite(ledPin5, HIGH);	}
void led6_on(){	digitalWrite(ledPin6, HIGH);	}
void led5_off(){	digitalWrite(ledPin5, LOW);	}
void led6_off(){	digitalWrite(ledPin6, LOW);	}
// LED すべてOFF
void led_all_off(){
    led5_off();
    led6_off();
    led_red_off();
    led_green_off();
    led_blue_off();
}

// 超音波エコー1回取得 5mで30ms
void get_Duration_1times(){
  char msg[256];
  delayMicroseconds( 3 );
  digitalWrite( Trig, HIGH );
  delayMicroseconds( 11 );
  digitalWrite( Trig, LOW );
  DurationAverage = pulseIn( Echo, HIGH );
//  Serial.println( DurationAverage );
}

//デジタル10オン,アナログ9=0→正転
//デジタル10オフ,アナログ9=255→逆転
//デジタル10オフ,アナログ9=0→停止フリー
//デジタル10オン,アナログ9=255→停止ブレーキ
//デジタル11オフ,アナログ6=255→正転
//デジタル11オン,アナログ6=0→逆転
//デジタル11オフ,アナログ6=0→停止フリー
//デジタル11オン,アナログ6=255→停止ブレーキ

void motor_on(){
//	digitalWrite(dcMotorPin1, HIGH);
//	analogWrite(dcMotorPin0, 0);
	digitalWrite(dcMotorPin1, LOW);
	analogWrite(dcMotorPin0, 255);
}
void motor_on12(){
//	digitalWrite(dcMotorPin1, HIGH);
//	analogWrite(dcMotorPin0, 128);
	digitalWrite(dcMotorPin1, LOW);
	analogWrite(dcMotorPin0, 128);
}
void motor_on23(){
//	digitalWrite(dcMotorPin1, HIGH);
//	analogWrite(dcMotorPin0, 85);
	digitalWrite(dcMotorPin1, LOW);
	analogWrite(dcMotorPin0, 85);
}
void motor_free(){
	digitalWrite(dcMotorPin1, LOW);
	analogWrite(dcMotorPin0, 0);
}
void motor_break(){
	digitalWrite(dcMotorPin1, HIGH);
	analogWrite(dcMotorPin0, 255);
}


// 温度取得
void get_temperature(){
	float a_in;          // アナログ入力値
	a_in = analogRead(0);
	// 入力値を摂氏に換算
	temp_c = ((5 * a_in) / 1024) * 100;
}
