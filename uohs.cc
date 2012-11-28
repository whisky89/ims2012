
#include <simlib.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
 


Queue vicePresWork;
Queue queueAssisten1;
Queue queueAssisten2;

Store filingOffice("Podatelna", 2);   //pracovnici na podatelne
Facility assistent1("Asistent 1",queueAssisten1); //prvni asistent
Facility assistent2("Asistent 2",queueAssisten2); //druhy asistent
Facility vicePresident ("Mistopredsedkyne",vicePresWork); //,vicePresWork
Facility director[5]; //pocet reditelu - na kazdy odbor jeden
Store officers[5]; //pocet oddeleni (pro kazde oddeleni 8 referentu)


//Facility isWorkingTime ("Je pracovni doba");
int verejne=0;
int soutez=0;
int podpora=0;
int eko=0;
int rozh=0;
//Queue qFilOffice; //fronta zpracovanych podnetu z podatelny
//Queue officersQueue[5][8];
Queue archive;

/**
 * Denni pracovni doba
 */

class WorkingTime : public Process{
  void Behavior(){
    while(1){
      Wait(8*60); //pracovni doba 8hodin
      //Seize(isWorkingTime,3);
      Seize(vicePresident,3);
      
      Enter(filingOffice,2);
      Seize(assistent1,3);
      Seize(assistent2,3);
      //for(int i=0;i<5;i++){
      //  Seize(director[i],3);
      //  Enter(officers[i],8);
      //}
      Wait(16*60); //nepracuje se
    //  Release(isWorkingTime);
      Release(vicePresident);
      Leave(filingOffice,2);
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
        if(Random()<=0.50) {
          Seize(assistent2);
          Wait(Exponential(2*60));
          Release(assistent2);
        } else {
          Seize(assistent1);
          Wait(Exponential(2*60));
          Release(assistent1);
          
        }
      }
      // TODO: vyresit problem pracovni doby 
      //vyrizovani viceprezidentem
      vicePresWork.SetName("fronta mistopredsedkyne");
      vicePresWork.Insert(this);
      Wait(Exponential(4*24*60)); //4 dny
      Print("LALA\n");
      archive.SetName("Archiv uzavrenych pripadu");
      //prideleni spisu na dany odbor
      percent = Random();
      if(percent <= 0.07){
        //Odbor druhostupnoveho rozhodovani 7%
        rozh++;
        //TODO: REDITELOVA PRACE
        Wait(Exponential(21*60)); //cca 21 dni kolobeh zpracovani zprav
        //TODO: DELBA PRACE
//        archive.Insert(this);
      }
      else if(percent <= 0.17){
        //verejna podpora 10%
//        Print("podpora\n");
        podpora++;
        Print("108\n");
        if(assistent2.Busy()){
          queueAssisten2.InsFirst(this);
          Passivate();
        }
        Seize(assistent2,15);
        Wait(Exponential(2*60));
//        archive.Insert(this);
        Release(assistent2);
      }
//      else if(percent <= 0.29){
//        //ekonomicky odbor 12%
////        Print("eko\n");
//        eko++;
//        Print("118\n");
//        Seize(assistent1);
//        Wait(Exponential(2*60));
////        archive.Insert(this);
//        Release(assistent1);
//      }
//      else if(percent <= 0.51){
//        //hospodarska soutez 22%
////        Print("soutez\n");
//        soutez++;
//        Print("128\n");
//        Print("beru asistenta\n");
//        Seize(assistent1);     
//        Wait(Exponential(2*60));
////        archive.Insert(this);
//        Release(assistent1);
//        Print("uvolneni asistenta\n");
//      }
//      else{
//        //verejne zakazky 49%
//        verejne++;
//        Print("137\n");
//        Print("beru asistenta\n");
//        Seize(assistent1);
//        Wait(Exponential(2*60));
////        archive.Insert(this);
//        Release(assistent1);
//        Print("uvolneni asistenta\n");
//      }     
    }

};

class WrittenMessage : public Message {
  void Behavior(){

    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);
//    qFilOffice.Insert(this);
//    Passivate();
    Message::Behavior();
  }
};

class DataMessage : public Message {
  void Behavior(){

    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);   
    Message::Behavior();
    
  }
};

class ElectronicMessage : public Message {
  void Behavior(){
    Enter(filingOffice,1);
    Leave (filingOffice,1);
    Message::Behavior();
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
    Activate(Time + Exponential(108));
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
//  Init(0,43829.0639); // mesic behu
  Init(0,7*24*60); // tyden behu
//  std::string ttrf;
  
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
  

//  (new WorkingTime)->Activate();
  (new GeneratorWrtMsg)->Activate();
  (new GeneratorDataMsg)->Activate();
  (new GeneratorWrtMsg)->Activate();
  Run();
//  filingOffice.Output();
//  assistent1.Output();
//  assistent2.Output();
////  vicePresident.Output();
//  vicePresWork.Output();
//  
//  archive.Output();
  
//  officersQueue[1][1].Output();
  
 // Print("verejne zakazky %d\n",verejne);
 // Print("hosp. soutez %d\n",soutez);
 // Print("eko %d\n",eko);
 // Print("podpora %d\n",podpora);
 // Print("rozhodovani %d\n",rozh);
//  qFilOffice.Output();
  return 0;
}
