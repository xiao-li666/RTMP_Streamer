#pragma once

#include <QtWidgets/QWidget>
#include "ui_XRtmpStreamer.h"

class XRtmpStreamer : public QWidget
{
    Q_OBJECT

public:
    XRtmpStreamer(QWidget *parent = nullptr);
    ~XRtmpStreamer();

private slots:
    void btnStartClicked();
	void cbLevelChanged(int index);

private:
    Ui::XRtmpStreamerClass ui;

	bool isStart = false;
};
