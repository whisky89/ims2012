
#include <simlib.h> 
#include <stdio.h>
#include <stdlib.h>
 
#define WRITTEN_MSG       2
#define DATA_MSG          3
#define ELECTRONICAL_MSG  4

Store filingOffice("Podatelna", 2);   //pracovnici na podatelne
Facility assistent1("Asistent 1"); //prvni asistent
Facility assistent2("Asistent 2"); //druhy asistent
Facility vicePresident ("Mistopredsedkyne");
Facility directorPublicOrder ("Reditel verejnych zakazek");
Facility directorEconomicCompet ("Reditel hospodarske souteze"); 
Facility directorEconomicDepartment ("Reditel ekonomickeho odboru"); 
Facility directorPublicSupport ("Reditel verejne podpory");
Facility directorDepartSecInstanceDecision ("Reditel odboru druhostupnoveho rozhodovani");
Store officersPublicOrder("Referent verejnych zakazek",8);
Store officersEconomicCompet("Referent hospodarske souteze",8);
Store officersEconomicDepartment("Referent ekonomickeho odboru",8);
Store officersPublicSupport("Referent verejne podpory",8);
Store officersDepartSecInstanceDecision("Referent odboru druhostupnoveho rozhodovani",8);

Facility isWorkingTime ("Je pracovni doba");

//Queue qFilOffice; //fronta zpracovanych podnetu z podatelny

class WorkingTime : public Process{
  void Behavior(){
    while(1){
      Wait(8*60); //pracovni doba 8hodin
      Seize(isWorkingTime,3);
      Seize(vicePresident,3);
      Enter(filingOffice,2);
      Seize(assistent1,3);
      Seize(assistent2,3);
      Wait(16*60); //nepracuje se
      Release(isWorkingTime);
      Release(vicePresident);
      Leave(filingOffice,2);
      Release(assistent1);
      Release(assistent2);
    }
  }
};


class Message : public Process {
  protected:
//    double inTime; //cas prichodu
    int typeMsg;   //typ zpravy
};

class WrittenMessage : public Message {
  void Behavior(){
//    inTime = Time; //cas vstupu do systemu
    typeMsg = WRITTEN_MSG;
    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);
    
    //kokotina
    if(assistent1.Busy()){ //prvni asistent zaneprazdnen
      Seize(assistent2);
      Wait(Exponential(2*60));
      Release(assistent2);
    }  
    else if(assistent2.Busy()){ //druhy asistent zaneprazdnen
      Seize(assistent1);
      Wait(Exponential(2*60));
      Release(assistent1);
    }else{ //jsou-li oba zaneprazdneni, zpracovava pozadavek asistent 1
      Seize(assistent1);
      Wait(Exponential(2*60));
      Release(assistent1);
    }
    ////////////////////
    //seize asistent 1 nebo asistent2 
    //Wait(Exponential(2*60));
    //relase asistent 1 nebo asistent2
    //Seize (vicePresident);
    //Wait(Exponential(4*480)); //maka jen pres pracovni dobu??? TODO:
    //Relase(vicePresident);
  }
  
};

class DataMessage : public Message {
  void Behavior(){
//    inTime = Time; //cas vstupu do systemu
    typeMsg = DATA_MSG;
    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);  
  }
};

class ElectronicMessage : public Message {
  void Behavior(){
//    inTime = Time; //cas vstupu do systemu
    typeMsg = ELECTRONICAL_MSG;
    Enter(filingOffice,1);
    Leave (filingOffice,1);
  }
};

class GeneratorWrtMsg : public Event {
  void Behavior(){
  //if (pracovni doba)
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
  Init(0,200*60); // 200 hodin casovy ramec
  (new WorkingTime)->Activate();
  (new GeneratorWrtMsg)->Activate();
  (new GeneratorDataMsg)->Activate();
  (new GeneratorWrtMsg)->Activate();
  Run();
  filingOffice.Output();
  assistent1.Output();
  assistent2.Output();
  return 0;
}
