#include <limits.h>			// USHRT_MAX
#include <avr/sleep.h>		// ��, ����� �����
#include <avr/interrupt.h>	// ����� �������������� ����������
#include <avr/pgmspace.h>	// ����������� ������ ��� ��������

#define ONE_SECOND	37		// ���������� ������������ �������� � 1 �������
#define QT_SECOND	9		// �������� �������
#define MAX_GLOBAL_TIMER_VALUE	(USHRT_MAX / 2)		// uint16_t globalTimer - ������ �� ������������. 65535 /2 
// ����� ������ ������ ���� ������, ��� MAX_GLOBAL_TIMER_VALUE - 1
#define PERIOD_FLASH_GREEN	QT_SECOND			//������ ������� ������� ������ (�������� ���) - ����� ������������� � ������
#define PERIOD_FLASH_YELLOW	ONE_SECOND * 1		//������ ������� ������ ������ - ����������� ���������� ��������� - �������
														// ����� --- ������
#define PERIOD_0		ONE_SECOND * 10		//R G R G	0. ������� --- �������	(15 ���)
#define PERIOD_1		ONE_SECOND * 3		//R g R g	1. ������� --- ������� �������� (3 ���)		
#define PERIOD_2		ONE_SECOND * 1		//R Y R Y	2. ������� --- ������ (1 ���)
#define PERIOD_3		ONE_SECOND * 2		//RY Y RY Y	3. �����+������ --- ������ (2 ���) 
#define PERIOD_4		ONE_SECOND * 7		//G R G R	4. ������� --- �������	(10 ���)
#define PERIOD_5		ONE_SECOND * 3		//g R g R	5. ������� �������� --- �������(3 ���)
#define PERIOD_6		ONE_SECOND * 1		//Y R Y R	6. ������ --- ������� (1���)
#define PERIOD_7		ONE_SECOND * 2		//Y RY Y RY	7. ������ --- �������+������ (2���)


typedef struct{
	const uint8_t ddr_val_0;	// DDRB value ��� ������ ��������� �������
	const uint8_t port_val_0;	// PORTB value	
	const uint8_t ddr_val_1;	// DDRB value ��� ������ ��������� �������
	const uint8_t port_val_1;	// PORTB value
	const uint16_t flash_period;	// period of flasing - ������������ ����� _val_1 � _val_0
	const uint16_t signal_period;	// period of this lighting state
}lightSignalization;			// ��������� ����� ���������, _0 � _1 - ��������� ��� �������, flash_long - ����� ������������ �������

// �� �������������� ����� (PINS === 0 0 0 g r y0 btt y1):
 // ����� �� ���� ������������ "����������" ����� ��������� ����� ��� ����������
#define BUTTON		PB1
#define RED_PIN		PB3		// OUT: 1 - "�����-��" �������, 0 - "�����-������" , IN - �� ����, ��������, �������� ��(!) ��������
#define	YELLOW0_PIN	PB2		// OUT: 1 - ������ "�����-��" 
#define	YELLOW1_PIN	PB0		// OUT: 1 - ������ "�����-������" 
#define	GREEN_PIN	PB4		// OUT: 1 - "�����-��" �������, 0 - "�����-������" , IN - �� ����, ��������, �������� ��(!) ��������
 
#define	BUTTON_ON  !(PINB & _BV(BUTTON))	//( (PINB & _BV(BUTTON)) == 0)	// ������� "������ ������"
#define	BUTTON_OFF	(PINB & _BV(BUTTON))	// ~(PINB & _BV(BUTTON)) -\\- "�� ������"

#define RED		_BV(RED_PIN)			// _BV - ����� ����� ������� �� ����������(), 1<<VALUE
#define YELL0	_BV(YELLOW0_PIN)		//
#define YELL1	_BV(YELLOW1_PIN)
#define GREEN	_BV(GREEN_PIN)

// ������ ������� ������ � ������� 	lightSignalization traffic_signals[]
#define LIGHT_NUM_YELLOW_FLASH	8		// ����� ��������� ����� ��� ������� ������ - �������� - flash yellows lights
#define LIGHT_NUM_STD_START		0		// � ������ ������ ���������� ������ ������������ ������
#define LIGHT_NUM_LIGHTS_OFF	9		// ����� ��������� ����� �� ��������� - � ������ - traffic lights off
#define LIGHT_NUM_STD_MASK		7		// �������_�����_���������_���������++ &LIGHT_NUM_STD_MASK - ������������ ������� �� 0 �� 7 �� ����� 

//.................................... ���������� ����������
// 
const lightSignalization traffic_signals[] PROGMEM= {	// ������� ����������� ��������, �������� �����������, �������� �� ����-������, PINS === 0 0 0 g r y0 btt y1
 // {DDRB0, PORTB0,   DDRB_flashing, PORTB_flasinf (if flashing),   continous of half-period flashing,   continous id mode runing}
	{RED|GREEN, RED,   0, 0,   0, PERIOD_0},					// R G R G
	{RED, RED,  RED|GREEN, RED,   QT_SECOND, PERIOD_1},		// R g R g - flash east green
	{RED|YELL1, RED|YELL1,   0, 0,   0, PERIOD_2 },				// R Y1 R Y1
	{RED|YELL0|YELL1, RED|YELL0|YELL1,  0, 0,   0, PERIOD_3 },	// RY0 Y1 RY0 Y1
	{RED|GREEN, GREEN,   0, 0,   0, PERIOD_4},					// G R G R
	{RED|GREEN, GREEN,   RED, 0,    QT_SECOND, PERIOD_5 },		// g R g R - flash nord green
	{RED|YELL0, YELL0,   0, 0,   0, PERIOD_6},					// Y0 R Y0 R
	{RED|YELL0|YELL1,  YELL0|YELL1,   0, 0,   0, PERIOD_7  },	// Y0 RY1 Y0 RY1

	{YELL0|YELL1, YELL0|YELL1,   YELL0|YELL1, 0,  ONE_SECOND, 0},	// y0 y1 y0 y1 - flash yellows lights 
	{0, 0,   0, 0,   0, 0}				//  traffic lights off, 
};



volatile uint16_t  globalTimer;		//����� ��� ����� ���������� �� 64 �� ���������� ������
uint8_t	current_signal;		// 1 ���� �� ������� ���������, ����� � traffic_signals
uint16_t tl_flash_end;		// 2 ����� �� ����� ��������� ������� ������� (���� !0), 
uint16_t tl_signal_end;		// 2 ����� �� ����� ��������� ������ �������� ������� � ������������ �� ��������� (���� !0)

//.................................... ��������� �������
void setPeriods(uint8_t num, bool set_both_flash_and_signal);	// ��������� tl_flash_end, tl_signal_end
void setPorts(uint8_t num, bool use_main_values);				// ��������� ������ ������ ������

//.................................... ������� ����������
//
// ������ - ������������ ������� �������������� globalTimer ������ 1/37 �������
ISR(TIM0_OVF_vect){
	globalTimer++;	// � ������ ������ ��� ������ �� ����. ���������, ����� �������� while(1) ���� � �����.
}

int main() {
//
bool use_main_values = true;			// lightSignalization.���_val_0 (1) ��� lightSignalization.���_val_1 (0)? - ���� �������
current_signal = LIGHT_NUM_STD_START;	// ��������� ������ ������. ������� ���� ��� �� �������������.

	// ����������� �������� �� ����� "� ����� ���-�������". 
	set_sleep_mode(SLEEP_MODE_IDLE);	//���������� ����� ��� - ��.�������
	sleep_enable();					// ��������� ���� � ���

	TCCR0B = _BV(CS02) | _BV(CS00); // ������������ �������0 - clock frequency / 1024
	TIMSK0 |= _BV(TOIE0);			// ��� ������������ ����� ������� ���������� overflow interrupt

	sei();		// ��������� ��������� ��������� ����������
	while(1){
		// ������������ ����������� �������? 
		if(globalTimer > MAX_GLOBAL_TIMER_VALUE){
			globalTimer -= MAX_GLOBAL_TIMER_VALUE;			// �������� ���������� ������
			// 
			if(tl_flash_end){
				tl_flash_end -= MAX_GLOBAL_TIMER_VALUE;		// �������� ������ �������, ���� ����
			}
			if(tl_signal_end){
				tl_signal_end -= MAX_GLOBAL_TIMER_VALUE;	// // �������� ������ ���������, ���� ����
			}
			// setPeriods(currentMode, false); // ��� �� 12 ���� ������, �� tl_.._end ��������� � ��������, ����� ��������� ����������� ���������� ������������
		}

				//���� � ������ ������ ���� ������� (tl_flash_end !=0 )
		if(tl_flash_end){
			// � ����� ����� ������� ������
			if(globalTimer > tl_flash_end){
				use_main_values = !use_main_values;				// !use_main_values - ��� ��� ���� �� ���� ))
				setPorts(current_signal, use_main_values);		// ����������� ����� �������� ��������� �� ���������������
				setPeriods(current_signal, false);				// �������� ������ ��������� ������ �������, �� �� ���������
			}
		}

		// ���� � ������ ������ ���� ����������� ������� ��������� - ���������� ��� ������ operating_std ����������� ������������������ �� ����� - �������-������-�������
		if(tl_signal_end){
			// � ��� ���� ������������ �� �������� (use_main_values - ����� ������������ ���� � �������� �������� - �� ������)
			if((globalTimer > tl_signal_end)  && use_main_values){
				current_signal ++;							// ��������� ������ ���������
				current_signal &= LIGHT_NUM_STD_MASK;		// �������� ���� ���� 3-��, � �������� ������ ������� ������ ��������� � 0 �� 7
				use_main_values = true;						// ���������� - � ������� ���� ���������
				setPorts(current_signal, use_main_values);	// ����������� ����� �������� ��������� �� ��������� � �������
				setPeriods(current_signal, true);			// �������� ��������� ������ ������� � ������ ���������
			}
		}
		
		sleep_cpu();	//� � ����� ����� ������������ ����� - ������ � ���.
	}
}



	// ��������� �������� ������
 void setPorts(uint8_t num, bool use_main_values){
	uint8_t val;
	DDRB = 0; PORTB = 0;
	// ���� �������� ����� (�������) - ddr_val_0, else = ddr_val_1
//	val = (use_main_values) ? pgm_read_byte_near(&(traffic_signals[num].ddr_val_0))
//							: pgm_read_byte_near(&(traffic_signals[num].ddr_val_1));

	// �� �� �����, �� ���������, ���������, ���������� ����������, ������ � ����, ���� �� 14 (!!!) ���� ��� ������.
	val = pgm_read_byte_near(&(traffic_signals[num].ddr_val_0)+( (use_main_values) ? 0 : 2) ); 
	val &= ~_BV(BUTTON);	// ����� ���� ���� ������ - ����
	DDRB = val;				// ��������� ������ ����� �����
	
	val = (use_main_values) ? pgm_read_byte_near(&(traffic_signals[num].port_val_0)) 
							: pgm_read_byte_near(&(traffic_signals[num].port_val_1));
	val|= _BV(BUTTON);	// �������� �� ���� ������ - ������������ - ������������ �� ������ �������
	PORTB = val;		// ��������� �������� ������� � ������ �����
}

 //���������� ����� ��������� ������ ������� (��� 
void setPeriods(uint8_t num, bool set_both_flash_and_signal){
// ���������� ����������
	tl_flash_end = pgm_read_word_near (&(traffic_signals[num].flash_period));	// ������ �������
	tl_flash_end = (tl_flash_end)? tl_flash_end + globalTimer : 0;	//����� ��������� ������ ������� - ���� �� ������� �������� �������	
	//if(tl_flash_end){ 		tl_flash_end += globalTimer; 	} <- ��� ��� ��� �� 8 ���� �������

	// ���� ���������� ��� ������� - � ��������� � �������
	if(set_both_flash_and_signal){
		tl_signal_end = pgm_read_word_near(&(traffic_signals[num].signal_period));	
		tl_signal_end = (tl_signal_end)? tl_signal_end + globalTimer : 0;			// ����� ������������ �� ��������� �����, ���� �� ������� ��������
	}
}

//Program size: 610 bytes (used 60% of a 1�024 byte maximum) (0,58 secs)
//Minimum Memory Usage: 7 bytes (11% of a 64 byte maximum)