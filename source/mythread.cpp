#include <QFile>

#include <QDebug>

#include "mythread.h"

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
    stopped = false;
    fileSize = 0;
}

void MyThread::stopThread()
{
    stopped = true;
    qDebug() << "stopping thread";
    quit();
}

void MyThread::getFilePath(QString path)
{
    filePath = path;
}

void MyThread::getFileSize()
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "open binary file error";
        return;
    }

    // get file size
    fileSize = file.size();
    QByteArray temp;
    while (1) {
        temp.append(fileSize % 10 + '0');
        fileSize /= 10;
        if (fileSize == 0) break;
    }
    QByteArray size;
    for (int i = temp.count() - 1; i >= 0; --i) {
       size.append(temp.at(i));
    }
    size.append('\r');
    qDebug() << "size(QByteArray): " << size << " bytes";
    file.close();

    emit getFileSizeDoneSignal(size);
}

void MyThread::run()
{
    getFileSize();

    sleep(2);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "open binary file error";
        return;
    }
    file.seek(0);
    qDebug() << "start reading file in thread";

    while (!stopped) {
        //qDebug() << "in the thread";
        //msleep(500);

        QByteArray array;
        array = file.read(1);
        emit sendBinaryFileToComSignal(array);
        usleep(1);
        if (file.atEnd()) {
            qDebug() << "file is the end now";
            stopped = true;
            break;
        }
    }
    file.close();

    stopped = true;
    emit sendBinaryFileDoneSignal();

}
