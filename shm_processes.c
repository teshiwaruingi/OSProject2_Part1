#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BankAccount 0

void ChildProcess(int[], int);
void MumProcess(int[]);

sem_t *mutex;

int main(int argc, char *argv[]) {
  int ShmID;
  int *ShmPTR;
  pid_t pid;
  int status;

  if (argc != 3) {
    printf("usage:  ref_psdd <parents> <children> (where parent = [1 | 2], "
           "children = [N > 0]\n");
    exit(1);
  }

  ShmID = shmget(IPC_PRIVATE, 1 * sizeof(int),
                 IPC_CREAT | 0666); // shared memory for 1 integer
  if (ShmID < 0) {
    printf("*** shmget error (server) ***\n");
    exit(1);
  }
  printf("Process has received a shared memory of 1 integer...\n");

  ShmPTR = (int *)shmat(ShmID, NULL, 0);
  if (*ShmPTR == -1) {
    printf("*** shmat error (server) ***\n");
    exit(1);
  }

  // create, initialize semaphore
  if ((mutex = sem_open("examplesemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
    perror("semaphore initilization");
    exit(1);
  }

  ShmPTR[BankAccount] = 0; // Initializing BankAccount

  printf("Process has attached the shared memory...\n");
  pid = fork();

  if (pid < 0) {
    printf("fork error\n");
    exit(1);
  } else if (pid == 0) {
    if (atoi(argv[2]) > 1) {
      int num_kids = atoi(argv[2]);
      for (int i = 1; i <= num_kids; i++) {
        if (fork() == 0) {
          ChildProcess(ShmPTR, i);
          exit(0);
        }
      }
    } else {
      ChildProcess(ShmPTR, 1);
      exit(0);
    }
  } else {
    if (atoi(argv[1]) == 2) {
      pid_t pid_m;
      pid_m = fork();
      if (pid_m == 0) {
        MumProcess(ShmPTR);
        exit(0);
      }
    }
    int localBalance;
    int random_deposit;
    int random_decision;
    srandom(getpid());
    while (true) {
      // ParentProcess
     / The "sleep" function causes the program to pause for a random amount of time (up to 5 seconds).
// The "sem_wait" function is used to acquire a semaphore (a synchronization object) before accessing the shared memory.
// "mutex" is the name of the semaphore.
// "ShmPTR" is a pointer to shared memory, where the bank account balance is stored.
// "BankAccount" is the index of the bank account in the shared memory.
// "localBalance" is a local variable that holds a copy of the bank account balance.
// "random_decision" is a random number that is used to decide whether to make a deposit or not.
// "random_deposit" is a random number that determines the amount of the deposit (if any).
sleep(random() % 6); // pause for a random amount of time (up to 5 seconds)
sem_wait(mutex); // acquire the semaphore before accessing the shared memory
printf("Dear Old Dad: Attempting to Check Balance\n");
localBalance = ShmPTR[BankAccount]; // copy the bank account balance from shared memory
random_decision = random(); // decide whether to make a deposit or not
if (random_decision % 2 == 0) { // if random_decision is even, make a deposit
if (localBalance < 100) { // if the balance is less than 100, make a deposit
random_deposit = random() % 101; // generate a random deposit amount (0-100 inclusive)
if (random_deposit % 2 == 0) { // if the deposit amount is even, make the deposit
localBalance += random_deposit; // update the local balance
printf("Dear old Dad: Deposits $%d / Balance = $%d\n", random_deposit, localBalance);
}
}
}




           else { // if random_decision is odd, do not make a deposit
printf("Dear old Dad: Doesn't have any money to give\n");
}
} else { // if the balance is greater than or equal to 100, do not make a deposit
printf("Dear old Dad: Thinks student has enough Cash ($%d)\n", localBalance);
}
} else { // if random_decision is odd, do not make a deposit
printf("Dear old Dad: Last Checking Balance = $%d\n", localBalance);
}
ShmPTR[BankAccount] = localBalance; // update the bank account balance in shared memory
sem_post(mutex); // release the semaphore after accessing the shared memory




    wait(&status);
    printf("Process has detected the completion of its child...\n");
    shmdt((void *)ShmPTR);
    printf("Process has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    printf("Process has removed its shared memory...\n");
    printf("Process exits...\n");
    exit(0);
  }
}

void ChildProcess(int SharedMem[], int nth) {
  int localBalance;
  int random_withdrawal;
  int random_decision;
  srandom(getpid());
  // keep looping until the program is terminated
while (true) {
    // sleep for a random amount of time
    sleep(random() % 6);

    // wait for the mutex lock before accessing shared memory
    sem_wait(mutex);

    // print a message indicating that the student is attempting to check their balance
    printf("Poor Student #%d: Attempting to Check Balance\n", nth);

    // retrieve the student's balance from shared memory
    localBalance = SharedMem[BankAccount];

    // decide whether to withdraw money or not
    random_decision = random();
    if (random_decision % 2 == 0) {
      // randomly select an amount of money to withdraw (between 0 and 50)
      random_withdrawal = random() % 51;
      printf("Poor Student needs $%d\n", random_withdrawal);

      // check if the student has enough money to make the withdrawal
      if (random_withdrawal <= localBalance) {
        // make the withdrawal and update the student's balance
        localBalance -= random_withdrawal;
        printf("Poor Student #%d: Withdraws $%d / Balance = $%d\n", nth,
               random_withdrawal, localBalance);
      }
      // if the student doesn't have enough money, print a message
      else {
        printf("Poor Student #%d: Not enough money to withdraw $%d.\n", nth,
               random_withdrawal);
      }
    }
    // if the student decides not to withdraw money, print a message
    else {
      printf("Poor Student #%d: Decides not to withdraw money.\n", nth);
    }

    // update the student's balance in shared memory
    SharedMem[BankAccount] = localBalance;

    // release the mutex lock
    sem_post(mutex);
}
 else {
        printf("Poor Student #%d: Not Enough Cash ($%d)\n", nth, localBalance);
      }
    } else {
      printf("Poor Student #%d: Last Checking Balance = $%d\n", nth,
             localBalance);
    }
    SharedMem[BankAccount] = localBalance;
    sem_post(mutex);
  }
}

// define a function to simulate the behavior of a mother process
void MumProcess(int SharedMem[]) {
  // initialize a variable to store the mother's balance
  int localBalance;

  // initialize a variable to store the amount of money the mother will deposit
  int random_deposit;

  // seed the random number generator with the process id
  srandom(getpid());

  // keep looping until the program is terminated
  while (true) {
    // sleep for a random amount of time
    sleep(random() % 11);

    // wait for the mutex lock before accessing shared memory
    sem_wait(mutex);

    // print a message indicating that the mother is attempting to check her balance
    printf("Lovable Mom: Attempting to Check Balance\n");

    // retrieve the mother's balance from shared memory
    localBalance = SharedMem[BankAccount];

    // check if the mother's balance is low
    if (localBalance <= 100) {
      // deposit a random amount of money (between 0 and 125)
      random_deposit = random() % 126;
      localBalance += random_deposit;
      printf("Lovable Mom: Deposits $%d / Balance = $%d\n", random_deposit,
             localBalance);
    }

    // update the mother's balance in shared memory
    SharedMem[BankAccount] = localBalance;

    // release the mutex lock
    sem_post(mutex);
  }
}

}
