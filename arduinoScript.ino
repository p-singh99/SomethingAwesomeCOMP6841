#include <Keyboard.h>
#include <SD.h>


#define KEY_SPACE         0xB4
#define KEY_BACKSPACE     0xB2

// Variables to store state of switch in the previous iteration
int prevSwitchOneState = 0;
int prevSwitchTwoState = 0;
int prevSwitchThreeState = 0;
int prevSwitchFourState = 0;

File file;
char *file_name;

void openFile(char *myFile);
void procLine(String line);
void procCommand(String command);

void setup() {
  
  // Instruct arduino to use pins 6, 7, 8 and 9 as Input (to read switch input)
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  delay(1000);

  if (!SD.begin(10)) {
        Serial.println("Couldn't read SD");
        return;
  }

  int switchOneState = 0;
  int switchTwoState = 0;
  int switchThreeState = 0;
  int switchFourState = 0;

  if (digitalRead(6) == LOW){Serial.println("Switch 1 on"); switchOneState = 1} else {Serial.println("Switch 1 off")}
  if (digitalRead(7) == LOW){Serial.println("Switch 2 on"); switchTwoState = 1} else {Serial.println("Switch 2 off")}
  if (digitalRead(8) == LOW){Serial.println("Switch 3 on"); switchThreeState = 1} else {Serial.println("Switch 3 off")}
  if (digitalRead(9) == LOW){Serial.println("Switch 4 on"); switchFourState = 1} else {Serial.println("Switch 4 off")}

  // Only execute script if switch is ON and switchState != prevSwitchState
  if ((prevSwitchOneState != switchOneState) && switchOneState == 1) {
     openFile("1.txt");
  }

  if ((prevSwitchTwoState != switchTwoState) && switchTwoState == 1) {
     openFile("2.txt");
  }

  if ((prevSwitchThreeState != switchThreeState) && switchThreeState == 1) {
     openFile("3.txt");
  }

  if ((prevSwitchFourState != switchFourState) && switchFourState == 1) {
     openFile("4.txt");
  }
  Keyboard.end();

  delay(10000);
}


void openFile(char *myFile){
  if (SD.exists(myFile)) {
        // Open the file for reading
        file = SD.open(myFile, FILE_READ); 

        // If the file opened successfully, continue
        if (file) {
          Keyboard.begin();
          String line = "";

          while(file.available()) { // Read every char
              char c = file.read();
              if ((int)c == 0x0a) { // You reached line ending
                 procLine(line); 
                 line = "";
              } else {
                line += c;  
              }
          }
          file.close();
        } else {
          Serial.println("File couldn't be loaded");  
        }
   } else {
    Serial.println("File doesn't exist"); 
   }
}


/*
  Based on the list of DUcky Script commands available from Github - https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript
*/
void procLine(String line) {
  int space = line.indexOf(' ');  // Find the first 'space' that'll be used to separate the payload from the command
  String command = "";
  String payload = "";  

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
          command = line;
      }
  else {  // There is a space in the line
    command = line.substring(0, space);   // Get chars in line from start to space position
    payload = line.substring(space + 1);  // Get chars in line from after space position to EOL

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
        command = "";
        payload = "";
     }
  }

  if (payload == "" && command != "") {                         // Command from (1)
        procCommand(command);                                   // Process command
    } else if (command == "DELAY") {                            // Delay before the next commande
        delay((int) payload.toInt());                           // Convert payload to integer and make pause for 'payload' time
    } else if (command == "STRING") {                           // String processing
        Keyboard.print(payload);                                // Type-in the payload
    } else if (command == "REM") {                              // Comment
    } else if (command != "") {                                 // Command from (2)
        String remaining = line;                                // Prepare commands to run
        while (remaining.length() > 0) {                        // For command in remaining commands
            int space = remaining.indexOf(' ');                 // Find the first 'space' that'll be used to separate commands
            if (space != -1) {                                  // If this isn't the last command
                procCommand(remaining.substring(0, space));     // Process command
                remaining = remaining.substring(space + 1);     // Pop command from remaining commands
            } else {                                            // If this is the last command
                procCommand(remaining);                         // Pop command from remaining commands
                remaining = "";                                 // Clear commands (end of loop)
            }
        }
    } else {
        // Invalid command
    }

    Keyboard.releaseAll();
  
  }

}

void procCommand(String command) {
  if (command.length() == 1) {            // Process key (used for example for WIN L command)
        char c = (char) command[0];       // Convert string (1-char length) to char
        Keyboard.press(c);                // Press the key on keyboard
    } else if (command == "ENTER") {
        Keyboard.press(KEY_RETURN);
    } else if (command == "DOWNARROW" || command == "DOWN") {
        Keyboard.press(KEY_DOWN_ARROW);
    } else if (command == "LEFTARROW" || command == "LEFT") {
        Keyboard.press(KEY_LEFT_ARROW);
    } else if (command == "RIGHTARROW" || command == "RIGHT") {
        Keyboard.press(KEY_RIGHT_ARROW);
    } else if (command == "UPARROW" || command == "UP") {
        Keyboard.press(KEY_UP_ARROW);
    } else if (command == "CAPSLOCK") {
        Keyboard.press(KEY_CAPS_LOCK);
    } else if (command == "DELETE" || command == "DEL") {
        Keyboard.press(KEY_DELETE);
    } else if (command == "END") {
        Keyboard.press(KEY_END);
    } else if (command == "ESC" || command == "ESCAPE") {
        Keyboard.press(KEY_ESC);
    } else if (command == "HOME") {
        Keyboard.press(KEY_HOME);
    } else if (command == "INSERT") {
        Keyboard.press(KEY_INSERT);
    } else if (command == "PAGEUP") {
        Keyboard.press(KEY_PAGE_UP);
    } else if (command == "PAGEDOWN") {
        Keyboard.press(KEY_PAGE_DOWN);
    } else if (command == "SPACE") {
        Keyboard.press(KEY_SPACE);
    } else if (command == "BACKSPACE") {
        Keyboard.press(KEY_BACKSPACE);
    } else if (command == "TAB") {
        Keyboard.press(KEY_TAB);
    } else if (command == "GUI" || command == "WINDOWS") {
        Keyboard.press(KEY_LEFT_GUI);
    } else if (command == "SHIFT") {
        Keyboard.press(KEY_RIGHT_SHIFT);
    } else if (command == "ALT") {
        Keyboard.press(KEY_LEFT_ALT);
    } else if (command == "CTRL" || command == "CONTROL") {
        Keyboard.press(KEY_LEFT_CTRL);
    } else if (command == "F1" || command == "FUNCTION1") {
        Keyboard.press(KEY_F1);
    } else if (command == "F2" || command == "FUNCTION2") {
        Keyboard.press(KEY_F2);
    } else if (command == "F3" || command == "FUNCTION3") {
        Keyboard.press(KEY_F3);
    } else if (command == "F4" || command == "FUNCTION4") {
        Keyboard.press(KEY_F4);
    } else if (command == "F5" || command == "FUNCTION5") {
        Keyboard.press(KEY_F5);
    } else if (command == "F6" || command == "FUNCTION6") {
        Keyboard.press(KEY_F6)i;
    } else if (command == "F7" || command == "FUNCTION7") {
        Keyboard.press(KEY_F7);
    } else if (command == "F8" || command == "FUNCTION8") {
        Keyboard.press(KEY_F8);
    } else if (command == "F9" || command == "FUNCTION9") {
        Keyboard.press(KEY_F9);
    } else if (command == "F10" || command == "FUNCTION10") {
        Keyboard.press(KEY_F10);
    } else if (command == "F11" || command == "FUNCTION11") {
        Keyboard.press(KEY_F11);
    } else if (command == "F12" || command == "FUNCTION12") {
        Keyboard.press(KEY_F12);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  setup();
}
