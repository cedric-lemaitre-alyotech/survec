#include "AlarmPluginRS232.h"
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

using namespace cv;

AlarmPluginRS232::AlarmPluginRS232()
{	
	qDebug() << ">CONSTRUCTOR: AlarmPluginRS232 !!";

	initPlugin();
}

AlarmPluginRS232::~AlarmPluginRS232()
{
	delete p_port;
}

QString AlarmPluginRS232::name() const
{
	return "RS232";
}

void AlarmPluginRS232::initPlugin()
{
	p_port = new QSerialPort(this);
		
	QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
	for (int i = 0; i < list.size(); i++)
	{
		qDebug() << "portName : " << list.at(i).portName();
		qDebug() << "portDesc : " << list.at(i).description();
									//	Prolific USB - to - Serial Comm Port
		if (list.at(i).description() == "Prolific USB-to-Serial Comm Port")
			p_port->setPortName(list.at(i).portName());
	}
		
	//qDebug() << "init plugin";
	//p_port->setBaudRate(QSerialPort::Baud9600);
	//p_port->setDataBits(QSerialPort::Data8);
	//p_port->setParity(QSerialPort::EvenParity);
	//p_port->setStopBits(QSerialPort::OneStop);
	//p_port->setPortName("COM3");
	//p_port->setFlowControl(QSerialPort::NoFlowControl);	
}

void AlarmPluginRS232::beginAlarm(Alarm* alarm)
{
	qDebug() << "begin alarm RS232 : " << p_port->portName();
	bool temp = p_port->open(QIODevice::ReadWrite);
	qDebug() << "open : " << temp;

	//if (p_port->isOpen())
	//	p_timerDisplay->start();
}

void AlarmPluginRS232::endAlarm(Alarm* alarm)
{
	//p_timerDisplay->stop();
	//p_port->write("0");
	//p_port->waitForBytesWritten(1000);
	p_port->close();
	qDebug() << "end alarm RS232";
}