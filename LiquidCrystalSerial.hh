#ifndef LIQUID_CRYSTAL_HH
#define LIQUID_CRYSTAL_HH

// TODO: Proper attribution

#include <stdint.h> //扩展整数类型
#include <avr/pgmspace.h>//支持访问flash中的数据？
#include "Pin.hh" /**  /shared/Pin.hh **/
#include "Configuration.hh"// 配置文件？

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

//液晶串口类
class LiquidCrystalSerial {
public://可以被使用该类型的所有代码访问
  LiquidCrystalSerial(Pin strobe, Pin data, Pin CLK);//strobe：选通脉冲

  void init(Pin strobe, Pin data, Pin CLK);
    
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  //列，行，字符大小=5*8点阵？ uint8_t=unsigned char 8位无符号=1字节


  void clear();//清屏？
  void home();//主界面？

  void noDisplay();//无显示
  void display();//显示
  void noBlink();//不闪烁
  void blink();//闪烁
  void noCursor();//无光标
  void cursor();//光标
  void scrollDisplayLeft();//屏幕左滚(scroll)
  void scrollDisplayRight();//屏幕右滚(scroll)
  void leftToRight();//从左到右
  void rightToLeft();//从右到左
  void autoscroll();//自动滚屏
  void noAutoscroll();//不自动滚屏

  void createChar(uint8_t, uint8_t[]);//创建字符？
  void setCursor(uint8_t, uint8_t); //设置光标
  void setCursorExt(int8_t col, int8_t row);//光标出口？

  virtual void write(uint8_t);//虚函数 派生类中重载函数名write

  /** Added by MakerBot Industries to support storing strings in flash **/
  /** 在flash中存放字符串的支持 **/

  void writeInt(uint16_t value, uint8_t digits);
  void writeInt32(uint32_t value, uint8_t digits);

  void writeString(char message[]);//写字符串

#ifdef STACK_PAINT //如果没有define  STACK_PAINT
  void writeFloat(float value, uint8_t decimalPlaces, uint8_t rightJustifyToCol);
  //写浮点数（浮点值，小数点位置，？）
#endif

  /** Display the given line until a newline or null is encountered.显示给定的line直到遇到一个新的line或者空line
   * Returns a pointer to the first character not displayed.返回一个指针，指向没有显示的第一个字符
   */

  char* writeLine(char* message);//写一行

  void writeFromPgmspace(const unsigned char message[]);//从 flash中写数据

  void command(uint8_t);

private://可被其他类成员访问
  void send(uint8_t, bool);//发送what？
  void writeSerial(uint8_t);//写串口
  void load(uint8_t);//加载what？
  void pulseEnable(uint8_t value);//脉冲使能？

  Pin _strobe_pin; // LOW: command.  HIGH: character. 选通脉冲 0：写命令 1：写数据
  Pin _data_pin; // LOW: write to LCD.  HIGH: read from LCD. 0：向LCD写数据 1：从LCD读数据
  Pin _clk_pin; // activated by a HIGH pulse. 时钟，高电平启动

  uint8_t _displayfunction;//显示参数
  uint8_t _displaycontrol;//显示控制
  uint8_t _displaymode;//显示模式

  uint8_t _initialized;//初始化？

  uint8_t _xcursor;//光标x
  uint8_t _ycursor;//光标y

  uint8_t _numlines,_numCols;
};

#endif // --->  LIQUID_CRYSTAL_HH  <----表明endif的结合
