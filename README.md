# QtCustomWindow

## 一、简述

在Qt开发中，有时候要使用自定义窗口的样式，这样既可以实现跨平台样式的统一，也往往也比默认的要美观。


![图1-1 自定义窗口效果](http://upload-images.jianshu.io/upload_images/189568-332e339039d80b5c.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

## 二、原理和方法

在实际开发中，可以采用如下方法把原窗口的边框去除。
```c++
this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
```
然后创建一个BaseWindow类以及一个BaseTitleBar类，前者用于封装一个窗口，可以继承QWidget/QMainWindow/QDialog等，后者是其标题栏，并添上相关信号和槽。

## 三、相关代码

---```BaseTitleBar.h```---

```c++
#ifndef BASETITLEBAR_H
#define BASETITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

enum ButtonType
{
	MIN_BUTTON = 0,			// 最小化和关闭按钮;
	MIN_MAX_BUTTON ,		// 最小化、最大化和关闭按钮;
	ONLY_CLOSE_BUTTON		// 只有关闭按钮;
};

class BaseTitleBar : public QWidget
{
	Q_OBJECT

public:
	BaseTitleBar(QWidget *parent = NULL);
	~BaseTitleBar();

	// 设置标题栏背景色;
	void setBackgroundColor(int r, int g, int b);
	// 设置标题栏图标;
	void setTitleIcon(QString filePath);
	// 设置标题内容;
	void setTitleContent(QString titleContent);
	// 设置标题栏长度;
	void setTitleWidth(int width);
	// 设置标题栏上按钮类型;
	void setButtonType(ButtonType buttonType);
	// 设置标题栏中的标题是否会滚动;具体可以看效果;
	void setTitleRoll();

	// 保存/获取 最大化前窗口的位置及大小;
	void saveRestoreInfo(const QPoint point, const QSize size);
	void getRestoreInfo(QPoint& point, QSize& size);

private:
	void paintEvent(QPaintEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	// 初始化控件;
	void initControl();
	// 信号槽的绑定;
	void initConnections();
	// 加载样式文件;
	void loadStyleSheet(const QString &sheetName);

signals:
	// 按钮触发的信号;
	void signalButtonMinClicked();
	void signalButtonRestoreClicked();
	void signalButtonMaxClicked();
	void signalButtonCloseClicked();

private slots:
	// 按钮触发的槽;
	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();
	void onRollTitle();

private:
	QLabel* m_pIcon;					// 标题栏图标;
	QLabel* m_pTitleContent;			// 标题栏内容;
	QPushButton* m_pButtonMin;			// 最小化按钮;
	QPushButton* m_pButtonRestore;		// 最大化还原按钮;
	QPushButton* m_pButtonMax;			// 最大化按钮;
	QPushButton* m_pButtonClose;		// 关闭按钮;
	
	// 标题栏背景色;
	int m_colorR;
	int m_colorG;
	int m_colorB;

	// 最大化，最小化变量;
	QPoint m_restorePos;
	QSize m_restoreSize;
	// 移动窗口的变量;
	bool m_isPressed;
	QPoint m_startMovePos;
	// 标题栏跑马灯效果时钟;
	QTimer m_titleRollTimer;
	// 标题栏内容;
	QString m_titleContent;
	// 按钮类型;
	ButtonType m_buttonType;
};

#endif // BASETITLEBAR_H
```
---```BaseTitleBar.cpp```---

```c++
#include "BaseTitleBar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>
#include <QDebug>

#define BUTTON_HEIGHT 30		// 按钮高度;
#define BUTTON_WIDTH 30			// 按钮宽度;
#define TITLE_HEIGHT 30			// 标题栏高度;

BaseTitleBar::BaseTitleBar(QWidget *parent)
	: QWidget(parent)
	, m_colorR(153)
	, m_colorG(153)
	, m_colorB(153)
	, m_isPressed(false)
	, m_buttonType(MIN_MAX_BUTTON)
{
	// 初始化;
	initControl();
	initConnections();
    loadStyleSheet("MyTitle");
}

BaseTitleBar::~BaseTitleBar()
{

}

// 初始化控件;
void BaseTitleBar::initControl()
{
	m_pIcon = new QLabel;
	m_pTitleContent = new QLabel;

	m_pButtonMin = new QPushButton;
	m_pButtonRestore = new QPushButton;
	m_pButtonMax = new QPushButton;
	m_pButtonClose = new QPushButton;

	m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
	m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

	m_pTitleContent->setObjectName("TitleContent");
	m_pButtonMin->setObjectName("ButtonMin");
	m_pButtonRestore->setObjectName("ButtonRestore");
	m_pButtonMax->setObjectName("ButtonMax");
	m_pButtonClose->setObjectName("ButtonClose");

	QHBoxLayout* mylayout = new QHBoxLayout(this);
	mylayout->addWidget(m_pIcon);
	mylayout->addWidget(m_pTitleContent);

	mylayout->addWidget(m_pButtonMin);
	mylayout->addWidget(m_pButtonRestore);
	mylayout->addWidget(m_pButtonMax);
	mylayout->addWidget(m_pButtonClose);

	mylayout->setContentsMargins(5, 0, 0, 0);
	mylayout->setSpacing(0);

	m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	this->setFixedHeight(TITLE_HEIGHT);
	this->setWindowFlags(Qt::FramelessWindowHint);
}

// 信号槽的绑定;
void BaseTitleBar::initConnections()
{
	connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
	connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}

// 设置标题栏背景色,在paintEvent事件中进行绘制标题栏背景色;
//在构造函数中给了默认值，可以外部设置颜色值改变标题栏背景色;
void BaseTitleBar::setBackgroundColor(int r, int g, int b)
{
	m_colorR = r;
	m_colorG = g;
	m_colorB = b;
	// 重新绘制（调用paintEvent事件）;
	update();
}

// 设置标题栏图标;
void BaseTitleBar::setTitleIcon(QString filePath)
{
	QPixmap titleIcon(filePath);
	m_pIcon->setPixmap(titleIcon.scaled(25 , 25));
}

// 设置标题内容;
void BaseTitleBar::setTitleContent(QString titleContent)
{
	m_pTitleContent->setText(titleContent);
	m_titleContent = titleContent;
}

// 设置标题栏长度;
void BaseTitleBar::setTitleWidth(int width)
{
	this->setFixedWidth(width);
}

// 设置标题栏上按钮类型;
// 由于不同窗口标题栏上的按钮都不一样，所以可以自定义标题栏中的按钮;
// 这里提供了四个按钮，分别为最小化、还原、最大化、关闭按钮，如果需要其他按钮可自行添加设置;
void BaseTitleBar::setButtonType(ButtonType buttonType)
{
	m_buttonType = buttonType;

	switch (buttonType)
	{
	case MIN_BUTTON:
		{
			m_pButtonRestore->setVisible(false);
			m_pButtonMax->setVisible(false);
		}
		break;
	case MIN_MAX_BUTTON:
		{
			m_pButtonRestore->setVisible(false);
		}
		break;
	case ONLY_CLOSE_BUTTON:
		{
			m_pButtonMin->setVisible(false);
			m_pButtonRestore->setVisible(false);
			m_pButtonMax->setVisible(false);
		}
		break;
	default:
		break;
	}
}

// 设置标题栏中的标题是否会自动滚动，跑马灯的效果;
// 一般情况下标题栏中的标题内容是不滚动的，但是既然自定义就看自己需要嘛，想怎么设计就怎么搞O(∩_∩)O！
void BaseTitleBar::setTitleRoll()
{
	connect(&m_titleRollTimer, SIGNAL(timeout()), this, SLOT(onRollTitle()));
	m_titleRollTimer.start(200);
}

// 保存窗口最大化前窗口的位置以及大小;
void BaseTitleBar::saveRestoreInfo(const QPoint point, const QSize size)
{
	m_restorePos = point;
	m_restoreSize = size;
}

// 获取窗口最大化前窗口的位置以及大小;
void BaseTitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = m_restorePos;
	size = m_restoreSize;
}

// 绘制标题栏背景色;
void BaseTitleBar::paintEvent(QPaintEvent *event)
{
	//设置背景色;
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);
	pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));

	// 当窗口最大化或者还原后，窗口长度变了，标题栏的长度应当一起改变;
	if (this->width() != this->parentWidget()->width())
	{
		this->setFixedWidth(this->parentWidget()->width());
	}
	QWidget::paintEvent(event);
}

// 双击响应事件，主要是实现双击标题栏进行最大化和最小化操作;
void BaseTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	// 只有存在最大化、还原按钮时双击才有效;
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		// 通过最大化按钮的状态判断当前窗口是处于最大化还是原始大小状态;
		// 或者通过单独设置变量来表示当前窗口状态;
		if (m_pButtonMax->isVisible())
		{
			onButtonMaxClicked();
		}
		else
		{
			onButtonRestoreClicked();
		}
	}	

	return QWidget::mouseDoubleClickEvent(event);
}

// 以下通过mousePressEvent、mouseMoveEvent、mouseReleaseEvent三个事件实现了鼠标拖动标题栏移动窗口的效果;
void BaseTitleBar::mousePressEvent(QMouseEvent *event)
{
	if (m_buttonType == MIN_MAX_BUTTON)
	{
		// 在窗口最大化时禁止拖动窗口;
		if (m_pButtonMax->isVisible())
		{
			m_isPressed = true;
			m_startMovePos = event->globalPos();
		}
	}
	else
	{
		m_isPressed = true;
		m_startMovePos = event->globalPos();
	}
	
	return QWidget::mousePressEvent(event);
}

void BaseTitleBar::mouseMoveEvent(QMouseEvent *event)
{
	if (m_isPressed)
	{
		QPoint movePoint = event->globalPos() - m_startMovePos;
		QPoint widgetPos = this->parentWidget()->pos();
		m_startMovePos = event->globalPos();
		this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
	}
	return QWidget::mouseMoveEvent(event);
}

void BaseTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
	m_isPressed = false;
	return QWidget::mouseReleaseEvent(event);
}

// 加载本地样式文件;
// 可以将样式直接写在文件中，程序运行时直接加载进来;
void BaseTitleBar::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

// 以下为按钮操作响应的槽;
void BaseTitleBar::onButtonMinClicked()
{
	emit signalButtonMinClicked();
}

void BaseTitleBar::onButtonRestoreClicked()
{
	m_pButtonRestore->setVisible(false);
 	m_pButtonMax->setVisible(true);
	emit signalButtonRestoreClicked();
}

void BaseTitleBar::onButtonMaxClicked()
{
 	m_pButtonMax->setVisible(false);
	m_pButtonRestore->setVisible(true);
	emit signalButtonMaxClicked();
}

void BaseTitleBar::onButtonCloseClicked()
{
	emit signalButtonCloseClicked();
}

// 该方法主要是让标题栏中的标题显示为滚动的效果;
void BaseTitleBar::onRollTitle()
{
	static int nPos = 0;
	QString titleContent = m_titleContent;
	// 当截取的位置比字符串长时，从头开始;
	if (nPos > titleContent.length())
		nPos = 0;

	m_pTitleContent->setText(titleContent.mid(nPos));
	nPos++;
}
```
---```BaseWindow.h```---

```c++
#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include "BaseTitleBar.h"

class BaseWindow : public QWidget
{
	Q_OBJECT

public:
	BaseWindow(QWidget *parent = 0);
	~BaseWindow();

private:
	void initTitleBar();
	void paintEvent(QPaintEvent *event);

private slots:
	void onButtonMinClicked();
	void onButtonRestoreClicked();
	void onButtonMaxClicked();
	void onButtonCloseClicked();

protected:
    BaseTitleBar* m_titleBar;
	
};

#endif // BASEWINDOW_H
```
---```BaseWindow.cpp```---
```c++
#include "BaseWindow.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QFile>

BaseWindow::BaseWindow(QWidget *parent)
    : QWidget(parent)
{
	// FramelessWindowHint属性设置窗口去除边框;
	// WindowMinimizeButtonHint 属性设置在窗口最小化时，点击任务栏窗口可以显示出原窗口;
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	// 设置窗口背景透明;
	setAttribute(Qt::WA_TranslucentBackground);
	// 初始化标题栏;
	initTitleBar();
}

BaseWindow::~BaseWindow()
{

}

void BaseWindow::initTitleBar()
{
    m_titleBar = new BaseTitleBar(this);
	m_titleBar->move(0, 0);
	
	connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
	connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
	connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
	connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

void BaseWindow::paintEvent(QPaintEvent* event)
{
	// 	Q_UNUSED(event);
	// 	QStyleOption opt;
	// 	opt.init(this);
	// 	QPainter painter(this);
	// 	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	//设置背景色;
	QPainter painter(this);
	QPainterPath pathBack;
	pathBack.setFillRule(Qt::WindingFill);
	pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(215, 221, 228)));

	return QWidget::paintEvent(event);
}

void BaseWindow::loadStyleSheet(const QString &sheetName)
{
	QFile file(":/Resources/" + sheetName + ".css");
	file.open(QFile::ReadOnly);
	if (file.isOpen())
	{
		QString styleSheet = this->styleSheet();
		styleSheet += QLatin1String(file.readAll());
		this->setStyleSheet(styleSheet);
	}
}

void BaseWindow::onButtonMinClicked()
{
	if (Qt::Tool == (windowFlags() & Qt::Tool))
	{
		hide();    //设置了Qt::Tool 如果调用showMinimized()则窗口就销毁了？？？
	}
	else
	{
		showMinimized();
	}
}

void BaseWindow::onButtonRestoreClicked()
{
	QPoint windowPos;
	QSize windowSize;
	m_titleBar->getRestoreInfo(windowPos, windowSize);
	this->setGeometry(QRect(windowPos, windowSize));
}

void BaseWindow::onButtonMaxClicked()
{
	m_titleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
	setGeometry(FactRect);
}

void BaseWindow::onButtonCloseClicked()
{
	close();
}
```
## 四、应用代码

创建一个自定义窗口类CustomMainWindow，继承自BaseWindow(带Ui文件)。

---```CustomMainWindow.h```---
```c++
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
```

---```CustomMainWindow.cpp```---
```c++
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
    m_titleBar->setTitleIcon(":/Resources/titleicon.png");
    m_titleBar->setTitleContent(QStringLiteral("自定义窗口-欢迎来到我的窗口"));
    m_titleBar->setButtonType(MIN_MAX_BUTTON);
    m_titleBar->setTitleWidth(this->width());
}
```

![图4-1 Ui文件](http://upload-images.jianshu.io/upload_images/189568-83279a37894142ad.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)
需要注意的是，ui文件要将其上方留出一定空白给标题栏。

---```main.cpp```---
```c++
#include "CustomMainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CustomMainWindow w;
    w.show();

    return a.exec();
}
```

参考资料:
[1].[[Qt 之 自定义窗口标题栏](http://blog.csdn.net/goforwardtostep/article/details/53494800)](http://blog.csdn.net/GoForwardToStep/article/details/53494800)
