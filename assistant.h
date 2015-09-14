#ifndef ASSISTANT
#define ASSISTANT

#include <QtCore/QString>

class QProcess;

class Assistant
{
public:
    Assistant();
    ~Assistant();
    void showDocumentation(const QString &file);

private:
    bool startAssistant();
    QProcess *proc;
};

#endif // ASSISTANT
