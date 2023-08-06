#ifndef FINDFILES_H
#define FINDFILES_H

#include "librairies.h"

class chercheFichiers : public QWidget
{
    Q_OBJECT

public:
    chercheFichiers();
    string nom;
private slots:
    void browse();
    void find();
    void animateFindClick();
    void openFileOfItem(int row, int column);
    void contextMenu(const QPoint &pos);
signals :
    void MySignalToIndicateThatTheWindowIsClosing();
    void fileReady();

private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &paths);
    QComboBox *createComboBox(const QString &text = QString());
    void closeEvent( QCloseEvent* event );
    void createFilesTable();

    QComboBox *fileComboBox;
    QComboBox *textComboBox;
    QComboBox *directoryComboBox;
    QLabel *filesFoundLabel;
    QPushButton *findButton;
    QTableWidget *filesTable;
    QDir currentDir;
    QString chemin;
};

#endif // FINDFILES_H
