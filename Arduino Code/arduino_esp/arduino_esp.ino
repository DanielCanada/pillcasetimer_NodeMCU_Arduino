#define WT 1500
char buffer[50];
char extract[47];
int extractSize = 0;
char nonExponent[47];
int nonExponentSize = 0;
char exponent[47];
int exponentSize = 0;
char ignored[47];
int ignoredSize = 0;
int intSize = 0;
int decimalPlaces = 0;
int tolerance = 0;
int digitsSize = 0;
int start = 0;
int end = 0;
String output = "";
int type = 0;

int containerNum = 0;

//L293D <----- PARA SA MOTOR ---->
//Motor A
const int motorPin1  = 5;  // Pin 14 of L293
const int motorPin2  = 6;  // Pin 10 of L293
//Motor B
const int motorPin3  = 10; // Pin  7 of L293
const int motorPin4  = 9;  // Pin  2 of L293
//Motor C
const int motorPin5 = 11;  // Pin  7 of L293 2nd
const int motorPin6 = 12;  // Pin  2 of L293 2nd

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
    lcd.begin(16, 2);
    Serial.begin(9600);
}

void loop()
{
    if (Serial.available() > 0)
    {
        String input = Serial.readString();
        injectToBuffer(input);
        displayData();
        delay(WT);
        // allClear();
        // ang imo code paano mag hulog bulong
        if(containerNum > 0){
          getCartridgeToDrop(containerNum); // para ka drop kung diin nga catridge
          Serial.println("Andar sensor!");// check sensor function 
        }
        clearAllArray();
    }
}

void getCartridgeToDrop(int cartridge){

  // code to activate servo motor(?)
  if(cartridge == 1){
    //This code  will turn Motor A clockwise for 2 sec.
    Serial.println("digitalWrite(motorPin1, HIGH)");
    delay (2000);

   //And this code will stop motors
    Serial.println("digitalWrite(motorPin1, LOW)");
    delay (2000);
  }
  if(cartridge == 2){
    //This code  will turn Motor B clockwise for 2 sec.
    Serial.println("digitalWrite(motorPin3, HIGH)");
    delay (2000);

   //And this code will stop motors
    Serial.println("digitalWrite(motorPin3, LOW)");
    delay (2000);
  }
  if(cartridge == 3){
     //This code  will turn Motor C clockwise for 2 sec.
    Serial.println("digitalWrite(motorPin5, HIGH)");
    delay (2000);

   //And this code will stop motors
    Serial.println("digitalWrite(motorPin5, LOW)");
    delay (2000);
  }
  
}

void injectToBuffer(String thisString)
{
    // INJECT THE STRING TO THE CHARARRAY[50]
    thisString.toCharArray(buffer, 50);
    int size = sizeof(buffer) / sizeof(char);
}

void displayData()
{
    float convertedValue = 0;
    if (detectString())
    {
        int extractValue = extractionProcess();

        // SERIAL DISPLAY
        printAll(extractValue);
        // LCD DISPLAY
        //displayAll(extractValue);
    }
    else
    {
        Serial.println("NONE FOUND");
        //allClear();
        //row1Display("NONE FOUND");
        delay(WT);
    }
}

void printAll(int validity)
{
    Serial.println();

    Serial.print("Incoming Message: ");
    Serial.println(extract);
    switch (validity)
    {
    case 0:
        Serial.println("INVALID");
        break;
    case 1:
        Serial.println("VALID");
        break;
    default:
        break;
    }
    if (validity)
    {
        Serial.print("Drop 1 med in container: ");
        Serial.println(containerNum);
        Serial.println();
    }
}

//void displayAll(int validity)
//{
//    delay(WT);
//    allClear();
//    row1Display("STRING VALUE:");
//    row2Display(extract);
//    delay(WT);
//    allClear();
//    row1Display("STRING IS");
//    switch (validity)
//    {
//    case 0:
//        row2Display("INVALID");
//        break;
//    case 1:
//        row2Display("VALID");
//        break;
//    default:
//        break;
//    }
//    if (validity)
//    {
//        delay(WT);
//        allClear();
//        row1Display("Drop Container: ");
//        row2Display(containerNum);
//        delay(WT);
//        allClear();
//    }
//}

bool detectString()
{
    // GO FIND THE STX / KKK
    start = findIndexSTX();
    if (start == -1)
    {
        return 0;
    }
    for (int i = 0; i < 50; i++)
    {
        // EOL DETECTION
        if (buffer[i] == '\0')
        {
            // MARK THE END OF FILE
            end = i;
            break;
        }
        extract[i] = buffer[i + start];
    }
    // ASSIGN EXTRACT SIZE TO REDUNDANT READING
    // OF EMPTY SPACES IN FOR LOOPS
    extractSize = end - start;
    return 1;
}

int findIndexSTX()
{
    //FIND KKK AND INDEX OTHERWISE RETURN DEFUNC
    for (int i = 0; i <= 50; i++)
    {
        if ((buffer[i] == 'K') && (buffer[i + 1] == 'K') && (buffer[i + 2] == 'K'))
        {
            return (i + 3);
        }
    }
    return -1;
}

bool checkValid(char toCheck)
{
    // RETURN 0 INVALID, 1 VALID
    if (isDigit(toCheck) || toCheck == '-' || toCheck == '.')
    {
        return 1;
    }
    return 0;
}

// JUST RESET PREVIOUS ARRAYS
void clearAllArray()
{
    for (int i = 0; i < extractSize; i++)
    {
        extract[i] = '\0';
    }
    extractSize = 0;
    containerNum = 0;
}

// METHODS FOR DISPLAY ------------------------
//void row1Display(String toDisplayText)
//{
//    output = "";
//    lcd.setCursor(0, 0);
//    output.concat(toDisplayText);
//    lcd.print(output);
//}
//
//void row2Display(String toDisplayText)
//{
//    output = "";
//    lcd.setCursor(0, 1);
//    output.concat(toDisplayText);
//    lcd.print(output);
//}
//
//void row2Display(int toDisplayFloat)
//{
//    output = "";
//    lcd.setCursor(0, 1);
//    lcd.print(toDisplayFloat, tolerance);
//}

//void allClear()
//{
//    lcd.setCursor(0, 0);
//    lcd.print("                ");
//    lcd.setCursor(0, 1);
//    lcd.print("                ");
//}

int extractionProcess()
{
    intSize = 0;
    decimalPlaces = 0;
    nonExponentSize = 0;
    exponentSize = 0;
    digitsSize = 0;
    bool invalid = 0;

    if (extract[0] == 'd')
    {
        if (extract[1] == 'r')
        {
            if (extract[2] == 'o')
            {
                if (extract[3] == 'p')
                {
                    containerNum = extract[4] - '0';
                    return 1;
                }
            }
        }
    }
    return 0;
}
