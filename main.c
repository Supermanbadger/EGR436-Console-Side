//-------------------------------------------
//EGR 436 - Lab 1 - Computer Serial Command Terminal
//Collin Maker and Thomas Quist
//-------------------------------------------

//Includes
#include <stdio.h>
#include <stdint-gcc.h>
#include <windows.h>
#include <conio.h>
#include "globals.h"
#include "support.h"
#include "windows.h"


int main()
{
    char inputChar;
    char* sendString;
    int err;


    if (InitSerialPort())
    {
        printf("Comport failed to initialize\n");
        return 1;
    }
    else {
        printf("Comport initialized successfully\n");
        splash();
        fflush(stdout);
    }

    while(1)
    {
        err = ReadUserInput();

}
    CloseHandle(hSerial);
    return(0);
}

void splash()
{
    printf("*-----------------------------------*\n");
    printf("You may issue the following commands\n"
           "STORE\n"
           "DIR\n"
           "MEM\n"
           "DELETE #\n"
           "READ #\n"
           "CLEAR\n");
    printf("*-----------------------------------*\n");


}
int InitSerialPort()
{
    hSerial =CreateFile(COMPORT,
                        GENERIC_READ|GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        0);
    if (hSerial == INVALID_HANDLE_VALUE)
    {
        if(GetLastError()==ERROR_FILE_NOT_FOUND) {
            fprintf(stderr, "Error Comport doesn't exist\n");
            return 1;
        }
    }
/* Setup Paramaters here*/
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error getting COM state\n");
    }
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if(!SetCommState(hSerial,&dcbSerialParams)) {
        fprintf(stderr, "Error setting comport params\n");
        return 1;
    }

    /*Setup Timeouts*/
    COMMTIMEOUTS timeouts = {0};

    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=50;
    timeouts.ReadTotalTimeoutMultiplier=10;
    timeouts.WriteTotalTimeoutConstant=50;
    timeouts.WriteTotalTimeoutMultiplier=10;

    if(!SetCommTimeouts(hSerial,&timeouts)) {
        fprintf(stderr, "Error setting comport timeouts\n");
        return 1;
    }




    return 0;
}


//---------------------------------------------------
int SendSerialString(char *inString)
{
    int n = strlen(inString);
    char szBuff[n];
    strcpy(szBuff,inString);

    DWORD dwBytesWrite = 0;

    if(!WriteFile(hSerial, szBuff, n, &dwBytesWrite, NULL )) {
        fprintf(stderr, "Error reading\n");
        return 1;

    }
if (dwBytesWrite < n)
    {
    return 1;
    }
else
{
    printf("Written to buffer:%s\n",szBuff);
}
    return 0;
}

int ReadSerialString() {
    char szBuff[READBUFF + 1] = {0};
    char* string;
    DWORD dwBytesRead = 0;
    if(!ReadFile(hSerial, szBuff, READBUFF, &dwBytesRead, NULL)){
        printf("Error reading COM string.\n");

    }
    else
    {
        string = malloc(dwBytesRead);
        strcpy(string,szBuff);
        printf("Recieved:%s\n\n",string);
    }
//error occurred.  Report to user.

}


int ReadUserInput() {
    char userString[INPUTSIZE];
    int fLen;
    char *passString;
    char *fNameStart;
    char *fNameEnd;
    char *fileName;
    char *fileTxt;
    int err;

    while (1)
    {
        fgets(userString, 100, stdin);

    if (strstr(userString, "STORE") != NULL) {
        char searchString[5] = "\"";
        fNameStart = strstr(userString, searchString) + 1;
        memset(searchString, '\0', sizeof(searchString));
        strcpy(searchString, ".txt");
        fNameEnd = 3 + strstr(userString, searchString);
        fLen = (fNameEnd) - fNameStart + 1;
        fileName = (char *) calloc(fLen + 1, sizeof(char));
        strncpy(fileName, fNameStart, fLen);
        strcat(fileName[fLen + 1], "\0");
        fileTxt = fileGet(fileName);
        passString = (char *)calloc(strlen(fileTxt)+strlen(fileName)+8,sizeof(char)); //7 is for STORE 2x\n and null end char
        strcpy(passString,"STORE");
        err = SendSerialString(passString);
        err = SendSerialETX();
    //    err = SendSerialString(fileName);
      //  err = SendSerialETX();
        err = SendSerialString(fileTxt);
        err = SendSerialETX();
       err = ReadSerialString();
       // printf("%s\n", fileTxt);

        return err;
    }

    if (strstr(userString, "DIR") != NULL) {
        passString = (char *) calloc(5, sizeof(char)); //4 is for DIR\n and null end char
        strcpy(passString, "DIR\n");
        err = SendSerialString(passString);
        err = SendSerialETX();
        err = ReadSerialString();
        return err;
    }
    if (strstr(userString, "MEM") != NULL) {
        passString = (char *) calloc(5, sizeof(char)); //4 is for MEM\n and null end char
        err = SendSerialString(passString);
        err = SendSerialETX();
        err = ReadSerialString();
        return err;
    }
    if (strstr(userString, "DELETE") != NULL) {
        passString = (char *) calloc(10, sizeof(char)); //10 is for DELETE_#\n and null end char
        strncpy(passString, userString, 6);
        err = SendSerialString(passString);
        err = SendSerialETX();
        memset(passString, '\0', sizeof(passString));
        strncpy(passString, userString+7*sizeof(char), 1);
        err = SendSerialString(passString);
        err = SendSerialETX();
        err = ReadSerialString();
        return err;
    }
    if (strstr(userString, "READ") != NULL) {
        passString = (char *) calloc(8, sizeof(char)); //8 is for READ_#\n and null end char
        strncpy(passString, userString, 4);
        err = SendSerialString(passString);
        err = SendSerialETX();
        memset(passString, '\0', sizeof(passString));
        strncpy(passString, userString+5*sizeof(char), 1);
        err = SendSerialString(passString);
        err = SendSerialETX();
        err = ReadSerialString();
        return err;
    }
    if (strstr(userString, "CLEAR") != NULL) {
        passString = (char *) calloc(7, sizeof(char)); //4 is for CLEAR\n and null end char
        strcpy(passString, "CLEAR\n");
        err = SendSerialString(passString);
        err = SendSerialETX();
        err = ReadSerialString();
        return err;
    }


}

}


char* fileGet(char* fileName)
{
    FILE *fp;
    char string[MAXFILESIZE];
    char buff[READBUFF];
    char *txtEnd;
    char *txtStr;
    unsigned int txtLen;
    int eofFlag;
    int firstline = 0;

    eofFlag = 0;

    fp = fopen(fileName,"r");


    while (eofFlag == 0)
    {
        while (fgets(buff, sizeof buff, fp) != NULL) {
            //printf("%s", buff);
            if (firstline == 0)
            {
                strcpy(string,buff);
                firstline = 1;
            }
            else
            {
                strncat(string, buff, sizeof(buff));
               // strcat(string, "\n");
            }

        }
        if (feof(fp)) {
            fclose(fp);
            strcat(string,"\000");
            eofFlag = 1;
        } else {
            // some other error interrupted the read

        }

    }

    txtLen =  strlen(string);
    txtStr = (char *) calloc(txtLen+1, sizeof(char));
    strncpy(txtStr,string,txtLen);
    strcat(txtStr,"\000");
    return txtStr;
}

int SendSerialETX(void)
{
    int n = 2;
    char szBuff[2];
    szBuff[0] = 0x04;
    szBuff[1] = NULL;

    DWORD dwBytesWrite = 0;

    if(!WriteFile(hSerial, szBuff, n, &dwBytesWrite, NULL )) {
        fprintf(stderr, "Error reading\n");
        return 1;

    }
    if (dwBytesWrite < n)
    {
        return 1;
    }
    else
    {
          printf("Written to buffer:%s \n",szBuff);
    }
    return 0;
}