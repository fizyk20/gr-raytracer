#ifndef __RENDERING_THREAD_H__
#define __RENDERING_THREAD_H__

#include <QtGui>
#include <QThread>
#include <entity.h>
#include <kerr.h>

class RenderingThread : public QThread
{
Q_OBJECT
	int x,y;
	double fov;
	
	vector4 pos0;
	vector4 v0;
	double theta, phi; //camera orientation
	
	QString file;
	int max_x, max_y;
	KerrManifold* m;
	
	QImage render;
	
	double step(double r, double rs);
public:
	double debug;
	
	RenderingThread(int, int, double, double, double, vector4, vector4, double, double, QString);
	~RenderingThread();
	int getX();
	int getY();
	const QImage& getImage();
	void run();
signals:
	void debugMsg(QString);
};

#endif
