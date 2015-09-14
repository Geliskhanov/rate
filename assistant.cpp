#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QtCore>
#include <QtCore/QProcess>

#include <QtWidgets/QMessageBox>

#include "assistant.h"

Assistant::Assistant()
    : proc(0)
{

}

Assistant::~Assistant()
{
    if(proc && proc->state() == QProcess::Running)
    {
        proc->terminate();
        proc->waitForFinished(3000);
    }
    delete proc;
}

void Assistant::showDocumentation(const QString &page)
{
    if(!startAssistant()) return;
}

bool Assistant::startAssistant()
{
    if(!proc) proc = new QProcess();

    if(proc->state() != QProcess::Running)
    {
        QString app = QCoreApplication::applicationDirPath() + QDir::separator();
        app += QLatin1String("assistant");
        QStringList args;
        args << QLatin1String("-collectionFile")
            << QCoreApplication::applicationDirPath()
            + QLatin1String("/documentation/col.qhc")
            << QLatin1String("-enableRemoteControl");

        proc->start(app, args);

        if(!proc->waitForStarted()) return false;
    }
    return true;
}
