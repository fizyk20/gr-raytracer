#include "RenderingThread.h"
#include <dpintegrator.h>

#define BOX_R 20.0
#define C 1.4

RenderingThread::RenderingThread(int a, int b, double fov1, double _M, double _angm, vector4 _p0, vector4 _v0, double th, double ph, QString _file)
	: render(a, b, QImage::Format_RGB32)
{
	m = new KerrManifold(_M, _angm);
	max_x = a;
	max_y = b;
	fov = fov1*M_PI/180;
	x = 0;
	y = 0;
	
	pos0 = _p0;
	v0 = _v0;
	theta = th*M_PI/180;
	phi = ph*M_PI/180;
	
	file = _file;
}

RenderingThread::~RenderingThread()
{
	delete m;
}

int RenderingThread::getX()
{
	return x;
}

int RenderingThread::getY()
{
	return y;
}

double RenderingThread::step(double r, double rs)
{
	double x = (r-rs)/rs;
	return atan(x*x)*0.1;
	//return 0.3/(1.0 + exp(-5.0*(x+1)));
}

void RenderingThread::run()
{	
	double M = m->getMass();
	double a = m->getAngMomentum();
	Metric* g = m->getMetric(EF);
	
	Point p0 = Point(EF, pos0);
	
	Entity* ship = new Entity(m, p0, vector4(1.0, 0.0, 0.0, 0.0),  vector4(0.0, 1.0, 0.0, 0.0),
									 vector4(0.0, 0.0, 0.0, 1.0), vector4(0.0, 0.0, -1.0, 0.0));
	
	vector4 newU = vector4(1.0, 0.0, 0.0, 0.0) + v0[1]*ship->getX() + v0[2]*ship->getZ() + v0[3]*ship->getY();
	ship -> setVel(newU);
	
	QImage sky(file);
	render.fill(qRgb(255,255,255));
			
	vector4 fwd0 = vector4() - ship->getX();
	vector4 up0 = ship->getZ();
	vector4 rgt0 = ship->getY();
	vector4 t = ship->getVel();
	Point pos = ship->getPos();
	
	vector4 fwd, up, rgt;
	fwd = cos(theta)*cos(phi)*fwd0 + cos(theta)*sin(phi)*rgt0 + sin(theta)*up0;
	rgt = cos(phi)*rgt0 - sin(phi)*fwd0;
	up = cos(theta)*up0 - sin(theta)*cos(phi)*fwd0 - sin(theta)*sin(phi)*rgt0;
	
	vector4 box_u(1,0,0,0);
	double rs = M + sqrt(M*M-a*a);
	double dmax = tan(fov/2);
	
	for(y=0; y<max_y; y++)
		for(x=0; x<max_x; x++)
		{
			//convert x,y to a starting vector;
			double dx = (double)x/max_x*2.0-1.0;
			double dy = (double)y/max_y*2.0-1.0;
			dx *= dmax*max_x/max_y;
			dy *= dmax;
			
			vector4 dir = (fwd + dx*rgt + dy*up)/sqrt(dx*dx+dy*dy+1);
			double dt = sqrt(-g->g(dir,dir,pos)/g->g(t,t,pos));
			dir -= dt*t; //vector directed to the past
			//emit debugMsg("dir=[" + QString::number(dir[0],'f',7) + "; " + QString::number(dir[1],'f',7) + "; " + QString::number(dir[2],'f',7) + "; " + QString::number(dir[3],'f',7) + "]");
			
			//double source_omega,target_omega = dt;
			
			Particle photon(m, pos, dir);
			DPIntegrator integrator(1e-6);
			integrator.setMinStep(0.01);
			integrator.setMaxStep(1.0);
			photon.setIntegrator(&integrator);
			
			double r = pos[1];
			
			while(r > C*rs && r < BOX_R)
			{
				photon.propagate();
				r = m->convertPointTo(photon.getPos(), EF)[1];
			}
			
			Point fin_pos = m->convertPointTo(photon.getPos(), EF);
			//vector4 box_u2 = box_u/g->g_v(box_u,box_u,fin_pos);
			
			//source_omega = g->g_v(photon.getVector(1),box_u2,fin_pos);
			double th,ph;
			th = fin_pos[2];
			ph = fin_pos[3];
			while(ph < 0.0) ph += 2*M_PI;
			while(ph > 2*M_PI) ph -= 2*M_PI;
			
			int sx,sy;
			sx = (int)(ph/M_PI/2*sky.width())%sky.width();
			sy = (int)(th/M_PI*sky.height())%sky.height();
			
			if(r > C*rs)
				render.setPixel(x,y,sky.pixel(sx,sy));
			else
				render.setPixel(x,y,qRgb(0,0,0));
		}
	
	render.save("result.jpg");
}

const QImage& RenderingThread::getImage()
{
	return render;
}
