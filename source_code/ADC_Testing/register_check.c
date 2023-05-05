/*	EE344 - Electronic Design Lab
	Groups: TUES-29 and MON-30
	Project: EEG Acquisition System
  
	This code is for initial testing of ADS1194 using PIC32MX clicker board
	IDE used - NECTO Studio v3.0.0
	
	Clock mode: internal

	Supply from clicker board:
	DVDD - 3.3V
	AVDD - 5V "CAUTION: Supply voltages higher than 5.25V can permanently damage the ADC"
	
	SPI Module (clicker):
	CS - pin RG9
	SCLK - pin RG6
	MOSI - pin RG8
	MISO - pin RG7
	RESET - pin RD7
	DRDY(INT) - pin RD11
	
	UART Module (clicker):
	Tx - pin RB14
	Rx - pin RB8
*/


// There are 26 registers
#define no_regs 26

// Defining the pin mappings
sbit cs at RG9_bit;			// chip select' pin
sbit sclk at RG6_bit;		// serial clock pin
sbit dout at RG8_bit;		// uC's data_out, i.e. MOSI pin
sbit din at RG7_bit;		// uC's data_in, i.e. MISO pin
sbit txd at RB14_bit;		// TX pin for UART
sbit rxd at RB8_bit;			// RX pin for UART
sbit reset at RD7_bit;		// RESET' pin
sbit drdy at RD11_bit;		// DRDY' (INT) pin

// Defining the input/output direction variables
sbit cs_direction at TRISG9_bit;
sbit sclk_direction at TRISG6_bit;
sbit dout_direction at TRISG8_bit;
sbit din_direction at TRISG7_bit;
sbit tx_direction at TRISB14_bit;
sbit rx_direction at TRISB8_bit;
sbit reset_direction at TRISD7_bit;
sbit drdy_direction at TRISD11_bit;

// Creating array of registers
char reg[no_regs];


void init(){
	cs_direction = 0;		// set CS' pin as output
	sclk_direction = 0;		// set SCLK pin as output
	dout_direction = 0;		// set MOSI pin as output
	din_direction = 1;		// set MISO pin as input
	tx_direction = 0;		// set TX pin as output
	rx_direction = 1;		// set RX pin as input
	reset_direction = 0;	// set RESET' pin as output
	drdy_direction = 0;		// set DRDY' pin as output

	cs = 1;					// deselect ADC (active low)

	reset = 0;			// reset the ADC (active low)
	Delay_ms(10);		// 10 ms delay
	reset = 1;
	Delay_ms(10);

	SPI2_Init_Advanced(_SPI_MASTER, _SPI_16_BIT, 80, _SPI_SS_DISABLE, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_LOW, _SPI_IDLE_2_ACTIVE);		// Initialize SPI Module (Fosc/80 = 1MHz)
	UART5_Init(9600);		// baud rate = 9600
}


int main(){
	
	init();

	cs = 0;				// select ADC (active low)

	SPI2_Write(0x06);	// reset command
	Delay_us(18);		// 18 microsecond delay (18 SCLK cycles required)
	SPI2_Write(0x08);	// start command (to start conversions)
	Delay_us(4);		// 4 SCLK cycles
	SPI2_Write(0x11);	// stop read data continuously mode
	Delay_us(4);		// 4 SCLK cycles		

	Delay_us(8);

	// Set GPIO pins as output
	SPI2_Write(0x4E);	// WREG command 1st opcode byte for GPIO register
	Delay_us(8);		// 8 SCLK cycles
	SPI2_Write(0x00);	// WREG command 2nd opcode byte for GPIO register
	Delay_us(8);
	SPI2_Write(0x00);	// lower nibble sets the 4 GPIO pins as output and upper nibble sets their values to 0

	Delay_us(8);

	UART5_Write_Text("Reading Registers\n");
	SPI2_Write(0x20);			// RREG command 1st opcode byte
	Delay_us(8);				// 8 SCLK cycles required
	SPI2_Write(0x19);			// RREG command 2nd opcode byte
	Delay_us(8);

	int i = 0;
	while(i<26){
		reg[i] = SPI2_Read(0);	// reading register data
		Delay_us(8);			// 8 SCLK cycles
		i++;
	}

	i = 0;
	while(i<26){
		char temp1 = reg[i] & 0x0F;		// separating the nibbles to see data in hex format (lower nibble here)
		char temp2 = reg[i] >> 4;		// upper nibble
		UART5_Write(temp2 + 48);		// ASCII value of 0 is 48
		UART5_Write(temp1 + 48);
		UART5_Write_Text("\n");
		i++;
	}

	while(1){

	}
	
	return 0;
}