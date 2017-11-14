#pragma once

#include <QtWidgets/QMainWindow>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeWidget>
#include "qcustomplot.h"
#include "ui_QtGuiSurveillanceMulti.h"
#include "stdafx.h"
using namespace std;
class QtGuiSurveillanceMulti : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiSurveillanceMulti(QWidget *parent = Q_NULLPTR);
	~QtGuiSurveillanceMulti();

	
	//***********************************************���б�������********************************************///
	QTabWidget *TabWidget;
	QGridLayout *AccountGridLayout, **P_AccountGridLayout, *MonitorPageGridLayout;
	QWidget *AccountWidget, **P_AccountWidget,*MonitorPageWidget;
	QTableView *AccountCapitalTable, **P_AccountCapitalTable, *AccountSummaryTable,**P_AccountSummaryTable,*MonitorPageStopTable, *MonitorPageSummayTable;
	QCustomPlot *AccountPlot,**P_AccountPlot, *MonitorPageTotalPnLPlot, *MonitorPageKindPnLPlot, *MonitorPagePositionPlot;
	QTabWidget *AccountTab,**P_AccountTab;
	QWidget *AccountHoldingWidget, **P_AccountHoldingWidget, *AccountTradingWidget, **P_AccountTradingWidget,*AccountLogWidget,**P_AccountLogWidget;
	QTableView *AccountHoldingTable, **P_AccountHoldingTable, *AccountTradingTable, **P_AccountTradingTable;
	QListView *AccountLogListview, **P_AccountLogListview;
	QHBoxLayout *AccountTabHorizontalLayout,**P_AccountTabHorizontalLayout;
	//�˺���Ϣ
	list<string> accounts;
	int n, num_accounts, sizeofplot;
	//ʱ�����
	double  startTime;
	QTime now_min,start_time;
	//��ͼ��������
	std::map<QTime, double>plotdata;
	//����ͳ����Ϣ
	Display_Monitor total_monitor;

	 

	//*************************************************��������**********************************************///
	///���ⲿ��ȡ�˺��б���˺���Ϣ
	void GetAccount();

	///UI�����ⲿ�����ܵĳ�ʼ��
	void InitUi();

	///UI�����м��ҳ���岼�ֵĳ�ʼ�����
	void InitUiMonitorPage();

	///UI�����и����˻�ҳ�����岼�ֵĳ�ʼ�����
	void InitUiAccountPages();

	///UI�����п���ڲ����ݵĳ�ʼ��
	void Init();

	///UI�����м��ҳ����ڲ����ݵĳ�ʼ��
	void InitMonitorPage();

	///UI�����и����˻�ҳ�����ڲ����ݵĳ�ʼ��
	void InitAccountPages();

	///ʵʱ���ܼ��㺯��
	void CalculateSummary();

	/// ��дQObject����麯�� ���̳���QObject)
	virtual void timerEvent(QTimerEvent * event);

	///���¼��ҳ����
	void UpdateMonitorPage();

	///���¼��ҳ��ӯ������ͼ
	void UpdateMonitorPageTotalPnLPlot();

	///���¼��ҳ����ӯ����״ͼ
	void UpdateMonitorPageKindPnLPlot();

	///���¼��ҳ���˻���λ�ֲ���״ͼ
	void UpdateMonitorPagePositionPlot();

	///���¸����˻�����
	void UpdateAccountPages();


private:
	Ui::QtGuiSurveillanceMultiClass ui;
	QStandardItemModel  *tablemodel_position, *tablemodel_trade, *tablemodel_summary, *tablemodel_account,*MonitorPageStopTableMode,*MonitorPageSummayTableModel;
	QStandardItemModel **p_tablemodel_position, **p_tablemodel_trade, **p_tablemodel_summary, **p_tablemodel_account;
	QStringListModel *listviewmodel,**p_listviewmodel;
	QCPBars *Agriculture, *Black, *BaseMetal, *Energy, *PreciousMetal, *Financial,*regen;
	QSharedPointer<QCPAxisTickerText> textTicker, positiontextTicker;
	int m_timerId; // ÿ��Timer��һ��id
};
