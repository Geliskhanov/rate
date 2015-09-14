#ifndef MAINWINDOW
#define MAINWINDOW

#include <QMainWindow>

class Assistant;
class Spreadsheet;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

    bool isStandardMode()const;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void setStandardMode(bool mode);
    void openRecentFile();
    void spreadsheetModified();
    void sort();
    void help();
    void about();

private:
    void createActions();
    void createMenus();
    void createContextMenu();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    Assistant *assistant;
    Spreadsheet *spreadsheet;

    static QStringList recentFiles;
    QString curFile;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    QAction *separatorAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *optionMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *closeAction;
    QAction *exitAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *deleteAction;
    QAction *addRowAction;
    QAction *delRowAction;
    QAction *standardModeAction;
    QAction *importAction;
    QAction *sortAction;
    QAction *helpAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
};

#endif // MAINWINDOW
