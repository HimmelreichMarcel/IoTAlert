/*
  
 Created by Rui Santos
 
 All the resources for this project:
 https://randomnerdtutorials.com/
 
*/


int switchReed=8;

void setup(){
  pinMode(switchReed, INPUT);
  Serial.begin(115200);
}

void loop(){
  
  if (digitalRead(switchReed)==HIGH){
    Serial.println("Your Door is Open");
  }
  else {
    Serial.println("Your Door is Closed");
  }
  Serial.println("Searching Door status");
  delay(5);
}
