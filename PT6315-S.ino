/******************************************************
 * Command 1: Display Mode Setting Command 
 * Command 2: Data Setting Command 
 * Command 3: Address Setting Command Data 1 to n: Transfer Display Data (24 Bytes max.) 
 * Command 4: Display Control Command
 * 
 * COMMAND 1: DISPLAY MODE SETTING COMMANDS
 * When Power is turned “ON”, the 8-digit, 8-segment mode is selected. MSB                                                          
 * LSB        0     0     -      -    b3   b2   b1   b0      Not Relevant  Display Mode Settings:                                                  
 * 0000:       4       digits,       12       segments                                                        
 * 0011:       5       digits,       11       segments                                                        
 * 0100:       6       digits,       10       segments                                                        
 * 0101:       7       digits,       9       segments                                                        
 * 0110:       8       digits,       8       segments       
 * 
 * COMMAND 2: DATA SETTING COMMANDS
 * When power is turned ON, the bit 4 to bit 1 (b3 to b0) are given the value of “0”. MSB                                                          
 * LSB        0     1     -      -    b3   b2   b1   b0                         Not Relevant    Data Write & Read Mode Settings:                                                         
 *                                    00:       Write       Data       to       Display       Mode                                                               
 *                                    01:       Read       Key       Data                                                               
 *                                    10:       Read       Key       Data               11: Irrelevant                                                         
 * Address       Increment       Mode       Settings       (Display       Mode):                                                               
 * 0:       Increment       Address       after       Data       has       been       Written                                                               
 * 1:       Fixed       Address       Mode Settings:                                                         
 * 0:       Normal       Operation       Mode                                                               
 * 1:       Test       Mode       
 * 
 * COMMAND 3: ADDRESS SETTING COMMANDS
 * Please refer to the diagram below. MSB                                                          
 * LSB        1     1     0    b4   b3   b2   b1   b0         
 * Address: 00H to 1DH 
 * 
 * COMMAND 4: DISPLAY CONTROL COMMANDS The Display Control Commands are used to turn ON or OFF a display. It also used to set the pulse width. Please refer to the diagram below. When the power is turned ON, a 1/16 pulse width is selected and the displayed is turned OFF (the key scanning is stopped). MSB                                                          
 * LSB        1     0     -      -    b3   b2   b1   b0                         Not Relevant    Dimming Quantity Settings:                                                         
 * 000:       Pulse       width       =       1/16                                                               
 * 001:       Pulse       width       =       2/16                                                               
 * 010:       Pulse       width       =       4/16                                                               
 * 011:       Pulse       width       =       10/16                                                               
 * 100:       Pulse       width       –       11/16                                                               
 * 101:       Pulse       width       =       12/16                                                               
 * 110:       Pulse       width       =       13/16                                                               
 * 111:       Pulse       width       =       14/16               
 * Display Settings:                                                         
 * 0:       Display       Off       (Key       scan       continues)               
 * 1: Display On 
*/

/* Port controls  (Platform dependent) */
//#define CS_LOW()  PORTB &= ~1     /* CS=low */
//#define CS_HIGH() PORTB |= 1      /* CS=high */

//#define LCD_Port PORTB
#define VFD_stb 7 // Must be pulsed to LCD fetch data of bus
#define VFD_clk 6 // if 0 is a command, if 1 is a data0
#define VFD_in 5// If 0 write LCD, if 1 read of LCD

#define delay_tcsh _delay_us(16)
#define AdjustPins    PORTB // before is C, but I'm use port C to VFC Controle signals

/*****************************************************************************/
void writeChar(void){
  for(unsigned char v=0; v<29; v=v+3){  //The PT6315-S have a max of 8 grid's!
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11000000 | v); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address

    send_data(0b00001111); // data
    send_data(0b00001111); // data
    send_data(0b00111100); // data
    
    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
    Serial.print("Byte: ");Serial.print(v, HEX);Serial.println(";!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  }
}

void writeFixGrid(void){
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11010101); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address

    send_data(0b00001111); // data   8:1 only use first 4 bits
    send_data(0b00001111); // data  16:9 only use first 4 bits
    send_data(0b00111100); // data  24:17 use the 4 bits of middle
    
    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
}

void writeHELLO(void){
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11000000); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address
//Grid1 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid2  "H" 
    send_data(0b00001000); // data   8:1 only use first 4 bits
    send_data(0b00001111); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid3 "E"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000110); // data  16:9 only use first 4 bits
    send_data(0b00000100); // data  24:17 use the 4 bits of middle
//Grid4 "L"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000100); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid  
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid5 "L"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000100); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid6 "O"    
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00001101); // data  16:9 only use first 4 bits
    send_data(0b00000100); // data  24:17 use the 4 bits of middle
//Grid7 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
   
    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
}

void writeAnd(void){
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11000000); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address
//Grid1 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid2  "A" 
    send_data(0b00001000); // data   8:1 only use first 4 bits
    send_data(0b00001111); // data  16:9 only use first 4 bits
    send_data(0b00000100); // data  24:17 use the 4 bits of middle
//Grid3 "n"
    send_data(0b00001000); // data   8:1 only use first 4 bits
    send_data(0b00000011); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid4 "d"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00001011); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid  
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid5 "L"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid6 "O"    
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid7 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle

    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
}

void writeJob(void){
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11000000); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address
//Grid1 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid2  "J" 
    send_data(0b00000100); // data   8:1 only use first 4 bits
    send_data(0b00001001); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid3 "o"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000011); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid4 "b"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000111); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid  
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid5 "L"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid6 "O"    
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid7 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle

    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
}

void writeFun(void){
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND = (1 << VFD_stb);
    __delay_us(48);
    send_data(0b11000000); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address
//Grid1 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid2  "F" 
    send_data(0b00001000); // data   8:1 only use first 4 bits
    send_data(0b00000110); // data  16:9 only use first 4 bits
    send_data(0b00000100); // data  24:17 use the 4 bits of middle
//Grid3 "u"
    send_data(0b00001100); // data   8:1 only use first 4 bits
    send_data(0b00000001); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid4 "n"
    send_data(0b00001000); // data   8:1 only use first 4 bits
    send_data(0b00000011); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Skip grid  
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid5 "L"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid6 "O"    
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
//Grid7 "empty"
    send_data(0b00000000); // data   8:1 only use first 4 bits
    send_data(0b00000000); // data  16:9 only use first 4 bits
    send_data(0b00000000); // data  24:17 use the 4 bits of middle
  
    PIND = (1 << VFD_stb);
    __delay_us(48);
//
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    // ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4 Dimming adjust, 7 is maximum of bright
    PIND = (1 << VFD_stb);
    __delay_us(48);
    delay(60000);
    clearCmd();
    delay(60000);
}

void wrSegGrid(unsigned int segment, unsigned int grid){
   unsigned char dd = 0xFF;
  for(unsigned char v=0; v<8; v++){  //The PT6315-S have a max of 8 grid's! Note: each grid use 3 bytes to fill segments... see map of RAM
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    send_data((0b11000000) | grid); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address

    dd= (segment & 0x000000ffUL) >>  0;
    send_data(dd);
   Serial.print(segment, HEX);Serial.print(", ");Serial.print(grid, HEX);Serial.println(";");
   
    PIND = (1 << VFD_stb);
    __delay_us(48);

     //ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4
    __delay_us(48);
    delay(200);
  }
}

void delay_ms(uint16_t ms) {
  //while loop that creates a delay for the duration of the millisecond countdown
  while ( ms ){
    _delay_ms(1);
    ms--;
  }
}

void __delay_ms(int x) {
  int y = (x * 1000) / 12;
  while (--y != 0)
    continue;
}

void __delay_us(int x) {
  int y = x / 12; // Note: x must be bigger at least 12
  while (--y != 0)
    continue;
}

void wrData(unsigned char ch, unsigned char grid){
  unsigned char data = 0x00;
  data=ch;
   PIND = (1 << VFD_stb);
    __delay_us(48);
    sendCmd(0b01000000); // cmd 2  Write to display, start fix and display ON
    PIND &= ~(1 << VFD_stb);
    __delay_us(48);
    send_data((0b11000000) | grid); //cmd 3 Memory 0x00 to 0x1D, Each grid use 3 bytes here define the 1º, 2º, 3º until max address
        
    send_data(ch); // data
        
    PIND = (1 << VFD_stb);
    __delay_us(48);

//    sendCmd(0b00000101); // cmd1 Here I define the 7 grids and 9 Segments ATT: this is at PT6515-S (DIP 28pins)
    sendCmd((0b10001000) | 7); //cmd 4
    __delay_us(48);
    Serial.print("Grid: ");Serial.println(grid, HEX);
}

void send_data(byte a) {
  unsigned char data = 0x00; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask
  data = a; // This pass the byte received to local variable!
  //This funtion don't send the strobe signal, to be used in burst data send
  PIND = (0 << VFD_clk);
  for (mask = 0b00000001; mask > 0; mask <<= 1) { //iterate through bit mask
    if (data & mask) { // if bitwise AND resolves to true
      PORTD = (1 << VFD_in);
      Serial.print(VFD_in, DEC);
    }
    else { //if bitwise and resolves to false
      PORTD = (0 << VFD_in); // send 0
      Serial.print(VFD_in, DEC);
    }
    _delay_us(48); //delay
    PIND = (1 << VFD_clk);
    _delay_us(48); //delay
    PIND &= ~(0 << VFD_clk);
    _delay_us(48); //delay
  }
  Serial.println();
  Serial.print("Sended a byte data: ");Serial.println(data, HEX);
}

void sendCmd(uint8_t a) {
  unsigned char data = 170; //value to transmit, binary 10101010
  unsigned char mask = 1; //our bitmask

  data = a;
  PIND &= ~(1 << VFD_stb);
  _delay_us(48);
  for (mask = 0b00000001; mask > 0; mask <<= 1) { //iterate through bit mask
    PIND = (0 << VFD_clk);
    if (data & mask) { // if bitwise AND resolves to true
      PORTD = (1 << VFD_in);
    }
    else { //if bitwise and resolves to false
      PORTD = (0 << VFD_in); // send 0
    }
    _delay_us(48); //delay
    PIND = (1 << VFD_clk);
    _delay_us(48); //delay
    PIND &= ~(0 << VFD_clk);
    _delay_us(48); //delay
  }
  PIND = (1 << VFD_stb);
  _delay_us(48);
}

void pt6315_init(void) {
  delay_ms(200); //power_up delay
  // Configure VFD display (grids)
  sendCmd(0b00000101);//  (0b01000000)    cmd2 7 grids and 9 segments.
  delay_tcsh; // 4/16 4 grids & 16 segments
  // Write to memory display, increment address, normal operation
  sendCmd(0b01000000);// cmd2 write to display, incremente address, Mode normal
  delay_tcsh;
  // Address 00H - 15H ( total of 11*2Bytes=176 Bits)
  sendCmd(0b11000000);//(BIN(01100110)); //(BIN(01100110)));
  delay_tcsh;
  // set DIMM/PWM to value
  sendCmd((0b10001000) | 7);//0 min - 7 max  )(0b01010000)
  delay_tcsh;
}

void clearCmd(void) {
  /*
    Here I clean all registers
    Could be done only on the number of grid
    to be more fast. The 12 * 3 bytes = 36 registers
  */
  sendCmd(0b01000000); // cmd 2
  PIND &= ~(1 << VFD_stb);
  __delay_us(48);
  send_data(0b11000000); //cmd 3

  for (unsigned int i = 0; i < 29; i++) {  //0x1D is 29
    send_data(0b00000000); // data
  }

  PIND = (1 << VFD_stb);
  __delay_us(48);
}

void setup() {
  // put your setup code here, to run once:
  pinMode( VFD_stb, OUTPUT);// Must be pulsed to LCD fetch data of bus
  pinMode( VFD_in, OUTPUT);//  If 0 write LCD, if 1 read of LCD
  pinMode( VFD_clk, OUTPUT);// if 0 is a command, if 1 is a data0
  pinMode( 13, OUTPUT); //Pin of pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
 
  //Set PORT
  DDRD = 0xFF;
  PORTD = 0xFF;

//  DDRB = 0xFF;
//  PORTB = 0xFF;
  
  pt6315_init();
  clearCmd();
  
  cli();  //Only put here the CLI to avoid colision with time of function "delay"
/*
  TCCR1B = (1 << CS12 | 1 << WGM12);
  OCR1A = 32768 - 1; //32768-1;
  TIMSK1 = 1 << OCIE1A;
*/
//set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15623;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  //a = 0x33;
  //b = 0x01;
  //You write a 1 to make a pin an output and a 0 to make it an input.
  CLKPR = (0x80);
  
  //only here I active the enable to allow run the test of VFD
  
  //sei();  //Deactivating of interrupts to debug commands!
}

void loop() {
  delayMicroseconds(200);
  pt6315_init();
  
      while(1){
      
          clearCmd();
          writeFixGrid(); //Here we write fixed to one GRID, change it on function to see the change!
          delay(60000);
          writeHELLO(); //Write fix a word, sending total of area ram map!
          delay(60000);
          writeJob(); //Write fix a word, sending total of area ram map!
          delay(60000);
          writeAnd(); //Write fix a word, sending total of area ram map!
          delay(60000);
          writeFun(); //Write fix a word, sending total of area ram map!
          delay(60000);
       
          for(uint8_t c=0; c < 5; c++){  //This is only to debug!
            writeChar();
                digitalWrite(LED_BUILTIN, HIGH);
                delay_ms(500);
                digitalWrite(LED_BUILTIN, LOW);
          }
          Serial.println("I'm in while!"); 
          clearCmd();
          delay_ms(100);  
      }

}
