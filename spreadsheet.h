#ifndef SPREADSHEET
#define SPREADSHEET

#include <QTableWidget>

class Company;
class CompanyCompare;

class Spreadsheet : public QTableWidget
{
    Q_OBJECT

public:
    Spreadsheet();

    QTableWidgetSelectionRange selectedRange() const;
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    void sort(const CompanyCompare &compare);
    void setStandardMode(const bool);
    bool isStandardMode()const;

    static const int Ndp = 3; // количество знаков после запятой при печати в таблицу

public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void addRow();
    void delRow();
    void import();

signals:
    void modified();

private slots:
    void somethingChanged();

private:
    void clearTable(const int = 10);
    void clearData();
    bool getDataFromTable();
    void calculateMaxIndicators();
    void calculate();
    void printCompany();
    bool isEmptyRow(int)const;
    void addRows(QTextStream&);

    QStringList headers;
    QVector<Company> company;
    bool standardMode;
    enum { MagicNumber = 0x7F51C883 };
};

class Company
{
public:
    Company();
    Company(const QString, const QVector<double> &, const double = 0.0);

    static void clearMaxIndicators();
    double rawIndicator(const int)const;
    double stdIndicator(const int)const;
    double getRating()const;
    QString getName()const;
    QVector<double>::const_iterator getStdIndicatorsBegin()const { return stdIndicators.begin(); }
    QVector<double>::const_iterator getStdIndicatorsEnd()const { return stdIndicators.end(); }
    QVector<double>::const_iterator getRawIndicatorsBegin()const { return rawIndicators.begin(); }
    QVector<double>::const_iterator getRawIndicatorsEnd()const { return rawIndicators.end(); }
    int rawIndicatorsSize()const;
    static void setMaxIndicator(const double);
    void calculateRating();

    static const int nColumn = 2;

private:
    QString name;
    QVector<double> rawIndicators;
    QVector<double> stdIndicators;
    double rating;
public:
    static QVector<double> maxIndicators;
};

class CompanyCompare
{
public:
    bool operator() (const Company &,
                     const Company &)const;

    int key;
    int ascending;
};

QDebug operator<< (QDebug debug, const QVector<Company> &company);

#endif // SPREADSHEET
