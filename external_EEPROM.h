#ifndef EEPROM_H
#define	EEPROM_H


#define SLAVE_READ_E		0xA1        //address of external eeprom
#define SLAVE_WRITE_E		0xA0


void write_extenal_EEPROM(unsigned char address, unsigned char data);
unsigned char read_extenal_EEPROM(unsigned char address);

#endif	

