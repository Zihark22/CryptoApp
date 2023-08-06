#include "createfile.h"
#include "cryptage.h"

createFile::createFile()
{
    init();
    setCurrentFile(QString());
}

createFile::createFile(const QString &fileName)
{
    init();
    loadFile(fileName);
}

void createFile::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void createFile::documentWasModified()
{
    setWindowModified(true);
}

void createFile::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    isUntitled = true;

    QWidget *page=new QWidget;
    QVBoxLayout *layout=new QVBoxLayout;

    texteFichier = new QTextEdit; // texte du fichier

    //-------------- Saisie de la clé César ------------------
    cryptCesareGroup=new QHBoxLayout;
    cle_label=new QLabel(tr("Clé de cryptage César :"));
    cle=new QSpinBox;
    cle->setValue(0);
    cryptCesareGroup->addWidget(cle_label);
    cryptCesareGroup->addWidget(cle);
    cle->setFixedWidth(350);

    //-------------- Saisie de la clé Vigenère ------------------
    cryptVigenGroup=new QHBoxLayout;
    cleVigen_label=new QLabel(tr("Clé de cryptage Vigenère :"));
    cleVigen=new QLineEdit(tr("ExempleCle"));
    cryptVigenGroup->addWidget(cleVigen_label);
    cryptVigenGroup->addWidget(cleVigen);

    //--------------- Choix du cryptage ----------------------
    GroupeBoutons=new QHBoxLayout;
    QLabel *messageChoix = new QLabel(this);
    messageChoix->setText(tr("Choix du cryptage : "));
    vigenBox = new QRadioButton("Vigenère",page);
    cesarBox = new QRadioButton("César",page);
    vigenBox->setChecked(true);
    GroupeBoutons->addWidget(messageChoix);
    GroupeBoutons->addWidget(vigenBox);
    GroupeBoutons->addWidget(cesarBox);
    connect(vigenBox,SIGNAL(clicked()),this,SLOT(afficheCles()));
    connect(cesarBox,SIGNAL(clicked()),this,SLOT(afficheCles()));

    //---------------- CACHER --------------
    cle_label->hide();
    cle->hide();
//    cleVigen_label->hide();
//    cleVigen->hide();

    //---------------- AJOUT LAYOUT --------------
    layout->addWidget(texteFichier);
    layout->addLayout(GroupeBoutons);
    layout->addLayout(cryptCesareGroup);
    layout->addLayout(cryptVigenGroup);
    page->setLayout(layout);
    setCentralWidget(page);

    createActions();
    createStatusBar();

    readSettings();

    connect(texteFichier->document(), &QTextDocument::contentsChanged,
            this, &createFile::documentWasModified);

    setUnifiedTitleAndToolBarOnMac(true);
}
void createFile::afficheCles(){
    if(vigenBox->isChecked())
    {
        cle_label->hide();
        cle->hide();
        cleVigen_label->show();
        cleVigen->show();
    }
    else if(cesarBox->isChecked())
    {
        cle_label->show();
        cle->show();
        cleVigen_label->hide();
        cleVigen->hide();
    }
}

void createFile::tile(const QMainWindow *previous)
{
    if (!previous)
        return;
    int topFrameWidth = previous->geometry().top() - previous->pos().y();
    if (!topFrameWidth)
        topFrameWidth = 40;
    const QPoint pos = previous->pos() + 2 * QPoint(topFrameWidth, topFrameWidth);
    if (screen()->availableGeometry().contains(rect().bottomRight() + pos))
        move(pos);
}

//! [implicit tr context]
void createFile::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&Fichier"));
//! [implicit tr context]
    QToolBar *fileToolBar = addToolBar(tr("Fichier"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&Nouveau"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Créer un nouveau fichier"));
    connect(newAct, &QAction::triggered, this, &createFile::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Ouvrir..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Ouvrir un fichier existant"));
    connect(openAct, &QAction::triggered, this, &createFile::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Sauvegarder"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Sauvegarder le document sur le disque"));
    connect(saveAct, &QAction::triggered, this, &createFile::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Sauvegarder-&sous..."), this, &createFile::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Sauvegarder le document sous un nouveau nom."));

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Récent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &createFile::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &createFile::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(createFile::hasRecentFiles());

    QAction *closeAct = fileMenu->addAction(tr("&Fermer"), this, &QWidget::close);
    closeAct->setShortcut(tr("Ctrl+F"));
    closeAct->setStatusTip(tr("Ferme cette fenêtre."));

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("&Quitter"), qApp, &QApplication::quit);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Quitte l'application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Editer"));
    QToolBar *editToolBar = addToolBar(tr("Editer"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cou&per"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Couper le contenu de la sélection actuelle dans le "
                            "presse-papier"));
    connect(cutAct, &QAction::triggered, texteFichier, &QTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copier"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copier le contenu de la sélection actuelle dans le "
                             "presse-papier"));
    connect(copyAct, &QAction::triggered, texteFichier, &QTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Coller"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Coller le contenu du presse-papier dans l'actuelle  "
                              "sélection"));
    connect(pasteAct, &QAction::triggered, texteFichier, &QTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();
#endif // !QT_NO_CLIPBOARD

    QMenu *helpMenu = menuBar()->addMenu(tr("&Aide"));
    QAction *aboutAct = helpMenu->addAction(tr("&A propos"), this, &createFile::about);
    aboutAct->setStatusTip(tr("Montre l'application About box"));

    QAction *aboutQtAct = helpMenu->addAction(tr("A propos &Qt"), qApp, &QApplication::aboutQt);
    aboutQtAct->setStatusTip(tr("Montre la librarie Qt About box"));

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(texteFichier, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(texteFichier, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}

void createFile::createStatusBar()
{
    statusBar()->showMessage(tr("Prêt !"));
}

void createFile::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void createFile::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool createFile::maybeSave()
{
    if (!texteFichier->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("SDI"),
                               tr("Le document a été modifié.\n"
                                  "Voulez-vous sauvegarder vos changements ?"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void createFile::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SDI"),
                             tr("Impossible de lire le fichier %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    texteFichier->setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Fichier chargé"), 2000);
}

void createFile::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool createFile::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void createFile::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void createFile::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = createFile::strippedName(recentFiles.at(i));
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void createFile::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

bool createFile::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << texteFichier->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Impossible d'écrire le fichier %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Impossible d'ouvrir le fichier %1 pour l'écriture:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("SDI"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);

    std::string nomFichier=fileName.toStdString();
    auto nomF=nomFichier.c_str();
    FILE *fichier=NULL;
    fichier=fopen(nomF,"r+");
    if(cesarBox->isChecked())
        chiffrCesar(fichier,cle->value());
    else if(vigenBox->isChecked())
        chiffrVigen(fichier,cleVigen->text());
    fclose(fichier);

    statusBar()->showMessage(tr("Fichier sauvegardé"), 2000);
    return true;
}

void createFile::setCurrentFile(const QString &fileName)
{
    //static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("documentCrypté.txt");
        //curFile = tr("document%1.txt").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    texteFichier->document()->setModified(false);
    setWindowModified(false);

    if (!isUntitled && windowFilePath() != curFile)
        createFile::prependToRecentFiles(curFile);

    setWindowFilePath(curFile);
}

QString createFile::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

createFile *createFile::findMainWindow(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QWidget *> topLevelWidgets = QApplication::topLevelWidgets();
    for (QWidget *widget : topLevelWidgets) {
        createFile *mainWin = qobject_cast<createFile *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }

    return nullptr;
}

void createFile::newFile()
{
    createFile *other = new createFile;
    other->tile(this);
    other->show();
}

void createFile::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

void createFile::openFile(const QString &fileName)
{
    createFile *existing = findMainWindow(fileName);
    if (existing) {
        existing->show();
        existing->raise();
        existing->activateWindow();
        return;
    }

    if (isUntitled && texteFichier->document()->isEmpty() && !isWindowModified()) {
        loadFile(fileName);
        return;
    }

    createFile *other = new createFile(fileName);
    if (other->isUntitled) {
        delete other;
        return;
    }
    other->tile(this);
    other->show();
}

bool createFile::save()
{
    return isUntitled ? saveAs() : saveFile(curFile);
}

bool createFile::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Sauvegarder-sous"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void createFile::about()
{
   QMessageBox::about(this, tr("A propos de SDI"),
            tr("L'exemple <b>SDI</b> démontre comment écrire des applications "
               "d'interface d'un simple document utilisant Qt."));
}
