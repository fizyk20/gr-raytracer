#ifndef __MAINWINDOW__
#define __MAINWINDOW__

#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QSlider>
#include <QString>
#include <QTimer>
#include <QWidget>
#include "RenderingThread.h"

class PaintArea : public QWidget
{
	QImage* img;
protected:
	void paintEvent(QPaintEvent*);
public:
	PaintArea(QWidget*);
	~PaintArea() {}
	void setImage(QImage*);
};

class MainWindow : public QMainWindow
{
Q_OBJECT
	QTimer* timer;
	int nThreads;
	RenderingThread** render;
	bool* render_running;

	QImage* image;
	QMutex imageMutex;
	
	PaintArea* paintArea;
	
	QLabel* lBg;
	QString background;
	QLineEdit *res_x, *res_y;
	
	QLineEdit *pos_r, *pos_theta, *pos_phi;
	QLineEdit *vel_r, *vel_theta, *vel_phi;
	QLineEdit *att_theta, *att_phi;
	
	QSlider* sAngMom;
	QSlider* sFov;

	int findThread(QThread*);
	bool allStopped();
	
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

signals:
	void finished();
};

#endif
