#ifndef SORTDIALOG
#define SORTDIALOG

#include <QDialog>

class QComboBox;
class QCheckBox;

class SortDialog : public QDialog
{
    Q_OBJECT

public:
    SortDialog(QWidget *parent = 0);

    QComboBox *comboBox;
    QCheckBox *checkAscending;

private:
    QStringList items;
};

#endif // SORTDIALOG
