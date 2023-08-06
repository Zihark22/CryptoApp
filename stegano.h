#ifndef STEGANO_H
#define STEGANO_H

#include "librairies.h"

class stegano : public QMainWindow
{
    Q_OBJECT
    public:
        stegano();
};

#endif // STEGANO_H

/*

public:
    createFile();
    //! [class definition with macro]
    explicit createFile(const QString &fileName);

    void tile(const QMainWindow *previous);

protected:
    void closeEvent(QCloseEvent *event)override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void updateRecentFileActions();
    void openRecentFile();
    void about();
    void documentWasModified();
    void afficheCles();

private:
    enum { MaxRecentFiles = 5 };

    void init();
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void openFile(const QString &fileName);
    void loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    static QString strippedName(const QString &fullFileName);
    createFile *findMainWindow(const QString &fileName) const;

    QTextEdit *texteFichier; // texte du fichier

    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;

    QSpinBox *cle;
    QLabel *cle_label;
    QHBoxLayout *GroupeBoutons;
    QRadioButton *vigenBox;
    QRadioButton *cesarBox;
    QHBoxLayout *cryptCesareGroup;
    QHBoxLayout *cryptVigenGroup;
    QLineEdit *cleVigen;
    QLabel *cleVigen_label;

    QString curFile;
    bool isUntitled;

*/
