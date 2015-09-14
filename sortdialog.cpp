#include <QtWidgets>

#include "sortdialog.h"

SortDialog::SortDialog(QWidget *parent)
    : QDialog(parent)
{
    comboBox = new QComboBox();

    items
            << tr("Предприятие")
            << tr("Рейтинговая оценка")

            << tr("Рентабельность активов")
            << tr("Чистая рентабельность активов")
            << tr("Рентабельность собственного капитала")
            << tr("Рентабельность производственных фондов")

            << tr("Чистая рентабельность")
            << tr("Рентабельность продаж")
            << tr("Общая рентабельность")
            << tr("Валовая рентабельность")

            << tr("Отдача всех активов")
            << tr("Фондоотдача")
            << tr("Оборачиваемость оборотных активов")
            << tr("Оборачиваемость запасов")
            << tr("Оборачиваемость дебиторской задолженности")
            << tr("Оборачиваемость банковских активов")
            << tr("Отдача собственного капитала")

            << tr("Коэффициент текущей ликвидности")
            << tr("Коэффициент абсолютной ликвидности")
            << tr("Индекс постоянного актива")
            << tr("Коэффициент автономии")
            << tr("Обеспеченность запасов собственными\nоборотными средствами");

    comboBox->addItems(items);
    comboBox->setCurrentIndex(1);
    comboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QLabel *lblColumn = new QLabel(tr("По колонке:"));
    checkAscending = new QCheckBox(tr("По возрастанию"));
    checkAscending->setChecked(true);

    QPushButton *okButton = new QPushButton(tr("OK"));
    okButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QPushButton *cancelButton = new QPushButton(tr("Отмена"));
    cancelButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(lblColumn);
    vLayout->addWidget(comboBox);
    vLayout->addWidget(checkAscending);

    QVBoxLayout *mainBox = new QVBoxLayout;
    mainBox->addLayout(vLayout);
    mainBox->addLayout(hLayout);
    setLayout(mainBox);

    setWindowTitle(tr("Сортировка"));

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}
