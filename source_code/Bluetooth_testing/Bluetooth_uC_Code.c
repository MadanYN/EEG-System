// This code is to transmit 24 channel data from clicker development board to PC through UART using HC05

// function to generate random number in [lower,upper]
int random(int lower, int upper){ 
    return (rand()%(upper - lower + 1)) + lower;
}

void main() {
    int channels = 24; //number of channels to transmit
    UART5_Init(38400); // initialising UART 
    Delay_ms(100);     
    while (1) {
        UART5_Write('b'); // begin byte
        for(int i=0;i<24;i++){ //sending 24 channel data 
            UART5_Write_Text(random(-32768,32768)); // 24 channel data is signed 16 bit value hence [-2^15,2^15]
        }
        Delay_ms(10); 
    } 
}