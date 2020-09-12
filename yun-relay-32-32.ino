#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <FileIO.h>
#include <HttpClient.h>
//#include <hashmap.h>
#include "pid.h"
#include "io.h"
#define PORTSCOUNT 32
//int inPorts[]={2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34};
//int outPorts[]={35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,53,A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
int inPorts[]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,53,12,11,10,9,8,7,6,5,4,3,2,14,15,16,17,18};
int outPorts[]={A15,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19};
//2- изменение значения , 1 - следовать за состоянием входа, 0 -ничего не делать
int actions[]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0};

int oldvalues[PORTSCOUNT];
YunServer server;
String serverLink="http://192.168.220.100/scripts/process_AliIPRelays.php";

/*  File dataFile = FileSystem.open("/tmp/ports.txt", FILE_WRITE);  
  dataFile.println(outPorts[i]);
  dataFile.close();  
*/

void bridge_update(String key,int i,int value)
{
    //if(inPorts[i]<10)key+="0";
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
  String A="";
  for (int i = 0; i < PORTSCOUNT; i++) {
      pinMode(inPorts[i], INPUT_PULLUP); 
      int value=digitalRead(inPorts[i]);
      oldvalues[i] = value;
      Bridge.put(String("D")+inPorts[i],String(!value));

      A+=inPorts[i];
      if(i!=(PORTSCOUNT-1))A=A+',';
  }
  
  Bridge.put("inPorts",A);//Записываем номера портов входов
  
  A="";
  for (int i = 0; i < PORTSCOUNT; i++) {
      pinMode(outPorts[i], OUTPUT); 
      //Устанавливаем ноли на порты  при запуске
      digitalWrite(outPorts[i], LOW); 
      Bridge.put(String("D")+outPorts[i],String(0));
      
      A+=outPorts[i];
      if(i!=(PORTSCOUNT-1))A=A+',';
  }
  Bridge.put("outPorts",A);;//Записываем номера портов выходов
  
  server.listenOnLocalhost();
  server.begin();
  
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
    digitalWrite(outPorts[i],!value2);
    dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + !value2);
    dataFile.println(String(" | because  action is:") + actions[i]+ "(swap)");
    }
    if(actions[i]==1)
    {
      //отрицание потому что инвертированы значения входов (INPUT_PULLUP)
      digitalWrite(outPorts[i],!value);
      dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + !value);
      dataFile.println(String(" | because  action is:") + actions[i]+ "(follow)");      
      }
    if(actions[i]==0)
    {
      dataFile.print(String(" | OutPort : ")+ outPorts[i] +" changed from: " + value2 + " to: " + value2);
      dataFile.println(String(" | because  action is:") + actions[i]+ "(no action)");      
    }

    value2 = digitalRead(outPorts[i]);
    HttpClient client;
    String link=String(serverLink)+"?inPort"+inPorts[i]+"=" + !value +"&outPort"+outPorts[i]+"=" + value2+"&i="+i;
    client.get(link);
    
    dataFile.print(link+" : ");      
    while (client.available()) {
      char c = client.read();
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
  delay(20); // Poll every 50ms
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
