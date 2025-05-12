#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
//for micro board SDA=pin2 SCL=pin3


#define ligthPin          4
#define LSwitchPin        5
#define RSwitchPin        6
#define OPSwitchPin       8
#define SuperSwitchPin    7
#define PotPisPin         A1
#define InfraPin          A2
#define LaserPin          A3
#define PistonPinA        9
#define PistonPinB        10


#define Down              101
#define Up                102
#define Hold              103


// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin            12
#define stepPin           11
#define motorInterfaceType 1

#define RightFast        -10000
#define RightSlow        -4000
#define LeftFast          10000
#define LeftSlow          4000

// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

int potVal;
bool A;
bool B;
bool C;
bool D;
bool E;
bool F;
bool G;
bool GO;

int count;
double timer[20];
double sub[20];
double sum[20];



void setup() {
  // put your setup code here, to run once:

  A = digitalRead(SuperSwitchPin);
  B = 0;
  C = 0;
  D = 0;
  E = 0;
  F = 0;
  G = 0;
  GO = 0;
  count = 0;


  Serial.begin(9600);
  stepper.setMaxSpeed(10000);
  lcd.begin();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Let's Paint!");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");

 // pinMode (interuptpin, INPUT)    ;
  pinMode (ligthPin, OUTPUT)       ;
  pinMode (RSwitchPin, INPUT)   ;
  pinMode (LSwitchPin, INPUT)  ;
  pinMode (OPSwitchPin, INPUT)  ;
  pinMode (SuperSwitchPin, INPUT);
  pinMode (PotPisPin, INPUT)  ;
  pinMode (InfraPin, INPUT)  ;
  pinMode (LaserPin, OUTPUT)  ;
  pinMode (PistonPinA, OUTPUT) ;
  pinMode (PistonPinB, OUTPUT)  ;
  
  

  //attachInterrupt(digitalPinToInterrupt(InfraPin),InfraPinPressed,RISING);
// attachInterrupt(digitalPinToInterrupt(interuptpin),SuperSwitchPinUnPressed,FALLING);


}

void loop() {
 



  digitalWrite(LaserPin, HIGH);


  // check condition for oppertion

  if (digitalRead(SuperSwitchPin) == HIGH ) {
    Serial.print("everything fine\n");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("We're good to go");
    F = 1;

    while (digitalRead(SuperSwitchPin) == HIGH) {



      // once it's all gLSwitchPinStated we write it on the screen and waiting for the operaitor to press
      if (digitalRead(OPSwitchPin) == HIGH ) {


        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  the program begin  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      
        B = 1;
        char buffer[50];
        sprintf(buffer, "A = %d, B = %d, C = %d, D = %d, E = %d \n", A, B, C, D, E);
        Serial.println(buffer);
        
        attachInterrupt(digitalPinToInterrupt(SuperSwitchPin),SuperSwitchPinUnPressed,FALLING);
        
           
        digitalWrite(LaserPin, LOW);
        digitalWrite(ligthPin, HIGH);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("we start!");
        Serial.print("we start\n");
        potVal = analogRead(PotPisPin);
        Serial.print("potVal=");
        Serial.print(potVal);


        PositionSetUp();
        
        //piston opertion

        PistonOpertion();
        delay(500);

        MoveLeft();

      //piston opertion
      PistonOpertion();
      delay(500);


      MoveRight();


      detachInterrupt(digitalPinToInterrupt(SuperSwitchPin));
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Well done!");
      lcd.setCursor(0, 1);
      lcd.print("Waiting for more");
      delay(5000);
      lcd.clear();


      digitalWrite(ligthPin, LOW);




    }

  

  
}

 
}
if (digitalRead(SuperSwitchPin) == LOW  && F == HIGH ) {
  F = 0;
  ////
  ////
  //we calculate the time space between presing and if it less then 3 sec
  //the piston will go to pressing point for callibration
  timer[count] = millis();
  sub[count] = timer[count] - timer[count - 1] ;
  sum[count] = sub[count] + sub[count - 1] + sub[count - 2] + sub[count - 3];
  Serial.println(count);
  Serial.println(timer[count]);
  Serial.println(sum[count]);

  if (count > 3 && sum[count] < 3000) {
    Serial.println("we got it");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Watch out");
    lcd.setCursor(0, 1);
    lcd.print("Moving to the point");
    delay(3000);
 
 PositionSetUp();

    MoveLeft();

    //piston opertion

    Serial.print("\nPistonOpertion\n");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Watch out! ");
    lcd.setCursor(0, 1);
    lcd.print("Piston on move");
    delay(1000);


    while (analogRead(PotPisPin) > 450) {
      digitalWrite (PistonPinB, HIGH);
    }
    digitalWrite (PistonPinB, LOW);
    delay (200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("press to go back ");
    lcd.setCursor(6, 1);
    lcd.print("(^_^) ");
    while (GO == 0) {
      if  (digitalRead(SuperSwitchPin) == HIGH) {
        GO = 1;
      }
    }

    GO = 0;
    while (analogRead(PotPisPin) < 1000) {
      digitalWrite (PistonPinA, HIGH);
    }
    digitalWrite (PistonPinA, LOW);

    MoveRight();

  }
  count = count + 1;

  if (count >= 10) {
    count = 0;
  }
  /////
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Let's Paint!");
  lcd.setCursor(0, 1);
  lcd.print("Press to start");



}

}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>  the program is over  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void PositionSetUp() {
  if (analogRead(PotPisPin) < 1000) {
    while (analogRead(PotPisPin) < 1000) {
      digitalWrite (PistonPinA, HIGH);
    }
    digitalWrite (PistonPinA, LOW);
  }
  if (digitalRead(RSwitchPin) == LOW) {
    while  (digitalRead(RSwitchPin) == LOW) {
      stepper.setSpeed(RightSlow);
      stepper.runSpeed();
    }
  }
 // stepper.stop();
  stepper.setCurrentPosition(0);
  Serial.print("\nAll set up to position\n");
}


void PistonOpertion() {

  Serial.print("\nPistonOpertion\n");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Watch out! ");
  lcd.setCursor(0, 1);
  lcd.print("Piston on move");
  delay(1000);

  potVal = analogRead(PotPisPin);
  Serial.print("potVal=");
  Serial.print(potVal);

  if (analogRead(PotPisPin) < 1000) {
    while (analogRead(PotPisPin) < 1000) {
      digitalWrite (PistonPinA, HIGH);
    }
    digitalWrite (PistonPinA, LOW);
  }

  while (analogRead(PotPisPin) > 450) {
    digitalWrite (PistonPinB, HIGH);
  }
  digitalWrite (PistonPinB, LOW);
  delay (200);
  while (analogRead(PotPisPin) < 650) {
    digitalWrite (PistonPinA, HIGH);
  }
  digitalWrite (PistonPinA, LOW);
  delay (200);


  while (analogRead(PotPisPin) > 450) {
    digitalWrite (PistonPinB, HIGH);
  }
  digitalWrite (PistonPinB, LOW);
  delay (200);
  while (analogRead(PotPisPin) < 650) {
    digitalWrite (PistonPinA, HIGH);
  }
  digitalWrite (PistonPinA, LOW);
  delay (200);


 /* 
  while (analogRead(PotPisPin) > 450) {
    digitalWrite (PistonPinB, HIGH);
  }
  digitalWrite (PistonPinB, LOW);
  delay (200);
  while (analogRead(PotPisPin) < 650) {
    digitalWrite (PistonPinA, HIGH);
  }
  digitalWrite (PistonPinA, LOW);
  delay (200);
*/


  while (analogRead(PotPisPin) > 450) {
    digitalWrite (PistonPinB, HIGH);
  }
  digitalWrite (PistonPinB, LOW);
  delay (200);
  while (analogRead(PotPisPin) < 1000) {
    digitalWrite (PistonPinA, HIGH);
  }
  digitalWrite (PistonPinA, LOW);

  lcd.clear();
}


void MoveLeft() {
  // train moving left fast for 29,000 step
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Train moving ");
  lcd.setCursor(0, 1);
  lcd.print("To the left ");


  // train moving slow antil tuching the button
  while (stepper.currentPosition() < 29000) {
    stepper.setSpeed(LeftFast);
    stepper.runSpeed();
  }
  int LSwitchPinState = digitalRead(LSwitchPin);
  while (LSwitchPinState == LOW) {

    stepper.setSpeed(LeftSlow);
    stepper.runSpeed();
    Serial.print("\n steps");
    Serial.print( stepper.currentPosition());
    Serial.print("\n ");

    Serial.print(LSwitchPinState);
    int LSwitchPinState = digitalRead(LSwitchPin);
    Serial.println(LSwitchPinState);

    if (stepper.currentPosition() > 31000) {
      break;
    }
  }
  
  //stepper.stop();
  stepper.setCurrentPosition(0);
  lcd.clear();


}


void MoveRight() {
  // train moving right fast for 20000 step
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Train moving ");
  lcd.setCursor(0, 1);
  lcd.print("To the right ");


  // train moving slow antil tuching the button
  while (stepper.currentPosition() > -29000) {
    stepper.setSpeed(RightFast);
    stepper.runSpeed();
  }
  int RSwitchPinState = digitalRead(RSwitchPin);
  while (RSwitchPinState == LOW) {

    stepper.setSpeed(RightSlow);
    stepper.runSpeed();
    Serial.print("\n steps");
    Serial.print( stepper.currentPosition());
    Serial.print("\n ");
    Serial.print(RSwitchPinState);
    int RSwitchPinState = digitalRead(RSwitchPin);
    Serial.println(RSwitchPinState);

    if (stepper.currentPosition() < -31000) {
      break;
    }
  }
  //stepper.stop();
  stepper.setCurrentPosition(0);
  lcd.clear();

}

void InfraPinPressed() {

  Serial.print("\nInfraPinPressed\n");
  if (A == 1 && B == 1) {

    E = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("inra RED alarm! ");
    lcd.setCursor(0, 1);
    lcd.print("Press to continue");
    while (C == 0) {
      if (digitalRead(SuperSwitchPin) == LOW ) {
        D = 1;
      }
      if (D == 1 && A == 1) {
        C == 1;
      }

    }
    C == 0;
    D == 0;
  }

}


void SuperSwitchPinUnPressed() {
    
     digitalWrite (PistonPinA, LOW);
     digitalWrite (PistonPinB, LOW);


       while (true){
 
            digitalRead(7);    
                if (digitalRead(7) == HIGH||digitalRead(13) == HIGH){
                   break;

                }

                }

}
