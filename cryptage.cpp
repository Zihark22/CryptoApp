#include "cryptage.h"
// -------- VAR GLOBALES -------------------------
QString AlphabetMIN="azertyuiopqsdfghjklmwxcvbn";
QString AlphabetMAX="AZERTYUIOPQSDFGHJKLMWXCVBN";
QString listeLettres=AlphabetMIN+AlphabetMAX;

QString listeChiffres="0123456789";
QString listeSpeciaux=":;<>=?@[]\\^_'";
QString listeASCII=listeChiffres+listeSpeciaux;

//----------------------    CESAR   ------------------------
void chiffrCesar(FILE*fichier,int cle_cryptage){
    int longueur,i,cle=cle_cryptage;
    int caract=0,fin=0,tours=0;
    char chaine[TAILLE_MAX]={0};
    long position;
    fseek(fichier,0,SEEK_SET);
    if (fichier != NULL)
    {
        caract=fgetc(fichier);
        while(caract!=EOF){
            caract=fgetc(fichier);
            if (caract=='\n')
            {
                fin++;
            }
        }
        fin++;
        if(cle>93){
            do{
                cle-=93;
            }while(cle>93);
        }
        fseek(fichier,0,0);
        do{
            position=ftell(fichier);
            fgets(chaine,TAILLE_MAX,fichier);
            longueur=strlen(chaine);

            for (i=0;i<longueur;++i)
            {
                if(cle==0){
                    chaine[i]=chaine[i];
                }
                else{
                    if(chaine[i]>=32 && chaine[i]<=125){
                        if(chaine[i]+cle<=125)
                            chaine[i]+=cle;
                        else
                            chaine[i]=chaine[i]+cle-93;
                    }
                    else
                        chaine[i]=chaine[i];
                }
            }
            fseek(fichier,position,0);
            fputs(chaine,fichier);
            tours++;
        }while(tours<fin);
    }
    else
    {
        printf("Impossible d'ouvrir le fichier.\n");
    }
}

void dechiffrCesar(FILE*fichier,int cle_decryptage){
    int longueur,i,cle=cle_decryptage;
    int caract=0;
    int fin=0;  // nombres de lignes
    int tours=0; // donne les répétitions
    char chaine[TAILLE_MAX]={0}; //une ligne
    long position=0;

    fseek(fichier,0,0);
    if (fichier != NULL)
    {
        caract=fgetc(fichier);
        while(caract!=EOF){
            caract=fgetc(fichier);
            if (caract=='\n')
            {
                fin++;
            }
        }
        fin++;
        if(cle>93){
            do{
                cle-=93;
            }while(cle>93);
        }
        fseek(fichier,0,0);
        do{
            position=ftell(fichier);
            fgets(chaine,TAILLE_MAX,fichier);
            longueur=strlen(chaine);
            for (i=0;i<longueur;++i)
            {
                if(chaine[i]>=32 && chaine[i]<=125){
                    if(chaine[i]-cle>=32)
                        chaine[i]-=cle;
                    else
                        chaine[i]=chaine[i]-cle+93;
                }
                else
                    chaine[i]=chaine[i];

            }
            fseek(fichier,position,0);
            fputs(chaine,fichier);
            tours++;
        }while(tours<fin);
        fseek(fichier,0,0);
    }
    else
    {
        printf("\nImpossible d'ouvrir le fichier.\n");
    }
}

//----------------------    VIGENERE   ------------------------

void chiffrVigen(FILE*fichier,QString cle_cryptage){
    int i;
    int indice=0;
    long nbrCar=0,tailleCle=0;
    int cryptage=0;
    char* crypter=NULL;
    char* texte=NULL;

    fseek(fichier,0,SEEK_SET);
    const char *cle;    // clé de chiffrement

    cle_cryptage=cle_cryptage.toLower();

    cle = cle_cryptage.toStdString().c_str();
    tailleCle = cle_cryptage.size();
//    qDebug() << "clé de Vigenère = " << cle << " de taille " << tailleCle;

 //////---------Avoir toute le texte dans var texte---------//////
    while(fgetc(fichier)!=EOF)
    {
        nbrCar++;
    }
//    qDebug() << "Nombre de caractères" << nbrCar ;
    if(nbrCar>0){
        texte = (char*) malloc( nbrCar * sizeof(char) );
    }
    else {
        qDebug() <<"Fichier vide";
        exit(-1);
    }
    fseek(fichier,0,SEEK_SET);
    for (i = 0; i < nbrCar; ++i)
    {
        texte[i]=fgetc(fichier);
    }
//    qDebug() << "texte fichier = " << texte;
//////-------Associer à chaque caractère une lettre de la clé-----////
    char *ListCle=(char*) malloc( nbrCar * sizeof(char) );
    indice=0;
    for(i=0;i<nbrCar;i++)
    {
        if (listeLettres.indexOf(texte[i])!=-1)
        {
            if (indice>tailleCle-1){ // fin de clé on recommence
                indice=0;
            }
            ListCle[i]=cle[indice];
            indice++;
        }
        else
        {
            ListCle[i]='_';
        }
    }
//    qDebug() << "ListeCle = " << ListCle;
// ////////--------------Cryptage du texte-------------/////////
    crypter = (char*) malloc(nbrCar*sizeof(char));
    for (i=0;i<nbrCar; ++i)
    {
        if(ListCle[i]=='_') // caractères pas de changements
        {
            crypter[i]=texte[i];
        }
        else
        {
            if(AlphabetMIN.indexOf(texte[i])!=-1) // texte en minuscule
            {
                cryptage=(int)texte[i]+(int)ListCle[i]-Amin;
                if(cryptage>Zmin)
                {
                    cryptage-=NBCHAR;
                }
                crypter[i]=(char)cryptage;
            }
            else if(AlphabetMAX.indexOf(texte[i])!=-1)  // texte en majuscule
            {
                cryptage=(int)texte[i]+(int)ListCle[i]-Amin;
                if(cryptage>zMax)
                {
                    cryptage-=NBCHAR;
                }
                crypter[i]=(char)cryptage;
            }
        }
    }
//    qDebug() << "fichier crypter = " << crypter;
//////---------Remplacement du texte par crypter--------////////
    fseek(fichier,0,SEEK_SET);
    fputs(crypter,fichier);
    free(crypter);
//    qDebug() << "FIN du Chiffrement";
}

void dechiffrVigen(FILE*fichier,QString cle_decryptage){
    int i;
    int indice=0;
    long nbrCar=0;
    int tailleCle=0,decryptage=0;
    char* texte=NULL;
    char* decrypter=NULL;
    const char *cle;    // clé de dechiffrement

    cle_decryptage=cle_decryptage.toLower();
    cle = cle_decryptage.toStdString().c_str();

    tailleCle = cle_decryptage.size();
    fseek(fichier,0,SEEK_SET);
//    qDebug() << "clé de Vigenère = " << cle << " de taille " << tailleCle;
 ///--------Avoir toute le texte dans var texte------///////
    while(fgetc(fichier)!=EOF)
    {
        nbrCar++;
    }
//    qDebug() << "nombre de caractères = " << nbrCar;
    if(nbrCar>0){
        texte=(char*) malloc(nbrCar*sizeof(char));
    }
    else {
        qDebug() << "Fichier vide";
        exit(-1);
    }
    fseek(fichier,0,SEEK_SET);
    for (i = 0; i < nbrCar; ++i)
    {
        texte[i]=fgetc(fichier);
    }
//    qDebug() << "texte fichier = " << texte;
/////---------Associer à chaque caractère une lettre de la clé-------/////
    char ListCle[nbrCar];
    indice=0;
    for(i=0;i<nbrCar;i++)
    {
        if (listeLettres.indexOf(texte[i])!=-1){
            if (indice>tailleCle-1){ // fin de clé on recommence
                indice=0;
            }
            ListCle[i]=cle[indice];
            indice++;
        }
        else {
            ListCle[i]='_';
        }
    }
//    qDebug() << "ListeCle = " << ListCle;
///////----------Décryptage du texte-------------/////////////
    decrypter= (char*) malloc(nbrCar*sizeof(char));
    for (i=0;i<nbrCar; ++i)
    {
        if(ListCle[i]=='_') // caractères pas de changements
        {
            decrypter[i]=texte[i];
        }
        else
        {
            if(AlphabetMIN.indexOf(texte[i])!=-1) // texte en minuscule
            {
                decryptage=(int)texte[i]-(int)ListCle[i]+Amin;
                if(decryptage<Amin)
                {
                    decryptage+=NBCHAR;
                }
                decrypter[i]=(char)decryptage;
            }
            else if(AlphabetMAX.indexOf(texte[i])!=-1)  // texte en majuscule
            {
                decryptage=(int)texte[i]-(int)ListCle[i]+Amin;
                if(decryptage<aMax)
                {
                    decryptage+=NBCHAR;
                }
                decrypter[i]=(char)decryptage;
            }
        }
    }
//    qDebug() << "fichier décrypter = " << decrypter;
///////---------Remplacement du texte par décrypter--------///////
    fseek(fichier,0,SEEK_SET);
    fputs(decrypter,fichier);
    free(decrypter);
//    qDebug() << "FIN du Déchiffrement";
}
