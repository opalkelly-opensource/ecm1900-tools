#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define SUPPLY_28 0
#define SUPPLY_67 1
#define SUPPLY_68 2
#define SUPPLY_87_88 3

#define PCAL_ADDR 0x20

#define PCAL_OUTPUT_PORT0_REG 0x02
#define PCAL_CONFIG_PORT0_REG 0x06



int i2c_read (int fd, uint16_t reg_addr, uint8_t* reg_data)
{
	uint8_t buf[2];

	printf("Reading from %02X\n", reg_addr);

	buf[0] = reg_addr;

	if (write(fd, buf, 1) != 1) {
		printf("Error setting read address: %02X\n", reg_addr);
		exit(1);
	}

	if (read(fd, reg_data, 1) != 1) {
		printf("Error reading from I2C\n");
		exit(1);
	}

	return 0;
}


int i2c_write (int fd, uint8_t reg_addr, uint8_t reg_data)
{
	uint8_t buf[34];

	printf("Writing to %02X: %02X\n", reg_addr, reg_data);

	// write I2C register
	buf[0] = reg_addr;
	buf[1] = reg_data;

	if (write(fd, buf, 2) != 2) {
		printf ("Error during page set: %02X, %02X\n", reg_addr, reg_data);
		exit(1);
	}

	return 0;
}

int disable_supply (int fd, int rail_index)
{
	uint8_t temp_reg;
	uint8_t pcal_port = 0;
	uint8_t en_offset = 0;

	switch(rail_index) {
		case SUPPLY_28:
			en_offset = 0;
			pcal_port = 0;
			break;
		case SUPPLY_67:
			en_offset = 4;
			pcal_port = 0;
			break;
		case SUPPLY_68:
			en_offset = 0;
			pcal_port = 1;
			break;
		case SUPPLY_87_88:
			en_offset = 4;
			pcal_port = 1;
			break;
		default:
			printf("Internal Error\n");
			exit(1);
	}

	// Read associated output port status
	i2c_read(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, &temp_reg);
	
	// Set associated output port to 0
	i2c_write(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, temp_reg & ~(0x1 << en_offset));

	// Read output port configuration
	i2c_read(fd, PCAL_CONFIG_PORT0_REG + pcal_port, &temp_reg);
	
	// Configure associated I/O direction to output
	i2c_write(fd, PCAL_CONFIG_PORT0_REG + pcal_port, temp_reg & ~(0x1 << en_offset));

	return 0;
}

int set_supply (int fd, int rail_index, uint8_t vs0, uint8_t vs1, uint8_t vs2)
{
	uint8_t temp_reg;
	uint8_t pcal_port = 0;
	uint8_t vs_offset = 0;

	switch(rail_index) {
		case SUPPLY_28:
			vs_offset = 0;
			pcal_port = 0;
			break;
		case SUPPLY_67:
			vs_offset = 4;
			pcal_port = 0;
			break;
		case SUPPLY_68:
			vs_offset = 0;
			pcal_port = 1;
			break;
		case SUPPLY_87_88:
			vs_offset = 4;
			pcal_port = 1;
			break;
		default:
			printf("Internal Error\n");
			exit(1);
	}

	// Read associated output port status
	i2c_read(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, &temp_reg);
	
	// Set associated output port to 0
	if (vs_offset == 0) {
		temp_reg = (temp_reg & (0xF << 4));
	} else {
		temp_reg = (temp_reg & (0xF << 0));
	}
	temp_reg = temp_reg | (((vs0 << 1) | (vs1 << 2) | (vs2 << 3)) << vs_offset);
	i2c_write(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, temp_reg);
	
	return 0;
}

int enable_supply (int fd, int rail_index)
{
	uint8_t temp_reg;
	uint8_t pcal_port = 0;
	uint8_t en_offset = 0;

	switch(rail_index) {
		case SUPPLY_28:
			en_offset = 0;
			pcal_port = 0;
			break;
		case SUPPLY_67:
			en_offset = 4;
			pcal_port = 0;
			break;
		case SUPPLY_68:
			en_offset = 0;
			pcal_port = 1;
			break;
		case SUPPLY_87_88:
			en_offset = 4;
			pcal_port = 1;
			break;
		default:
			printf("Internal Error\n");
			exit(1);
	}

	// Read associated output port status
	i2c_read(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, &temp_reg);
	
	// Set associated output port to 1
	i2c_write(fd, PCAL_OUTPUT_PORT0_REG + pcal_port, temp_reg | (0x1 << en_offset));

	// Read output port configuration
	i2c_read(fd, PCAL_CONFIG_PORT0_REG + pcal_port, &temp_reg);
	
	// Configure associated I/O direction to output
	i2c_write(fd, PCAL_CONFIG_PORT0_REG + pcal_port, temp_reg & ~(0xF << en_offset));

	return 0;
}

int main (int argc, char *argv[])
{
	int i2c_fd;
	int error = 0;
	uint8_t vs0, vs1, vs2;
	int i;


	char filename[20];

	if (argc != 4) {
		printf("Usage: %s [I2C device] [bank] [voltage]\n", argv[0]);
		exit(1);
	}

	strcpy(filename, argv[1]);

	i2c_fd = open(filename, O_RDWR);
	if (i2c_fd < 0) {
		printf("Error opening I2C device\n");
		exit(1);
	}

	if (ioctl(i2c_fd, I2C_SLAVE, PCAL_ADDR) < 0) {
		printf("Error during I2C address set\n");
		exit(1);
	}

	if (strcmp(argv[3],"3.3") == 0) {
		vs0 = 0;
		vs1 = 0;
		vs2 = 0;
	} else if (strcmp(argv[3],"2.5") == 0) {
		vs0 = 1;
		vs1 = 0;
		vs2 = 0;
	} else if (strcmp(argv[3],"1.8") == 0) {
		vs0 = 0;
		vs1 = 1;
		vs2 = 0;
	} else if (strcmp(argv[3],"1.5") == 0) {
		vs0 = 1;
		vs1 = 1;
		vs2 = 0;
	} else if (strcmp(argv[3],"1.25") == 0) {
		vs0 = 0;
		vs1 = 0;
		vs2 = 1;
	} else if (strcmp(argv[3],"1.2") == 0) {
		vs0 = 1;
		vs1 = 0;
		vs2 = 1;
	} else if (strcmp(argv[3],"0.8") == 0) {
		vs0 = 0;
		vs1 = 1;
		vs2 = 1;
	} else {
		printf("Voltage setting not supported, options are:\n\
				3.3  2.5  1.8  1.5  1.25  1.2  0.8\n");
	}

	if (strcmp(argv[2],"28") == 0) {
		if (vs2 == 0 && vs1 == 0) {
			printf("Voltage not supported on this rail\n");
			exit(1);
		}
		disable_supply(i2c_fd, SUPPLY_28);
		set_supply(i2c_fd, SUPPLY_28, vs0, vs1, vs2);
		enable_supply(i2c_fd, SUPPLY_28);
	} else if (strcmp(argv[2],"67") == 0) {
		if (vs2 == 0 && vs1 == 0) {
			printf("Voltage not supported on this rail\n");
			exit(1);
		}
		disable_supply(i2c_fd, SUPPLY_67);
		set_supply(i2c_fd, SUPPLY_67, vs0, vs1, vs2);
		enable_supply(i2c_fd, SUPPLY_67);
	} else if (strcmp(argv[2],"68") == 0) {
		if (vs2 == 0 && vs1 == 0) {
			printf("Voltage not supported on this rail\n");
			exit(1);
		}
		disable_supply(i2c_fd, SUPPLY_68);
		set_supply(i2c_fd, SUPPLY_68, vs0, vs1, vs2);
		enable_supply(i2c_fd, SUPPLY_68);
	} else if (strcmp(argv[2],"87_88") == 0) {
		disable_supply(i2c_fd, SUPPLY_87_88);
		set_supply(i2c_fd, SUPPLY_87_88, vs0, vs1, vs2);
		enable_supply(i2c_fd, SUPPLY_87_88);
	} else {
		printf("Error: unrecognized supply rail, options are:\n\
				28  67  68  87_88\n");
		exit(1);
	}

	return error;
}


