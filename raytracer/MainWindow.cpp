#include "MainWindow.h"
#include <math.h>

#define POS_PREC 4
#define VEL_PREC 7
#define ROW_MIN_H 15

MainWindow::MainWindow()
{
	render = NULL;
	setWindowTitle("Black Hole Imaging");
	resize(800,530);
	move(100,100);
	setFixedSize(800,530);
	createComponents();
	show();
	statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
	delete timer;
}

void MainWindow::createComponents()
{	
	paintArea = new PaintArea(this);
	paintArea -> move(10,10);
	paintArea -> resize(512,512);
	
	lBg = new QLabel("Background: sky.jpg", this);
	background = "./sky.jpg";
	lBg -> move(530, 10);
	lBg -> resize(260, 20);
	
	QPushButton* selectBg = new QPushButton("Select background", this);
	selectBg -> move(530, 40);
	selectBg -> resize(260, 20);
	connect(selectBg, SIGNAL(clicked()), this, SLOT(selectBg()));
	
	QLabel* resolution = new QLabel("Resolution:", this);
	resolution -> move(530, 70);
	resolution -> resize(260, 20);
	
	res_x = new QLineEdit("128", this);
	res_y = new QLineEdit("128", this);
	QIntValidator* resval = new QIntValidator(1, 512, this);
	res_x -> setText("128");
	res_y -> setText("128");
	res_x -> setValidator(resval);
	res_y -> setValidator(resval);
	res_x -> move(530, 100);
	res_x -> resize(100, 20);
	res_y -> move(655, 100);
	res_y -> resize(100, 20);
	
	QLabel* lres_x = new QLabel("x", this);
	lres_x -> move(635, 100);
	lres_x -> resize(15, 20);
	
	QLabel* mass = new QLabel("Mass: 0.1", this);
	mass -> move(530, 130);
	mass -> resize(260, 20);
	
	QLabel* lAngMom = new QLabel("Angular momentum / mass:", this);
	lAngMom -> move(530, 160);
	lAngMom -> resize(260, 20);
	
	sAngMom = new QSlider(Qt::Horizontal, this);
	sAngMom -> move(530, 180);
	sAngMom -> resize(260, 20);
	sAngMom -> setMinimum(0);
	sAngMom -> setMaximum(100);
	sAngMom -> setTickPosition(QSlider::TicksBelow);
	
	QLabel* slider0 = new QLabel("0", this);
	QLabel* slider1 = new QLabel("1", this);
	slider0 -> resize(15,15);
	slider1 -> resize(15,15);
	slider0 -> move(535,200);
	slider1 -> move(780,200);
	
	QLabel* fov = new QLabel("FoV (degrees):", this);
	fov -> move(530, 230);
	fov -> resize(260, 20);
	
	sFov = new QSlider(Qt::Horizontal, this);
	sFov -> move(530, 250);
	sFov -> resize(260, 20);
	sFov -> setMinimum(10);
	sFov -> setMaximum(120);
	sFov -> setTickPosition(QSlider::TicksBelow);
	sFov -> setSliderPosition(60);
	
	QLabel* sFov0 = new QLabel("10", this);
	QLabel* sFov1 = new QLabel("120", this);
	sFov0 -> resize(25,15);
	sFov1 -> resize(35,15);
	sFov0 -> move(530,270);
	sFov1 -> move(770,270);
	
	QLabel* lpos = new QLabel("Observer's position:", this);
	lpos -> move(530, 300);
	lpos -> resize(260, 20);
	
	pos_r = new QLineEdit("3.0", this);
	pos_r -> setText("3.0");
	pos_r -> move(530, 330);
	pos_r -> resize(80, 20);
	pos_r -> setValidator(new QDoubleValidator(0.2, 10.0, 4, this));
	
	pos_theta = new QLineEdit("1.5707", this);
	pos_theta -> setText("1.5707");
	pos_theta -> move(620, 330);
	pos_theta -> resize(80, 20);
	pos_theta -> setValidator(new QDoubleValidator(0.01, 3.14, 4, this));
	
	pos_phi = new QLineEdit("0.0", this);
	pos_phi -> setText("0.0");
	pos_phi -> move(710, 330);
	pos_phi -> resize(80, 20);
	pos_phi -> setValidator(new QDoubleValidator(0.0, 2*M_PI, 4, this));
	
	QLabel* lvel = new QLabel("Observer's velocity:", this);
	lvel -> move(530, 360);
	lvel -> resize(260, 20);
	
	vel_r = new QLineEdit("0.0", this);
	vel_r -> setText("0.0");
	vel_r -> move(530, 390);
	vel_r -> resize(80, 20);
	vel_r -> setValidator(new QDoubleValidator(-0.5, 0.5, 4, this));
	
	vel_theta = new QLineEdit("0.0", this);
	vel_theta -> setText("0.0");
	vel_theta -> move(620, 390);
	vel_theta -> resize(80, 20);
	vel_theta -> setValidator(new QDoubleValidator(-0.5, 0.5, 4, this));
	
	vel_phi = new QLineEdit("0.0", this);
	vel_phi -> setText("0.0");
	vel_phi -> move(710, 390);
	vel_phi -> resize(80, 20);
	vel_phi -> setValidator(new QDoubleValidator(-0.5, 0.5, 4, this));
	
	QLabel* latt = new QLabel("Observer's orientation:", this);
	latt -> move(530, 420);
	latt -> resize(260, 20);
	
	att_theta = new QLineEdit("0.0", this);
	att_theta -> setText("0.0");
	att_theta -> move(530, 450);
	att_theta -> resize(80, 20);
	att_theta -> setValidator(new QDoubleValidator(-M_PI/2, M_PI/2, 4, this));
	
	att_phi = new QLineEdit("0.0", this);
	att_phi -> setText("0.0");
	att_phi -> move(620, 450);
	att_phi -> resize(80, 20);
	att_phi -> setValidator(new QDoubleValidator(-M_PI, M_PI, 4, this));
	
	QPushButton* run = new QPushButton("Run",this);
	run -> move(530,500);
	run -> resize(260,20);
	
	timer = new QTimer(this);
	timer -> setInterval(50);
	
	connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
	
	connect(run, SIGNAL(clicked()), this, SLOT(startRendering()));
	connect(run, SIGNAL(clicked()), timer, SLOT(start()));
}

void MainWindow::startRendering()
{
	if(background == "")
	{
		QMessageBox::warning(this, "Error", "No background was selected");
		return;
	}
	
	int resx, resy;
	resx = res_x->text().toInt();
	resy = res_y->text().toInt();
	if(resx == 0 || resy == 0)
	{
		QMessageBox::warning(this, "Error", "Invalid resolution specified");
		return;
	}
	
	double a;
	a = (double)(sAngMom -> value())/100;
	
	double fov = (double)(sFov -> value());
	
	double p_r, p_th, p_ph, v_r, v_th, v_ph, a_th, a_ph;
	
	p_r = pos_r->text().toDouble();
	p_th = pos_theta->text().toDouble();
	p_ph = pos_phi->text().toDouble();
	
	v_r = vel_r->text().toDouble();
	v_th = vel_theta->text().toDouble();
	v_ph = vel_phi->text().toDouble();
	
	a_th = att_theta->text().toDouble();
	a_ph = att_phi->text().toDouble();
	
	if(render != NULL) delete render;
	render = new RenderingThread(resx, resy, fov, 0.1, a*0.1, vector4(0.0, p_r, p_th, p_ph), vector4(0.0, v_r, v_th, v_ph),
						a_th, a_ph, background);
	connect(render, SIGNAL(finished()), this, SLOT(finish()));
	connect(render, SIGNAL(finished()), timer, SLOT(stop()));
	connect(render, SIGNAL(debugMsg(QString)), this, SLOT(debugMsg(QString)));
	render -> start();
}

void MainWindow::tick()
{
	int x,y;
	if(render != NULL)
	{
		x = render->getX();
		y = render->getY();
		statusBar()->showMessage("X: " + QString::number(x) + "\tY: " + QString::number(y), 20000);
	
		paintArea->setImage(render->getImage());
		paintArea->repaint();
	}
}

void MainWindow::finish()
{
	QMessageBox::information(this, "Renderer", "Rendering finished!");
}

void MainWindow::debugMsg(QString msg)
{
	printf("%s\n",msg.toAscii().data());
}

void MainWindow::selectBg()
{
	background = QFileDialog::getOpenFileName(this, "Select background image", "", "Images (*.jpg *.png *.gif *.bmp)");
	
	QStringList a = background.split("/");
	QString b = a[a.size()-1];
	
	lBg -> setText(QString("Background: ") + ((b == "")?"<none>":b));
}

/*
 * PaintArea
 */

PaintArea::PaintArea(QWidget* parent)
	: QWidget(parent)
{
}

void PaintArea::setImage(const QImage& _img)
{
	img = _img;
}

void PaintArea::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);
	p.drawImage(0, 0, img);
}
