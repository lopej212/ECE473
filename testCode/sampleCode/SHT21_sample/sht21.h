
#ifndef SHT21_H_
#define SHT21_H_

#define polynomial 0x131
#define SHT21_add 0x40
#define SHT21_i2c_write 0x80
#define SHT21_i2c_read 0x81
#define temperature_hold_mode 0xE3
#define humidity_hold_mode 0xE5
#define user_register_write 0xE6
#define user_register_read 0xE7
#define SHT21_reset_cmd 0xFE

void SHT21_reset();
uint16_t checksum(unsigned char data[],uint8_t byte, uint8_t check);
uint8_t read_user_register();
void write_user_register();
uint16_t read_value(uint8_t reg);
void get_humidity();
void get_temperature();
void dew_point();
void serial_number();

#endif /* SHT21_H_ */