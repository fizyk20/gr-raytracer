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
	int resx, resy, min_y, max_y;
	KerrManifold* m;
	
	QImage* image;
	QPainter render;
	QMutex* imageMutex;
public:
	double debug;
	
	RenderingThread(int resx, int resy, int min_y, int max_y, double fov1, double _M, double _angm, vector4 _p0, vector4 _v0, double th, double ph, QString _file, QImage* image, QMutex* mutex);
	~RenderingThread();
	int getX();
	int getY();
	void run();
signals:
	void debugMsg(QString);
};

#endif
