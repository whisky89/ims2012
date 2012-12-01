/*
 * OPTIMALIZACE: priorita u elektronicke posty - efektivnejsi vyuziti casu 
 */
#include <simlib.h> 
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream> 

// vstupni doba generatoru
#define IN_ELECTRONIC 120
#define IN_TEXT 84
#define IN_DATA 108

// zpracovani na podatelne
#define PODATELNA_EL 5
#define PODATELNA_TEXT 4 * 60
#define PODATELNA_DATA 4 * 60

// assistenti
#define ASSISTENT_ZPRACOVANI 2 * 60
#define ASSISTENT_ZALOZENI 2 * 60

// pracovni doba
#define DEN 8 * 60
#define NOC 16 * 60

// mistopredsedkyne
#define MISTOP_CEKANI 4 * 24 * 60
#define MISTOP_ZPRACOVANI 30  // upresnit

// reditel odboru
#define RED_ZPRACOVANI 30

// referenti
#define REF_ZPRACOVANI 21 * 24 * 60


Facility podatelna1("1. pracovnik na podatelne");
Facility podatelna2("2. pracovnik na podatelne");
Facility assistent1("Asistent 1"); //prvni asistent ,queueAssisten1
Facility assistent2("Asistent 2"); //druhy asistent ,queueAssisten2
Facility mistopredsedkyne ("Mistopredsedkyne"); //
Facility reditel[5]; //pocet reditelu - na kazdy odbor jeden
Store referent[5]; //pocet oddeleni (pro kazde oddeleni 8 referentu)


Facility isWorkingTime ("Je pracovni doba"); // pracovni doba

int cnt=0;
int verejne=0;
int soutez=0;
int podpora=0;
int eko=0;
int rozh=0;
int el=0;


/**
 * Denni pracovni doba
 */
class WorkingTime : public Process{
  void Behavior(){
//    Priority=3;
    while(1){
      Wait(8*60); //pracovni doba 8hodin
      Seize(isWorkingTime,3);
      Seize(podatelna1,3);
      Seize(podatelna2,3);
      Seize(assistent1,3);
      Seize(assistent2,3);
      Seize(mistopredsedkyne,3);
      Wait(16*60);//nepracuje se
      Release(isWorkingTime);
      Release(podatelna1);
      Release(podatelna2);
      Release(assistent1);
      Release(assistent2);
      Release(mistopredsedkyne);
    }
  }
};

/**
 * Rodicovska trida pro jednotlive typy zprav
 */
class Message : public Process {
  protected:
    /**
      * popis chovani zpracovani na podatelne
      * @param waitTime - doba zpracovani zpavy
      */
    void podatelnaBehavior(double waitTime){
      // pro rovnomerne zatizeni pracovniku
      if(podatelna1.QueueLen()<podatelna2.QueueLen()){
        Seize(podatelna1);
        Wait(Exponential(waitTime));
        Release(podatelna1);
      } else if(podatelna1.QueueLen()>podatelna2.QueueLen()){
        Seize(podatelna2);
        Wait(Exponential(waitTime));
        Release(podatelna2);
      }else{
        if(Random()<0.50){
          Seize(podatelna1);
          Wait(Exponential(waitTime));
          Release(podatelna1);
        }else{
          Seize(podatelna2);
          Wait(Exponential(waitTime));
          Release(podatelna2);
        }
      }
    }
    // popis zbytku chovani v systemu
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

      Seize(mistopredsedkyne);
      Wait(5);
      Release(mistopredsedkyne);
      
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
    Message::podatelnaBehavior(60 * 4);
//    Print("posledni: %lf\n",Time);
    Message::Behavior();
  }
};

class DataMessage : public Message {
  void Behavior(){
    cnt++;
    Message::podatelnaBehavior(60 * 4);
//    Print("posledni: %lf\n",Time);
    Message::Behavior();
    
  }
};

class ElectronicMessage : public Message {
  void Behavior(){
    cnt++;
    el++;
    Priority=2;
    Message::podatelnaBehavior(5);
//    Print("posledni: %lf\n",Time);
    Priority=0;
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
  Init(0,43829.1); // mesic behu
//  Init(0,9*60); //10 hodin behu
//  Init(0,7*24*60); // tyden behu
//  Init(0,1051897.44);//2roky
//  std::string ttrf;
  
//  mistopredsedkyne ("fronta mistopredsedkyne");

  reditel[0].SetName("Reditel verejnych zakazek");
  reditel[1].SetName("Reditel hospodarske souteze");
  reditel[2].SetName("Reditel ekonomickeho odboru");
  reditel[3].SetName("Reditel verejne podpory");
  reditel[4].SetName("Reditel odboru druhostupnoveho rozhodovani");
  
  referent[0].SetName("Referent verejnych zakazek"); 
  referent[1].SetName("Referent hospodarske souteze");
  referent[2].SetName("Referent ekonomickeho odboru");
  referent[3].SetName("Referent verejne podpory");
  referent[4].SetName("Referent odboru druhostupnoveho rozhodovani");
  
//  for(int i=0;i<5;i++){ //nastaveni kapacit referentu
//    referent[i].SetCapacity(8);
//    for(int j=0;j<8;j++){
//      referent[i].SetQueue(referentQueue[i][j]);
//      ttrf= "";
//      ttrf.append(referent[i].Name());
//      ttrf.append(" c.");
//      ttrf.append(1,(j+'0'));
//      referentQueue[i][j].SetName(ttrf.c_str());
//    }
//  }
  

  (new WorkingTime)->Activate();
  (new GeneratorWrtMsg)->Activate();
  (new GeneratorDataMsg)->Activate();
  (new GeneratorElMsg)->Activate();
  Run();
  
  
  podatelna1.Output();
  podatelna2.Output();
  Print("prichozich zprav: %d\n",cnt);
  Print("prichozich elektronickych zprav: %d\n",el);
//  assistent1.Output();
//  assistent2.Output();
//  mistopredsedkyne.Output();


  
//  referentQueue[1][1].Output();
    
 // Print("verejne zakazky %d\n",verejne);
 // Print("hosp. soutez %d\n",soutez);
 // Print("eko %d\n",eko);
 // Print("podpora %d\n",podpora);
 // Print("rozhodovani %d\n",rozh);
// qFilOffice.Output();
  return 0;
}
