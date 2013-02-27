/*
// EXTERNS.H
//
// All of our classes will be in constant contact with each other, and
// each class will be unique.  Therefore, they need to be globally accessible--
// but making the actual class objects themselves global is un bad idea.
//
// Instead, we will make pointers to all the classes and make those pointers
// globally accessible.  Then, everybody wins.
*/

#ifndef __MASTER__SERVER__EXTERNS_H_
#define __MASTER__SERVER__EXTERNS_H_



/*** Class pointers ***/

extern class InputOutputStats * ptrIO;
extern class IPBanList *  ptrBanlist;
extern class PacketManagement * ptrSocketClass;
extern class WON2ServerNetwork * ptrMods;
#endif
