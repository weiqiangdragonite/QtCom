#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>


class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);

    quint32 fileSize;

    void stopThread();
    void getFilePath(QString path);
    void getFileSize();

protected:
    void run();

private:
    volatile bool stopped;

    QString filePath;

signals:
    void getFileSizeDoneSignal(QByteArray);
    void sendBinaryFileToComSignal(QByteArray);
    void sendBinaryFileDoneSignal();

public slots:

};

#endif // MYTHREAD_H
