//
// Created by colli on 1/8/2019.
//

#ifndef EGR436_LAB1_SERIALCOMMANDS_SUPPORT_H
#define EGR436_LAB1_SERIALCOMMANDS_SUPPORT_H

//Macros
#define COMPORT "COM6"
#define READBUFF  100
#define INPUTSIZE 50
#define MAXFILESIZE 1024

//Prototypes
int InitSerialPort ();
int SendSerialString(char *inString);
int SendSerialETX(void);
int ReadSerialString();
int ReadUserInput();
int nDigits (int number);
void splash();
char* fileGet(char* fileName);


#endif //EGR436_LAB1_SERIALCOMMANDS_SUPPORT_H
