/*
 * OPTIMALIZACE: priorita u elektronicke posty - efektivnejsi vyuziti casu 
 */

/*
 * NAVRH K MONITOROVANI ZPRAV - pridat typ zpravy a pocitadla techto zprav
 *  -meli by jsme k dispozici informaci o tom, kolik zprav kterych typu 
 *   bylo uspesne zpracovano a pote archivovano
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

// asistenti
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

//#define debug

Facility podatelna1("1. pracovnik na podatelne");
Facility podatelna2("2. pracovnik na podatelne");
Facility asistent1("Asistent 1"); //prvni asistent ,queueAssisten1
Facility asistent2("Asistent 2"); //druhy asistent ,queueAssisten2
Facility mistopredsedkyne ("Mistopredsedkyne"); //
Facility reditel[5]; //pocet reditelu - na kazdy odbor jeden


//Facility isWorkingTime ("Je pracovni doba"); // pracovni doba

Queue archiv;

int cnt=0;
int verejne=0;
int soutez=0;
int podpora=0;
int eko=0;
int rozh=0;
int el=0;


/**
 * Simulace denni pracovni doby
 * 
 * Pracovni doba je stanovena na 8 hodin denne
 * (neuvazujeme prescasy, svatky ci dovolene)
 */
class PracovniDoba : public Process{
  void Behavior(){
//    Priority=3;
    while(1){
      Wait(8*60); //pracovni doba 8hodin
//      Seize(isWorkingTime,3);
      Seize(podatelna1,3);
      Seize(podatelna2,3);
      Seize(asistent1,3);
      Seize(asistent2,3);
      Seize(mistopredsedkyne,3);
      Wait(16*60);//nepracuje se
//      Release(isWorkingTime);
      Release(podatelna1);
      Release(podatelna2);
      Release(asistent1);
      Release(asistent2);
      Release(mistopredsedkyne);
    }
  }
};

/**
 * Rodicovska trida pro jednotlive typy zprav
 */
class Zprava : public Process {
  protected:
    /**
      * Popis chovani zpracovani na podatelne
      * @param cas - doba zpracovani zpavy
      */
    void podatelnaBehavior(double cas){
      // rovnomerne zatizeni pracovniku na podatelne
      if(podatelna1.QueueLen()<podatelna2.QueueLen()){
        Seize(podatelna1);
        Wait(Exponential(cas));
        Release(podatelna1);
      } else if(podatelna1.QueueLen()>podatelna2.QueueLen()){
        Seize(podatelna2);
        Wait(Exponential(cas));
        Release(podatelna2);
      }else{
        if(Random()<0.50){
          Seize(podatelna1);
          Wait(Exponential(cas));
          Release(podatelna1);
        }else{
          Seize(podatelna2);
          Wait(Exponential(cas));
          Release(podatelna2);
        }
      }
#ifdef debug
      Print("PODATELNA - zpracovano v: %lf\n",Time);      
#endif
    }
    /**
     *  Popis zbytku systemu
     */
    void Behavior(){
      double percent;
      if(asistent1.QueueLen() < asistent2.QueueLen()){
        Seize(asistent1);
        Wait(Exponential(2*60));
        Release(asistent1);
      } else if(asistent1.QueueLen() > asistent2.QueueLen()){
        Seize(asistent2);
        Wait(Exponential(2*60));
        Release(asistent2);
      } 
      else {
        if(Random()<0.50) {
          Seize(asistent2);
          Wait(Exponential(2*60));
          Release(asistent2);
        } else {
          Seize(asistent1);
          Wait(Exponential(2*60));
          Release(asistent1);
          
        }
      }
#ifdef debug
      Print("ASISTENT-PRIJEM - zpracovano v: %lf\n",Time);      
#endif

      Seize(mistopredsedkyne);
      //prideleni na konkretni odbor na zaklade predchozi analyzy pozadavku
//      Wait(5); 
      Wait(Exponential(5));
      Release(mistopredsedkyne);
      
      percent = Random();
      
      if(percent <= 0.07){ //Odbor druhostupnoveho rozhodovani 7%
        rozh++;
        //TODO: REDITELOVA PRACE
        Wait(60*21*24);//cca 21 dni kolobeh zpracovani zprav
        if(asistent1.QueueLen() < asistent2.QueueLen()){
          Seize(asistent1);
          Wait(Exponential(2*60));
          Release(asistent1);
        } else if(asistent1.QueueLen() >= asistent2.QueueLen()){
          Seize(asistent2);
          Wait(Exponential(2*60));
          Release(asistent2);
        }
      }
      else if(percent <= 0.17){//verejna podpora 10%
        podpora++;
        Wait(60*21*24);
        Seize(asistent2,1);
        Wait(Exponential(2*60));     
        Release(asistent2);
      }
      else if(percent <= 0.29){//ekonomicky odbor 12%
        eko++;
        Wait(60*21*24);
        Seize(asistent1,1);
        Wait(Exponential(2*60));
        Release(asistent1);
      }
      else if(percent <= 0.51){//hospodarska soutez 22%
        soutez++;
        Wait(60*21*24);
        Seize(asistent2,1);     
        Wait(Exponential(2*60));
        Release(asistent2);
//        Print("uvolneni asistenta\n");
      }
      else{//verejne zakazky 49%    
        verejne++;
        Wait(60*21*24);
        Seize(asistent1,1);
        Wait(Exponential(2*60));
        Release(asistent1);
//        Print("uvolneni asistenta\n");
      }
      archiv.InsLast(this);
      Passivate();
    }

};

/**
 * Specifikace Zpravy
 * Pisemna zprava
 */
class PisemnaZprava : public Zprava {
  void Behavior(){
    cnt++;
    Zprava::podatelnaBehavior(60 * 4);
    Zprava::Behavior();
  }
};

/**
 * Specifikace Zpravy
 * Datova zprava
 */
class DatovaZprava : public Zprava {
  void Behavior(){
    cnt++;
    Zprava::podatelnaBehavior(60 * 4);
    Zprava::Behavior();
    
  }
};

/**
 * Specifikace Zpravy
 * Elektronicka zprava (e-mail)
 */
class ElektronickeZprava : public Zprava {
  void Behavior(){
    cnt++;
    el++;
    Priority=2;
    Zprava::podatelnaBehavior(5);
    Priority=0;
    Zprava::Behavior();
 
  }
};


/* ***************************************** *
 *            Generatory udalosti            * 
 * ***************************************** */

/**
 * Generovani prichodu pisemnych zprav do systemu
 */
class GeneratorPisemnaZprava : public Event {
  void Behavior(){
	 (new PisemnaZprava)->Activate();
    Activate(Time + Exponential(84));
  }
};


/**
 * Generovani prichodu datovych zprav do systemu
 */
class GeneratorDatovaZprava : public Event {
  void Behavior(){
	 (new DatovaZprava)->Activate();
    Activate(Time + Exponential(108)); //108
  }
};


/**
 * Generovani prichodu zprav v elektronicke podobe do systemu
 */
class GeneratorElZprava : public Event {
  void Behavior(){
	 (new ElektronickeZprava)->Activate();
    Activate(Time + Exponential(120));
  }
};

int main(int argc, char **argv)
{
  Init(0,43829.1); // mesic behu
//  Init(0,15*60); //10 hodin behu
//  Init(0,7*24*60); // tyden behu
//  Init(0,1051897.44);//2roky
  
  archiv.SetName("Archiv vyrizenych zadosti");
  reditel[0].SetName("Reditel verejnych zakazek");
  reditel[1].SetName("Reditel hospodarske souteze");
  reditel[2].SetName("Reditel ekonomickeho odboru");
  reditel[3].SetName("Reditel verejne podpory");
  reditel[4].SetName("Reditel odboru druhostupnoveho rozhodovani");

  (new PracovniDoba)->Activate();
  (new GeneratorPisemnaZprava)->Activate();
  (new GeneratorDatovaZprava)->Activate();
  (new GeneratorElZprava)->Activate();
  Run();
  
  
  podatelna1.Output();
  podatelna2.Output();
//  Print("prichozich elektronickych zprav: %d\n",el);
//  asistent1.Output();
//  asistent2.Output();
//  mistopredsedkyne.Output();
//  archiv.Output();
#ifdef debug
  SIMLIB_statistics.Output();

  Print("Pocet zprav na odbor verejnych zakazek %d\n",verejne);
  Print("Pocet zprav na odbor hospodarske souteze: %d\n",soutez);
  Print("Pocet zprav na ekonomicky odbor: %d\n",eko);
  Print("Pocet zprav na odbor verejne podpory: %d\n",podpora);
  Print("Pocet zprav na odboru druhostupnoveho rozhodovani: %d\n",rozh);
#endif
  Print("Prichozich zprav celkem: %d\n",cnt);
  
  return 0;
}
