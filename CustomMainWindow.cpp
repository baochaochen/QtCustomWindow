#include "CustomMainWindow.h"
#include "ui_CustomMainWindow.h"

CustomMainWindow::CustomMainWindow(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::CustomMainWindow)
{
    initTitleBar();
    ui->setupUi(this);
}

CustomMainWindow::~CustomMainWindow()
{
    delete ui;
}

void CustomMainWindow::initTitleBar()
{
    // 设置标题栏跑马灯效果，可以不设置;
    m_titleBar->setTitleRoll();
    m_titleBar->setBackgroundColor(56,70,85);
    m_titleBar->setTitleIcon(":/icon.png");
    m_titleBar->setTitleContent(QStringLiteral("自定义窗口-欢迎来到我的窗口"));
    m_titleBar->setButtonType(MIN_MAX_BUTTON);
    m_titleBar->setTitleWidth(this->width());
}
