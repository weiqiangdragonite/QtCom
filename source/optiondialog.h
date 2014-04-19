#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>

#include "qextserialport/qextserialport.h"

typedef struct global_font {
    QString fontName;
    QString fontSize;
    bool bold;
    bool italic;
} GlobalFont;

typedef struct com_settings {
    QString comName;
    QString baudRate;
    QString dataBits;
    QString parity;
    QString stopBits;
    QString flow;
    QString timeout;
} ComSettings;


namespace Ui {
class OptionDialog;
}

class OptionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OptionDialog(QWidget *parent = 0);
    ~OptionDialog();

    ComSettings comSettings;

signals:
    void resetFontReturn(GlobalFont);
    void resetComSettings(ComSettings);
    
private slots:
    void on_okPushButton_clicked();

    void initDefaultFont(GlobalFont myFont);

    void on_applyPushButton_clicked();

    void on_fontNameComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::OptionDialog *ui;

    bool resetFont;
    QFont font;

    // function
    void initUartOptions();
    void setUartOptions();
};


#endif // OPTIONDIALOG_H
