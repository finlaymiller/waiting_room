## Doctor's waiting room problem 

A hospital has a doctor who treats patients during her night shift. The doctor has a small examination room with room for only one patient. Attached to her examination room is a waiting room with three seats where patients can sit and wait if the doctor is currently examining another patient. When there are no patients who need help, the doctor takes a nap. If a patient arrives during the night and finds the doctor sleeping, the patient must awaken the doctor to ask for help. If a patient arrives and finds the doctor currently examining another patient, the patient sits on one of the seats in the waiting room and waits. If no seats are available, the patient goes away to have a coffee from the hospital cafeteria and will come back later. Using multithreading, write a program (using C or C++) that coordinates the activities of the doctor and the patients. Use mutex locks and semaphores to ensure that two patients are not in the examination room at the same time and that the patients waiting in the waiting room get their turn.

## Solution
Create n patients using PThreads, with each patient running as a separate thread. The doctor runs as a separate thread as well. Patient threads will alternate between having coffee in the cafeteria and being treated by the doctor. If the doctor is available, they will be treated immediately. Otherwise, they will either sit in a chair in the waiting room or, if no chairs are available, will go back to the cafeteria for a random amount of time and will return to see the doctor at a later time. If a patient arrives and notices that the doctor is taking a nap, the patient must notify the doctor using a semaphore. When the doctor finishes treating a patient, she must check to see if there are patients waiting for help in the waiting room. If so, the doctor must help each of these patients in turn. If no patients are present, the doctor may return to napping. Perhaps the best option for simulating the time that patients spend in the cafeteria as well as the doctor treating a patient, is to have the appropriate threads sleep for a random period of time. Each patient thread should print its state (drinking coffee, waiting for doctor, getting treated, doctor not available etc.) along with its threadID. Similarly, the doctor should print its state (treating, checking for next patient, sleeping etc.).

## To Use
* To build ` gcc -std=c99 -o <executable-name> main.c `
* To run ` ./<executable-name> `
* To exit ` ^C `
