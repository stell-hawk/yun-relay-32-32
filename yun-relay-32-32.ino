#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <FileIO.h>
#include <HttpClient.h>
//#include <hashmap.h>
#define PORTSCOUNT 32
//int inPorts[]={2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34};
//int outPorts[]={35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,53,A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
int inPorts[]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,53,12,11,10,9,8,7,6,5,4,3,2,14,15,16,17,18};
int outPorts[]={A15,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19};
//2- изменение значения , 1 - следовать за состоянием входа, 0 -ничего не делать
int actions[]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
int states[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//состояния выходов
int oldvalues[PORTSCOUNT];//состояия входов

YunServer server;
String serverLink="http://192.168.220.100/scripts/process_AliIPRelays.php";

void(* resetFunc) (void) = 0; // объявляем функцию reset

void bridge_update(String key,int i,int value)
{
    key+= String(inPorts[i]);
    Bridge.put(key,String(value));
    String out=String(millis())+" ";
    out+=key+" ";
    out+=String(oldvalues[i])+" ";
    out+=String(value)+" ";
    Bridge.put("lastupdate",out);
}

void setup() {
  Bridge.begin();
  File dataFile = FileSystem.open("/tmp/log.txt", FILE_APPEND); 
  dataFile.println(String(getTimeStamp())+" ->Restart");
  dataFile.close();
  String A="";
  ReadArrayFromFile(states,"/root/states");
  ReadArrayFromFile(actions,"/root/actions");
  for (int i = 0; i < PORTSCOUNT; i++) {
      pinMode(inPorts[i], INPUT_PULLUP); 
  }
  delay(300);                  // ожидаем секунду
  for (int i = 0; i < PORTSCOUNT; i++) {
      int value=digitalRead(inPorts[i]);
      oldvalues[i] = value;
      Bridge.put(String("D")+inPorts[i],String(!value));
      A+=inPorts[i];
      if(i!=(PORTSCOUNT-1))A=A+',';
  }
  
  Bridge.put("inPorts",A);//Записываем номера портов входов
  delay(300);                  // ожидаем секунду  
  A="";
  for (int i = 0; i < PORTSCOUNT; i++) {
      pinMode(outPorts[i], OUTPUT); 
      //Устанавливаем ноли на порты  при запуске
      if(actions[i]==1)
      {
        ChangeState(i,!oldvalues[i]);
      }
      else digitalWrite(outPorts[i], states[i]); 
      Bridge.put(String("D")+outPorts[i], String(states[i]));
      A+=outPorts[i];
      if(i!=(PORTSCOUNT-1))A=A+',';
  }
  Bridge.put("outPorts",A);;//Записываем номера портов выходов
  delay(300);                  // ожидаем секунду  
  server.listenOnLocalhost();
  server.begin();
}

void ChangeState(int i,int value)
{
   digitalWrite(outPorts[i], value); 
   states[i]=value;
   SaveStatesToFile();
}

void loop() {
  // Get clients coming from server
  YunClient client = server.accept();
  // There is a new client?
  if (client) {
    process(client);
    client.stop();
  }
  bool value=0;
  for (int i = 0; i < PORTSCOUNT; i++) {
    value = digitalRead(inPorts[i]);
    if(value!=oldvalues[i])
    {
    int value2 = digitalRead(outPorts[i]);
    File dataFile = FileSystem.open("/tmp/log.txt", FILE_APPEND); 
    dataFile.print(String(getTimeStamp())+" -> InPort: "+ inPorts[i] +" changed from: "+!oldvalues[i] + " to: " + !value);
    if(actions[i]==2)
    {
      ChangeState(i,!value2);
      dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + !value2);
      dataFile.println(String(" | because  action is:") + actions[i]+ "(swap)");
    }
    if(actions[i]==1)
    {
      //отрицание потому что инвертированы значения входов (INPUT_PULLUP)
      ChangeState(i,!value);
      dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + !value);
      dataFile.println(String(" | because  action is:") + actions[i]+ "(follow)");      
    }
    
    if(actions[i]==0)
    {
      dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + value2);
      dataFile.println(String(" | because  action is:") + actions[i]+ "(no action)");      
    }

    value2 = digitalRead(outPorts[i]);
    HttpClient client2;
    String link=String(serverLink)+"?inPort"+inPorts[i]+"=" + !value +"&outPort"+outPorts[i]+"=" + value2+"&i="+i;
    client2.getAsynchronously(link);
    
    dataFile.print(link+" : ");      
    while (client2.available()) {
      char c = client2.read();
      dataFile.print(c);
    }    
    dataFile.println("."); 
    dataFile.close();
    bridge_update("D",i,!value);
    String key="D";
    key+=outPorts[i];
    Bridge.put(key,String(value2));
    oldvalues[i]=value;
    }
  }
  delay(10); // Poll every 50ms
}





float ReadDataFromFile(String name)
{
  //float ReturnValue;
  String Str="";
  String FileName="/root/";
  FileName+=name;
  File dataFile = FileSystem.open(FileName.c_str(), FILE_READ); 
  if (dataFile) {
    while (dataFile.available()) {
      Str+=char(dataFile.read());
    }
    dataFile.close();
  }
  return Str.toFloat();
}

void SaveStatesToFile()
{
  File dataFile = FileSystem.open("/root/states", FILE_WRITE); 
  if (dataFile) {
    for (int i = 0; i < PORTSCOUNT; i++) {
    dataFile.write(states[i]+48);
    }
    dataFile.close();
  }
}

void ReadArrayFromFile(int* arr,char* name)
{
    File dataFile = FileSystem.open(name, FILE_READ); 
  if (dataFile) {
    int i=0;
    while (dataFile.available()) {
      arr[i++]=int(dataFile.read())-48;
    }
    dataFile.close();
  }
}

void SaveArrayToFile(int* arr,char* name)
{
  File dataFile = FileSystem.open(name, FILE_WRITE); 
  if (dataFile) {
    for (int i = 0; i < PORTSCOUNT; i++) {
    dataFile.write(arr[i]+48);
    }
    dataFile.close();
  }
}



void digitalCommand(YunClient client) {
  int pin, value;

  // Read pin number
  pin = client.parseInt();

  // If the next character is a '/' it means we have an URL
  // with a value like: "/digital/13/1"
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
    //HttpClient client2;
    //String link=String(serverLink)+"?outPort"+pin+"=" + value;
    //client2.get(link);

    File dataFile = FileSystem.open("/tmp/log.txt", FILE_APPEND); 
    dataFile.println(String(getTimeStamp())+" ->digitalCommand  changed pin from: "+ pin + " to: " + value);
    //dataFile.print(link+" : ");      
    /*while (client2.available()) {
      char c = client2.read();
      dataFile.print(c);
    }*/    
    dataFile.println("."); 
    dataFile.close();
    
      for (int i = 0; i < PORTSCOUNT; i++) {
        if(int(outPorts[i])==int(pin)){
          states[i]=value;SaveStatesToFile();
          }
      }
  } 
  else {
    value = digitalRead(pin);
  }

  // Send feedback to client
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);
  String key ="D";
  key+= String(pin);
  Bridge.put(key,String(value));
}

void SetOutPortByArrayNumber(YunClient client) {
  int pin, value;
  // Read pin number
  pin = client.parseInt();
  pin=outPorts[pin];
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
    states[pin]=value;
    SaveStatesToFile();
    File dataFile = FileSystem.open("/tmp/log.txt", FILE_APPEND); 
    dataFile.println(String(getTimeStamp())+" ->SetOutPortByArrayNumber  changed pin from: "+ pin + " to: " + value);
    String key ="D";
    key+= String(pin);
    Bridge.put(key,String(value));
   }
}




void process(YunClient client) {
  // read the command
  String command = client.readStringUntil('/');
  command.trim();
  // is "digital" command?
  if (command == "digital") {
    digitalCommand(client);
    return;
  }
  if (command == "restart") {
    resetFunc(); //вызываем reset
    return;
  }
  
  if (command == "setoutport") {
    SetOutPortByArrayNumber(client);
    return;
  }

  // is "data" command?
  if (command == "data") {
    dataCommand(client);
    return;
  }
  
  if (command == "states") {
    client.println("states InPort is:");  
    for (int i = 0; i < PORTSCOUNT; i++) {
    client.print(i);  
    client.print("[");client.print(inPorts[i]);client.print("]: ");  
    client.println(oldvalues[i]);  
    }

    client.println("states OutPorts is:");  
    for (int i = 0; i < PORTSCOUNT; i++) {
    client.print(i);  
    client.print("[");client.print(outPorts[i]);client.print("]: ");  
    client.print(states[i]);  
    client.print(" mode: ");client.println(actions[i]);  
    }
    return;
  }
  if (command == "array") {
    client.println("states is:");  
    ReadArrayFromFile(states,"/root/states");
    for (int i = 0; i < PORTSCOUNT; i++)client.print(states[i]);    
    client.println();  
    client.println("actions is:");  
    ReadArrayFromFile(actions,"/root/actions");
    for (int i = 0; i < PORTSCOUNT; i++)client.print(actions[i]);    
    client.println();  
    
    return;
  }
    client.println(command);  
}

String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time 
  // in different formats depending on the additional parameter 
  time.begin("date");
  time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
                                //             T for the time hh:mm:ss    
  time.run();  // run the command

  // read the output of the command
  while(time.available()>0) {
    char c = time.read();
    if(c != '\n')
      result += c;
  }
  
  return result;
}
