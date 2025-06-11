#include <msp430.h>
#include "lcd.h"

// LCD pin tanımları
#define RS BIT1   // P1.1
#define EN BIT2   // P1.2
#define D4 BIT3   // P1.3
#define D5 BIT4   // P1.4
#define D6 BIT5   // P1.5
#define D7 BIT0   // P2.0

void Pulse_Gonder(void) {
    P1OUT |= EN;
    __delay_cycles(1000);
    P1OUT &= ~EN;
    __delay_cycles(1000);
}

void nibble_Gonder(unsigned char nibble) {
    P1OUT &= ~(D4 | D5 | D6);
    P2OUT &= ~D7;

    if (nibble & 0x01) P1OUT |= D4;    // Bit 0
    if (nibble & 0x02) P1OUT |= D5;    // Bit 1
    if (nibble & 0x04) P1OUT |= D6;    // Bit 2
    if (nibble & 0x08) P2OUT |= D7;    // Bit 3

    Pulse_Gonder();
}

void LCD_Komut_Gonder(unsigned char komut) {
    P1OUT &= ~RS;  // Komut modu
    nibble_Gonder(komut >> 4);   // Üst nibble
    nibble_Gonder(komut & 0x0F); // Alt nibble
    __delay_cycles(2000);
}

void LCD_HarfYaz(char karakter) {
    P1OUT |= RS;   // Veri modu
    nibble_Gonder(karakter >> 4);
    nibble_Gonder(karakter & 0x0F);
    __delay_cycles(2000);
}

void LCD_YaziYaz(char *yazi) {
    while (*yazi) {
        LCD_HarfYaz(*yazi++);
    }
}

void LCD_ImleciAyarla(unsigned char satir, unsigned char sutun) {
    unsigned char adres = (satir == 1) ? 0x80 + sutun - 1 : 0xC0 + sutun - 1;
    LCD_Komut_Gonder(adres);
}

void LCD_Temizle(void) {
    LCD_Komut_Gonder(0x01); 
    __delay_cycles(2000);
}

void LCD_Baslat(void) {
    // LCD pinlerini çıkış yap
    P1DIR |= RS | EN | D4 | D5 | D6;
    P2DIR |= D7;
    P1OUT &= ~(RS | EN | D4 | D5 | D6);
    P2OUT &= ~D7;

    __delay_cycles(15000); // Başlangıç gecikmesi
    nibble_Gonder(0x03);
    __delay_cycles(5000);
    nibble_Gonder(0x03);
    __delay_cycles(5000);
    nibble_Gonder(0x03);
    __delay_cycles(5000);
    nibble_Gonder(0x02); // 4-bit moda geçiş
    __delay_cycles(1000);

    LCD_Komut_Gonder(0x28); // 4-bit, 2 satır, 5x8 font
    LCD_Komut_Gonder(0x0C); // Display ON, cursor OFF
    LCD_Komut_Gonder(0x06); // Cursor sağa
    LCD_Temizle();
}
