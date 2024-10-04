/*

Lab 5
Operating Systems
Bo Broyles
3/21/23

*/


#include <unistd.h>
#include <iostream>
//#include <ostream>
#include <pthread.h>  
#include <semaphore.h>
#include <stdio.h>
//using std::cout;

//i need a structure for chopsticks
//available represents whether a chopstick has been picked up or not
//i need 5 of them becuase there are 5 on the table
//if picked up then available = 1  (not available anymore)
//if chopstick is on the table then available = 0
typedef struct chopstick
{
    int available = 0; //availability of the chop stick to be picked up, 0 = available, 1 = not available
    int nr = 0; //priority of chopstick 
};

//there needs to be a structure for a person
//each person can have a right and a left chopstick
//a value of 0 means they dont have the chopstick
//a value of 1 means they have the chopstick 
typedef struct person
{
    int rightStick;
    int leftStick;
    int personNum;
};

//semaphores for the chopsticks
//if they are locked it means the associated chopstick has been picked up 
//the semaphores are meant to represent each chopstick 
//they will be used to represent if a stick has been picked up and therefore locked
sem_t Locks[5];
//array of chopsticks
//5 for the 5 chopsticks on the table
chopstick sticks[5];
//philosopher number
//5 philosophers at the table
int PhilNum[5] = {0, 1, 2, 3, 4};

void assignnum(int r){
   // srand(time(NULL));
    int num = rand() % 100 + 3;
    sticks[r].nr = num;
}
//pickup is when a philosopher picks up a chopstick
//int Num represents the chopstick number that was picked up
void pickup(int Num){ 
    //i thought i needed the semaphores here but i moved them to testingEat
   // sem_wait(&Locks[Num]);
    sticks[Num].available = 1; //setting chopstick availability to 1 because it is now picked up and not available
   // sem_post(&Locks[Num]);
  

}

//putdown is when a philosopher puts down a chopstick 
//int stickNum represents the chopstick number that was put down
void putdown(int stickNum){ 
    //I thought i needed the semaphores here but i moved them to testingEat
   // sem_wait(&Locks[stickNum]);
    sticks[stickNum].available = 0; //chopstick availability set to 0 because it is being put down and available to others
   // sem_post(&Locks[stickNum]);
  
}

//eating is when the philosopher has already picked up both chopsticks and now will eat
//a random amount of time to eat for is generated and then the philosopher is "put to sleep" for that amount of time
void eating(int personNum) { 

    int eatTime = 0; //seconds person is eating for
    eatTime = rand() % 8 + 1; //random eating time between 1 and 10 seconds
    std::cout << "Philosopher " << personNum << " is eating for " << eatTime << " seconds.\n";  //print messaage to terminal
    sleep(eatTime); //put person thread to sleep for amount of time they eat for 
    
}


//testingEat is called when the philosopher is done thinking and ready to eat
//int philosopherNum is the number of the philosopher, used to see what chopsticks he needs
// void testingEat(int philosopherNum){

//     int left = philosopherNum; //the left chopstick the philosopher needs to pick up 
//     int right = (philosopherNum+1) % 5; //the right chopstick the philosopher needs to pick up, it is +1 and % 5 because it must be 1 greater than his own number and if he is philosopher 5 then he must pick up chopstick 0

//     sem_wait(&Locks[left]); //locking the left chopstick, and waiting if chopstick is already picked up
//     pickup(left); //picking up the left chopstick
//     std::cout << "Philosopher " << philosopherNum+1 << " picked up chopstick " << left << "\n"; 


//     sleep(20); //wait 20 seconds before pickingup right to ensure deadlock

//     sem_wait(&Locks[right]); //locking the right chopstick, and waiting if chopstick is already picked up
//     pickup(right); //picking up the right chopstick
//     std::cout << "Philosopher " << philosopherNum+1 << " picked up chopstick " << right << "\n";

  

//     eating(philosopherNum); //when both chop sticks have been picked up philosopher can begin eating

//     putdown(left); //putting down the left chopstick after eating
//     std::cout << "Philosopher " << philosopherNum+1 << " put down chopstick " << left << "\n";
//     putdown(right); //putting down the right chopstick after eating
//     std::cout << "Philosopher " << philosopherNum+1 << " put down chopstick " << right << "\n";
    
//     sem_post(&Locks[left]); //unlocking the left chopstick because it has been put down
//     sem_post(&Locks[right]); //unlocking the right chopstick because it has been put down
// }


// The following code is an interpreation of an algorithm presented by Penn State Professors Catuscia Palamidessi and Oltea Mihaela Herescu
// The link to their publication is here: https://arxiv.org/pdf/cs/0109003.pdf
// The idea of the algorithm is that the philosopers are not picking up the left chopstick first everytime but instead they are randomly picking up 
// either the right or the left chopstick first, this is decided by assignnum. The chopstick with the larger nr value is picked up first. 
// Then the chopstick is picked up, the thread is then put to sleep for 10 seconds because this was how i ensured deadlock earlier. Next the philosopher 
// checks to see if their other chopstick is available, if it is then the philosopher picks it up and eats, if not then the held chopstick is placed down 
// and the process begins again.
void newtestingEat(int philnum){

    int left = philnum; //the left chopstick the philosopher needs to pick up 
    int right = (philnum+1) % 5; //the right chopstick the philosopher needs to pick up, it is +1 and % 5 because it must be 1 greater than his own number and if he is philosopher 5 then he must pick up chopstick 0
    
    //flag to be signaled when philosopher eats, repeats until they eat    
    bool flag = false;
    while(flag == false){

    int RL = 0; //holds chopstick num of highest nr value
    int RL2 = 0; //holds chopstick num of lowest nr value
    assignnum(left); //assigning random nr value to chopsticks 
    assignnum(right); //assigning random nr value to chopsticks
    
    std::cout << sticks[left].nr << ": left ";
    std::cout << sticks[right].nr << ": right \n";  
    
    //seeing which chopstick has the highest nr value and assigning them to RL or RL2
    if (sticks[left].nr > sticks[right].nr){ RL = left; RL2 = right; }
        else { RL = right; RL2 = left; }
    
    
    //locking higher nr value chopstick
    sem_wait(&Locks[RL]);
    pickup(RL);
    std::cout << "Philosopher " << philnum << " picked up chopstick " << RL << "\n"; 
    
    sleep(10); //to try and ensure deadlock
    
    //if the other chopstick is available then the philosopher picks it up and begins to eat 
    if (sticks[RL2].available == 0){
            
        sem_wait(&Locks[RL2]);
        std::cout << "Philosopher " << philnum << " picked up chopstick " << RL2 << "\n";
        eating(philnum);
        sticks[RL].nr = 0;
        sticks[RL2].nr = 0;
        putdown(left); //putting down the left chopstick after eating
        std::cout << "Philosopher " << philnum << " put down chopstick " << RL << "\n";
        putdown(right); //putting down the right chopstick after eating
        std::cout << "Philosopher " << philnum << " put down chopstick " << RL2 << "\n";
    
        sem_post(&Locks[RL]); //unlocking the left chopstick because it has been put down
        sem_post(&Locks[RL2]); //unlocking the right chopstick because it has been put down
        flag = true;
    
    }
    //if the other chopstick is not available then the chopstick is put down and the philosopher begins the process again
    else {
        putdown(RL);
        std::cout << "Philosopher " << philnum << " put down chopstick " << RL << "\n";
        sem_post(&Locks[RL]);
        }

    }
    

}

//thinking is what all the philosophers do before eating
//they think for a certain amount of time before getting hungry
//once hungry they try to pick up both chopsticks 
void* thinking( void* i){ 
    
    int num = *reinterpret_cast<int*>(i); //got this from cplusplus.com link: https://cplusplus.com/forum/general/269149/
    
    do {
    
        int thinkTime; //number of seconds philosopher will sleep for
        thinkTime = rand() % 5 + 1; //generate random number of seconds to sleep between 1 and 9
        std::cout << "Philosopher " << num << " is thinking for " << thinkTime << " seconds.\n";
        sleep(thinkTime); //think/sleep for generated amount of time

        newtestingEat(num); //after thinking the philosopher gets hungry

    } while (true); //always running
}

//array to hold the philosophers sitting at the table
//based on problem the the seating arrangment of the philosophers is based on philosopher index +1 and index -1 (+1 is to the right -1 is to the left)
//person philosophers[5]; 
// ^^^^ didnt end up needing person types


//the main function has the simulation running 
//first the semaphores are initialized, then the threads
//the threads are then joined together and call the thinking function
int main(){
    srand(time(NULL));
    
    //initializing the semaphores that protect the chopsticks 
        for (int i = 0; i<5; i++){
            sem_post(&Locks[i]);
        }
    //creating the philosophers 
    pthread_t th1, th2, th3, th4, th5;
    pthread_attr_t attr1, attr2, attr3, attr4, attr5;

    //thinking is the function that they all start out calling
    pthread_create(&th1, &attr1, thinking, &PhilNum[0]);
    pthread_create(&th2, &attr2, thinking, &PhilNum[1]);
    pthread_create(&th3, &attr3, thinking, &PhilNum[2]);
    pthread_create(&th4, &attr4, thinking, &PhilNum[3]);
    pthread_create(&th5, &attr5, thinking, &PhilNum[4]);

    //joinging all the threads (philosophers) to the main thread 
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);
    pthread_join(th4, NULL);
    pthread_join(th5, NULL);  

    return 0;
}