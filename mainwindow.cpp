#include <QtWidgets>

#include "mainwindow.h"
#include "spreadsheet.h"
#include "sortdialog.h"
#include "assistant.h"

MainWindow::MainWindow()
{
    spreadsheet = new Spreadsheet;
    setCentralWidget(spreadsheet);
    createActions();
    createMenus();
    createContextMenu();
    createToolBars();
    createStatusBar();

    readSettings();
    setCurrentFile("");
    setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue())
    {
        writeSettings();
        event->accept();
    }
    else event->ignore();
}

void MainWindow::newFile()
{
    MainWindow *mainWin = new MainWindow;
    mainWin->showMaximized();
}

void MainWindow::open()
{
    if (okToContinue())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                   tr("Открыть"), ".",
                                   tr("Рейтинговая оценка (*.ra)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) return saveAs();
    else return saveFile(curFile);
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                               tr("Сохранить"), ".",
                               tr("Рейтинговая оценка (*.ra)"));
    if (fileName.isEmpty())
        return false;
    return saveFile(fileName);
}

bool MainWindow::isStandardMode()const
{
    return standardModeAction->isChecked();
}

void MainWindow::setStandardMode(bool mode)
{
    cutAction->setDisabled(mode);
    copyAction->setDisabled(mode);
    pasteAction->setDisabled(mode);
    deleteAction->setDisabled(mode);
    addRowAction->setDisabled(mode);
    delRowAction->setDisabled(mode);
    importAction->setDisabled(mode);

    editMenu->setDisabled(mode);
    spreadsheet->setStandardMode(mode);
}

void MainWindow::openRecentFile()
{
    if(okToContinue())
    {
        QAction *action = qobject_cast<QAction *>(sender());
        if(action)
            loadFile(action->data().toString());
    }
}

void MainWindow::spreadsheetModified()
{
    setWindowModified(true);
}

void MainWindow::sort()
{
    SortDialog dialog(this);
    if(dialog.exec())
    {
        CompanyCompare compare;
        compare.key = dialog.comboBox->currentIndex();
        compare.ascending = dialog.checkAscending->isChecked();
        spreadsheet->sort(compare);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"),
            tr("Программа предназначена для комплексной сравнительной рейтинговой оценки финансового состояния"
               " и деловой активности предприятия"));
}

void MainWindow::createActions()
{
    newAction = new QAction(tr("&Создать..."), this);
    newAction->setIcon(QIcon(":/images/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Создать новый файл"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(tr("&Открыть..."), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Открыть документ"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Сохранить"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Сохранить документ"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Сохранить &как..."), this);
    saveAsAction->setStatusTip(tr("Сохранить документ с новым именем"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    closeAction = new QAction(tr("&Закрыть"), this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Закрыть окно"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    exitAction = new QAction(tr("&Выход"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Закрыть приложение"));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    cutAction = new QAction(tr("&Вырезать"), this);
    cutAction->setIcon(QIcon(":/images/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip(tr("Поместить содержимое выделенной области в буфер обмена"));
    connect(cutAction, SIGNAL(triggered()), spreadsheet, SLOT(cut()));

    copyAction = new QAction(tr("&Копировать"), this);
    copyAction->setIcon(QIcon(":/images/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip(tr("Копировать содержимое выделенной области в буфер обмена"));
    connect(copyAction, SIGNAL(triggered()), spreadsheet, SLOT(copy()));

    pasteAction = new QAction(tr("&Вставить"), this);
    pasteAction->setIcon(QIcon(":/images/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip(tr("Вставить содержимое буфера обмена в выделенную область"));
    connect(pasteAction, SIGNAL(triggered()), spreadsheet, SLOT(paste()));

    deleteAction = new QAction(tr("&Удалить"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("Удалить содержимое выделенной области"));
    connect(deleteAction, SIGNAL(triggered()), spreadsheet, SLOT(del()));

    addRowAction = new QAction(tr("&Добавить строку"), this);
    addRowAction->setIcon(QIcon(":/images/addRow.png"));
    addRowAction->setShortcut(tr("Ctrl+R"));
    addRowAction->setStatusTip(tr("Добавить пустую строку в таблицу"));
    connect(addRowAction, SIGNAL(triggered()), spreadsheet, SLOT(addRow()));

    delRowAction = new QAction(tr("&Удалить строку"), this);
    delRowAction->setIcon(QIcon(":/images/delRow.png"));
    delRowAction->setShortcut(tr("Ctrl+D"));
    delRowAction->setStatusTip(tr("Удалить выделенную строку"));
    connect(delRowAction, SIGNAL(triggered()), spreadsheet, SLOT(delRow()));

    standardModeAction = new QAction(tr("&Стандартизированные данные"), this);
    standardModeAction->setCheckable(true);
    standardModeAction->setChecked(spreadsheet->isStandardMode());
    standardModeAction->setStatusTip(tr("Отображать данные в стандартизированном виде"));
    connect(standardModeAction, SIGNAL(toggled(bool)), this, SLOT(setStandardMode(bool)));

    importAction = new QAction(tr("&Импорт..."), this);
    importAction->setStatusTip(tr("Импорт данных из файла"));
    connect(importAction, SIGNAL(triggered()), spreadsheet, SLOT(import()));

    sortAction = new QAction(tr("&Сортировать"), this);
    sortAction->setShortcut(tr("F9"));
    sortAction->setStatusTip(tr("Сортировать таблицу"));
    connect(sortAction, SIGNAL(triggered()), this, SLOT(sort()));

    helpAction = new QAction(tr("&Вызов справки"), this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    helpAction->setStatusTip(tr("Открыть окно справки"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    aboutAction = new QAction(tr("&О программе"), this);
    aboutAction->setStatusTip(tr("Открыть окно с описанием программы"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("О &Qt"), this);
    aboutQtAction->setStatusTip(tr("Открыть окно с описанием Qt"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::help()
{
    assistant = new Assistant;
    assistant->showDocumentation("index.html");
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    separatorAction = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActions[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Правка"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(deleteAction);
    editMenu->addSeparator();
    editMenu->addAction(addRowAction);
    editMenu->addAction(delRowAction);

    optionMenu = menuBar()->addMenu(tr("&Настройки"));
    optionMenu->addAction(standardModeAction);

    toolsMenu = menuBar()->addMenu(tr("&Инструменты"));
    toolsMenu->addAction(importAction);
    toolsMenu->addAction(sortAction);

    helpMenu = menuBar()->addMenu(tr("&Справка"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createContextMenu()
{
    spreadsheet->addAction(cutAction);
    spreadsheet->addAction(copyAction);
    spreadsheet->addAction(pasteAction);
    spreadsheet->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&Файл"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("&Правка"));
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(addRowAction);
    editToolBar->addAction(delRowAction);
}

void MainWindow::createStatusBar()
{
    statusBar();
    connect(spreadsheet, SIGNAL(modified()), this, SLOT(spreadsheetModified()));
}

void MainWindow::readSettings()
{
    QSettings settings("StGAU", "Method rate");
    recentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

void MainWindow::writeSettings()
{
    QSettings settings("StGAU", "Method rate");
    settings.setValue("recentFiles", recentFiles);
}

bool MainWindow::okToContinue()
{
    if(isWindowModified())
    {
        QMessageBox msg(QMessageBox::Warning,
                        tr("Рейтинговая оценка"),
                        tr("<center>Документ был изменен.<br /> Сохранить изменения?</center>"),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        msg.setWindowIcon(QIcon(":/images/icon.png"));

        msg.setButtonText(QMessageBox::Yes, tr("Да"));
        msg.setButtonText(QMessageBox::No, tr("Нет"));
        msg.setButtonText(QMessageBox::Cancel, tr("Отмена"));

        int r = msg.exec();
        if(r == QMessageBox::Yes)
            return save();
        else if(r == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool MainWindow::loadFile(const QString &fileName)
{
    if(!spreadsheet->readFile(fileName))
    {
        statusBar()->showMessage(tr("ошибка загрузки"), 2000);
        return false;
    }
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Файл загрежен"), 2000);
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    if(!spreadsheet->writeFile(fileName))
    {
        statusBar()->showMessage(tr("Ошибка сохранения"), 2000);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Файл сохранен"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = tr("Безымянный");
    if(!curFile.isEmpty())
    {
        shownName = strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        foreach(QWidget *win, QApplication::topLevelWidgets())
        {
            if(MainWindow *mainWin = qobject_cast<MainWindow *>(win))
                mainWin->updateRecentFileActions();
        }
    }

    setWindowTitle(tr("%1[*] - %2").arg(shownName)
                                   .arg(tr("Рейтинговая оценка")));
}

void MainWindow::updateRecentFileActions()
{
    QMutableStringListIterator i(recentFiles);
    while(i.hasNext())
    {
        if(!QFile::exists(i.next()))
            i.remove();
    }

    for(int j = 0; j < MaxRecentFiles; ++j)
    {
        if(j < recentFiles.count())
        {
            QString text = tr("&%1 %2")
                           .arg(j + 1)
                           .arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else
        {
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible(!recentFiles.isEmpty());
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QStringList MainWindow::recentFiles;
