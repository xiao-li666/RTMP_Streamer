#include "XRtmpStreamer.h"
#include "XController.h"
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit> // Include QLineEdit for rtmpUrl

//rtmp://10.210.155.30:1935/live

XRtmpStreamer::XRtmpStreamer(QWidget *parent)
   : QWidget(parent)
{
   ui.setupUi(this);

   connect(ui.btnStart, &QPushButton::clicked, this, &XRtmpStreamer::btnStartClicked);
   connect(ui.cbLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &XRtmpStreamer::cbLevelChanged);
}

XRtmpStreamer::~XRtmpStreamer()
{
	XController::Get()->Stop();
}

void XRtmpStreamer::cbLevelChanged(int index)
{
	if (index < 0) return;
	// 美颜等级 1/5/9
	if (index == 0) index = 1;
	else if (index == 1) index = 5;
	else if (index == 2) index = 9;

	XController::Get()->Set("d", index);
}

void XRtmpStreamer::btnStartClicked()
{
   if (!isStart)
   {
       if (ui.rtmpUrl->text().isEmpty())
       {
           return;
       }
	   isStart = true;
       ui.btnStart->setText(QString::fromLocal8Bit("暂停"));
       bool ok = false;
	   int index = ui.inUrl->text().toInt(&ok);
       if (ok)
       {
           XController::Get()->videoIndex = ui.inUrl->text().toInt();
       }
       else
       {
		   XController::Get()->inUrl = ui.inUrl->text().toStdString().c_str();
       }
       // Ensure rtmpUrl is a QLineEdit, as QTextEdit does not have a text() method
       XController::Get()->outUrl = ui.rtmpUrl->text().toStdString().c_str();
       XController::Get()->StartController();
	   XController::Get()->Set("d", ui.cbLevel->currentIndex() + 1); // 默认美颜等级1
       //XController::Get()->wait();
   }
   else
   {
	   isStart = false;
       ui.btnStart->setText(QString::fromLocal8Bit("开始"));
	   XController::Get()->Stop();
	   //XController::Get()->wait();
   }
}