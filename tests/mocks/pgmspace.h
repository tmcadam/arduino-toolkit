/* pgmspace.h: Mock definitions for testing */
#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_

#define PSTR(x) x

uint8_t TCCR0A;
uint8_t TCCR0B;

uint8_t OCR0A;

uint8_t TIMSK0;

uint8_t ADMUX;
uint8_t ADCSRA;

uint8_t EEAR;
uint8_t EECR;
uint8_t EEDR;

uint8_t DDRC;
uint8_t PORTC;
uint8_t PCMSK1;
uint8_t PCICR;

uint8_t SMCR;

#define PC0 0x00
#define PC1 0x01
#define PC2 0x02
#define PC3 0x03
#define PC4 0x04

#define PCINT8 0x00
#define PCINT9 0x01
#define PCINT10 0x02
#define PCINT11 0x03
#define PCINT12 0x04

#define PCIE1 0x01
#define PINC 0x02

#define CS00 0x01
#define CS02 0x02

#define OCIE0A 0x01

#define WGM01 0x01

#define ADPS0 0x00
#define ADPS1 0x01
#define ADPS2 0x02
#define ADEN 0x03
#define ADSC 0x04
#define ADCL 0x05
#define ADCH 0x06

#define EEPE 0x01
#define EERE 0x02
#define EEMPE 0x03

#define SM1 0x01
#define SM2 0x02

#endif
