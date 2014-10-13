#include <QMessageBox>
#include <QKeyEvent>
#include <QSettings>
#include <QCloseEvent>
#include <QtXml/QtXml>
#include <QFile>
#include <QFileDialog>
#include <QTime>
#include <QProgressDialog>
#include <QFileDialog>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mythread.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ui
    ui->mainToolBar->hide();
    textEdit = new MyTextEdit(this);

    // read the default setting(size, position)
    readSettings();

    //
    isConnected = false;

    // set the option dialog
    optionDialog = new OptionDialog(this);

    myComSetting = optionDialog->comSettings;
    portSettings(myComSetting);


    connect(optionDialog, SIGNAL(resetComSettings(ComSettings)),
            this, SLOT(resetDefaultCom(ComSettings)));

    connect(optionDialog, SIGNAL(resetFontReturn(GlobalFont)),
            this, SLOT(resetFont(GlobalFont)));
    connect(this, SIGNAL(initFont(GlobalFont)),
            optionDialog, SLOT(initDefaultFont(GlobalFont)));

    initTextEditFont();

    // I have already set a vertical layout in the mainwindow.ui
    ui->verticalLayout->addWidget(textEdit);
    textEdit->setCursorWidth(5);
    //textEdit->setLineWrapMode(QTextEdit::NoWrap);


    receiveByte = 0;
    sendBuffer = 0;
    cursorPosition = 0;
    shiftKey = false;
    ctrlKey = false;
    getFileSizeDone = false;
    sendBinaryFileDone = false;
    sendBinaryFileProgress = 0;

}

MainWindow::~MainWindow()
{
    // close qtCom is if connected
    if (isConnected) {
        qtCom->close();
        delete qtCom;
    }

    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!isConnected) return;

    //
    QByteArray byteArray;

    //qDebug() << "You press: " << event->key();

    // check the shift/ctrl key
    if (event->modifiers() == Qt::ShiftModifier) {
        shiftKey = true;
        qDebug() << "shift key press";
    } else if (event->modifiers() == Qt::ControlModifier) {
        ctrlKey = true;
        qDebug() << "ctrl key press";
    }


    // check the letter key
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) {
        byteArray.append(event->key());
        // if the shift key doesn't press, convert to lower letter
        if (!shiftKey) byteArray = byteArray.toLower();
    }



    // check the number key
    else if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) {

        // under linux if uneffective
        // if shift key is press, convert to the symbol
        if (shiftKey) {
            switch (event->key()) {
                case Qt::Key_1:
                    byteArray.append(0x21);
                    break;
                case Qt::Key_2:
                    byteArray.append(0x40);
                    break;
                case Qt::Key_3:
                    byteArray.append(0x23);
                    break;
                case Qt::Key_4:
                    byteArray.append(0x24);
                    break;
                case Qt::Key_5:
                    byteArray.append(0x25);
                    break;
                case Qt::Key_6:
                    byteArray.append(0x5E);
                    break;
                case Qt::Key_7:
                    byteArray.append(0x26);
                    break;
                case Qt::Key_8:
                    byteArray.append(0x2A);
                    break;
                case Qt::Key_9:
                    byteArray.append(0x28);
                    break;
                case Qt::Key_0:
                    byteArray.append(0x29);
                    break;
                default:
                    break;
            }
        } else {
            // shift key doesn't press, show the number
            byteArray.append(event->key());
        }
    }

    // check the other key
    // space
    else if (event->key() == Qt::Key_Space) {
        byteArray.append(0x20);

    //
    } else if (event->key() == Qt::Key_Apostrophe) {
        if (shiftKey) byteArray.append(0x22);
        else byteArray.append(0x27);

    // plus and equal
    } else if (event->key() == Qt::Key_Equal) {
        if (shiftKey) byteArray.append(0x2B);
        else byteArray.append(0x3D);

    // comma and less
    } else if (event->key() == Qt::Key_Comma) {
        if (shiftKey) byteArray.append(0x3C);
        else byteArray.append(0x2C);

    // minus and underscore
    } else if (event->key() == Qt::Key_Minus) {
        if (shiftKey) byteArray.append(0x5F);
        else byteArray.append(0x2D);

    // period and greater
    } else if (event->key() == Qt::Key_Period) {
        if (shiftKey) byteArray.append(0x3E);
        else byteArray.append(0x2E);

    // slash and question
    } else if (event->key() == Qt::Key_Slash) {
        if (shiftKey) byteArray.append(0x3F);
        else byteArray.append(0x2F);

    // colon and semicolon
    } else if (event->key() == Qt::Key_Semicolon) {
        if (shiftKey) byteArray.append(0x3A);
        else byteArray.append(0x3B);

    // bracket left and brace left
    } else if (event->key() == Qt::Key_BracketLeft) {
        if (shiftKey) byteArray.append(0x7B);
        else byteArray.append(0x5B);

    // bracket right and brace right
    } else if (event->key() == Qt::Key_BracketRight) {
        if (shiftKey) byteArray.append(0x7D);
        else byteArray.append(0x5D);

    // backslash and bar
    } else if (event->key() == Qt::Key_Backslash) {
        if (shiftKey) byteArray.append(0x7C);
        else byteArray.append(0x5C);

    // quote left and ascii tilde
    } else if (event->key() == Qt::Key_QuoteLeft) {
        if (shiftKey) byteArray.append(0x7E);
        else byteArray.append(0x60);

    // tab key
    } else if (event->key() == Qt::Key_Tab) {
        byteArray = "    ";
    }


    // backspace
    else if (event->key() == Qt::Key_Backspace && sendBuffer != 0) {
        //qDebug() << "backspace";
        byteArray.append(0x8);

        QTextCursor cursor = textEdit->textCursor();
        cursor.movePosition(QTextCursor::PreviousCharacter);
        textEdit->setTextCursor(cursor);
    }

    // enter is press
    else if (event->key() == Qt::Key_Return) {
        // new line \r\n
        byteArray.append(0x0D);
        //byteArray.append(0x0A);
    }


    // send the data to COM
    if (!byteArray.isEmpty()) {
        //qDebug() << "ASCII: " << byteArray << ", Hex: " << byteArray.toHex();

        if (event->key() == Qt::Key_Backspace) {
            --sendBuffer;
        } else if (event->key() == Qt::Key_Return) {
            sendBuffer = 0;
        } else {
            ++sendBuffer;
        }

        qtCom->write(byteArray);
    }

}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << "mainwindow key release";
    //if (event->type() == QEvent::KeyRelease)
    //    qDebug() << "key release";


#if defined (Q_OS_UNIX)
    if (event->modifiers() == Qt::ShiftModifier) {
        shiftKey = false;
        qDebug() << "shift key release";
    } else if (event->modifiers() == Qt::ControlModifier) {
        ctrlKey = false;
        qDebug() << "ctrl key release";
    }
#elif defined (Q_OS_WIN)
    if (event->key() == Qt::Key_Shift) {
        shiftKey = false;
        qDebug() << "shift key release";
    } else if (event->key() == Qt::Key_Control) {
        ctrlKey = false;
        qDebug() << "ctrl key release";
    }
#endif
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mian window: mouse press event";
    return;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "mian window: mouse release event";
    return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::readQtCom()
{
    QByteArray temp = qtCom->readAll();

    //qDebug() << temp.toHex();


    // delete one of the new line
    for (int i = 0, n = temp.size(); i < n; ++i) {
        // \n = 0x0A, \r = 0x0D
        if (i == (n - 1)) break;

        // \r \n
        if ((temp.at(i) == 0x0D) && (temp.at(i+1) == 0x0A)) {
            temp.remove(i+1, 1);
            --n;
            qDebug() << "r n - remove n";
        }
    }


    QByteArray array = textEdit->document()->toPlainText().toAscii();
    //qDebug() << "array size: " << array.size();
    QByteArray newLine;
    newLine.append(0x08);
    newLine.append(0x20);
    newLine.append(0x08);

    if (temp.size() == 3 && (temp.toHex() == newLine.toHex())) {
        array.remove(array.size() - 1, 1);
        textEdit->setText(array);
    } else {
        textEdit->setText(array + temp);
    }

    //textEdit->setText(textEdit->document()->toPlainText() + temp);

    // move the cursor to the end
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);

}

void MainWindow::resetFont(GlobalFont myFont)
{
    QDomDocument dom;
    QFile file("../QtCom/qtcom.xml");
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "open xml error";
        return;
    }

    //
    if (!dom.setContent(&file)) {
        file.close();
        qDebug() << "read dom error in resetFont";
        return;
    }

    //
    file.seek(0);

    // xml version and encoding
    QDomNode firstNode = dom.firstChild();
    qDebug() << qPrintable(firstNode.nodeName()) << " "
             << qPrintable(firstNode.nodeValue());

    // get the root element
    QDomElement root = dom.documentElement();

    // get the comsettings
    QDomNode node = root.firstChild();
    node = node.nextSibling();
    QDomElement element = node.toElement();

    QDomNodeList list = element.childNodes();
    QDomNode childNode;

    childNode = list.at(0);
    childNode.toElement().firstChild().setNodeValue(myFont.fontName);

    childNode = list.at(1);
    childNode.toElement().firstChild().setNodeValue(myFont.fontSize);

    childNode = list.at(2);
    childNode.toElement().firstChild().setNodeValue(myFont.bold ? "true":"false");

    childNode = list.at(3);
    childNode.toElement().firstChild().setNodeValue(myFont.italic ? "true":"false");

    QTextStream out(&file);
    dom.save(out, 4);

    //
    file.close();

    QFont font = QFont(myFont.fontName, myFont.fontSize.toInt(),
                       myFont.bold ? QFont::Bold : QFont::Normal,
                       myFont.italic);

    textEdit->setFont(font);
}

void MainWindow::resetDefaultCom(ComSettings comSettings)
{

    QDomDocument dom;
    QFile file("../QtCom/qtcom.xml");
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "open xml error";
        return;
    }

    //
    if (!dom.setContent(&file)) {
        file.close();
        qDebug() << "read dom error in resetDefaultCom";
        return;
    }

    //
    file.seek(0);

    // xml version and encoding
    //QDomNode firstNode = dom.firstChild();
    //qDebug() << qPrintable(firstNode.nodeName()) << " "
    //         << qPrintable(firstNode.nodeValue());

    // get the root element
    QDomElement root = dom.documentElement();

    // get the comsettings
    QDomNode node = root.firstChild();
    QDomElement element = node.toElement();

    QDomNodeList list = element.childNodes();
    QDomNode childNode;

    // com name
    childNode = list.at(0);
    childNode.toElement().firstChild().setNodeValue(comSettings.comName);

    // baud rate
    childNode = list.at(1);
    childNode.toElement().firstChild().setNodeValue(comSettings.baudRate);

    // data bits
    childNode = list.at(2);
    childNode.toElement().firstChild().setNodeValue(comSettings.dataBits);

    // parity
    childNode = list.at(3);
    childNode.toElement().firstChild().setNodeValue(comSettings.parity);

    // stop bits
    childNode = list.at(4);
    childNode.toElement().firstChild().setNodeValue(comSettings.stopBits);

    // flow
    childNode = list.at(5);
    childNode.toElement().firstChild().setNodeValue(comSettings.flow);

    // timeout
    childNode = list.at(6);
    childNode.toElement().firstChild().setNodeValue(comSettings.timeout);

    QTextStream out(&file);
    dom.save(out, 4);

    //
    file.close();

    //
    portSettings(comSettings);
    myComSetting = comSettings;
}

void MainWindow::getFileSizeDoneSlot(QByteArray size)
{
    getFileSizeDone = true;
    qDebug() << "";
    qtCom->write(size);
}

void MainWindow::snedBinaryFileToComSlot(QByteArray array)
{
    //qDebug() << "write to com: " << array.toHex();
    //qDebug() << "start send binary file";
    qtCom->write(array);
    //qDebug() << "send binary file over";
    ++sendBinaryFileProgress;
    //sendBinaryFileDoneSlot();

    return;
}

void MainWindow::sendBinaryFileDoneSlot()
{
    sendBinaryFileDone = true;
}

void MainWindow::on_actionClear_buffers_triggered()
{
    textEdit->clear();
    sendBuffer = 0;
}

void MainWindow::on_actionOptions_triggered()
{
    qDebug() << "options";

    //optionDialog->setModal(true);
    if (optionDialog->exec() == QDialog::Accepted) {

        qDebug() << "ok";
    } else {
        qDebug() << "cancel";
    }

    //
}

void MainWindow::readSettings()
{
    QSettings settings("weiqiangdragonite@gmail.com", "QtCom");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(800, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::saveSettings()
{
    QSettings settings("weiqiangdragonite@gmail.com", "QtCom");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::portSettings(ComSettings comSettings)
{

    //
    qtComSettings.BaudRate = (BaudRateType) comSettings.baudRate.toInt();
    qtComSettings.DataBits = (DataBitsType) comSettings.dataBits.toInt();

    // StopBitsType
    if (comSettings.stopBits == "1") {
        qtComSettings.StopBits = STOP_1;
    // WINDOWS ONLY
#if defined(Q_OS_WIN) || defined(qdoc)
    } else if (comSettings.stopBits  == "1.5") {
        qtComSettings.StopBits = STOP_1_5;
#endif
    } else if (comSettings.stopBits == "2") {
        qtComSettings.StopBits = STOP_2;
    }

    // ParityType
    if (comSettings.parity == "None") {
        qtComSettings.Parity = PAR_NONE;
    } else if (comSettings.parity  == "Odd") {
        qtComSettings.Parity = PAR_ODD;
    } else if (comSettings.parity == "Even") {
        qtComSettings.Parity = PAR_EVEN;
#if defined(Q_OS_WIN) || defined(qdoc)
    } else if (comSettings.parity == "PAR_MARK") {
        qtComSettings.Parity = PAR_MARK;    //WINDOWS ONLY
#endif
    }

    if (comSettings.flow == "OFF") {
        qtComSettings.FlowControl = FLOW_OFF;
    } else if (comSettings.flow  == "HARDWARE") {
        qtComSettings.FlowControl = FLOW_HARDWARE;
    } else if (comSettings.flow == "XON/XOFF") {
        qtComSettings.FlowControl = FLOW_XONXOFF;
    }

    qtComSettings.Timeout_Millisec = comSettings.timeout.toLong();

}

void MainWindow::initTextEditFont()
{
    QDomDocument dom;
    QFile file("../QtCom/qtcom.xml");
    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "open xml error";
        return;
    }

    //
    if (!dom.setContent(&file)) {
        file.close();
        qDebug() << "read dom error in initTextEditFont";
        return;
    }

    //
    file.close();

    // xml version and encoding
    //QDomNode firstNode = dom.firstChild();
    //qDebug() << qPrintable(firstNode.nodeName()) << " "
    //         << qPrintable(firstNode.nodeValue());

    // get the root element
    QDomElement root = dom.documentElement();

    // get the comsettings
    QDomNode node = root.firstChild();
    node = node.nextSibling();
    QDomElement element = node.toElement();

    QDomNodeList list = element.childNodes();
    QDomNode childNode;


    GlobalFont myFont;

    childNode = list.at(0);
    myFont.fontName = childNode.toElement().text();

    childNode = list.at(1);
    myFont.fontSize = childNode.toElement().text();

    childNode = list.at(2);
    if (childNode.toElement().text() == "true") {
        myFont.bold = true;
    } else {
        myFont.bold = false;
    }

    childNode = list.at(3);
    if (childNode.toElement().text() == "true") {
        myFont.italic = true;
    } else {
        myFont.italic = false;
    }


    bool ok;
    QFont font = QFont(myFont.fontName, myFont.fontSize.toInt(&ok, 10),
                       myFont.bold ? QFont::Bold : QFont::Normal,
                       myFont.italic);

    textEdit->setFont(font);
    emit initFont(myFont);
}

void MainWindow::on_actionConnect_triggered()
{
#if defined (Q_OS_UNIX)
    qtCom = new QextSerialPort("/dev/" + myComSetting.comName,
                               qtComSettings, QextSerialPort::EventDriven);
#elif defined (Q_OS_WIN)
    qtCom = new QextSerialPort(myComSetting.comName,
                               qtComSettings, QextSerialPort::EventDriven);
#endif
    //
    if (qtCom->open(QIODevice::ReadWrite)) {
        QMessageBox::information(this, tr("Open Com successful"),
                                 tr("Connected to %1").arg(myComSetting.comName),
                                 QMessageBox::Ok);
    } else {
        QMessageBox::critical(this, tr("Open Com error"),
                              tr("Connected failed"), QMessageBox::Ok);
        return;
    }

    //textEdit->clear();
    ui->actionDisconnect->setEnabled(true);
    ui->actionConnect->setEnabled(false);

    isConnected = true;

    connect(qtCom, SIGNAL(readyRead()), this, SLOT(readQtCom()));
}

void MainWindow::on_actionDisconnect_triggered()
{
    qtCom->close();
    delete qtCom;
    qtCom = NULL;

    isConnected = false;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

}

void MainWindow::on_actionBinary_File_triggered()
{
    if (!isConnected) return;


    QString filePath = QFileDialog::getOpenFileName(this, tr("File Dialog"),
                                                    "", "*.*");
    qDebug() << "file name: " << filePath;


    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "open binary file error";
        return;
    }
    file.close();


    /*
    // get file size
    quint32 fileSize = file.size();
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
    qDebug() << "size: " << size << " bytes";
    //qtCom->write(size);
    */


    MyThread thread;

    connect(&thread, SIGNAL(getFileSizeDoneSignal(QByteArray)),
            this, SLOT(getFileSizeDoneSlot(QByteArray)));
    connect(&thread, SIGNAL(sendBinaryFileToComSignal(QByteArray)),
            this, SLOT(snedBinaryFileToComSlot(QByteArray)));
    connect(&thread, SIGNAL(sendBinaryFileDoneSignal()),
            this, SLOT(sendBinaryFileDoneSlot()));

    thread.getFilePath(filePath);
    thread.start();


    QProgressDialog dialog(tr("Sending File"), tr("Cancel"), 0, thread.fileSize, this);
    //QProgressDialog dialog(this);
    dialog.setWindowTitle(tr("Progress Dialog"));
    dialog.setWindowModality(Qt::WindowModal);
    dialog.show();

    while (true) {
        if (sendBinaryFileDone) break;
        dialog.setValue(sendBinaryFileProgress);
        QCoreApplication::processEvents();
        if (dialog.wasCanceled()) break;
    }
    qDebug() << "finished";


    /*
    // wait for 2 second and send file
    QTime time;
    time.start();
    int startTime = 0;
    while (true) {
        // milliseconds
        int endTime = time.elapsed();
        if ((endTime - startTime) >= 2000) break;
    }
    */


    //file.seek(0);
    //QByteArray array;
    //array = file.readAll();
    //qDebug() << "content: " << array.toHex();

    //qtCom->write(array);


    //
    //file.close();


    while (thread.isRunning()) {
        thread.stopThread();
        thread.quit();
    }
    qDebug() << "thread is stopped";

    getFileSizeDone = false;
    sendBinaryFileDone = false;
    sendBinaryFileProgress = 0;

    return;
}

void MainWindow::on_actionSave_File_triggered()
{
    QFile file("E:/vbox_share/serial.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Open file error"),
                              file.errorString(), QMessageBox::Ok);
    }

    file.write(textEdit->toPlainText().toAscii());

    textEdit->clear();
    sendBuffer = 0;

    file.close();
}
