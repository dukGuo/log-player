#ifndef FILESOURCE_H
#define FILESOURCE_H

#include <QFuture>
#include <QFutureSynchronizer>
#include "ilogsource.h"
#include "memlog.h"

class FileSource: public ILogSource
{
public:
    FileSource(QObject* eventHandler);
    ~FileSource();

    void setFileName(const QString& path);

    bool open() override;
    void close() override;

    std::shared_ptr<ILog> getLog() override {return mLog;}

    QString getSimpleDesc() override;

private:
    void parse();
    void doClose();

private:
    QFile mFile;
    uchar* mMem{nullptr};
    QFuture<void> mParseTask;
    std::shared_ptr<MemLog> mLog;
};

#endif // FILESOURCE_H
