#define DRUM1 0    //Sensor 1
#define DRUM2 1    //Sensor 2
#define DRUM3 2    //Sensor 3

int valA = 0;
int valB = 0;
int valC = 0;
int sensA = 335;
int sensB = 250;
int sensC = 750;
int LED = 13;

void setup()
{
  pinMode(13,OUTPUT);
  Serial.begin(9600);  // Insitializing the serial port at 9600 baud
}

void loop()
{
  readInputs();

  while(valA < sensA){
    readInputs();
    tone(8, 262, 100);
  }
  while(valB < sensB){
    readInputs();
    tone(8, 294, 100);
  }
}

void readInputs() {
  valA = analogRead(DRUM1);  // Read the voltage
  valB = analogRead(DRUM2);  // Read the voltage
  valC = analogRead(DRUM3);  // Read the voltage
  Serial.println(valA, DEC); // Print the voltage to the terminal
}


