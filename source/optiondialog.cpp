#include <QDebug>
#include <QFontDialog>
#include <QtXml>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "optiondialog.h"
#include "ui_optiondialog.h"



OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);

    //
    resetFont = false;

    //
    initUartOptions();
    setUartOptions();

    on_applyPushButton_clicked();

}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::on_okPushButton_clicked()
{
    on_applyPushButton_clicked();

    accept();
}

void OptionDialog::initDefaultFont(GlobalFont myFont)
{
    int index;

    index = ui->fontNameComboBox->findText(myFont.fontName);
    ui->fontNameComboBox->setCurrentIndex(index);

    index = ui->fontSizeComboBox->findText(myFont.fontSize);
    ui->fontSizeComboBox->setCurrentIndex(index);

    ui->fontBoldCheckBox->setChecked(myFont.bold);
    ui->fontItalicCheckBox->setChecked(myFont.italic);
}

void OptionDialog::on_applyPushButton_clicked()
{
    GlobalFont myFont;
    // font style
    myFont.fontName = ui->fontNameComboBox->currentText();
    myFont.fontSize = ui->fontSizeComboBox->currentText();
    if (ui->fontBoldCheckBox->isChecked()) {
        myFont.bold = true;
    } else {
        myFont.bold = false;
    }
    if (ui->fontItalicCheckBox->isChecked()) {
        myFont.italic = true;
    } else {
        myFont.italic = false;
    }

    emit resetFontReturn(myFont);

    // set port
    comSettings = {
        // com name
        ui->portNameComboBox->currentText(),
        // BaudRateType
        ui->baudRateComboBox->currentText(),
        // DataBitsType
        ui->dataBitsComboBox->currentText(),
        // ParityType
        ui->parityBitsComboBox->currentText(),
        // StopBitsType
        ui->stopBitsComboBox->currentText(),
        // FlowType
        ui->flowTypeComboBox->currentText(),
        // Timeout_Millisec(ms)
        // The delay time of read/write to COM
        ui->timeoutLineEdit->text()
    };

    emit resetComSettings(comSettings);
}

void OptionDialog::on_fontNameComboBox_currentIndexChanged(const QString &arg1)
{

}

void OptionDialog::initUartOptions()
{
    // linux port
#if defined (Q_OS_UNIX)
    ui->portNameComboBox->addItem("ttyS0");
    ui->portNameComboBox->addItem("ttyS1");
    ui->portNameComboBox->addItem("ttyS2");
    ui->portNameComboBox->addItem("ttyS3");
    ui->portNameComboBox->addItem("ttyUSB0");
    ui->portNameComboBox->addItem("ttyUSB1");
    ui->portNameComboBox->addItem("ttyUSB2");
    ui->portNameComboBox->addItem("ttyUSB3");
    // window port
#elif defined (Q_OS_WIN)
    ui->portNameComboBox->addItem("COM1");
    ui->portNameComboBox->addItem("COM2");
    ui->portNameComboBox->addItem("COM3");
    ui->portNameComboBox->addItem("COM4");
    ui->portNameComboBox->addItem("COM5");
#endif


    // BaudRateType
    ui->baudRateComboBox->addItem("4800");
    ui->baudRateComboBox->addItem("9600");
    ui->baudRateComboBox->addItem("19200");
    ui->baudRateComboBox->addItem("38400");
    ui->baudRateComboBox->addItem("57600");
    ui->baudRateComboBox->addItem("115200");
#if defined(Q_OS_WIN) || defined(qdoc)
    ui->baudRateComboBox->addItem("128000");    // WINDOWS ONLY
    ui->baudRateComboBox->addItem("256000");    // WINDOWS ONLY
#endif  // Q_OS_WIN
#if defined(Q_OS_UNIX) && (defined(B230400) && defined(B4000000)) || defined(qdoc)
    ui->baudRateComboBox->addItem("230400");    // POSIX ONLY
    ui->baudRateComboBox->addItem("460800");    // POSIX ONLY
    ui->baudRateComboBox->addItem("500000");    // POSIX ONLY
    ui->baudRateComboBox->addItem("576000");    // POSIX ONLY
    ui->baudRateComboBox->addItem("921600");    // POSIX ONLY
#endif  // Q_OS_UNIX


    // DataBitsType
    ui->dataBitsComboBox->addItem("5");
    ui->dataBitsComboBox->addItem("6");
    ui->dataBitsComboBox->addItem("7");
    ui->dataBitsComboBox->addItem("8");

    // ParityType
    ui->parityBitsComboBox->addItem("None");
    ui->parityBitsComboBox->addItem("Odd");
    ui->parityBitsComboBox->addItem("Even");
#if defined(Q_OS_WIN) || defined(qdoc)
    ui->parityBitsComboBox->addItem("PAR_MARK");    // WINDOWS ONLY
#endif

    // StopBitsType
    ui->stopBitsComboBox->addItem("1");
#if defined(Q_OS_WIN) || defined(qdoc)
    ui->stopBitsComboBox->addItem("1.5");   // WINDOWS ONLY
#endif
    ui->stopBitsComboBox->addItem("2");

    // FlowType
    ui->flowTypeComboBox->addItem("OFF");
    ui->flowTypeComboBox->addItem("HARDWARE");
    ui->flowTypeComboBox->addItem("XON/XOFF");
}

void OptionDialog::setUartOptions()
{
    // read/write the file
    QFile file("../QtCom/qtcom.xml");
    bool isExists = file.exists();

    qDebug() << "file exists: " << isExists;

    QFileInfo info(file);
    qDebug() << "path: " << info.absoluteFilePath();

    // the file not exist
    if (!isExists) {

        if (!file.open(QIODevice::ReadWrite)) {
            qDebug() << "open xml file error";
            return;
        }

        qDebug() << "file doesn't exist, create it";

        QXmlStreamWriter writer(&file);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();

        writer.writeStartElement("settings");
        writer.writeStartElement("uart");
#if defined (Q_OS_UNIX)
        writer.writeTextElement("comname", "ttyS0");
#elif defined (Q_OS_WIN)
        writer.writeTextElement("comname", "COM0");
#endif
        writer.writeTextElement("baudrate", "115200");
        writer.writeTextElement("databits", "8");
        writer.writeTextElement("parity", "None");
        writer.writeTextElement("stopbits", "1");
        writer.writeTextElement("flow", "OFF");
        writer.writeTextElement("timeout", "10");
        writer.writeEndElement();

        writer.writeStartElement("text");
#if defined (Q_OS_UNIX)
        writer.writeTextElement("fontname", "Monospace");
#elif defined (Q_OS_WIN)
        writer.writeTextElement("fontname", "Courier");
#endif
        writer.writeTextElement("fontsize", "12");
        writer.writeTextElement("bold", "false");
        writer.writeTextElement("italic", "false");
        writer.writeEndElement();
        writer.writeEndElement();

        writer.writeEndDocument();

        file.close();
    }



    if (!file.open(QIODevice::ReadWrite)) {
        qDebug() << "open xml file error";
        return;
    }
    // read file
    QDomDocument dom;
    //
    if (!dom.setContent(&file)) {
        file.close();
        qDebug() << "read dom error in setDefaultPortOptions";
        return;
    }

    //
    file.close();

    // xml version and encoding
    //QDomNode firstNode = dom.firstChild();

    // get the root element
    QDomElement root = dom.documentElement();

    // get the comsettings
    QDomNode node = root.firstChild();
    QDomElement element = node.toElement();

    QDomNodeList list = element.childNodes();
    QDomNode childNode;
    int index;

    // com name
    childNode = list.at(0);
    index = ui->portNameComboBox->findText(childNode.toElement().text());
    ui->portNameComboBox->setCurrentIndex(index);

    // baud rate
    childNode = list.at(1);
    index = ui->baudRateComboBox->findText(childNode.toElement().text());
    ui->baudRateComboBox->setCurrentIndex(index);

    // data bits
    childNode = list.at(2);
    index = ui->dataBitsComboBox->findText(childNode.toElement().text());
    ui->dataBitsComboBox->setCurrentIndex(index);

    // parity
    childNode = list.at(3);
    index = ui->parityBitsComboBox->findText(childNode.toElement().text());
    ui->parityBitsComboBox->setCurrentIndex(index);

    // stop bits
    childNode = list.at(4);
    index = ui->stopBitsComboBox->findText(childNode.toElement().text());
    ui->stopBitsComboBox->setCurrentIndex(index);

    // flow
    childNode = list.at(5);
    ui->flowTypeComboBox->findText(childNode.toElement().text());
    ui->flowTypeComboBox->setCurrentIndex(index);

    // timeout
    childNode = list.at(6);
    ui->timeoutLineEdit->setText(childNode.toElement().text());
}
