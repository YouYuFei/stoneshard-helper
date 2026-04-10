#ifndef SPEED_H
#define SPEED_H

#include <QtGlobal>
#ifdef Q_OS_WIN
#include <windows.h>
#include <tlhelp32.h>
#endif
#include <QProcess>

class Speed : public QObject
{
public:
    Speed(){}
    static bool GetStatus();
    static bool setSpeed(double factor);
    static void stop();
private:

#ifdef Q_OS_WIN
    static void getProcessId();
    static bool startBridge64();
    static QProcess* m_bridge64;
    static DWORD m_processId;
#endif
};

#endif // SPEED_H
