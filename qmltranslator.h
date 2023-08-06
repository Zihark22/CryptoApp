#ifndef QMLTRANSLATOR_H
#define QMLTRANSLATOR_H

#include "findfiles.h"
#include "createfile.h"
#include "cryptage.h"
#include "librairies.h"
#include "stegano.h"

class QmlTranslator : public QObject
{
    Q_OBJECT

public:
    explicit QmlTranslator(QObject *parent = 0);
signals:
    void languageChanged();
    void quitter();
public slots :
    Q_INVOKABLE void lanceCode();

public:
    Q_INVOKABLE void setTranslation(QString translation);
    Q_INVOKABLE void arret();
    Q_INVOKABLE void rechercheFile();
    Q_INVOKABLE void decrypte();
    Q_INVOKABLE void crypte();
    Q_INVOKABLE void stegano();
    Q_INVOKABLE void nvxFiche();
    Q_INVOKABLE void debutCrypteCesar();
    Q_INVOKABLE void debutDecrypteCesar();
    Q_INVOKABLE void debutCrypteVigen();
    Q_INVOKABLE void debutDecrypteVigen();
    Q_INVOKABLE void choixCode();

private:
    QTranslator m_translator;
    chercheFichiers *m_searchFile;
    createFile *m_newFile;
    QRadioButton *vigenBox;
    QRadioButton *cesarBox;
    QDialog *fenetreChoixCodes;
    int cle;
    QString cleVigen;
    bool choixCrypter;
    bool choixDecrypter;
};

#endif // QMLTRANSLATOR_H  
