#include <QDebug>
#include <QVector>
#include <QPair>
#include <QtConcurrent>
#include <QFileInfo>
#include <QMessageBox>
#include "filesource.h"
#include "event.h"

FileSource::FileSource(QObject* eventHandler)
    :ILogSource(eventHandler, "file")
{
    mLog.reset(new MemLog);
}

FileSource::~FileSource()
{
    doClose();
}

void FileSource::setFileName(const QString &path)
{
    mFile.setFileName(path);
}

bool FileSource::open()
{
    if (!mFile.open(QFile::ReadOnly)) {
        qCritical()<<"failed to open";
        return false;
    }

    mMem = mFile.map(0, mFile.size(), QFile::MapPrivateOption);
    if (!mMem) {
        qCritical("mmap failed");
        mFile.close();
        return false;
    }

    mLog->init(mMem);
    mLog->setSource(shared_from_this());

    parse();
    return true;
}

void FileSource::close()
{
    doClose();
}

QString FileSource::getSimpleDesc()
{
    return QFileInfo(mFile.fileName()).fileName();
}


void FileSource::parse()
{
    QtConcurrent::run([this]{
        auto size = mFile.size();
        qDebug()<<mFile.fileName()<< ":"<<size;
 
        QVector<qint64> lines;
        lines.reserve(210000);//20M按一行100字符，估计21w行

        for (auto i = 0; i < size ; i++) {
            
            auto c = mMem[i];

            if (c == 0) {
                mMem[i] = ' ';
                //修复一些错误的日志，其中包含了二进制的0，这会意外导致字符串中断
            } else if (c == '\n') {
                lines.push_back(i);
                if (lines.size() >= lines.capacity()) {
                    qDebug()<<"post in parse"<<lines.size();
                    post(new LogChangeEvent(mLog, [this, lines]{mLog->addLines(lines);}));
                    lines.clear();
                }
            }
        }
        if(lines.at(lines.size()-1) != size-1){
            qDebug()<<"test end"<<lines.size();
            lines.push_back(size-1);
            if (lines.size() >= lines.capacity()) {
                qDebug()<<"post in parse";
                post(new LogChangeEvent(mLog, [this, lines]{mLog->addLines(lines);}));
                lines.clear();
            }
        }
        if (!lines.isEmpty()) {
            qDebug()<< "post log Change"<< lines.size();
            post(new LogChangeEvent(mLog, [this, lines]{mLog->addLines(lines);}));
        }

    });
}

void FileSource::doClose()
{
    if (mMem) {

        mParseTask.waitForFinished();
        mFile.unmap(mMem);
        mFile.close();
        mMem = nullptr;
    }

    mLog.reset();
}
