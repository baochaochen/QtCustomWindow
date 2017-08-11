#ifndef CUSTOMMAINWINDOW_H
#define CUSTOMMAINWINDOW_H

#include <QWidget>

#include "BaseWindow.h"

namespace Ui {
class CustomMainWindow;
}

class CustomMainWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit CustomMainWindow(QWidget *parent = 0);
    ~CustomMainWindow();

private:
    void initTitleBar();

    Ui::CustomMainWindow *ui;
};

#endif // CUSTOMMAINWINDOW_H
