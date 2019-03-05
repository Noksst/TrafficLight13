#include <avr/io.h>			// ��� ���������� �� �� IDE - ����������� ���������
#include <avr/sleep.h>		// ��, ����� �����
#include <avr/interrupt.h>	        // ����� �������������� ����������

volatile uint16_t  globalTimer;		//����� ��� ����� ���������� �� 64 �� ���������� ������

// ������ - ������������ ������� �������������� globalTimer ������ 1/37 �������
ISR(TIM0_OVF_vect){
	globalTimer++;	// � ������ ������ ��� ������ �� ����. ���������, ��������, �������� while(1) ���� � �����.
}

int main() {
	// ����������� �������� �� ����� "� ����� ���-�������". 
	set_sleep_mode(SLEEP_MODE_IDLE);	//���������� ����� ��� - ��.�������
	sleep_enable();					// ��������� ���� � ���
	TCCR0B = _BV(CS02) | _BV(CS00); // ������������ �������0 - clock frequency / 1024
	TIMSK0 |= _BV(TOIE0);			// ��� ������������ ����� ������� ���������� overflow interrupt
	sei();							// ��������� ��������� ��������� ����������
	while(1){
		//......... �������� ��������� �����
		sleep_cpu();	//� � ����� ����� ����� - ������ � ���.
	}
}

//Program size: 128 bytes (used 13% of a 1�024 byte maximum) (0,86 secs)
//Minimum Memory Usage: 2 bytes (3% of a 64 byte maximum)