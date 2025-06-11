#include <msp430.h>
#include "lcd.h"

volatile unsigned int adc_result = 0;
volatile unsigned int temp_int = 0;
volatile unsigned int temp_frac = 0;
volatile unsigned int ref_temp = 25;

volatile unsigned char ref_temp_degisti = 1;
volatile unsigned char buton_arttir = 0;
volatile unsigned char buton_azalt = 0;

unsigned char fan_acik = 0;
unsigned char isitici_acik = 0;

void delay_ms(unsigned int ms) {
    while (ms--) __delay_cycles(1000);
}

void lcd_yaz_sayi(unsigned int sayi) {
    LCD_HarfYaz((sayi / 10) + '0');
    LCD_HarfYaz((sayi % 10) + '0');
}

void adc_init(void) {
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = SREF_1 + REFON + ADC10SHT_3 + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_6 + ADC10SSEL_3 + ADC10DIV_3;
    ADC10AE0 |= BIT6;
}

void gpio_init(void) {
    // LCD pinleri: RS→P1.1, EN→P1.2, D4–D6→P1.3–P1.5, D7→P2.0
    P1DIR |= BIT1 | BIT2 | BIT3 | BIT4 | BIT5;
    P2DIR |= BIT0;
    P1OUT &= ~(BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    P2OUT &= ~BIT0;

    // Fan: P2.6, Isıtıcı: P1.7
    P2DIR |= BIT6;
    P2OUT &= ~BIT6;
    P2SEL &= ~BIT6;     // Alternatif fonksiyonları kapat
    P2SEL2 &= ~BIT6;

    P1DIR |= BIT7;
    P1OUT &= ~BIT7;
    P1SEL &= ~BIT7;     // Alternatif fonksiyonları kapat
    P1SEL2 &= ~BIT7;


    // Butonlar: P2.4 (↑), P2.3 (↓)
    P2DIR &= ~(BIT3 | BIT4);        //Giriş
    P2REN |= BIT3 | BIT4;           // pull-up/down direnç aktif etme
    P2OUT |= BIT3 | BIT4;           // pull-up ayarlanır
    P2IES |= BIT3 | BIT4;           // hıgh dan low a tetikleme (buton basımı)
    P2IFG &= ~(BIT3 | BIT4);        // flaglar temizlenir
    P2IE |= BIT3 | BIT4;            // interrupt etkin
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;           //mikrodenetleyicinin 1 mhzde çalışacak şekilde başlatmaya yarıyor
    BCSCTL1 = CALBC1_1MHZ;

    gpio_init();
    adc_init();
    LCD_Baslat();
    LCD_Temizle();

    __enable_interrupt();

    // Timer: ADC tetiklemesi   
    CCTL0 = CCIE;                       // CCR0 kesmesi aktif
    CCR0 = 50000;                       // 50000 clock cycle (50 ms)
    TACTL = TASSEL_2 + MC_1;            // SMCLK ayarlanır, up mode- sayaç 0’dan CCR0 değerine kadar sayar, sonra sıfırlanır ve tekrar başlar.

    while (1) {
        // Buton işleme (debounce + bayrak kontrolü)
        if (buton_arttir) {
            delay_ms(30);
            if (!(P2IN & BIT4)) {
                ref_temp++;
                ref_temp_degisti = 1;
            }
            buton_arttir = 0;
        }

        if (buton_azalt) {
            delay_ms(30);
            if (!(P2IN & BIT3)) {
                ref_temp--;
                ref_temp_degisti = 1;
            }
            buton_azalt = 0;
        }

        // LCD 1. satır: hedef sıcaklık
        if (ref_temp_degisti) {
            LCD_ImleciAyarla(1, 1);
            LCD_YaziYaz("Hedef: ");
            lcd_yaz_sayi(ref_temp);
            LCD_YaziYaz(" C ");
            ref_temp_degisti = 0;
        }

        // LCD 2. satır: ölçüm sıcaklık
        LCD_ImleciAyarla(2, 1);
        LCD_YaziYaz("Olcum: ");
        lcd_yaz_sayi(temp_int);
        LCD_HarfYaz('.');
        LCD_HarfYaz(temp_frac + '0');
        LCD_YaziYaz(" C ");

        // Fan ve Isıtıcı kontrol (sabit çıkış mantığıyla)
        if (temp_int > ref_temp && fan_acik == 0) {
            P2OUT |= BIT6;      // Fan aç
            P1OUT &= ~BIT7;     // Isıtıcı kapat
            fan_acik = 1;
            isitici_acik = 0;
        } else if (temp_int < ref_temp && isitici_acik == 0) {
            P1OUT |= BIT7;      // Isıtıcı aç
            P2OUT &= ~BIT6;     // Fan kapat
            fan_acik = 0;
            isitici_acik = 1;
        } else if (temp_int == ref_temp) {
            P2OUT &= ~BIT6;
            P1OUT &= ~BIT7;
            fan_acik = 0;
            isitici_acik = 0;
        }
        delay_ms(200);
    }
}
#pragma vector = PORT2_VECTOR
__interrupt void Port_2_ISR(void) {
    if (P2IFG & BIT4) {
        buton_arttir = 1;
        P2IFG &= ~BIT4;
    }

    if (P2IFG & BIT3) {
        buton_azalt = 1;
        P2IFG &= ~BIT3;
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void timer_A(void) {
    ADC10CTL0 |= ENC + ADC10SC;             // Ölçüm başlat Timer tetiklemeli
}

#pragma vector = ADC10_VECTOR
__interrupt void adc_isr(void) {
    adc_result = ADC10MEM;
    unsigned long mv = (unsigned long)adc_result * 1500UL / 1023UL;         //undigned long tipinde olunca sıcaklık degeri düzeldi
    temp_int = mv / 10;
    temp_frac = mv % 10;
    ADC10CTL0 &= ~REFON;
}
