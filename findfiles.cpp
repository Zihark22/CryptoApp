#include "findfiles.h"

enum { absoluteFileNameRole = Qt::UserRole + 1 };

static inline QString fileNameOfItem(const QTableWidgetItem *item)
{
    return item->data(absoluteFileNameRole).toString();
}

static inline void openFile(const QString &fileName)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

chercheFichiers::chercheFichiers()
{
    nom="";
    chemin="/Users/badajozj/Desktop"; // QDir::currentPath()
    setWindowTitle(tr("Find Files"));
    QPushButton *browseButton = new QPushButton(tr("&Look into..."), this);
    connect(browseButton, &QAbstractButton::clicked, this, &chercheFichiers::browse);
    findButton = new QPushButton(tr("&Search"), this);
    connect(findButton, &QAbstractButton::clicked, this, &chercheFichiers::find);

    fileComboBox = createComboBox(tr("*"));
    connect(fileComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &chercheFichiers::animateFindClick);
    textComboBox = createComboBox();
    connect(textComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &chercheFichiers::animateFindClick);
    directoryComboBox = createComboBox(QDir::toNativeSeparators(chemin));
    connect(directoryComboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &chercheFichiers::animateFindClick);

    filesFoundLabel = new QLabel;

    createFilesTable();

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Named:")), 0, 0);
    mainLayout->addWidget(fileComboBox, 0, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("With the text:")), 1, 0);
    mainLayout->addWidget(textComboBox, 1, 1, 1, 2);
    mainLayout->addWidget(new QLabel(tr("In the file:")), 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 4, 0, 1, 2);
    mainLayout->addWidget(findButton, 4, 2);

    connect(new QShortcut(QKeySequence::Quit, this), &QShortcut::activated,qApp, &QApplication::quit);
}

void chercheFichiers::browse()
{
    QString directory =
        QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Find files"), chemin));

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

void chercheFichiers::find()
{
    filesTable->setRowCount(0);

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = QDir::cleanPath(directoryComboBox->currentText());
    currentDir = QDir(path);

    updateComboBox(fileComboBox);
    updateComboBox(textComboBox);
    updateComboBox(directoryComboBox);

    QStringList filter;
    if (!fileName.isEmpty())
        filter << fileName;
    QDirIterator it(path, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
        files << it.next();
    if (!text.isEmpty())
        files = findFiles(files, text);
    files.sort();
    showFiles(files);
}

void chercheFichiers::animateFindClick()
{
    findButton->animateClick();
}

QStringList chercheFichiers::findFiles(const QStringList &files, const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Return"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find files"));

    QMimeDatabase mimeDatabase;
    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching the file number %1 of %n...", nullptr, files.size()).arg(i));
        QCoreApplication::processEvents();

        if (progressDialog.wasCanceled())
            break;

        const QString fileName = files.at(i);
        const QMimeType mimeType = mimeDatabase.mimeTypeForFile(fileName);
        if (mimeType.isValid() && !mimeType.inherits(QStringLiteral("text/plain"))) {
            qWarning() << "Not searching binary file " << QDir::toNativeSeparators(fileName);
            continue;
        }
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text, Qt::CaseInsensitive)) {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    return foundFiles;
}

void chercheFichiers::showFiles(const QStringList &paths)
{
    for (const QString &filePath : paths) {
        const QString toolTip = QDir::toNativeSeparators(filePath);
        const QString relativePath = QDir::toNativeSeparators(currentDir.relativeFilePath(filePath));
        const qint64 size = QFileInfo(filePath).size();
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(relativePath);
        fileNameItem->setData(absoluteFileNameRole, QVariant(filePath));
        fileNameItem->setToolTip(toolTip);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(QLocale().formattedDataSize(size));
        sizeItem->setData(absoluteFileNameRole, QVariant(filePath));
        sizeItem->setToolTip(toolTip);
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }
    filesFoundLabel->setText(tr("%n files found (Double clic on the file to open it)", nullptr, paths.size()));
    filesFoundLabel->setWordWrap(true);
}

QComboBox *chercheFichiers::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

void chercheFichiers::createFilesTable()
{
    filesTable = new QTableWidget(0, 2);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("File's name ") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);

    filesTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filesTable, &QTableWidget::customContextMenuRequested,
            this, &chercheFichiers::contextMenu);
    connect(filesTable, &QTableWidget::cellActivated,
            this, &chercheFichiers::openFileOfItem);
}

void chercheFichiers::openFileOfItem(int row, int /* column */)
{
    const QTableWidgetItem *item = filesTable->item(row, 0);
    QString nomFichier=fileNameOfItem(item);
    nom += nomFichier.toStdString();
    emit fileReady();
    close();
}

void chercheFichiers::contextMenu(const QPoint &pos)
{
    const QTableWidgetItem *item = filesTable->itemAt(pos);
    if (!item)
        return;
    QMenu menu;
#ifndef QT_NO_CLIPBOARD
    QAction *copyAction = menu.addAction(tr("Copy the name"));
#endif
    QAction *openAction = menu.addAction(tr("Open"));
    QAction *action = menu.exec(filesTable->mapToGlobal(pos));
    if (!action)
        return;
    const QString fileName = fileNameOfItem(item);
    if (action == openAction)
        openFile(fileName);
#ifndef QT_NO_CLIPBOARD
    else if (action == copyAction)
        QGuiApplication::clipboard()->setText(QDir::toNativeSeparators(fileName));
#endif
}
void chercheFichiers::closeEvent( QCloseEvent* event )
{
    emit MySignalToIndicateThatTheWindowIsClosing();
    event->accept();
}
