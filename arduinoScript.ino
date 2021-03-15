#include <Keyboard.h>
#include <SD.h>


#define KEY_SPACE         0xB4
#define KEY_BACKSPACE     0xB2

// Variables to store state of switch in the previous iteration
int prevSwitchOneState = 0&semi;
int prevSwitchTwoState = 0&semi;
int prevSwitchThreeState = 0&semi;
int prevSwitchFourState = 0&semi;

File file&semi;
char *file_name&semi;

void openFile(char *myFile)&semi;
void procLine(String line)&semi;
void procCommand(String command)&semi;

void setup() {
  
  // Instruct arduino to use pins 6, 7, 8 and 9 as Input (to read switch input)
  pinMode(6, INPUT_PULLUP)&semi;
  pinMode(7, INPUT_PULLUP)&semi;
  pinMode(8, INPUT_PULLUP)&semi;
  pinMode(9, INPUT_PULLUP)&semi;

  delay(1000)&semi;

  if (!SD.begin(10)) {
        Serial.println("Couldn't read SD")&semi;
        return&semi;
  }

  int switchOneState = 0&semi;
  int switchTwoState = 0&semi;
  int switchThreeState = 0&semi;
  int switchFourState = 0&semi;

  if (digitalRead(6) == LOW){Serial.println("Switch 1 on")&semi; switchOneState = 1&semi;} else {Serial.println("Switch 1 off")&semi;}
  if (digitalRead(7) == LOW){Serial.println("Switch 2 on")&semi; switchTwoState = 1&semi;} else {Serial.println("Switch 2 off")&semi;}
  if (digitalRead(8) == LOW){Serial.println("Switch 3 on")&semi; switchThreeState = 1&semi;} else {Serial.println("Switch 3 off")&semi;}
  if (digitalRead(9) == LOW){Serial.println("Switch 4 on")&semi; switchFourState = 1&semi;} else {Serial.println("Switch 4 off")&semi;}

  // Only execute script if switch is ON and switchState != prevSwitchState
  if ((prevSwitchOneState != switchOneState) && switchOneState == 1) {
     openFile("1.txt")&semi;
  }

  if ((prevSwitchTwoState != switchTwoState) && switchTwoState == 1) {
     openFile("2.txt")&semi;
  }

  if ((prevSwitchThreeState != switchThreeState) && switchThreeState == 1) {
     openFile("3.txt")&semi;
  }

  if ((prevSwitchFourState != switchFourState) && switchFourState == 1) {
     openFile("4.txt")&semi;
  }
  Keyboard.end()&semi;

  delay(10000)&semi;
}


void openFile(char *myFile){
  if (SD.exists(myFile)) {
        // Open the file for reading
        file = SD.open(myFile, FILE_READ)&semi; 

        // If the file opened successfully, continue
        if (file) {
          Keyboard.begin()&semi;
          String line = ""&semi;

          while(file.available()) { // Read every char
              char c = file.read()&semi;
              if ((int)c == 0x0a) { // You reached line ending
                 procLine(line)&semi; 
                 line = ""&semi;
              } else {
                line += c&semi;  
              }
          }
          file.close()&semi;
        } else {
          Serial.println("File couldn't be loaded")&semi;  
        }
   } else {
    Serial.println("File doesn't exist")&semi; 
   }
}


/*
  Based on the list of DUcky Script commands available from Github - https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript
*/
void procLine(String line) {
  int space = line.indexOf(' ')&semi;  // Find the first 'space' that'll be used to separate the payload from the command
  String command = ""&semi;
  String payload = ""&semi;  

  if (space == -1) {  // There is no space -> (1)
      if (
          line == "ENTER" ||
          line == "MENU" || line == "APP" |
          line == "DOWNARROW" || line == "DOWN" ||
          line == "LEFTARROW" || line == "LEFT" ||
          line == "RIGHTARROW" || line == "RIGHT" ||
          line == "UPARROW" || line == "UP" ||
          line == "BREAK" || line == "PAUSE" ||
          line == "CAPSLOCK" ||
          line == "DELETE" ||
          line == "END" ||
          line == "ESC" || line == "ESCAPE" ||
          line == "HOME" ||
          line == "INSERT" ||
          line == "NUMLOCK" ||
          line == "PAGEUP" ||
          line == "PAGEDOWN" ||
          line == "PRINTSCREEN" ||
          line == "SCROLLLOCK" ||
          line == "SPACE" ||
          line == "TAB"
      ) {
          command = line&semi;
      }
  else {  // There is a space in the line
    command = line.substring(0, space)&semi;   // Get chars in line from start to space position
    payload = line.substring(space + 1)&semi;  // Get chars in line from after space position to EOL

    if (
        command == "DELAY" ||
        command == "STRING" ||
        command == "GUI" || command == "WINDOWS" ||
        command == "SHIFT" ||
        command == "ALT" ||
        command == "CTRL" || command == "CONTROL" ||
        command == "REM"
     ) { } else {
        // Invalid command
        command = ""&semi;
        payload = ""&semi;
     }
  }

  if (payload == "" && command != "") {                              // Command from (1)
        procCommand(command)&semi;                                   // Process command
    } else if (command == "DELAY") {                                 // Delay before the next commande
        delay((int) payload.toInt())&semi;                           // Convert payload to integer and make pause for 'payload' time
    } else if (command == "STRING") {                                // String processing
        Keyboard.print(payload)&semi;                                // Type-in the payload
    } else if (command == "REM") {                                   // Comment
    } else if (command != "") {                                      // Command from (2)
        String remaining = line&semi;                                // Prepare commands to run
        while (remaining.length() > 0) {                             // For command in remaining commands
            int space = remaining.indexOf(' ')&semi;                 // Find the first 'space' that'll be used to separate commands
            if (space != -1) {                                       // If this isn't the last command
                procCommand(remaining.substring(0, space))&semi;     // Process command
                remaining = remaining.substring(space + 1)&semi;     // Pop command from remaining commands
            } else {                                                 // If this is the last command
                procCommand(remaining)&semi;                         // Pop command from remaining commands
                remaining = ""&semi;                                 // Clear commands (end of loop)
            }
        }
    } else {
        // Invalid command
    }

    Keyboard.releaseAll()&semi;
  
  }

}

void procCommand(String command) {
  if (command.length() == 1) {            // Process key (used for example for WIN L command)
        char c = (char) command[0]&semi;  // Convert string (1-char length) to char
        Keyboard.press(c)&semi;           // Press the key on keyboard
    } else if (command == "ENTER") {
        Keyboard.press(KEY_RETURN)&semi;
    } else if (command == "DOWNARROW" || command == "DOWN") {
        Keyboard.press(KEY_DOWN_ARROW)&semi;
    } else if (command == "LEFTARROW" || command == "LEFT") {
        Keyboard.press(KEY_LEFT_ARROW)&semi;
    } else if (command == "RIGHTARROW" || command == "RIGHT") {
        Keyboard.press(KEY_RIGHT_ARROW)&semi;
    } else if (command == "UPARROW" || command == "UP") {
        Keyboard.press(KEY_UP_ARROW)&semi;
    } else if (command == "CAPSLOCK") {
        Keyboard.press(KEY_CAPS_LOCK)&semi;
    } else if (command == "DELETE" || command == "DEL") {
        Keyboard.press(KEY_DELETE)&semi;
    } else if (command == "END") {
        Keyboard.press(KEY_END)&semi;
    } else if (command == "ESC" || command == "ESCAPE") {
        Keyboard.press(KEY_ESC)&semi;
    } else if (command == "HOME") {
        Keyboard.press(KEY_HOME)&semi;
    } else if (command == "INSERT") {
        Keyboard.press(KEY_INSERT)&semi;
    } else if (command == "PAGEUP") {
        Keyboard.press(KEY_PAGE_UP)&semi;
    } else if (command == "PAGEDOWN") {
        Keyboard.press(KEY_PAGE_DOWN)&semi;
    } else if (command == "SPACE") {
        Keyboard.press(KEY_SPACE)&semi;
    } else if (command == "BACKSPACE") {
        Keyboard.press(KEY_BACKSPACE)&semi;
    } else if (command == "TAB") {
        Keyboard.press(KEY_TAB)&semi;
    } else if (command == "GUI" || command == "WINDOWS") {
        Keyboard.press(KEY_LEFT_GUI)&semi;
    } else if (command == "SHIFT") {
        Keyboard.press(KEY_RIGHT_SHIFT)&semi;
    } else if (command == "ALT") {
        Keyboard.press(KEY_LEFT_ALT)&semi;
    } else if (command == "CTRL" || command == "CONTROL") {
        Keyboard.press(KEY_LEFT_CTRL)&semi;
    } else if (command == "F1" || command == "FUNCTION1") {
        Keyboard.press(KEY_F1)&semi;
    } else if (command == "F2" || command == "FUNCTION2") {
        Keyboard.press(KEY_F2)&semi;
    } else if (command == "F3" || command == "FUNCTION3") {
        Keyboard.press(KEY_F3)&semi;
    } else if (command == "F4" || command == "FUNCTION4") {
        Keyboard.press(KEY_F4)&semi;
    } else if (command == "F5" || command == "FUNCTION5") {
        Keyboard.press(KEY_F5)&semi;
    } else if (command == "F6" || command == "FUNCTION6") {
        Keyboard.press(KEY_F6)&semi;
    } else if (command == "F7" || command == "FUNCTION7") {
        Keyboard.press(KEY_F7)&semi;
    } else if (command == "F8" || command == "FUNCTION8") {
        Keyboard.press(KEY_F8)&semi;
    } else if (command == "F9" || command == "FUNCTION9") {
        Keyboard.press(KEY_F9)&semi;
    } else if (command == "F10" || command == "FUNCTION10") {
        Keyboard.press(KEY_F10)&semi;
    } else if (command == "F11" || command == "FUNCTION11") {
        Keyboard.press(KEY_F11)&semi;
    } else if (command == "F12" || command == "FUNCTION12") {
        Keyboard.press(KEY_F12)&semi;
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  setup()&semi;
}
