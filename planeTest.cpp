#include "AirportAnimator.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <sys/sem.h>

#define NUM_SEMS 1
#define NUM_COMMANDS 1
#define SEM_KEY 1234 

using namespace std;

// Command-line arguments
int total_passengers;
int total_tours;
int sem_id; 
int completedTours = 0;

// Plane function
void* planeSim(void* arg) {

    int planeNum = *((int *) arg);
    struct sembuf waitCommand[NUM_COMMANDS];
    struct sembuf signalCommand[NUM_COMMANDS];
    

    
    waitCommand[0].sem_num = 0;
    waitCommand[0].sem_op = -1;
    waitCommand[0].sem_flg = 0;

    // Semaphore signal setup
    signalCommand[0].sem_num = 0;
    signalCommand[0].sem_op = 1;
    signalCommand[0].sem_flg = 0;
    
//while loop is checking if total tours is 0 and updating with done and ending the thread.
   
    while(true)
    {
      if(total_tours <= 0){
	AirportAnimator::updateStatus(planeNum, "DONE");
      	return NULL;
      }

      
      
      
      AirportAnimator::updateStatus(planeNum, "BOARD");
      for(int passenger=1; passenger<=10; passenger++)
      {

	while(total_passengers <= 0){
		sleep(rand() % 3);
	}      

        AirportAnimator::updatePassengers(planeNum, passenger);
        total_passengers--;//tracking the passengers in pool of passengers and subtracting 1 each time 1 passenger is boarded.
        sleep(rand() % 3);
     
      }
  
    AirportAnimator::updateStatus(planeNum, "TAXI");
    AirportAnimator::taxiOut(planeNum);

    semop(sem_id, waitCommand, NUM_COMMANDS);

  

    AirportAnimator::updateStatus(planeNum, "TKOFF");
    AirportAnimator::takeoff(planeNum);

    semop(sem_id, signalCommand, NUM_COMMANDS);


    AirportAnimator::updateStatus(planeNum,"TOUR");
    sleep(5 + rand() % 41);

    AirportAnimator::updateStatus(planeNum,"LNDRQ");
    sleep(2);

    if (semop(sem_id, waitCommand, NUM_COMMANDS) == -1) {
      cerr << "semop() wait failed with errno = " << errno << endl;
      pthread_exit(NULL);
    }// used as more of a debug
     // used if statement to track if these wait and signal commands were working
  

    AirportAnimator::updateStatus(planeNum,"LAND");
    AirportAnimator::land(planeNum);

    if (semop(sem_id, signalCommand, NUM_COMMANDS) == -1) {
      cerr << "semop() signal failed with errno = " << errno << endl;
      pthread_exit(NULL);
    }

    AirportAnimator::updateStatus(planeNum, "TAXI");
    AirportAnimator::taxiIn(planeNum);

    AirportAnimator::updateStatus(planeNum, "DEPLN");
    for(int passenger=9; passenger>=0; passenger--)
    {
      AirportAnimator::updatePassengers(planeNum, passenger);
      total_passengers++;//deplaneing passengers adds passengers back into the pool of total_passengers available
      sleep(1);
    }
  
    AirportAnimator::updateStatus(planeNum, "DEPLN");
    completedTours = completedTours + 1; //tracking the tours completed and passing it into the updateTours function
    AirportAnimator::updateTours(completedTours);
    total_tours--;//decreasing the inputted tours completed until 0


  }
    

  

  return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <total_passengers> <total_tours>" << endl;
        return 1;
    }

    total_passengers = atoi(argv[1]);//setting the total_passengers to the total_passengers argument which is the first one after the ./run com
    total_tours = atoi(argv[2]);//same for total tours but the second command arg
    cerr.flush();
    sleep(3);
    AirportAnimator::init();
     // Directly use the defined semaphore key
    sem_id = semget(SEM_KEY, NUM_SEMS, IPC_CREAT | 0666);//sem key is define as 1234 
    if (sem_id == -1) {
        cerr << "semget() failed with errno = " << errno << endl;
        return 2;
    }

    // Initialize semaphore to 1 if it's a new semaphore
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        cerr << "semctl() SETVAL failed with errno = " << errno << endl;
        return 3;
    }

   
    //creating planeNums that correspond to each plane value
    int planeNum1 = 1;
    int planeNum2 = 2;
    int planeNum3 = 3;
    int planeNum4 = 4;
    int planeNum5 = 5;
    int planeNum6 = 6;
    int planeNum7 = 7;
    int planeNum0 = 0;
    pthread_t pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7;
   //intiaizing 8 threads total 

    //creating each thread pass in the thread pointer ex pt0, nothing, void function with critical code and the void * parameter or planenum in this case
    pthread_create(&pt0, NULL, planeSim, (void*)&planeNum0);
  

    pthread_create(&pt1, NULL, planeSim, (void*)&planeNum1);

    pthread_create(&pt2, NULL, planeSim, (void*)&planeNum2);
    

    pthread_create(&pt3, NULL, planeSim, (void*)&planeNum3);
 

    pthread_create(&pt4, NULL, planeSim, (void*)&planeNum4);
    

    pthread_create(&pt5, NULL, planeSim, (void*)&planeNum5);
    
    pthread_create(&pt6, NULL, planeSim, (void*)&planeNum6);
   
    pthread_create(&pt7, NULL, planeSim, (void*)&planeNum7);


    // Waiting for all threads to finish
    pthread_join(pt0, NULL);
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);
    pthread_join(pt3, NULL);
    pthread_join(pt4, NULL);
    pthread_join(pt5, NULL);
    pthread_join(pt6, NULL);
    pthread_join(pt7, NULL);
    

    AirportAnimator::end();

    return 0;
}
