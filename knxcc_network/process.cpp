#include "process.h"

Process::Process()
{
    _buffer = QString::null;
    _process = new KProcess();

    connect(_process, SIGNAL(receivedStdout(KProcess*, char*, int)),
            this, SLOT(slotProcessOutput(KProcess*, char*, int)));

}

Process::~Process()
{
}

void Process::setCommand(QString command)
{
    // make clean
    _process->clearArguments();
    _buffer = QString::null;
    
    *_process << "/bin/sh";
    *_process << "-c";
    *_process << command;
}


void Process::start(bool block)
{
    if( block )
        _process->start(KProcess::Block, KProcess::Stdout);
    else
        _process->start(KProcess::DontCare, KProcess::Stdout);
}

QString Process::getBuffer()
{
    return _buffer;
}

int Process::exitStatus()
{
    return _process->exitStatus();
}

bool Process::normalExit()
{
    return _process->normalExit();
}

void Process::slotProcessOutput(KProcess* process, char* buffer, int len)
{
    if (process != _process) return;

    _buffer.append(QString::fromLocal8Bit(buffer, len));
}

