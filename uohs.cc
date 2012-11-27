
#include <simlib.h> 
#include <stdio.h>
#include <stdlib.h>
 


Store filingOffice("Podatelna", 2);   //pracovnici na podatelne
Facility assistent1("Asistent 1"); //prvni asistent
Facility assistent2("Asistent 2"); //druhy asistent
Facility vicePresident ("Mistopredsedkyne");
Facility director[5]; //pocet reditelu - na kazdy odbor jeden
Store officers[5]; //pocet oddeleni (pro kazde oddeleni 8 referentu)


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
    void Behavior(){
      if(Random()<=0.50){
        Seize(assistent1);
        Wait(Exponential(2*60));
        Release(assistent1);
      }else{
        Seize(assistent2);
        Wait(Exponential(2*60));
        Release(assistent2);
      }
      // TODO: vyresit problem pracovni doby 
      Seize(vicePresident);
      Wait(Exponential(4*480)); //4 dny
      Release(vicePresident);
      
      //spis zpracovan a pripraven na prideleni
    }

};

class WrittenMessage : public Message {
  void Behavior(){

    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);
  }
};

class DataMessage : public Message {
  void Behavior(){

    Enter(filingOffice,1);
    Wait(Exponential(4*60)); //doba zpracovani
    Leave (filingOffice,1);  
  }
};

class ElectronicMessage : public Message {
  void Behavior(){
    Enter(filingOffice,1);
    Leave (filingOffice,1);
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
  Init(0,200*60); // 200 hodin casovy ramec
  director[0].SetName("Reditel verejnych zakazek");
  director[1].SetName("Reditel hospodarske souteze");
  director[2].SetName("Reditel ekonomickeho odboru");
  director[3].SetName("Reditel verejne podpory");
  director[4].SetName("Reditel odboru druhostupnoveho rozhodovani");
  
  for(int i=0;i<5;i++){ //nastaveni kapacit referentu
    officers[i].SetCapacity(8);
  }
  officers[0].SetName("Referent verejnych zakazek"); 
  officers[1].SetName("Referent hospodarske souteze");
  officers[2].SetName("Referent ekonomickeho odboru");
  officers[3].SetName("Referent verejne podpory");
  officers[4].SetName("Referent odboru druhostupnoveho rozhodovani");

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
