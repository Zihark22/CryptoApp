# This Python file uses the following encoding: utf-8

# if __name__ == "__main__":
#     pass



import cv2
import os
import numpy as np
import matplotlib.image as npimg
import matplotlib.pyplot as plt
from os import makedirs
from os.path import splitext, dirname, basename, join
from PIL.Image import *
try:
    from PIL import Image
except ImportError:
    import Image
from PIL.PngImagePlugin import PngInfo
from datetime import datetime, timezone


chemin="/Users/badajozj/Desktop/Steganographie"

reduction = 31  # 31 pour tout l'alphabet et 64 pour seulement les lettres de base

alphabet = " .eaisnrtoludcmpégbvhfqyxjèàkwzêçôâîûùïüë"

"""
Etapes cacher :
    - saisir le chemin (nom) de l'image
    - saisir le texte à cacher
    - decouper l'image en 3 matrices (rouge,vert, bleu)
    - recuperer les valeurs
    - enlever ou ajouter l'équivalent ASCII de la premiere lettre du texte à une des matrices (au début = en haut à gauche)
    - refaire sur toutes les lettres
    - recombiner les matrices
    - créer l'image finale
"""

## FONCTION UTILES
def MatriceToImage(img,cheminImg):
    [lignes,cols]=img.shape[:2]
    # img = img.astype(int)
    image = new('RGB', (cols,lignes))

    for i in range(cols) :
        for j in range(lignes):
            image.putpixel((i,j),(img[j,i,2],img[j,i,1],img[j,i,0]))

    image.save("new"+cheminImg,"PNG")


def textToASCII(txt):
    liste=np.zeros(len(txt))
    for i in range(len(txt)):
        liste[i]=ord(txt[i])
    liste = liste.astype(int)
    print("Liste des caractères en ASCII : \n",liste,"\n")
    for i in range(len(txt)):
        liste[i]-=reduction
    return liste

def decompoCalques(valeurs):
    n=len(valeurs)
    print(n, "valeurs à décomposer !")
    liste=np.zeros((n,3))
    for i in range (n):
        quotient = valeurs[i]//3
        reste = valeurs[i]%3
        if reste==0 :
            liste[i,0]= quotient
            liste[i,1]= quotient
            liste[i,2]= quotient
        elif reste==1 :
            liste[i,0]= quotient + 1
            liste[i,1]= quotient
            liste[i,2]= quotient
        elif reste==2 :
            liste[i,0]= quotient + 1
            liste[i,1]= quotient + 1
            liste[i,2]= quotient
        else :
            print("erreur de décompo")

    liste = liste.astype(int)
    print("Valeurs décomposées : \n",liste)
    return liste


def milieu(img):
    [n,p,k]=img.shape
    for i in range(n):
        for j in range(p):
            for l in range(k):
                if(img[i,j,l]==255):
                    img[i,j,l]=245
                elif(img[i,j,l]==0):
                    img[i,j,l]=10
    return img

def transfoBinaire(valeurs):
    n=len(valeurs)
    print(n, "valeurs à décomposer !")
    liste=[]
    for i in range (n):
        value=bin(valeurs[i])[2:].rjust(8,'0')
        for j in range(8):
            liste.append(value[j])

    print("Valeurs binaires : \n",liste)
    return liste


## CACHER TEXTE DANS IMAGE
def cacher(cheminImg,texte):
    img=cv2.imread(cheminImg)
    # img=milieu(img)
    [n,p,k]=img.shape
    print("Dimension :  n=",n,", p=",p," et k=",k)
    print("\n",img[:,:,2],"\n")
    crypt=img

    valASCII=textToASCII(texte)
    print("Il y a ",len(texte)," valeurs à cacher : \n",valASCII,"\n")
    valBin = transfoBinaire(valASCII)

    for i in range(n):
        for j in range(p):
            if((j+i*p) < len(valBin)):
                if(img[i,j,2]-int(valBin[j+i*p]) >= 0 ):
                    crypt[i,j,2]=img[i,j,2]-int(valBin[j+i*p])
                else :
                    crypt[i,j,2]=img[i,j,2]-int(valBin[j+i*p])+256
            else :
                crypt[i,j,2]=img[i,j,2]

    crypt = crypt.astype(int)
    print("\n",crypt[:,:,2],"\n")
    print("Pixel 0 :  origine=",img[0,0]," et crypte=",crypt[0,0])
    print("Pixel 23 :  origine=",img[2,7]," et crypte=",crypt[2,7])

    MatriceToImage(crypt,cheminImg)
    print("------------------------------------")
    return crypt


## TROUVER LE MESSAGE CACHE

"""
Etapes trouver :
    - saisir le chemin (nom) de l'image originale
    - saisir le chemin (nom) de l'image cryptée
    - decouper les images en 3 matrices (rouge,vert, bleu)
    - recuperer les valeurs
    - faire la différence entre les matrices (attention au sens)
    - traduire les nombres différent de 0 en ASCII
    - recombiner les lettres pour former le texte caché
"""
def trouver(chem_orig):
    chem_crypt="new"+chem_orig
    img_orig=cv2.imread(chem_orig)
    img_crypt=cv2.imread(chem_crypt)
    [n,p,k]=img_orig.shape
    # print(n,p,k)
    diff = img_orig[:,:,2]-img_crypt[:,:,2]
    # print(diff,diff.shape)
    cmpt = 0
    cmptLettre = 0
    lstLettres = []

    for i in range(n):
        for j in range(p):
            if(diff[i,j]>255):
                diff[i,j]-=256


    for i in range(n):
        for j in range(p):
            cmpt+=1
            if(cmpt==8):
                indiceC=8*cmptLettre%p
                indiceL=8*cmptLettre//p
                if(indiceC+8>=p):
                    letLstInt=list(diff[indiceL,indiceC:p])+list(diff[indiceL+1,0:8-(p-indiceC)])
                else :
                    letLstInt=list(diff[indiceL,indiceC:indiceC+8])
                for i in range(len(letLstInt)):
                    letLstInt[i]=str(int(letLstInt[i]))
                # print(letLstInt)
                val="".join(letLstInt)
                cmpt=0;
                if(val=='00000000'):
                    k=diff.size
                    break;
                else:
                    print("valeur =",val)
                    lstLettres.append(val)
                    cmptLettre+=1

    print("Nombre de lettres cachées = ",cmptLettre)
    diff=diff.astype(int)
    # print(diff)
    listeVal = np.zeros(cmptLettre)
    for lettre in range(cmptLettre):
        listeVal[lettre]=int(lstLettres[lettre],2)+reduction
    listeVal=listeVal.astype(int)
    print("Les valeurs décryptées sont : ",listeVal)

    msg = ""
    for lettre in range(cmptLettre):
        msg += chr(listeVal[lettre])
    print("Le message caché est : ",msg)
    return msg


"""
Améliorations :
    - utiliser propre alphabet

"""

## EXECUTION
nom_img="space.jpg"
message = "Julien tu as été hacké par Anonimous. MERDE !"

img_crypt = cacher(nom_img,message);
# img_crypt = cacher(nom_img,"AB");

msg = trouver(nom_img);
