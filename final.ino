//Controller of Left tyers
const int EnableL =5;                                          
const int HighL =6;   //Left moter pins number
const int LowL = 7;


//Controller of Right Tyers
const int EnableR =10;
const int HighR =8;   //Right moter pins number
const int LowR = 9;

//Input pins for raspberry pi 
const int D0=0;  //Raspberry pin 21
const int D1=1; //Raspberry pin 22
const int D2=2; //Raspberry pin 23
const int D3 =3; //Raspberry pin 24


//variables used to take binrary input from Rapsberry
int a,b,c,d,data=0;

//Function used to capture value and convert them into decimal.
void Data(){
  a = digitalRead(D0);
  b = digitalRead(D1);
  c = digitalRead(D2);
  d = digitalRead(D3);

  data = 8*d+4*c+2*b+a;
}

//Basic function set innput and output pins
void setup() {

  //Ardino to motor
  pinMode(EnableL,OUTPUT);
  pinMode(HighL,OUTPUT);
  pinMode(LowL,OUTPUT);

  
  pinMode(EnableR,OUTPUT);
  pinMode(HighR,OUTPUT);
  pinMode(LowR,OUTPUT);

  
  //pi to andino
  pinMode(D0,INPUT_PULLUP);
  pinMode(D1,INPUT_PULLUP);
  pinMode(D2,INPUT_PULLUP);
  pinMode(D3,INPUT_PULLUP);

}


//Command motor to more forward
void Forward(){
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,250);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,250);
}


//Command motor to stop moving
void Stop(){
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,0);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,0);
}


//Command motor to move backword
void Backward(){
  digitalWrite(HighL,HIGH);
  digitalWrite(LowL,LOW);
  analogWrite(EnableL,255);

  digitalWrite(HighR,HIGH);
  digitalWrite(LowR,LOW);
  analogWrite(EnableR,255);
  
}


//Command motor to left [large, medium, small cut]
void Left1(){ //Small cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,90);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,250);
}
void Left2(){ //medium cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,40);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,250);
}
void Left3(){ //small cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,0);
  

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,250);
}


//Command motor to Right [small, medium,large cut]
void Right1(){ //small cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,250);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,90);
}
void Right2(){ //medium cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,250);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,40);
}
void Right3(){ //large cut
  digitalWrite(HighL,LOW);
  digitalWrite(LowL,HIGH);
  analogWrite(EnableL,250);

  digitalWrite(HighR,LOW);
  digitalWrite(LowR,HIGH);
  analogWrite(EnableR,0);
}

void loop() {      
  // put your main code here, to run repeatedly:
  Data();  //grab data


  //conditions used to counter moments 
  if(data==0)
    {
    Forward();
    }
  else if(data==1)
    {
    Right1();
    }
  else if(data==2)
    {
    Right2();
    }
  else if(data==3)
    {
    Right3();
    }
  else if(data==4)
    {
    Left1();
    }
  else if(data==5)
    {
    Left2();
    }
  else if(data==6)
    {
    Left3();
    }
  else if(data==7){
    Stop();
  }
  else
    {
    Stop();
    }
}
