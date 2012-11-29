
#include <simlib.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream> 


Queue vicePresWork;

//Store filingOffice("Podatelna", 2);   //pracovnici na podatelne
Facility filingOfficer1("1. pracovnik na podatelne");
Facility filingOfficer2("2. pracovnik na podatelne");
Facility assistent1("Asistent 1"); //prvni asistent ,queueAssisten1
Facility assistent2("Asistent 2"); //druhy asistent ,queueAssisten2
Facility vicePresident ("Mistopredsedkyne"); //
Facility director[5]; //pocet reditelu - na kazdy odbor jeden
Store officers[5]; //pocet oddeleni (pro kazde oddeleni 8 referentu)


Facility isWorkingTime ("Je pracovni doba");
int cnt=0;
int verejne=0;
int soutez=0;
int podpora=0;
int eko=0;
int rozh=0;
//Queue qFilOffice; //fronta zpracovanych podnetu z podatelny
//Queue officersQueue[5][8];

/**
 * Denni pracovni doba
 */

class WorkingTime : public Process{
  void Behavior(){
//    Priority=3;
    while(1){
      Wait(8*60); //pracovni doba 8hodin
      Seize(isWorkingTime,3);
      /** POKUD BUDEME CHTIT ABY PRACOVNIK KROUTIL PRESCASY*/
      Seize(filingOfficer1); 
      Seize(filingOfficer2);
      /** PRACOVNIK NESTIHNE VYRIDIT ZAKAZKU = BEHEM PRACE JE PRERUSEN *
      Seize(filingOfficer1,3); 
      Seize(filingOfficer2,3);
      * ********************************/
      Seize(vicePresident,3);
      Seize(assistent1,3);
      Seize(assistent2,3);
      //for(int i=0;i<5;i++){
      //  Seize(director[i],3);
      //  Enter(officers[i],8);
      //}
      Wait(16*60); //nepracuje se
      Release(isWorkingTime);
      
      Release(filingOfficer1);
      Release(filingOfficer2);
      Release(vicePresident);
      Release(assistent1);
      Release(assistent2);
      //for(int i=0;i<5;i++){
      //  Release(director[i]);
      //  Leave(officers[i],8);
      //}
    }
  }
};




class Message : public Process {
  protected:
    void Behavior(){
      double percent;
      if(assistent1.QueueLen() < assistent2.QueueLen()){
        Seize(assistent1);
        Wait(Exponential(2*60));
        Release(assistent1);
      } else if(assistent1.QueueLen() > assistent2.QueueLen()){
        Seize(assistent2);
        Wait(Exponential(2*60));
        Release(assistent2);
      } 
      else {
        if(Random()<0.50) {
          Seize(assistent2);
          Wait(Exponential(2*60));
          Release(assistent2);
        } else {
          Seize(assistent1);
          Wait(Exponential(2*60));
          Release(assistent1);
          
        }
      }  
      Seize(vicePresident);
      Wait(5);
      Release(vicePresident);
      percent = Random();
      if(percent <= 0.07){
        //Odbor druhostupnoveho rozhodovani 7%
        //rozh++;
        //TODO: REDITELOVA PRACE
        Wait(60*21*24);//cca 21 dni kolobeh zpracovani zprav
        if(assistent1.QueueLen() < assistent2.QueueLen()){
          Seize(assistent1);
          Wait(Exponential(2*60));
          Release(assistent1);
        } else if(assistent1.QueueLen() >= assistent2.QueueLen()){
          Seize(assistent2);
          Wait(Exponential(2*60));
          Release(assistent2);
        }
      }
      else if(percent <= 0.17){
        //verejna podpora 10%
//        Print("podpora\n");
        //podpora++;
        Wait(60*21*24);
        Seize(assistent2,1);
        Wait(Exponential(2*60));
        
        Release(assistent2);
      }
      else if(percent <= 0.29){
//        //ekonomicky odbor 12%
////        Print("eko\n");
//        eko++;
        Wait(60*21*24);
        Seize(assistent1,1);
        Wait(Exponential(2*60));
        Release(assistent1);
      }
      else if(percent <= 0.51){
//        //hospodarska soutez 22%
////        Print("soutez\n");
//        soutez++;

        Wait(60*21*24);
        Seize(assistent2,1);     
        Wait(Exponential(2*60));
      
        Release(assistent2);
//        Print("uvolneni asistenta\n");
      }
      else{
//        //verejne zakazky 49%
//        verejne++;
        Wait(60*21*24);
        Seize(assistent1,1);
        Wait(Exponential(2*60));    
        Release(assistent1);
//        Print("uvolneni asistenta\n");
      }    
    }

};

class WrittenMessage : public Message {
  void Behavior(){
    cnt++;
    if(filingOfficer1.QueueLen() > filingOfficer2.QueueLen()){
      Seize(filingOfficer2);
//      if(!isWorkingTime.Busy()){
      Wait(Exponential(4*60)); // prodleva mezi preposlanim asistentovi
      Release(filingOfficer2);
//      Print("\nwrt Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//      }
    }else{
      Seize(filingOfficer1);
//    if(!isWorkingTime.Busy()){
//      Print("\nprijem wrt: ");
//      Print(Time);
      Wait(Exponential(4*60)); //doba zpracovani
      Release(filingOfficer1);
//      Print("\nwrt Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//    }
    }
  }
};

class DataMessage : public Message {
  void Behavior(){
    cnt++;
    if(filingOfficer1.QueueLen() > filingOfficer2.QueueLen()){
      Seize(filingOfficer2);
//      if(!isWorkingTime.Busy()){
      Wait(Exponential(4*60)); // prodleva mezi preposlanim asistentovi
      Release(filingOfficer2);
//      Print("\ndata Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//      }
    }else{
//      Print("\nprijem data: ");
//      Print(Time);
      Seize(filingOfficer1);
//    if(!isWorkingTime.Busy()){  
//      Print("\nprijem data: ");
//      Print(Time);
      Wait(Exponential(4*60)); //doba zpracovani
      Release(filingOfficer1);   
//      Print("\ndata Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//    }
    }
  }
};

class ElectronicMessage : public Message {
  void Behavior(){
    cnt++;
 
//      Print("\nprijem elect: ");
//      Print(Time);
    if(filingOfficer1.QueueLen() > filingOfficer2.QueueLen()){
      Seize(filingOfficer2);
//      if(!isWorkingTime.Busy()){
      Wait(Exponential(5)); // prodleva mezi preposlanim asistentovi
      Release(filingOfficer2);
//      Print("\n elect Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//    }
    }else{
//    if(!isWorkingTime.Busy()){
//      Print("\nprijem elect: ");
//      Print(Time);
      Seize(filingOfficer1);
//      if(!isWorkingTime.Busy()){
      Wait(Exponential(5)); // prodleva mezi preposlanim asistentovi
      Release(filingOfficer1);
//      Print("\n elect Zpracovano v: ");
//      Print(Time);
      Message::Behavior();
//    }
    }
  }
};

class GeneratorWrtMsg : public Event {
  void Behavior(){
	 (new WrittenMessage)->Activate();
    Activate(Time + Exponential(84));
  }
};

class GeneratorDataMsg : public Event {
  void Behavior(){
	 (new DataMessage)->Activate();
    Activate(Time + Exponential(108)); //108
  }
};

class GeneratorElMsg : public Event {
  void Behavior(){
	 (new ElectronicMessage)->Activate();
    Activate(Time + Exponential(120));
  }
};

int main(int argc, char **argv)
{
  Init(0,43829); // mesic behu
//  Init(0,16*60); //10 hodin behu
//  Init(0,7*24*60); // tyden behu
//  std::string ttrf;
  
//  vicePresident ("fronta mistopredsedkyne");

  director[0].SetName("Reditel verejnych zakazek");
  director[1].SetName("Reditel hospodarske souteze");
  director[2].SetName("Reditel ekonomickeho odboru");
  director[3].SetName("Reditel verejne podpory");
  director[4].SetName("Reditel odboru druhostupnoveho rozhodovani");
  
  officers[0].SetName("Referent verejnych zakazek"); 
  officers[1].SetName("Referent hospodarske souteze");
  officers[2].SetName("Referent ekonomickeho odboru");
  officers[3].SetName("Referent verejne podpory");
  officers[4].SetName("Referent odboru druhostupnoveho rozhodovani");
  
//  for(int i=0;i<5;i++){ //nastaveni kapacit referentu
//    officers[i].SetCapacity(8);
//    for(int j=0;j<8;j++){
//      officers[i].SetQueue(officersQueue[i][j]);
//      ttrf= "";
//      ttrf.append(officers[i].Name());
//      ttrf.append(" c.");
//      ttrf.append(1,(j+'0'));
//      officersQueue[i][j].SetName(ttrf.c_str());
//    }
//  }
  

  (new WorkingTime)->Activate();
  (new GeneratorWrtMsg)->Activate();
  (new GeneratorDataMsg)->Activate();
  (new GeneratorElMsg)->Activate();
  Run();
  
  
  filingOfficer1.Output();
  filingOfficer2.Output();
  Print("prichozich zprav: %d\n",cnt);
//  assistent1.Output();
//  assistent2.Output();
//  vicePresident.Output();


  
//  officersQueue[1][1].Output();
    
 // Print("verejne zakazky %d\n",verejne);
 // Print("hosp. soutez %d\n",soutez);
 // Print("eko %d\n",eko);
 // Print("podpora %d\n",podpora);
 // Print("rozhodovani %d\n",rozh);
// qFilOffice.Output();
  return 0;
}
