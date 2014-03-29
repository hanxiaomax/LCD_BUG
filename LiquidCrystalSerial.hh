#ifndef LIQUID_CRYSTAL_HH
#define LIQUID_CRYSTAL_HH

// TODO: Proper attribution

#include <stdint.h>
#include <avr/pgmspace.h>
#include "Pin.hh"
#include "Configuration.hh"

// commands 指令
#define LCD_CLEARDISPLAY 0x01//请屏
#define LCD_RETURNHOME 0x02//返回？
#define LCD_ENTRYMODESET 0x04//进入模式设置
#define LCD_DISPLAYCONTROL 0x08//显示设置
#define LCD_CURSORSHIFT 0x10//光标移动？
#define LCD_FUNCTIONSET 0x20//函数设置
#define LCD_SETCGRAMADDR 0x40//设置自定义字型(cgram)地址(addr)
#define LCD_SETDDRAMADDR 0x80//设置显示数据随机存储器(ddram)地址

// flags for display entry mode  显示输入模式标志？
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control 显示开关控制标志
#define LCD_DISPLAYON 0x04  //显示on
#define LCD_DISPLAYOFF 0x00 //显示off
#define LCD_CURSORON 0x02   //光标on
#define LCD_CURSOROFF 0x00  //光标off
#define LCD_BLINKON 0x01    //闪烁on
#define LCD_BLINKOFF 0x00   //闪烁off

// flags for display/cursor shift  显示/光标 的移动
#define LCD_DISPLAYMOVE 0x08    //显示移动
#define LCD_CURSORMOVE 0x00     //光标移动
#define LCD_MOVERIGHT 0x04      //右移
#define LCD_MOVELEFT 0x00       //左移

// flags for function set   函数设置标志
#define LCD_8BITMODE 0x10   //8bit模式
#define LCD_4BITMODE 0x00   //4bit模式
#define LCD_2LINE 0x08      //2行
#define LCD_1LINE 0x00      //1行
#define LCD_5x10DOTS 0x04   //5*10 点阵？
#define LCD_5x8DOTS 0x00    //5*8 点阵？

// TODO:  make variable names for rs, rw, e places in the output vector

class LiquidCrystalSerial {
public:
  LiquidCrystalSerial(Pin strobe, Pin data, Pin CLK);

  void init(Pin strobe, Pin data, Pin CLK);
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
  void setCursorExt(int8_t col, int8_t row);
  virtual void write(uint8_t);

  /** Added by MakerBot Industries to support storing strings in flash **/
  void writeInt(uint16_t value, uint8_t digits);
  void writeInt32(uint32_t value, uint8_t digits);

  void writeString(char message[]);
#ifdef STACK_PAINT
  void writeFloat(float value, uint8_t decimalPlaces, uint8_t rightJustifyToCol);
#endif

  /** Display the given line until a newline or null is encountered.
   * Returns a pointer to the first character not displayed.
   */
  char* writeLine(char* message);

  void writeFromPgmspace(const unsigned char message[]);

  void command(uint8_t);

private:
  void send(uint8_t, bool);
  void writeSerial(uint8_t);
  void load(uint8_t);
  void pulseEnable(uint8_t value);

  Pin _strobe_pin; // LOW: command.  HIGH: character.
  Pin _data_pin; // LOW: write to LCD.  HIGH: read from LCD.
  Pin _clk_pin; // activated by a HIGH pulse.

  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;

  uint8_t _initialized;

  uint8_t _xcursor;
  uint8_t _ycursor;

  uint8_t _numlines,_numCols;
};

#endif // LIQUID_CRYSTAL_HH
