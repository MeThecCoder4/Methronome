#include <avr/io.h>
#include <util/delay.h>

/* All values have been calculated for 1MHz clock speed */

#define MIN_BPM			60
#define MAX_BPM			300
#define BPM60_CNT		7812 // This is the value of counter TOP for 60BPM
#define HALF_VIB		300 // Number TOP-HALF_VIB determines half of the length (in counter units) of engine vibration
#define PWM_PIN_DDR		DDRB
#define PWM_PIN			PORTB1
#define BUTTONS_DDR		DDRC
#define BUTTON1			PORTC5
#define BUTTON2			PORTC4

char button1_clicked = 0, button2_clicked = 0;

static void set_counter(const int16_t bpm);

static void init_pwm(void);

static int32_t bpm_to_cnt(const int32_t bpm);

static void manage_buttons(int16_t* bpm_var);

static void init_pwm(void)
{
	PWM_PIN_DDR |= (1 << PWM_PIN);
	// Timer1 count = 0
	TCNT1 = 0;
	// Enable phase correct PWM mode
	// Look for TOP in ICR1, prescaler = 64
	// Set OC1A on compare match when up-counting. Clear OC1A on compare match when down-counting.
	TCCR1A |= (1 << COM1A1) | (1 << COM1A0) | (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << CS11) | (1 << CS10);
	set_counter(MIN_BPM);
}

static int32_t bpm_to_cnt(int32_t bpm)
{
	return (BPM60_CNT * (int32_t)(60)) / bpm;
}

static void set_counter(const int16_t bpm)
{
	int32_t temp = bpm_to_cnt(bpm);
	
	if(temp > 0 && temp - HALF_VIB > 0 && bpm >= MIN_BPM
		//&& bpm <= MAX_BPM)
		)
	{
		ICR1 = temp;
		OCR1A = ICR1 - HALF_VIB;	
	}
}

static void manage_buttons(int16_t* bpm_var)
{
	const int16_t step = 5;
	// Change bpm at button press
	// Increase BPM
	if((PINC & (1 << BUTTON1)) == 0 && button1_clicked == 0)
	{
		button1_clicked = 1;
		
		if(*bpm_var > MIN_BPM && *bpm_var - step > MIN_BPM)
		{
			(*bpm_var) -= step;
			set_counter(*bpm_var);
		}
		
		_delay_ms(10);
	}
	else
	{
		button1_clicked = 0;
	}
	
	// Decrease BPM
	if((PINC & (1 << BUTTON2)) == 0 && button2_clicked == 0)
	{
		button2_clicked = 1;
		
		if(*bpm_var < MAX_BPM && *bpm_var - step < MAX_BPM)
		{
			(*bpm_var) += step;
			set_counter(*bpm_var);
		}
		
		_delay_ms(20);
	}
	else
	{
		button2_clicked = 0;
	}
}

int main(void)
{
	// Initialize buttons
	// Buttons as inputs
	BUTTONS_DDR &= ~(1 << BUTTON1) | ~(1 << BUTTON2);
	// Activate pull-up resistors
	PORTC |= (1 << BUTTON1) | (1 << BUTTON2);
	
	init_pwm();
	int16_t current_bpm = MIN_BPM;
	set_counter(700);
	
    while (1) 
    {
		//manage_buttons(&current_bpm);
    }
}

