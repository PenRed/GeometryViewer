#ifndef TEXTCONFIG_H
#define TEXTCONFIG_H

#include <QWidget>
#include <QTextEdit>
#include <QMimeData>
#include <QDropEvent>

class textConfig : public QTextEdit
{
    Q_OBJECT
public:
    textConfig(QWidget* = nullptr);

    void dropEvent(QDropEvent *event);
};

#endif // TEXTCONFIG_H
