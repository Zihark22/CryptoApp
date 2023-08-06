#ifndef CRYPTAGE_H
#define CRYPTAGE_H

#include "librairies.h"

//#define chaineARechercher "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN";
// regarder assurer avoir un entier

#define TAILLE_MAX 1000

#define aMax 65 // A
#define zMax 90 // Z

#define Amin 97 // a
#define Zmin 122 // z

#define NBCHAR 26

/*

a =	97		A = 65
z =	122		Z = 90
0 = 48		9 = 57

*/
void chiffrCesar(FILE*fichier,int cle_cryptage);
void dechiffrCesar(FILE*fichier,int cle_decryptage);

void chiffrVigen(FILE*fichier,QString cle_cryptage);
void dechiffrVigen(FILE*fichier,QString cle_decryptage);

#endif // CRYPTAGE_H
