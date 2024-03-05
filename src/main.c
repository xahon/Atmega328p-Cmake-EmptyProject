#include <avr/io.h>
#include <avr/delay.h>

int main(void)
{
    DDRD |= _BV(1);

    while(1)
    {
        PORTD |= _BV(1);
        _delay_ms(1000);
        PORTD &= ~_BV(1);
        _delay_ms(1000);
    }

    return 0;
}
