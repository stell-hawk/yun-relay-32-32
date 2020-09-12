String PINSTR="";
String  OLDPINSTR="";

bool dataCommand(YunClient client) {
  String name="";
  long value=0;
  PINSTR="";
  //name =client.readStringUntil('/');
  //name.trim();
  unsigned long time=millis();
  //client.println(millis());
  for (long i = 2; i < 35; i++) {
  //int j=i%100;
  value = digitalRead(i);
  /*client.print(i);
  client.print(F(" : "));
  client.println(value);*/
  PINSTR+=value;
  /*if(value)PINSTR+=value*pow(2,(i-2));
  client.println(PINSTR);
  client.println(PINSTR,BIN);*/
  }
  if(OLDPINSTR!=PINSTR){
  client.println(OLDPINSTR);
  }
  client.println(PINSTR);
  OLDPINSTR=PINSTR;
  client.println(millis()-time);
  
  
  time=millis();
  client.print(F("PORTA: "));client.println(PORTA,BIN);// Pins 22-29
  client.print(F("PINA: "));client.println(PINA,BIN);// Pins 22-29
  client.print(F("PORTB: "));client.println(PORTB,BIN);// 53,52,51,50,10,11,12,13
  client.print(F("PINB: "));client.println(PINB,BIN);// 53,52,51,50,10,11,12,13
  client.print(F("PORTC: "));client.println(PORTC,BIN); //37 -30
  client.print(F("PINC: "));client.println(PINC,BIN); //37 -30
  client.print(F("PORTD: "));client.println(PORTD,BIN);//21-18,х,х,х,38
  client.print(F("PIND: "));client.println(PIND,BIN);//21-18,х,х,х,38
  client.print(F("PORTE: "));client.println(PORTE,BIN);//0,1,x,5,2,3,x,x
  client.print(F("PINE: "));client.println(PINE,BIN);//0,1,x,5,2,3,x,x
  client.print(F("PORTF: "));client.println(PORTF,BIN);//A0-A7 -хз
  client.print(F("PINF: "));client.println(PINF,BIN);//A0-A7 -хз
  client.print(F("PORTG: "));client.println(PORTG,BIN);//41,40,39,х,x,4,x,x
  client.print(F("PING: "));client.println(PING,BIN);//41,40,39,х,x,4,x,x
  client.print(F("PORTH: "));client.println(PORTH,BIN);//17,16,х,6,7,8,9
  client.print(F("PINH: "));client.println(PINH,BIN);//41,40,39,х,x,4,x,x
  client.print(F("PORTJ: "));client.println(PORTJ,BIN);//15,14
  client.print(F("PINJ: "));client.println(PINJ,BIN);//41,40,39,х,x,4,x,x
  client.print(F("PORTK: "));client.println(PORTK,BIN);//A8-A15 -хз
  client.print(F("PINK: "));client.println(PINK,BIN);//41,40,39,х,x,4,x,x
  client.print(F("PORTL: "));client.println(PORTL,BIN);//49-42
  client.print(F("PINL: "));client.println(PINL,BIN);//41,40,39,х,x,4,x,x
  //PORTF,PORTK
  client.println(millis()-time);

  
}
