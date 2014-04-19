#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


#include "qextserialport/qextserialport.h"
#include "mytextedit.h"
#include "optiondialog.h"

namespace Ui {
class MainWindow;
}

class QextSerialPort;
class MyTextEdit;
class OptionDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *event);

signals:
    void initFont(GlobalFont);

private slots:
    void readQtCom();
    void resetFont(GlobalFont myFont);
    void resetDefaultCom(ComSettings comSettings);
    void getFileSizeDoneSlot(QByteArray);
    void snedBinaryFileToComSlot(QByteArray);
    void sendBinaryFileDoneSlot();

    void on_actionClear_buffers_triggered();

    void on_actionOptions_triggered();

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

    void on_actionBinary_File_triggered();

private:
    Ui::MainWindow *ui;

    OptionDialog *optionDialog;

    // declare QtCom project
    QextSerialPort *qtCom;
    struct PortSettings qtComSettings;
    ComSettings myComSetting;

    // text edit
    MyTextEdit *textEdit;
    bool isConnected;

    bool shiftKey;
    bool ctrlKey;

    //
    QByteArray receiveData;
    qint64 receiveByte;
    qint64 sendBuffer;

    qint64 cursorPosition;

    qint64 sendBinaryFileProgress;
    bool getFileSizeDone;
    bool sendBinaryFileDone;

    // function
    void readSettings();
    void saveSettings();
    void portSettings(ComSettings comSettings);
    void initTextEditFont();

};

#endif // MAINWINDOW_H
