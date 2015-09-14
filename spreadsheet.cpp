#include <QtWidgets>

#include "mainwindow.h"
#include "spreadsheet.h"

Spreadsheet::Spreadsheet()
{
    standardMode = false;
    headers
            << tr("Предприятие")
            << tr("Рейтинговая\nоценка")

            << tr("Рентабельность\nактивов")
            << tr("Чистая\nрентабельность\nактивов")
            << tr("Рентабельность\nсобственного\nкапитала")
            << tr("Рентабельность\nпроизводственных\nфондов")

            << tr("Чистая\nрентабельность")
            << tr("Рентабельность\nпродаж")
            << tr("Общая\nрентабельность")
            << tr("Валовая\nрентабельность")

            << tr("Отдача\nвсех активов")
            << tr("Фондоотдача")
            << tr("Оборачиваемость\nоборотных\nактивов")
            << tr("Оборачиваемость\nзапасов")
            << tr("Оборачиваемость\nдебиторской\nзадолженности")
            << tr("Оборачиваемость\nбанковских\nактивов")
            << tr("Отдача\nсобственного\nкапитала")

            << tr("Коэффициент\nтекущей\nликвидности")
            << tr("Коэффициент\nабсолютной\nликвидности")
            << tr("Индекс\nпостоянного\nактива")
            << tr("Коэффициент\nавтономии")
            << tr("Обеспеченность\nзапасов\nсобственными\nоборотными\nсредствами");
    clearTable();
    connect(this, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(somethingChanged()));
}

QTableWidgetSelectionRange Spreadsheet::selectedRange()const
{
    QList<QTableWidgetSelectionRange> ranges = selectedRanges();
    if(ranges.isEmpty())
        return QTableWidgetSelectionRange();
    return ranges.first();
}

bool Spreadsheet::readFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Рейтинговая оценка"),
                             tr("Невозможно прочесть файл %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_4);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber)
    {
        QMessageBox::warning(this, tr("Рейтинговая оценка"),
                             tr("Неизвестное расширение файла."));
        return false;
    }
    quint16 rowCount;
    in >> rowCount;
    clearTable(rowCount);

    quint16 row;
    quint16 column;
    QString str;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    blockSignals(true);
    while(!in.atEnd())
    {
        in >> row >> column >> str;
        item(row, column)->setText(str);
    }
    getDataFromTable();
    blockSignals(false);
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Рейтинговая оценка"),
                             tr("Невозможно записать файл %1:\n%2.")
                             .arg(file.fileName())
                             .arg(file.errorString()));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_5);

    out << quint32(MagicNumber);
    out << quint16(rowCount());

    QApplication::setOverrideCursor(Qt::WaitCursor);

    for(int row = 0; row < company.size(); ++row)
    {
        out << quint16(row) << quint16(0) << company[row].getName();
        out << quint16(row) << quint16(1) << QString("");
        for (int column = Company::nColumn; column < columnCount(); ++column)
        {
            out << quint16(row) << quint16(column) << QString::number(company[row].rawIndicator(column - Company::nColumn));
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void Spreadsheet::sort(const CompanyCompare &compare)
{
    qStableSort(company.begin(), company.end(), compare);
    printCompany();
}

void Spreadsheet::setStandardMode(const bool mode)
{
    standardMode = mode;
    if(mode)
    {
        clearData();
        if(getDataFromTable())
        {
            calculate();
            printCompany();
        }
    }
    else
    {
        blockSignals(true);
        for(int i = 0; i < rowCount(); ++i)
            item(i, 1)->setText("");
        blockSignals(false);
        printCompany();
        clearData();
    }
    blockSignals(true);
    if(isStandardMode())
    {
        for(int col = 0; col < columnCount(); ++col)
            for(int row = 0; row < rowCount(); ++row)
                if(col != 1) item(row, col)->setFlags(item(row, col)->flags() ^ Qt::ItemIsEditable);
    }
    else
    {
        for(int col = 0; col < columnCount(); ++col)
            for(int row = 0; row < rowCount(); ++row)
                if(col != 1) item(row, col)->setFlags(item(row, col)->flags() | Qt::ItemIsEditable);
    }
    blockSignals(false);
}

bool Spreadsheet::isStandardMode()const
{
    return standardMode;
}

void Spreadsheet::cut()
{
    copy();
    del();
}

void Spreadsheet::copy()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str;

    for(int i = 0; i < range.rowCount(); ++i)
    {
        if(i > 0)
            str += "\n";
        for(int j = 0; j < range.columnCount(); ++j)
        {
            if(j > 0)
                str += "\t";
            str += item(range.topRow() + i, range.leftColumn() + j)->text();
        }
    }
    QApplication::clipboard()->setText(str);
}

void Spreadsheet::paste()
{
    QTableWidgetSelectionRange range = selectedRange();
    QString str = QApplication::clipboard()->text();
    QStringList rows = str.split('\n');
    int numRows = rows.count();
    int numColumns = rows.first().count('\t') + 1;

    if (range.rowCount() * range.columnCount() != 1
            && (range.rowCount() != numRows
                || range.columnCount() != numColumns))
    {
        QMessageBox::information(this, tr("Рейтинговая оценка"),
                                 tr("Невозможно вставить данные, так как область копирования и "
                                    "область вставки имеют разные размеры."));
        return;
    }
    blockSignals(true);
    for(int i = 0; i < numRows; ++i)
    {
        QStringList columns = rows[i].split('\t');
        for(int j = 0; j < numColumns; ++j)
        {
            int row = range.topRow() + i;
            int column = range.leftColumn() + j;
            if (row < rowCount() && column < columnCount())
                item(row, column)->setText(columns[j]);
        }
    }
    blockSignals(false);
    somethingChanged();
}

void Spreadsheet::del()
{
    QList<QTableWidgetItem *> items = selectedItems();
    if(!items.isEmpty())
    {
        blockSignals(true);
        foreach (QTableWidgetItem *item, items)
            item->setText("");
        blockSignals(false);
        somethingChanged();
    }
}

void Spreadsheet::addRow()
{
    blockSignals(true);
    insertRow(rowCount());
    for(int col = 0; col < columnCount(); ++col)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem();
        setItem(rowCount() - 1, col, newItem);
        if(col == 1) item(rowCount() - 1, col)->setFlags(item(rowCount() - 1, col)->flags() ^ Qt::ItemIsEditable);
    }
    blockSignals(false);
    emit modified();
}

void Spreadsheet::delRow()
{
    QTableWidgetSelectionRange range = selectedRange();
    for(int row = range.bottomRow(); row >= range.topRow(); --row) removeRow(row);
    somethingChanged();
}

void Spreadsheet::import()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Импорт"), ".",
                                                    tr("CSV (разделители - запятые) (*.csv)"));
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Рейтинговая оценка"),
                                 tr("Невозможно открыть файл %1:\n%2.")
                                 .arg(file.fileName())
                                 .arg(file.errorString()));
            return;
        }
        QTextStream in(&file);
        addRows(in); // добавить недостающее количество строк в таблице
        in.readLine(); // пропустить первую строку;
        int row = 0;
        while(!in.atEnd())
        {
            QString name; // название предприятия
            QString line;
            QStringList strList; // показатели предприятия

            line = in.readLine();
            strList = line.split(";");
            name = strList.at(0);

            blockSignals(true);
            while(true)
            {
                if(isEmptyRow(row))
                {
                    item(row, 0)->setText(name);
                    for(int i = Company::nColumn; i < columnCount(); ++i)
                        item(row, i)->setText(strList[i - 1]);
                    ++row;
                    break;
                }
                else ++row;
            }
            blockSignals(false);
        }
        somethingChanged();
    }
}

void Spreadsheet::somethingChanged()
{
    clearData();
    getDataFromTable();
    emit modified();
}

void Spreadsheet::clearTable(const int row)
{
    blockSignals(true);
    setRowCount(0);
    setColumnCount(0);
    setRowCount(row);
    setColumnCount(headers.size());
    for(int col = 0; col < columnCount(); ++col)
    {
        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setText(headers.at(col));
        setHorizontalHeaderItem(col, newItem);
        for(int row = 0; row < rowCount(); ++row)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            setItem(row, col, newItem);
            if(col == 1) item(row, col)->setFlags(item(row, col)->flags() ^ Qt::ItemIsEditable);
        }
    }
    blockSignals(false);
}

void Spreadsheet::clearData()
{
    company.clear();
    Company::clearMaxIndicators();
}

bool Spreadsheet::getDataFromTable()
{
    QString sName;
    QVector<double> v;
    for(int row = 0; row < rowCount(); ++row)
    {
        if(!item(row, 0)->text().isEmpty())
        {
            sName = item(row, 0)->text();
            for(int col = Company::nColumn; col < columnCount(); ++col)
            {
                if(!item(row, col)->text().isEmpty())
                    v << item(row, col)->text().toDouble();
                else
                {
                    sName.clear();
                    v.clear();
                    break;
                }
            }
            if(!sName.isEmpty())
            {
                company << Company(sName, v);
                sName.clear();
                v.clear();
            }
        }
    }
    if(!company.isEmpty()) return true;
    return false;
}

void Spreadsheet::calculateMaxIndicators()
{
    double max = 0.0;
    for(int i = 0; i < company[0].rawIndicatorsSize(); ++i)
    {
        max = company[0].rawIndicator(i);
        for(int j = 0; j < company.size(); ++j)
            if(max < company[j].rawIndicator(i)) max = company[j].rawIndicator(i);
        Company::setMaxIndicator(max);
    }
}

void Spreadsheet::calculate()
{
    calculateMaxIndicators();
    for(int i = 0; i < company.size(); ++i)
        company[i].calculateRating();
}

void Spreadsheet::printCompany()
{
    blockSignals(true);
    if(isStandardMode())
    {
        for(int i = 0; i < company.size(); ++i)
        {
            item(i, 0)->setText(company[i].getName());
            item(i, 1)->setText(QString::number(company[i].getRating(), 'd', Ndp));
            for(int j = Company::nColumn; j < columnCount(); ++j)
                item(i, j)->setText(QString::number(company[i].stdIndicator(j - Company::nColumn), 'd', Ndp));
        }
    }
    else
    {
        for(int i = 0; i < company.size(); ++i)
        {
            item(i, 0)->setText(company[i].getName());
            for(int j = Company::nColumn; j < columnCount(); ++j)
                item(i, j)->setText(QString::number(company[i].rawIndicator(j - Company::nColumn), 'd', Ndp));
        }
    }
    blockSignals(false);
}

bool Spreadsheet::isEmptyRow(int row)const
{
    for(int col = 0; col < columnCount(); ++col)
        if(!item(row, col)->text().isEmpty()) return false;
    return true;
}

void Spreadsheet::addRows(QTextStream &in)
{
    int lines = 0;
    int rows = 0;
// сколько строк в файле:
    in.readLine(); // пропускаем первую строку в файле
    while(!in.atEnd())
    {
        in.readLine();
        ++lines;
    }
// сколько пустых строк в таблице:
    for(int i = 0; i < rowCount(); ++i)
        if(isEmptyRow(i)) ++rows;
    for(int i = 0; i < lines - rows; ++i) addRow();
    in.seek(0);
}

Company::Company()
{

}

Company::Company(const QString n, const QVector<double> &v, const double r)
    : name(n), rawIndicators(v), rating(r)
{

}

void Company::clearMaxIndicators()
{
    maxIndicators.clear();
}

double Company::rawIndicator(const int i)const
{
    return rawIndicators[i];
}

double Company::stdIndicator(const int i)const
{
    return stdIndicators[i];
}

double Company::getRating()const
{
    return rating;
}

QString Company::getName()const
{
    return name;
}

int Company::rawIndicatorsSize()const
{
    return rawIndicators.size();
}

void Company::setMaxIndicator(const double max)
{
    maxIndicators << max;
}

void Company::calculateRating()
{
    for(int i = 0; i < rawIndicatorsSize(); ++i)
    {
        stdIndicators << rawIndicator(i) / maxIndicators[i];
        rating += pow(1.0 - stdIndicators[i], 2.0);
    }
    rating = sqrt(rating);
}

QVector<double> Company::maxIndicators;

bool CompanyCompare::operator ()(const Company &company1,
                                 const Company &company2)const
{
    if(key == 0)
    {
        if(ascending == 0) return company1.getName() > company2.getName();
        else return company1.getName() < company2.getName();
    }
    else if(key == 1)
    {
        if(ascending == 0) return company1.getRating() > company2.getRating();
        else return company1.getRating() < company2.getRating();
    }
    else
    {
        if(ascending == 0) return company1.stdIndicator(key - Company::nColumn) > company2.stdIndicator(key - Company::nColumn);
        else return company1.rawIndicator(key - Company::nColumn) < company2.rawIndicator(key - Company::nColumn);
    }
    return false;
}

QDebug operator<< (QDebug debug, const QVector<Company> &company)
{
    for(auto i = company.begin(); i != company.end(); ++i)
    {
        debug.nospace() << (*i).getName() << " ";
        for(auto j = (*i).getRawIndicatorsBegin(); j != (*i).getRawIndicatorsEnd(); ++j)
            debug << (*j) << " ";
        debug << '\n';
    }
    return debug;
}
