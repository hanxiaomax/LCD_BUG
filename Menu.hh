#ifndef MENU_HH_
#define MENU_HH_

#include "Types.hh"
#include "ButtonArray.hh"
#include "LiquidCrystalSerial.hh"
#include "Configuration.hh"
#include "CircularBuffer.hh"
#include "Timeout.hh"
#include "Host.hh"
#include "UtilityScripts.hh"
#include "Point.hh"


/// this puts a max value on the number of items per menu
/// TODO : alert during build if number of items exceeds this.
const static uint8_t MAX_INDICES = 20;

/// states for Welcome Menu
enum WeclomeStates{
	WELCOME_START,
	WELCOME_BUTTONS1,
	WELCOME_BUTTONS2,
	WELCOME_EXPLAIN,
	WELCOME_LEVEL,
	WELCOME_LEVEL_ACTION,
	WELCOME_LEVEL_OK,
	WELCOME_LOAD_PLASTIC,
	WELCOME_LOAD_ACTION,
	WELCOME_READY,
	WELCOME_LOAD_SD,
	WELCOME_PRINT_FROM_SD,
	WELCOME_DONE
};

/// states for Welcome Menu
enum FilamentStates{
	FILAMENT_HEATING,
	FILAMENT_EXPLAIN2,
	FILAMENT_EXPLAIN3,
	FILAMENT_EXPLAIN4,
	FILAMENT_HEAT_BAR,
	FILAMENT_WAIT,
	FILAMENT_START,
	FILAMENT_TUG,
	FILAMENT_STOP,
	FILAMENT_OK,
	FILAMENT_DONE,
	FILAMENT_EXIT,
	FILAMENT_TIMEOUT
};

enum FilamentScript{
	FILAMENT_RIGHT_FOR,
	FILAMENT_LEFT_FOR,
	FILAMENT_RIGHT_REV,
	FILAMENT_LEFT_REV,	
	FILAMENT_STARTUP_SINGLE,
	FILAMENT_STARTUP_DUAL,
	};

enum AlignmentState {
	ALIGNMENT_START,
	ALIGNMENT_PRINT,
	ALIGNMENT_EXPLAIN1,
	ALIGNMENT_EXPLAIN2,
	ALIGNMENT_SELECT,
	ALIGNMENT_END,
	ALIGNMENT_QUIT
};


/// The screen class defines a standard interface for anything that should
/// be displayed on the LCD.
class Screen {
public:
        /// Get the rate that this display should be updated. This is called
        /// after every screen display, so it can be used to dynamically
        /// adjust the update rate. This can be as fast as you like, however
        /// refreshing too fast during a build is certain to interfere with
        /// the serial and stepper processes, which will decrease build quality.
        /// \return refresh interval, in microseconds.
	virtual micros_t getUpdateRate() = 0;

        /// Update the screen display,
        /// \param[in] lcd LCD to write to
        /// \param[in] forceRedraw if true, redraw the entire screen. If false,
        ///                        only updated sections need to be redrawn.
	virtual void update(LiquidCrystalSerial& lcd, bool forceRedraw) = 0;

        /// Reset the screen to it's default state
	virtual void reset() = 0;

        /// Get a notification that a button was pressed down.
        /// This function is called for every button that is pressed. Screen
        /// logic can be updated, however the screen should not be redrawn
        /// until update() is called again.
        ///
        /// Note that the current implementation only supports one button
        /// press at a time, and will discard any other events.
        /// \param button Button that was pressed
	virtual void notifyButtonPressed(ButtonArray::ButtonName button) = 0;

	virtual void pop_actions() {return;} 
};


/// The menu object can be used to display a list of options on the LCD
/// screen. It handles updating the display and responding to button presses
/// automatically.
class Menu: public Screen {
public:
	micros_t getUpdateRate() {return 500L * 1000L;}

	void update(LiquidCrystalSerial& lcd, bool forceRedraw);

	void reset();

	virtual void resetState();

	void notifyButtonPressed(ButtonArray::ButtonName button);

protected:

	bool needsRedraw;               ///< set to true if a menu item changes out of sequence
	bool lineUpdate;				///< flags the menu to update the current line
	bool cursorUpdate;				///< flag to update the menu cursor
	volatile uint8_t itemIndex;     ///< The currently selected item
	uint8_t lastDrawIndex;          ///< The index used to make the last draw
	volatile uint8_t zeroIndex;     ///< The index corresponding to the zeroth display line
	uint8_t lastZeroIndex;          ///< The last zeroIndex displayed
	uint8_t itemCount;              ///< Total number of items
	uint8_t firstItemIndex;         ///< The first selectable item. Set this
                                  ///< to greater than 0 if the first
                                  ///< item(s) are a title)
	bool selectMode;       			///< true if in counter change state
	uint8_t selectIndex;        	///< The currently selected item, in a counter change state
	bool sliding_menu;				///< the menu either slides, or scrolls down in groups of SCREEN_HEIGHT
                                      
	uint8_t counter_item[MAX_INDICES];	///< array defining which idices are counters

        /// Draw an item at the current cursor position.
        /// \param[in] index Index of the item to draw
        /// \param[in] LCD screen to draw onto
	virtual void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

        /// Handle selection of a menu item
        /// \param[in] index Index of the menu item that was selected
	virtual void handleSelect(uint8_t index);

        /// Handle the menu being cancelled. This should either remove the
        /// menu from the stack, or pop up a confirmation dialog to make sure
        /// that the menu should be removed.
	void handleCancel();
	
		/// Handle update of a menu counter item
        /// \param[in] index Index of the menu item that was selected
        /// \param[up] direction of counter, up or down
	virtual void handleCounterUpdate(uint8_t index, bool up){return;}
};

/// Display a welcome splash screen, that removes itself when updated.
class SplashScreen: public Screen {

private:
	bool hold_on;
public:
	micros_t getUpdateRate() {return 50L * 1000L;}

	void SetHold(bool on);
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);

	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class FilamentOKMenu: public Menu {
public:
	FilamentOKMenu();
    
	void resetState();
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
    
};

class ReadyMenu: public Menu {
public:
	ReadyMenu();
    
	void resetState();
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
};

class LevelOKMenu: public Menu {
public:
	LevelOKMenu();
    
	void resetState();
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
};

class CancelBuildMenu: public Menu {
public:
	CancelBuildMenu();
    
	void resetState();
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
      
  bool paused;
};

class BuildStats: public Screen {

private:

	const static uint8_t UPDATE_COUNT_MAX = 20;
	uint8_t update_count;

public:

	micros_t getUpdateRate() {return 500L * 1000L;}
	
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
	
	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class BuildFinished: public Screen {

private:
	bool waiting;

public:
	
	micros_t getUpdateRate() {return 500L * 1000L;}
	
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
	
	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class BotStats: public Screen {

private:

public:

	micros_t getUpdateRate() {return 500L * 1000L;}
	
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
	
	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
};


/// Display a message for the user, and provide support for
/// user-specified pauses.
class MessageScreen: public Screen {
private:
	uint8_t x, y;
	const static int BUF_SIZE = LCD_SCREEN_WIDTH*LCD_SCREEN_HEIGHT + 1;
	char message[BUF_SIZE];
	uint8_t cursor;
	bool needsRedraw;
	bool incomplete;
	bool waiting_for_user;
	Timeout timeout;
    
public:
	MessageScreen() : needsRedraw(false) { message[0] = '\0'; }

	void setXY(uint8_t xpos, uint8_t ypos) { x = xpos; y = ypos; }

	void addMessage(CircularBuffer& buf);
	void addMessage(const unsigned char * msg);
	void clearMessage();
	void setTimeout(uint8_t seconds);//, bool pop);
	void refreshScreen();

	micros_t getUpdateRate() {return 100L * 1000L;}
  
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
	
	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
  
	bool screenWaiting(void);
	void WaitForUser(bool wait);
};


class JogMode: public Screen {
private:
	enum distance_t {
	  DISTANCE_SHORT,
	  DISTANCE_LONG,
	};
	enum jogmode_t {
		JOG_MODE_X,
		JOG_MODE_Y,
		JOG_MODE_Z
	};

	distance_t jogDistance;
	bool distanceChanged, modeChanged;
	jogmode_t  JogModeScreen;

	void jog(ButtonArray::ButtonName direction);

public:
	micros_t getUpdateRate() {return 50L * 1000L;}

	void update(LiquidCrystalSerial& lcd, bool forceRedraw);

	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
     
};

/// This is an easter egg.
class SnakeMode: public Screen {

#define MAX_SNAKE_SIZE 20      ///< Maximum length our snake can grow to
#define APPLES_BEFORE_GROW 4   ///< Number of apples the snake must eat before growing
#define START_SPEED  60        ///< Starting speed, in screen refresh times per turn


private:
	micros_t updateRate;

	struct coord_t {
		int8_t x;
		int8_t y;
	};

	enum direction_t {
	  DIR_NORTH,
	  DIR_EAST,
	  DIR_SOUTH,
	  DIR_WEST
	};

	int snakeLength;					// Length of our snake; this grows for every x 'apples' eaten
	coord_t snakeBody[MAX_SNAKE_SIZE];	// Table of each piece of the snakes body
	bool snakeAlive;					// The state of our snake
	direction_t snakeDirection;			// The direction the snake is heading
	coord_t applePosition;				// Location of the apple
	uint8_t applesEaten;				// Number of apples that have been eaten

public:
	micros_t getUpdateRate() {return updateRate;}


	// Refresh the display information
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);

	void reset();

	// Get notified that a button was pressed
        void notifyButtonPressed(ButtonArray::ButtonName button);
};

class SDMenu: public Menu {
public:
	SDMenu();

	void resetState();
	
	micros_t getUpdateRate() {return 50L * 1000L;}

	static const uint8_t SD_CARD_MAX_FILE_LENGTH = 31; 

protected:
	bool cardNotFound;
	bool cardReadError;
	bool cardBadFormat;
	bool cardTooBig;
	
	uint8_t countFiles();

	bool getFilename(uint8_t index,
                       char buffer[],
                       uint8_t buffer_size);

	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

	void handleSelect(uint8_t index);

};

/// load / unload filament options
class FilamentScreen: public Screen {
    
private:
    
	uint8_t filamentState;
	uint8_t axisID, toolID;
	bool forward;
	bool dual;
	bool startup;
	Timeout filamentTimer;
	bool toggleBlink;
	int toggleCounter;
	bool helpText;   
	bool needsRedraw;
	uint16_t filament_heat_temp[2];
    
	void startMotor();
	void stopMotor();
    
public:
	micros_t getUpdateRate() {return 500L * 1000L;}
    
	void setScript(FilamentScript script);
    
    
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
    
	void reset();
    
	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class SelectAlignmentMenu : public Menu{
	
public:
	SelectAlignmentMenu();
    
protected:
	int8_t xCounter;
	int8_t yCounter;
    
	void resetState();
    
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
    
	void handleCounterUpdate(uint8_t index, bool up);
};

class NozzleCalibrationScreen: public Screen {
	
private:
    
	uint8_t alignmentState;
	bool needsRedraw;               ///< set to true if a menu item changes out of sequence
	
public:
	micros_t getUpdateRate() {return 50L * 1000L;}
    
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
    
	void reset();
    
	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class ActiveBuildMenu: public Menu {
	
private:
	
	bool FanOn;
	int8_t LEDColor;

	bool is_paused;
	bool is_sleeping;
	
public:
	ActiveBuildMenu();
    
	void resetState();
	
	micros_t getUpdateRate() {return 100L * 1000L;}
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
	
	void handleCounterUpdate(uint8_t index, bool up);
};

/// Display a welcome splash screen on first user boot
class WelcomeScreen: public Screen {
    
private:
	int8_t welcomeState;
	int level_offset;
    
	bool needsRedraw;
public:
	micros_t getUpdateRate() {return 50L * 1000L;}
    
    
	void update(LiquidCrystalSerial& lcd, bool forceRedraw);
    
	void reset();
    
	void notifyButtonPressed(ButtonArray::ButtonName button);
};

class PreheatSettingsMenu: public Menu {
public:
	PreheatSettingsMenu();
	
	micros_t getUpdateRate() {return 50L * 1000L;}
    
protected:
	int16_t counterRight;
	int16_t counterLeft;
	int16_t counterPlatform;
	bool singleTool;
    
	void resetState();
    
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
    
	void handleCounterUpdate(uint8_t index, bool up);
};

class ResetSettingsMenu: public Menu {
public:
	ResetSettingsMenu();
    
	void resetState();
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
};

class MonitorMode: public Screen {
private:
	uint8_t updatePhase;
	uint8_t buildPercentage;
	bool singleTool; 
	bool heating;
	bool hasHBP;
	uint8_t num_update_phases;
    
public:
	micros_t getUpdateRate() {return 500L * 1000L;}


	void update(LiquidCrystalSerial& lcd, bool forceRedraw);

	void reset();

	void notifyButtonPressed(ButtonArray::ButtonName button);
    
	void setBuildPercentage(uint8_t percent);
};

class HeaterPreheat: public Menu {
	
public:
	
	HeaterPreheat();
	
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

	void handleSelect(uint8_t index);

private:

	int8_t _rightActive, _leftActive, _platformActive;
    
	void storeHeatByte();
	void resetState();
     
	bool singleTool;

};

class SettingsMenu: public Menu {
public:
	SettingsMenu();
	
	micros_t getUpdateRate() {return 100L * 1000L;}
    
    
protected:
	void resetState();
    
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
	
	void handleCounterUpdate(uint8_t index, bool up);
    
private:
    
	uint8_t  singleExtruder;
	uint8_t  soundOn;
	int8_t   LEDColor;
	int8_t   heaterTimeout;
	uint8_t  heatingLEDOn;
	uint8_t  helpOn;
	uint8_t  accelerationOn;
	uint8_t  HBPPresent;
    
};

class StopHeightMenu: public Menu {
public:
	StopHeightMenu();
	
	micros_t getUpdateRate() {return 100L * 1000L;}
    
    
protected:
	void resetState();
    
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
	
	void handleCounterUpdate(uint8_t index, bool up);
    
private:
    
	// Enable for stopping at a specified height to change filament
	uint8_t  stopHeightEnabled;
	// Value in millimeters for stopping at a specified height to change filament
  int16_t   stopHeightValue;
  int32_t   stop_height_max_mm;  
};

class FilamentMenu: public Menu {
public:
	FilamentMenu();
	void pop_actions();
    
    
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);
    
	void handleSelect(uint8_t index);
	
	void resetState();
    
private:

	bool singleTool;
    
};


class UtilitiesMenu: public Menu {
public:
	UtilitiesMenu();
	
	micros_t getUpdateRate() {return 100L * 1000L;}
	
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

	void handleSelect(uint8_t index);
	
	void resetState();

private:
    
	bool singleTool; 
	bool stepperEnable;
	bool blinkLED;

};


class InfoMenu: public Menu {
public:
	InfoMenu();
	
	micros_t getUpdateRate() {return 100L * 1000L;}
	
protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

	void handleSelect(uint8_t index);
	
	void resetState();

private:

	bool singleTool;
};



class MainMenu: public Menu {
public:
	MainMenu();


protected:
	void drawItem(uint8_t index, LiquidCrystalSerial& lcd, uint8_t line_number);

	void handleSelect(uint8_t index);
	
	void resetState();

private:
     

};



#endif
