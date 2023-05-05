/*	EE344 - Electronic Design Lab
	Groups: TUES-29 and MON-30
	Project: EEG Acquisition System
  
	This code is for transmitting the data converted by ADS1194 to PC, via bluetooth or UART, using PIC32MX clicker board
	IDE used - NECTO Studio v3.0.0
	
	Clock mode: internal

	Supply from clicker board:
	DVDD - 3.3V
	AVDD - 5V "CAUTION: Supply voltages higher than 5.25V can permanently damage the ADC"
	AVSS - GND

	RESET - pin RD7
	DRDY(INT) - pin RD11
	
	UART Module (clicker):
	Tx - pin RB14
	Rx - pin RB8
*/


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

	// Set reference voltage to 4V and bias (RLDREF) to (AVDD-AVSS)/2
	SPI2_Write(0x43);	// WREG command 1st opcode byte for CONFIG3 register
	Delay_us(8);		// 8 SCLK cycles
	SPI2_Write(0x00);	// WREG command 2nd opcode byte for CONFIG3 register
	Delay_us(8);
	SPI2_Write(0x48);	// writing to the register

	Delay_us(8);

	short adc_data;			// 16 bit data will arrive from ADC

	while(1){
		if(drdy == 0){				// DRDY' active low => data ready when 0
			SPI2_Write(0x12);		// RDATA command opcode to get converted data from ADC
			Delay_us(8);			// 8 SCLK cycles
			Delay_us(24);			// 24 SCLK cycles for status data
			int i = 0;
			while(i<4){
				adc_data = SPI2_Read(0);				// channel (i+1) data
				char temp1 = adc_data & 0x0F;			// lowest nibble
				char temp2 = (adc_data >> 4) & 0x0F;	// second lowest nibble
				char temp3 = (adc_data >> 8) & 0x0F;	// second highest nibble
				char temp4 = adc_data >> 12;
				UART5_Write(temp4 + 48);			//ASCII value of 0 is 48
				UART5_Write(temp3 + 48);
				UART5_Write(temp2 + 48);
				UART5_Write(temp1 + 48);
				UART5_Write_Text("\n");
				Delay_us(16);						// 16 SCLK cycles before next channel data arrives
				i++;
			}
		}
	}

	return 0;
}