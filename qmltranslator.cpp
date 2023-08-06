#include "qmltranslator.h"
#include "findfiles.h"
#include "createfile.h"
#include "cryptage.h"
#include "stegano.h"

QmlTranslator::QmlTranslator(QObject *parent) : QObject(parent)
{
    choixCrypter=false;
    choixDecrypter=false;
    m_searchFile = new chercheFichiers();
}

void QmlTranslator::setTranslation(QString translation)
{
    if(translation=="Pусский")
        m_translator.load(":/MultiLanguagesCryptoApp_ru_RU.qm");
    else if(translation=="Français")
        m_translator.load(":/MultiLanguagesCryptoApp_fr_FR.qm");
    else if(translation=="English")
        m_translator.load(":/MultiLanguagesCryptoApp_en_GB.qm");
    else if(translation=="Español")
        m_translator.load(":/MultiLanguagesCryptoApp_es_ES.qm");
    else if(translation=="Deutsch")
        m_translator.load(":/MultiLanguagesCryptoApp_al_AL.qm");
    else
        m_translator.load(":/MultiLanguagesCryptoApp_en_GB.qm");

    qApp->installTranslator(&m_translator);
    emit languageChanged();
}
void QmlTranslator::arret()
{
    QApplication::quit();
}
void QmlTranslator::nvxFiche(){
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file(s) to open.");

    const QStringList posArgs = parser.positionalArguments();

    for (const QString &file : posArgs) {
        createFile *newWin = new createFile(file);
        newWin->tile(m_newFile);
        newWin->show();
        m_newFile = newWin;
    }

    if (!m_newFile or posArgs.size()==0)
        m_newFile = new createFile;
    m_newFile->show();
}
void QmlTranslator::rechercheFile(){
    chercheFichiers *newWin = new chercheFichiers();
    newWin->show();
    m_searchFile = newWin;
}

void QmlTranslator::crypte(){   // quand on click sur le bouton de crpytage du menu
    rechercheFile();
    choixCrypter=true;
    choixDecrypter=false;
    QObject::connect(m_searchFile,&chercheFichiers::fileReady,this,&QmlTranslator::choixCode);
}
void QmlTranslator::decrypte(){  // quand on click sur le bouton de decrpytage du menu
    rechercheFile();
    choixDecrypter=true;
    choixCrypter=false;
    QObject::connect(m_searchFile,&chercheFichiers::fileReady,this,&QmlTranslator::choixCode);
}

void QmlTranslator::choixCode(){ // Vigenere ou Cesar
    QObject::disconnect(m_searchFile,&chercheFichiers::fileReady,this,&QmlTranslator::choixCode);
    fenetreChoixCodes = new QDialog();
    vigenBox = new QRadioButton("Vigenère",fenetreChoixCodes);
    cesarBox = new QRadioButton("César",fenetreChoixCodes);
    vigenBox->setChecked(true);
    QPushButton *valide = new QPushButton("Ok",fenetreChoixCodes);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(cesarBox);
    layout->addWidget(vigenBox);
    layout->addWidget(valide);

    fenetreChoixCodes->setLayout(layout);
    fenetreChoixCodes->setWindowTitle("Choix du cryptage");
    fenetreChoixCodes->setFixedSize(200,200);
    fenetreChoixCodes->setModal(true);
    fenetreChoixCodes->show();

    QObject::connect(valide,SIGNAL(clicked()),this,SLOT(lanceCode()));
}
void QmlTranslator::lanceCode(){
//    qDebug() << "début lanceCode";
//    qDebug() << "choixCrypter=" << choixCrypter;
//    qDebug() << "choixDecrypter=" << choixDecrypter;
    fenetreChoixCodes->close();
    if(choixCrypter==true and choixDecrypter==false)
    {
        if(vigenBox->isChecked()){
            debutCrypteVigen();
        }
        else if(cesarBox->isChecked()){
            debutCrypteCesar();
        }
    }
    else if(choixCrypter==false and choixDecrypter==true)
    {
        if(vigenBox->isChecked()){
            debutDecrypteVigen();
        }
        else if(cesarBox->isChecked()){
            debutDecrypteCesar();
        }
    }
}

//-----------------------  CESAR --------------------------------------------
void QmlTranslator::debutCrypteCesar(){
    bool valide=false;
    cle = QInputDialog::getInt(NULL,tr("Clé de cryptage"),tr("Veuillez-saisir la clé pour commencer le cryptage du fichier sélectionné :"),0,0,1000,1,&valide);

    if(valide){
        string nomFichier=m_searchFile->nom;
        auto nomF=nomFichier.c_str();
        FILE *fichier=NULL;
        fichier=fopen(nomF,"r+");
        chiffrCesar(fichier,cle);
        fclose(fichier);
        fichier=NULL;
        m_searchFile->nom="";
    }
    else{
        m_searchFile->nom="";
        cle=0;
        m_searchFile->close();
    }

}
void QmlTranslator::debutDecrypteCesar(){
    QObject::disconnect(m_searchFile,&chercheFichiers::fileReady,this,&QmlTranslator::debutDecrypteCesar);
    bool valide=false;
    cle=QInputDialog::getInt(NULL,tr("Clé de décryptage"),tr("Veuillez-saisir la clé pour commencer le décryptage du fichier sélectionné :"),0,0,1000,1,&valide);
    //QString password=QInputDialog::getText(this,"Hello","Insert your password",QLineEdit::Password);

    if(valide){
        string nomFichier2=m_searchFile->nom;
        auto nomF2=nomFichier2.c_str();
        FILE *fichier2=NULL;
        fichier2=fopen(nomF2,"r+");
        dechiffrCesar(fichier2,cle);
        fclose(fichier2);
        fichier2=NULL;
        m_searchFile->nom="";
    }
    else{
        m_searchFile->nom="";
        cle=0;
        m_searchFile->close();
    }
}

//-----------------------  VIGENERE --------------------------------------------
void QmlTranslator::debutCrypteVigen(){
    bool valide=false;
    cleVigen = QInputDialog::getText(NULL,tr("Clé de cryptage"),tr("Veuillez-saisir la clé sous forme de mot pour commencer le cryptage du fichier sélectionné :"),QLineEdit::PasswordEchoOnEdit,"cle",&valide);
//    qDebug() << "Clé Vigenère = " << cleVigen ;
    if(valide){
        string nomFichier=m_searchFile->nom;
        auto nomF=nomFichier.c_str();
        FILE *fichier=NULL;
        fichier=fopen(nomF,"r+");
        chiffrVigen(fichier,cleVigen);
//        qDebug() << "Chiffrement fait du fichier " << nomF ;
        fclose(fichier);
        fichier=NULL;
        m_searchFile->nom="";
        valide=false;
    }
    else{
        m_searchFile->nom="";
        cleVigen=NULL;
        m_searchFile->close();
    }
}
void QmlTranslator::debutDecrypteVigen(){
    QObject::disconnect(m_searchFile,&chercheFichiers::fileReady,this,&QmlTranslator::debutDecrypteVigen);
    bool valide=false;
    cleVigen=QInputDialog::getText(NULL,tr("Clé de décryptage"),tr("Veuillez-saisir la clé sous forme de mot pour commencer le décryptage du fichier sélectionné :"),QLineEdit::Password,"cle",&valide);
//    qDebug() << "Clé Vigenère = " << cleVigen ;
    if(valide){
        string nomFichier2=m_searchFile->nom;
        auto nomF2=nomFichier2.c_str();
        FILE *fichier2=NULL;
        fichier2=fopen(nomF2,"r+");
        dechiffrVigen(fichier2,cleVigen);
//        qDebug() << "Déchiffrement fait du fichier " << nomF2 ;
        fclose(fichier2);
        fichier2=NULL;
        m_searchFile->nom="";
        valide=false;
    }
    else{
        m_searchFile->nom="";
        cleVigen=NULL;
        m_searchFile->close();
    }
}

void QmlTranslator::stegano(){
//    qDebug() << "Debut stegano";
//    stegano *steg = new stegano();
}
