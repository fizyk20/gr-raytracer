#ifndef __MAINWINDOW__
#define __MAINWINDOW__

#include <QtGui>
#include <QTimer>
#include "RenderingThread.h"

class PaintArea : public QWidget
{
	QImage img;
protected:
	void paintEvent(QPaintEvent*);
public:
	PaintArea(QWidget*);
	~PaintArea() {}
	void setImage(const QImage&);
};

class MainWindow : public QMainWindow
{
Q_OBJECT
	QTimer* timer;
	RenderingThread* render;
	
	PaintArea* paintArea;
	
	QLabel* lBg;
	QString background;
	QLineEdit *res_x, *res_y;
	
	QLineEdit *pos_r, *pos_theta, *pos_phi;
	QLineEdit *vel_r, *vel_theta, *vel_phi;
	QLineEdit *att_theta, *att_phi;
	
	QSlider* sAngMom;
	QSlider* sFov;
	
public:
    MainWindow();
    ~MainWindow();
    
    void createComponents();
	
public slots:
	void tick();
	void finish();
	void debugMsg(QString);
	void startRendering();
	void selectBg();
};

#endif
