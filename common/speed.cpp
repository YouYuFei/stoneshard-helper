#include "speed.h"
#include <QFile>
#include <QDebug>

#define PROCESS_NAMME "StoneShard.exe"
#define BRIDGE64_EXE "bridge64.exe"
#define SPEEDPATCH64_DLL "speedpatch64.dll"

#ifdef Q_OS_WIN
QProcess *Speed::m_bridge64 = nullptr;
DWORD Speed::m_processId = 0;
#endif

#ifdef Q_OS_WIN
bool Speed::startBridge64()
{
    getProcessId();
    if (m_processId == 0) {
        return false;
    }
    if (!QFile::exists(BRIDGE64_EXE)) {
        QFile::copy(":/speedpatch/bridge64.exe",BRIDGE64_EXE);
    }
    if (!QFile::exists(SPEEDPATCH64_DLL)) {
        QFile::copy(":/speedpatch/speedpatch64.dll",SPEEDPATCH64_DLL);
    }
    m_bridge64 = new QProcess();
    QStringList params64;
    m_bridge64->start(BRIDGE64_EXE, params64);
    if (!m_bridge64->waitForStarted()) {
        qDebug() << "64位桥接子进程启动失败";
        m_bridge64->deleteLater();
        return false;
    }
    m_bridge64->setProcessChannelMode(QProcess::MergedChannels);
    QString cmd = QString("inject %1\n").arg(m_processId);
    m_bridge64->write(cmd.toUtf8(), cmd.size());
    m_bridge64->waitForBytesWritten();
    return true;
}
#endif

bool Speed::GetStatus()
{
    bool result = false;
#ifdef Q_OS_WIN
    if (!m_bridge64) {
        return startBridge64();
    }
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, m_processId);
    if (hProcess == NULL) {
        stop();
        return result;
    }
    CloseHandle(hProcess);
    result = true;
#endif
    return result;
}

bool Speed::setSpeed(double factor)
{
    if (!GetStatus()) {
        return false;
    }
#ifdef Q_OS_WIN
    QString cmd = QString("change %1\n").arg(factor);
    m_bridge64->write(cmd.toUtf8(), cmd.size());
    m_bridge64->waitForBytesWritten();
#endif
    return true;
}

void Speed::stop()
{
#ifdef Q_OS_WIN
    if (!m_bridge64) {
        return;
    }
    QString cmd = QString("unhook %1\n").arg(m_processId);
    m_bridge64->write(cmd.toUtf8(), cmd.size());
    m_bridge64->waitForBytesWritten();
    m_bridge64->close();
    m_bridge64->deleteLater();
#endif
}

#ifdef Q_OS_WIN
void Speed::getProcessId()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hProcess;
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (QString::fromWCharArray(pe.szExeFile) == PROCESS_NAMME) {
                hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe.th32ProcessID);
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    m_processId = GetProcessId(hProcess);
}
#endif


