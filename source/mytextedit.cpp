#include <QKeyEvent>
#include <QDebug>

#include "mytextedit.h"

MyTextEdit::MyTextEdit(QWidget *parent) :
    QTextEdit(parent)
{

}

void MyTextEdit::keyPressEvent(QKeyEvent *event)
{
    //qDebug() << "QTextEdit key press event";
    //qDebug() << event->type();
    //if (event->key() == Qt::Key_A) qDebug() << "A press";
    //QTextEdit::keyPressEvent(event);
    event->ignore();
}

void MyTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    //qDebug() << "QTextEdit key release event";
    //qDebug() << event->type();
    event->ignore();
}

void MyTextEdit::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "text edit: mouse press event";
    return;
}

void MyTextEdit::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "text edit: mouse release event";
    return;
}
