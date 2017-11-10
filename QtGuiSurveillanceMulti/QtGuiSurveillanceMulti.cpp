#include "QtGuiSurveillanceMulti.h"
#include "stdafx.h"
#define color_gray 50
#define color_white 200
extern map<string, Display_GUI>Display;
using namespace std;
QtGuiSurveillanceMulti::QtGuiSurveillanceMulti(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	InitUi();
	
	Init();

	m_timerId = startTimer(500);
}
QtGuiSurveillanceMulti::~QtGuiSurveillanceMulti()
{
	//�ͷŶ�̬�����UI�ؼ�ָ��
	delete[]P_AccountWidget;
	delete[]P_AccountGridLayout ;
	delete[]P_AccountCapitalTable ;
	delete[]P_AccountSummaryTable ;
	delete[]P_AccountPlot ;
	delete[]P_AccountTab ;
	delete[]P_AccountHoldingTable ;
	delete[]P_AccountTradingTable ;
	delete[]P_AccountHoldingWidget ;
	delete[]P_AccountTradingWidget;
	delete[]P_AccountLogWidget;
	delete[]P_AccountLogListview;
	delete[]P_AccountTabHorizontalLayout ;
	delete[]AccountWidget;
	
}
void QtGuiSurveillanceMulti::GetAccount()
{
	char path[MAX_PATH];
	wchar_t path_w[MAX_PATH];
	GetModuleFileName(NULL, path_w, sizeof(path_w));
	(wcsrchr(path_w, _T('\\')))[1] = 0;
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, path_w, -1, path, MAX_PATH, NULL, NULL);
	string path_str = path;
	string accounts_file_fn = path_str + "�˺��б�.txt";
	ifstream in_file(accounts_file_fn);
	if (in_file.good() && in_file.is_open())
	{
		string s;
		while (getline(in_file, s))
		{
			stringstream line(s);
			string account_s;
			line >> account_s;
			accounts.push_back(account_s);
		}
	}
}

void QtGuiSurveillanceMulti::InitUi()
{
	
	this->setStyleSheet(QStringLiteral("background-color: rgb(60, 60, 60);"));//�������屳����ɫ
	GetAccount();
	num_accounts = accounts.size();
	if (num_accounts == 0)
		num_accounts = 1;
	TabWidget = new QTabWidget(ui.centralWidget);

	//��Ƽ��ҳUI���
	InitUiMonitorPage();

	//��ʼ�������˻�ҳ���UI��ܡ�
	InitUiAccountPages();

	QHBoxLayout *layout = new QHBoxLayout(ui.centralWidget);
	layout->addWidget(TabWidget);
	this->setLayout(layout);

	
}
void QtGuiSurveillanceMulti::InitUiMonitorPage()
{
	//�ܼ��ҳ��Widget����
	MonitorPageWidget = new  QWidget();
	QBrush qBrush(QColor(50, 50, 50));//���ñ���ɫ  
	//���ҳչ�����ֱ���
	MonitorPageGridLayout = new QGridLayout(MonitorPageWidget);
	//���ҳ���ܱ��
	MonitorPageSummayTable = new QTableView(MonitorPageWidget);
	MonitorPageSummayTable->setObjectName(QStringLiteral("tableView_2"));
	MonitorPageSummayTable->setMaximumSize(QSize(360, 16777215));
	MonitorPageSummayTable->horizontalHeader()->font();
	MonitorPageSummayTable->verticalHeader()->hide();
	MonitorPageSummayTable->setEditTriggers(QAbstractItemView::NoEditTriggers);						//����ڲ��ɱ༭

	MonitorPageGridLayout->addWidget(MonitorPageSummayTable, 0, 0, 2, 1);
	//���ҳֹ����
	MonitorPageStopTable = new QTableView(MonitorPageWidget);
	MonitorPageStopTable->setObjectName(QStringLiteral("tableView"));
	MonitorPageStopTable->horizontalHeader()->font();
	MonitorPageStopTable->verticalHeader()->hide();
	MonitorPageStopTable->setMaximumSize(QSize(360, 16777215));

	MonitorPageGridLayout->addWidget(MonitorPageStopTable, 0, 3, 2, 1);
	//���ҳ��ӯ������ͼ
	MonitorPageTotalPnLPlot = new QCustomPlot(MonitorPageWidget);
	MonitorPageTotalPnLPlot->setObjectName(QStringLiteral("listView"));
	MonitorPageTotalPnLPlot->setBackground(qBrush);
	MonitorPageTotalPnLPlot->addGraph(); // blue line
	MonitorPageTotalPnLPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	MonitorPageTotalPnLPlot->plotLayout()->insertRow(0);//���ͼƬ����
	MonitorPageTotalPnLPlot->plotLayout()->addElement(0, 0, new QCPTextElement(MonitorPageTotalPnLPlot, "TotalPnL", QFont("sans", 12, QFont::Bold)));

	MonitorPageGridLayout->addWidget(MonitorPageTotalPnLPlot, 0, 1, 1, 2);
	//���ҳ�ִ���ӯ����״ͼ
	MonitorPageKindPnLPlot = new QCustomPlot(MonitorPageWidget);
	MonitorPageKindPnLPlot->setObjectName(QStringLiteral("listView_2"));
	MonitorPageKindPnLPlot->setBackground(qBrush);
	MonitorPageKindPnLPlot->addGraph();
	MonitorPageKindPnLPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	MonitorPageKindPnLPlot->plotLayout()->insertRow(0);//���ͼƬ����
	MonitorPageKindPnLPlot->plotLayout()->addElement(0, 0, new QCPTextElement(MonitorPageKindPnLPlot, "ClassifiedTotalPnL", QFont("sans", 12, QFont::Bold)));

	MonitorPageGridLayout->addWidget(MonitorPageKindPnLPlot, 1, 1, 1, 1);
	//���ҳ���˻���λ�ֲ���״ͼ
	MonitorPagePositionPlot = new QCustomPlot(MonitorPageWidget);
	MonitorPagePositionPlot->setObjectName(QStringLiteral("listView_3"));
	MonitorPagePositionPlot->setBackground(qBrush);
	MonitorPagePositionPlot->addGraph(); // blue line
	MonitorPagePositionPlot->plotLayout()->insertRow(0);//���ͼƬ����
	MonitorPagePositionPlot->plotLayout()->addElement(0, 0, new QCPTextElement(MonitorPagePositionPlot, "Position", QFont("sans", 12, QFont::Bold)));

	MonitorPageGridLayout->addWidget(MonitorPagePositionPlot, 1, 2, 1, 1);
	//������õ�MonitorPageWidget��Ϊ������Tabwidget��һ��tab��
	TabWidget->addTab(MonitorPageWidget, QString::fromLocal8Bit("���ҳ"));
	TabWidget->tabBar()->setTabTextColor(0, QColor(50, 50, 50));						//���ñ��Tabbar��������ɫ������ͬ�˺��б��������ɫ
}
void QtGuiSurveillanceMulti::InitUiAccountPages()
{
	QBrush qBrush(QColor(50, 50, 50));//���ñ���ɫ  
	///�����˻���Widget����
	int i = 0;
	AccountWidget = new QWidget[num_accounts];
	P_AccountWidget = new QWidget*[num_accounts];
	P_AccountGridLayout = new  QGridLayout*[num_accounts];
	P_AccountCapitalTable = new QTableView *[num_accounts];
	P_AccountSummaryTable = new QTableView *[num_accounts];
	P_AccountPlot = new QCustomPlot*[num_accounts];
	P_AccountTab = new QTabWidget *[num_accounts];
	P_AccountHoldingTable = new QTableView *[num_accounts];
	P_AccountTradingTable = new QTableView *[num_accounts];
	P_AccountHoldingWidget = new QWidget*[num_accounts];
	P_AccountTradingWidget = new QWidget*[num_accounts];
	P_AccountLogWidget = new QWidget*[num_accounts];
	P_AccountLogListview = new QListView*[num_accounts];
	P_AccountTabHorizontalLayout = new QHBoxLayout*[num_accounts];

	for (list<string>::iterator it = accounts.begin(); it != accounts.end(); it++)
	{

		P_AccountWidget[i] = &AccountWidget[i];
		AccountGridLayout = new QGridLayout(P_AccountWidget[i]);

		///���岼��
		P_AccountGridLayout[i] = AccountGridLayout;
		P_AccountGridLayout[i]->setSpacing(6);
		P_AccountGridLayout[i]->setContentsMargins(11, 11, 11, 11);
		P_AccountGridLayout[i]->setObjectName(QStringLiteral("gridLayout"));
		P_AccountGridLayout[i]->setContentsMargins(0, 0, 0, 0);

		///�˻��ʽ���Ϣ���
		AccountCapitalTable = new QTableView(P_AccountWidget[i]);
		P_AccountCapitalTable[i] = AccountCapitalTable;
		P_AccountCapitalTable[i]->setObjectName(QStringLiteral("tableView"));
		P_AccountCapitalTable[i]->setMaximumSize(QSize(16777215, 60));
		P_AccountCapitalTable[i]->horizontalHeader()->font();												//������������Ӵ�
		P_AccountCapitalTable[i]->verticalHeader()->hide();													//����б���������
		P_AccountCapitalTable[i]->setStyleSheet(QStringLiteral("background-color: rgb(50, 50, 50);"));		//���ñ�񱳾���ɫ
		P_AccountCapitalTable[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);						//����ڲ��ɱ༭

		P_AccountGridLayout[i]->addWidget(P_AccountCapitalTable[i], 0, 0, 1, 2);
		///ӯ�����ܱ��
		AccountSummaryTable = new QTableView(P_AccountWidget[i]);
		P_AccountSummaryTable[i] = AccountSummaryTable;
		P_AccountSummaryTable[i]->setObjectName(QStringLiteral("tableView"));
		P_AccountSummaryTable[i]->setMaximumSize(QSize(1020, 270));
		P_AccountSummaryTable[i]->horizontalHeader()->font();											//������������Ӵ�
		P_AccountSummaryTable[i]->verticalHeader()->hide();												//����б���������
		P_AccountSummaryTable[i]->setStyleSheet(QStringLiteral("background-color: rgb(50, 50, 50);"));	//���ñ�񱳾���ɫ
		P_AccountSummaryTable[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);						//����ڲ��ɱ༭

		P_AccountGridLayout[i]->addWidget(P_AccountSummaryTable[i], 1, 0, 1, 1);
		///��ͼ
		AccountPlot = new  QCustomPlot(P_AccountWidget[i]);
		P_AccountPlot[i] = AccountPlot;
		P_AccountPlot[i]->setObjectName(QStringLiteral("plotwidget"));
		P_AccountPlot[i]->setBackground(qBrush);
		P_AccountPlot[i]->addGraph(); // blue line
		P_AccountPlot[i]->graph(0)->setPen(QPen(QColor(40, 110, 255)));
		P_AccountGridLayout[i]->addWidget(P_AccountPlot[i], 1, 1, 1, 1);

		///�ɽ��ֲֺ���־
		AccountTab = new QTabWidget(P_AccountWidget[i]);
		P_AccountTab[i] = AccountTab;
		//�ֲ�
		AccountHoldingWidget = new QWidget(AccountTab);
		P_AccountHoldingWidget[i] = AccountHoldingWidget;
		AccountTabHorizontalLayout = new QHBoxLayout(P_AccountHoldingWidget[i]);
		P_AccountTabHorizontalLayout[i] = AccountTabHorizontalLayout;
		AccountHoldingTable = new QTableView(P_AccountHoldingWidget[i]);
		P_AccountHoldingTable[i] = AccountHoldingTable;
		P_AccountHoldingTable[i]->setObjectName(QStringLiteral("tableView"));
		P_AccountHoldingTable[i]->setStyleSheet(QStringLiteral("background-color: rgb(50, 50, 50);"));	//���ñ�񱳾���ɫ
		P_AccountHoldingTable[i]->horizontalHeader()->font();											//������������Ӵ�
		P_AccountHoldingTable[i]->verticalHeader()->hide();												//����б���������	
		P_AccountHoldingTable[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);					//����ڲ��ɱ༭

		P_AccountTabHorizontalLayout[i]->addWidget(P_AccountHoldingTable[i]);
		P_AccountTab[i]->addTab(P_AccountHoldingWidget[i], QString::fromLocal8Bit("�ֲ�"));
		P_AccountTab[i]->raise();
		//�ɽ�
		AccountTradingWidget = new QWidget(AccountTab);
		P_AccountTradingWidget[i] = AccountTradingWidget;
		AccountTabHorizontalLayout = new QHBoxLayout(P_AccountTradingWidget[i]);
		P_AccountTabHorizontalLayout[i] = AccountTabHorizontalLayout;
		AccountTradingTable = new QTableView(P_AccountTradingWidget[i]);
		P_AccountTradingTable[i] = AccountTradingTable;
		P_AccountTradingTable[i]->setObjectName(QStringLiteral("tableView"));
		P_AccountTradingTable[i]->setStyleSheet(QStringLiteral("background-color: rgb(50, 50, 50);"));	//���ñ�񱳾���ɫ
		P_AccountTradingTable[i]->horizontalHeader()->font();											//������������Ӵ�
		P_AccountTradingTable[i]->verticalHeader()->hide();												//����б���������	
		P_AccountTradingTable[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);					//����ڲ��ɱ༭

		P_AccountTabHorizontalLayout[i]->addWidget(P_AccountTradingTable[i]);
		P_AccountTab[i]->addTab(P_AccountTradingWidget[i], QString::fromLocal8Bit("�ɽ�"));
		P_AccountTab[i]->raise();
		//��־
		AccountLogWidget = new QWidget(AccountTab);
		P_AccountLogWidget[i] = AccountLogWidget;
		AccountTabHorizontalLayout = new QHBoxLayout(P_AccountLogWidget[i]);
		P_AccountTabHorizontalLayout[i] = AccountTabHorizontalLayout;
		AccountLogListview = new QListView(P_AccountLogWidget[i]);
		P_AccountLogListview[i] = AccountLogListview;
		P_AccountLogListview[i]->setObjectName(QStringLiteral("tableView"));
		P_AccountLogListview[i]->setStyleSheet(QStringLiteral("background-color: rgb(50, 50, 50);"));	//���ñ�񱳾���ɫ
		P_AccountLogListview[i]->setEditTriggers(QAbstractItemView::NoEditTriggers);					//����ڲ��ɱ༭

		P_AccountTabHorizontalLayout[i]->addWidget(P_AccountLogListview[i]);
		P_AccountTab[i]->addTab(P_AccountLogWidget[i], QString::fromLocal8Bit("��־"));

		P_AccountGridLayout[i]->addWidget(P_AccountTab[i], 2, 0, 1, 2);




		TabWidget->addTab(P_AccountWidget[i], QString::fromLocal8Bit(it->c_str()));
		TabWidget->tabBar()->setTabTextColor(i, QColor(50, 50, 50));						//���ñ��Tabbar��������ɫ������ͬ�˺��б��������ɫ
		i++;
	}
	
}
void QtGuiSurveillanceMulti::Init()
{
	//UI�����м��ҳ����ڲ����ݵĳ�ʼ��
	InitMonitorPage();
	//UI�����и����˻�ҳ�����ڲ����ݵĳ�ʼ��
	InitAccountPages();

	//�趨���ҳ�����˺�ҳ��Ļ�ͼ����
	sizeofplot = 60 * 5.5;//ҹ�̻�ͼX�������ҹ�̽���ʱ��Ϊ21:00:00-�ڶ���02��30:00����5.5��Сʱ
	QDateTime start = QDateTime(QDate(2017, 9, 19), QTime(21, 00, 00));//ҹ�̴�ÿ�������21:00:00��ʼ
	start_time = QTime(21, 00, 00);
	start.setTimeSpec(Qt::UTC);
	startTime = start.toTime_t();


	//sizeofplot = 60 * 6;//���컭ͼX����������콻��ʱ��Ϊ09:00:00-�ڶ���15��00:00����6��Сʱ
	//QDateTime start = QDateTime(QDate(2017, 9, 19), QTime(9, 00, 00));//ҹ�̴�ÿ�������21:00:00��ʼ
	//start_time = QTime(9, 00, 00);
	//start.setTimeSpec(Qt::UTC);
	//startTime = start.toTime_t();
}
void QtGuiSurveillanceMulti::InitMonitorPage()
{
	/// ******************************************************** ���ü��ҳ�ı������***********************************************************///
	MonitorPageStopTableMode = new  QStandardItemModel();
	///*******���˻�ӯ�����ܱ��ĳ�ʼ��***********///
	//����ͷ�趨����ɫ�趨
	MonitorPageSummayTableModel = new QStandardItemModel();
	MonitorPageSummayTableModel->setColumnCount(8);
	MonitorPageSummayTableModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Account"));					//��������1
	MonitorPageSummayTableModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("PnlSettle"));					//��������1
	MonitorPageSummayTableModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("PnlClose"));					//��������2
	MonitorPageSummayTableModel->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("ReturnOnPosition"));			//��������2
	MonitorPageSummayTableModel->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("Position"));					//��������3
	MonitorPageSummayTableModel->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("NetPosition"));				//��������4
	MonitorPageSummayTableModel->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("CommondityPosition"));		//��������5
	MonitorPageSummayTableModel->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("FinancialPosition"));			//��������6
	MonitorPageSummayTableModel->horizontalHeaderItem(0)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(1)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(2)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(3)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(4)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(5)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(6)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	MonitorPageSummayTableModel->horizontalHeaderItem(7)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
	//��������ݵĳ�ʼ��
	for (int i = 0; i<20; i++)
	{
		MonitorPageSummayTableModel->setItem(i, 0, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 1, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 2, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 3, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 4, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 5, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 6, new QStandardItem(NULL));
		MonitorPageSummayTableModel->setItem(i, 7, new QStandardItem(NULL));
	}

	MonitorPageSummayTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);//���ñ��������ʾλ�ã�����
	MonitorPageSummayTable->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(50, 50, 50)}");//���ñ��ⱳ����ɫ
	//���趨�õı�����ݿ��Ʊ���MonitorPageSummayTableModel���뵽�����Ʊ���MonitorPageSummayTable
	MonitorPageSummayTable->setModel(MonitorPageSummayTableModel);
	///*******���˻�ӯ�����ܱ��ĳ�ʼ��***********///


	///*******��ͼ���ֳ�ʼ��***********///
	//��ӯ����̬����ͼ
	//�趨X������Ϊʱ��̶�
	QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
	timeTicker->setDateTimeSpec(Qt::UTC);
	timeTicker->setDateTimeFormat("hh:mm:ss");
	MonitorPageTotalPnLPlot->xAxis->setTicker(timeTicker);


	//����ʵʱӯ����״ͼ
	//�趨X������Ϊ���̶ֿ�
	regen = new QCPBars(MonitorPageKindPnLPlot->xAxis, MonitorPageKindPnLPlot->yAxis);
	QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
	QVector<double> ticks;
	QVector<QString> labels;
	ticks << 1 << 2 << 3 << 4 << 5 << 6;
	labels << "Agriculture" << "Black" << "BaseMetal" << "Energy" << "PreciousMetal" << "Financial";
	textTicker->addTicks(ticks, labels);
	MonitorPageKindPnLPlot->xAxis->setTicker(textTicker);

	//�˻���λ��״ͼ
	Agriculture = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	Black = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	BaseMetal = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	Energy = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	PreciousMetal = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	Financial = new QCPBars(MonitorPagePositionPlot->xAxis, MonitorPagePositionPlot->yAxis);
	QSharedPointer<QCPAxisTickerText> positiontextTicker(new QCPAxisTickerText);
	QVector<QString> positionlabels; QVector<double> positionticks;
	int i = 1;
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{
		positionticks << i;
		positionlabels << QString::fromLocal8Bit(its->c_str());
		i++;
	}
	positiontextTicker->addTicks(positionticks, positionlabels);
	MonitorPagePositionPlot->xAxis->setTicker(positiontextTicker);
	///*******��ͼ���ֳ�ʼ��***********///
}
void QtGuiSurveillanceMulti::InitAccountPages()
{
	/// ******************************************************** ����ÿ���˻��ı������***********************************************************///
	//�����˻�������Ϊ�������б�Ŀ��Ʊ�������̬�����ڴ�
	tablemodel_position = new QStandardItemModel[num_accounts];
	tablemodel_trade = new QStandardItemModel[num_accounts];
	tablemodel_summary = new QStandardItemModel[num_accounts];
	tablemodel_account = new QStandardItemModel[num_accounts];
	listviewmodel = new QStringListModel[num_accounts];
	//���ڶԱ�����ݵ��趨��ֻ����ָ���������������Ҫ������Ӧ��ָ�����飬�������ݵ��趨
	p_tablemodel_position = new  QStandardItemModel*[num_accounts];
	p_tablemodel_trade = new  QStandardItemModel*[num_accounts];
	p_tablemodel_summary = new  QStandardItemModel*[num_accounts];
	p_tablemodel_account = new  QStandardItemModel*[num_accounts];
	p_listviewmodel = new QStringListModel*[num_accounts];
	//�����˻������ֱ���г�ʼ��
	int k = 0;
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{
		
		p_tablemodel_position[k] = &tablemodel_position[k];
		//�趨�ֲ����ݱ��ı�ͷ����ɫ��
		p_tablemodel_position[k]->setColumnCount(19);																//����������
		p_tablemodel_position[k]->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("ID"));					//��������1
		p_tablemodel_position[k]->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("KIND"));					//��������2
		p_tablemodel_position[k]->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("NumPosition"));			//��������3
		p_tablemodel_position[k]->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("YesNumPosition"));		//��������3
		p_tablemodel_position[k]->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("TodayNumPosition"));		//��������3
		p_tablemodel_position[k]->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("LastPrice"));				//��������4
		p_tablemodel_position[k]->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("T-1CLose"));				//��������5
		p_tablemodel_position[k]->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("T-1Settle"));				//��������6
		p_tablemodel_position[k]->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("Multiplie"));				//��������7
		p_tablemodel_position[k]->setHeaderData(9, Qt::Horizontal, QString::fromLocal8Bit("Position"));				//��������8
		p_tablemodel_position[k]->setHeaderData(10, Qt::Horizontal, QString::fromLocal8Bit("NetPosition"));			//��������9
		p_tablemodel_position[k]->setHeaderData(11, Qt::Horizontal, QString::fromLocal8Bit("PnlSettle"));			//��������14
		p_tablemodel_position[k]->setHeaderData(12, Qt::Horizontal, QString::fromLocal8Bit("PnlClose"));				//��������15
		p_tablemodel_position[k]->setHeaderData(13, Qt::Horizontal, QString::fromLocal8Bit("HoldingPnlSettle"));		//��������10
		p_tablemodel_position[k]->setHeaderData(14, Qt::Horizontal, QString::fromLocal8Bit("HoldingPnlClose"));		//��������11
		p_tablemodel_position[k]->setHeaderData(15, Qt::Horizontal, QString::fromLocal8Bit("TradingPnlSettle"));		//��������12
		p_tablemodel_position[k]->setHeaderData(16, Qt::Horizontal, QString::fromLocal8Bit("TradingPnlClose"));		//��������13
		p_tablemodel_position[k]->setHeaderData(17, Qt::Horizontal, QString::fromLocal8Bit("PctSettle"));			//��������16
		p_tablemodel_position[k]->setHeaderData(18, Qt::Horizontal, QString::fromLocal8Bit("PctClose"));				//��������17
		p_tablemodel_position[k]->horizontalHeaderItem(0)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(1)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(2)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(3)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(4)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(5)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(6)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(7)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(8)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(9)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(10)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(11)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(12)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(13)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(14)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(15)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(16)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(17)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_position[k]->horizontalHeaderItem(18)->setForeground(QBrush(QColor(color_white, color_white, color_white)));

		
		p_tablemodel_trade[k] = &tablemodel_trade[k];
		//���ý��׼�¼���ı�ͷ����ɫ��
		p_tablemodel_trade[k]->setColumnCount(5);													//����������
		p_tablemodel_trade[k]->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("��Լ����"));	//��������1
		p_tablemodel_trade[k]->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("��������"));	//��������2
		p_tablemodel_trade[k]->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("��ƽ��־"));	//��������3
		p_tablemodel_trade[k]->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("�ɽ�����"));	//��������4
		p_tablemodel_trade[k]->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("�ɽ�����"));	//��������5
		p_tablemodel_trade[k]->horizontalHeaderItem(0)->setForeground(QBrush(QColor(200, 200, 200))); //��������1��������ɫ
		p_tablemodel_trade[k]->horizontalHeaderItem(1)->setForeground(QBrush(QColor(200, 200, 200))); //��������2��������ɫ
		p_tablemodel_trade[k]->horizontalHeaderItem(2)->setForeground(QBrush(QColor(200, 200, 200))); //��������3��������ɫ
		p_tablemodel_trade[k]->horizontalHeaderItem(3)->setForeground(QBrush(QColor(200, 200, 200))); //��������4��������ɫ
		p_tablemodel_trade[k]->horizontalHeaderItem(4)->setForeground(QBrush(QColor(200, 200, 200))); //��������5��������ɫ

																									 
		p_tablemodel_summary[k] = &tablemodel_summary[k];
		//���÷���ͳ�Ʊ��ı�ͷ����ɫ��
		p_tablemodel_summary[k]->setColumnCount(10);
		p_tablemodel_summary[k]->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Kind"));		//��������1
		p_tablemodel_summary[k]->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("PnlSettle"));		//��������1
		p_tablemodel_summary[k]->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("PnlClose"));		//��������2
		p_tablemodel_summary[k]->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("HoldingPnlSettle"));		//��������1
		p_tablemodel_summary[k]->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("HoldingPnlClose"));		//��������2
		p_tablemodel_summary[k]->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("TradingPnlSettle"));		//��������1
		p_tablemodel_summary[k]->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("TradingPnlClose"));		//��������2
		p_tablemodel_summary[k]->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("Position"));		//��������3
		p_tablemodel_summary[k]->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("NetPosition"));	//��������4
		p_tablemodel_summary[k]->setHeaderData(9, Qt::Horizontal, QString::fromLocal8Bit("ReturnOnPosition"));	//��������5
		p_tablemodel_summary[k]->horizontalHeaderItem(0)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(1)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(2)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(3)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(4)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(5)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(6)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(7)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(8)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_summary[k]->horizontalHeaderItem(9)->setForeground(QBrush(QColor(color_white, color_white, color_white)));


		p_tablemodel_account[k] = &tablemodel_account[k];
		//�����˻��ʽ���Ϣҳ�ı�ͷ����ɫ��
		p_tablemodel_account[k]->setColumnCount(7);
		p_tablemodel_account[k]->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("��̬Ȩ��"));	//��������1
		p_tablemodel_account[k]->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("ƽ��ӯ��"));	//��������1
		p_tablemodel_account[k]->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("�ֲ�ӯ��"));	//��������1
		p_tablemodel_account[k]->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("��̬Ȩ��"));	//��������1
		p_tablemodel_account[k]->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("ռ�ñ�֤��"));	//��������1
		p_tablemodel_account[k]->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("�����ʽ�"));	//��������1
		p_tablemodel_account[k]->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("���ն�"));		//��������1
		p_tablemodel_account[k]->horizontalHeaderItem(0)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(1)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(2)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(3)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(4)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(5)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->horizontalHeaderItem(6)->setForeground(QBrush(QColor(color_white, color_white, color_white)));
		p_tablemodel_account[k]->setItem(0, 0, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 1, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 2, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 3, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 4, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 5, new QStandardItem(NULL));
		p_tablemodel_account[k]->setItem(0, 6, new QStandardItem(NULL));
		//��ʼ������ͳ�Ʊ��
		for (int i = 0; i<8; i++)
		{
			p_tablemodel_summary[k]->setItem(i, 0, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 1, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 2, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 3, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 4, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 5, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 6, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 7, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 8, new QStandardItem(NULL));
			p_tablemodel_summary[k]->setItem(i, 9, new QStandardItem(NULL));
		}
		//��ʼ���ֲֺͽ���ҳ���
		for (int i = 0; i<60; i++)
		{
			p_tablemodel_position[k]->setItem(i, 0, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 1, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 2, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 3, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 4, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 5, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 6, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 7, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 8, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 9, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 10, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 11, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 12, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 13, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 14, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 15, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 16, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 17, new QStandardItem(NULL));
			p_tablemodel_position[k]->setItem(i, 18, new QStandardItem(NULL));

			p_tablemodel_trade[k]->setItem(i, 0, new QStandardItem(NULL));
			p_tablemodel_trade[k]->setItem(i, 1, new QStandardItem(NULL));
			p_tablemodel_trade[k]->setItem(i, 2, new QStandardItem(NULL));
			p_tablemodel_trade[k]->setItem(i, 3, new QStandardItem(NULL));
			p_tablemodel_trade[k]->setItem(i, 4, new QStandardItem(NULL));
		}

		P_AccountHoldingTable[k]->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
		P_AccountHoldingTable[k]->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(50, 50, 50)}");
		P_AccountHoldingTable[k]->setModel(p_tablemodel_position[k]);

		P_AccountTradingTable[k]->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
		P_AccountTradingTable[k]->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(50, 50, 50)}");//���ñ��ⱳ����ɫ
		P_AccountTradingTable[k]->setModel(p_tablemodel_trade[k]);

		P_AccountSummaryTable[k]->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
		P_AccountSummaryTable[k]->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(50, 50, 50)}");//���ñ��ⱳ����ɫ
		P_AccountSummaryTable[k]->setModel(p_tablemodel_summary[k]);

		P_AccountCapitalTable[k]->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
		P_AccountCapitalTable[k]->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:rgb(50, 50, 50)}");//���ñ��ⱳ����ɫ
		P_AccountCapitalTable[k]->setModel(p_tablemodel_account[k]);


		/// ��ͼ��ʼ��
		QBrush qBrush(QColor(50, 50, 50));//���ñ���ɫ  
		P_AccountPlot[k]->setBackground(qBrush);
		P_AccountPlot[k]->addGraph(); // blue line
		P_AccountPlot[k]->graph(0)->setPen(QPen(QColor(40, 110, 255)));

		QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
		timeTicker->setDateTimeSpec(Qt::UTC);
		timeTicker->setDateTimeFormat("hh:mm:ss");
		P_AccountPlot[k]->xAxis->setTicker(timeTicker);
		n = 0;
		//��ʼ����־�б�ָ�����
		p_listviewmodel[k] = &listviewmodel[k];
		k++;

	}
}
void QtGuiSurveillanceMulti::CalculateSummary()
{
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{
		Display[*its].bIsEmpty=Display[*its].position1.empty();
		if (Display[*its].hEventUi)
		{
			std::string list_kind[8] = { "Total","Commondity","Financial","Agriculture","Black","Energy","BaseMetal","PreciousMetal" };
			for (int i = 0; i <= 7; i++)
			{
				Display_Summary s;

				if (strcmp(list_kind[i].c_str(), "Total") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						s.NetPosition += it->second.NetPosition;
						s.PnlClose += it->second.PnlClose;
						s.PnlSettle += it->second.PnlSettle;
						s.Position += it->second.Position;
						s.HoldingPnlClose += it->second.HoldingPnlClose;
						s.HoldingPnlSettle += it->second.HoldingPnlSettle;
						s.TradingPnlClose += it->second.TradingPnlClose;
						s.TradingPnlSettle += it->second.TradingPnlSettle;
					}


					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "Financial") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "Financial") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "Agriculture") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "Agriculture") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "Black") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "Black") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "Energy") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "Energy") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "BaseMetal") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "BaseMetal") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "PreciousMetal") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;
					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "PreciousMetal") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
				else if (strcmp(list_kind[i].c_str(), "Commondity") == 0)
				{
					s.NetPosition = 0;
					s.PnlClose = 0;
					s.PnlSettle = 0;
					s.Position = 0;
					s.HoldingPnlClose = 0;
					s.HoldingPnlSettle = 0;
					s.TradingPnlClose = 0;
					s.TradingPnlSettle = 0;
					s.ReturnOnPosition = 0;

					for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
					{
						if (strcmp(it->second.KIND.c_str(), "Agriculture") == 0
							|| strcmp(it->second.KIND.c_str(), "Black") == 0
							|| strcmp(it->second.KIND.c_str(), "Energy") == 0
							|| strcmp(it->second.KIND.c_str(), "BaseMetal") == 0
							|| strcmp(it->second.KIND.c_str(), "PreciousMetal") == 0)
						{
							s.NetPosition += it->second.NetPosition;
							s.PnlClose += it->second.PnlClose;
							s.PnlSettle += it->second.PnlSettle;
							s.Position += it->second.Position;
							s.HoldingPnlClose += it->second.HoldingPnlClose;
							s.HoldingPnlSettle += it->second.HoldingPnlSettle;
							s.TradingPnlClose += it->second.TradingPnlClose;
							s.TradingPnlSettle += it->second.TradingPnlSettle;
						}

					}
					s.ReturnOnPosition = s.PnlClose / s.Position;	//�������Ĳ�λ�ر���Ϊ�����̼ۼ����ӯ��/��ǰ��λ��
					Display[*its].summary.insert({ list_kind[i] ,s });
					Display[*its].summary[list_kind[i]] = s;
				}
			}
			///��̬��ɼ�����
			//�ֲ�ӯ��
			Display[*its].account.CloseProfit = Display[*its].summary["Total"].TradingPnlSettle;
			Display[*its].account.PositionProfit = Display[*its].summary["Total"].HoldingPnlSettle;
			//ƽ��ӯ��
			//��̬Ȩ�棬��̬Ȩ��=��̬Ȩ��+�ֲ�ӯ��+ƽ��ӯ��
			Display[*its].account.DynamicEquity = Display[*its].account.StaticEquity + Display[*its].account.PositionProfit + Display[*its].account.CloseProfit;
			//�����ʽ𣬿����ʽ�=��̬Ȩ��-��ǰռ�ñ�֤��
			Display[*its].account.AvailableAmount = Display[*its].account.DynamicEquity - Display[*its].account.CurrMargin;
			//���նȣ����ն�=ռ�ñ�֤��/��̬Ȩ��
			Display[*its].account.RiskDegree = Display[*its].account.CurrMargin / Display[*its].account.DynamicEquity;
		}
	}
}
void QtGuiSurveillanceMulti::timerEvent(QTimerEvent * event)
{
	// �����ж����ʱ����ÿ���Ķ�ʱ���в�ͬ�Ĵ���
	if (event->timerId() == m_timerId)
	{
		//��ȡ��ǰʱ�����
		QTime now = QTime::currentTime();
		now_min = QTime(now.hour(), now.minute(), 0, 0);
		

		//����ͳ����Ϣ
		CalculateSummary();
		//���»���ҳ��Ϣ
		UpdateMonitorPage();

		//���¸����˻���Ϣ
		UpdateAccountPages();
		
		//��ʱ�رճ��򣬱����¼ʧ�����δ֪crash
		if (now.hour() == 2 && now.minute() == 40)
			this->close();//��ʱ�رճ���
	 }
}
void QtGuiSurveillanceMulti::UpdateMonitorPage() 
{
	/// ******************************************************** ���ü��ҳ�ı������***********************************************************///
	//SummayTable����
	int i = 1;
	total_monitor = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };
	QString temp;
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{

		MonitorPageSummayTableModel->item(i, 0)->setText(QString::fromLocal8Bit(its->c_str()));//��Ʒ����
		MonitorPageSummayTableModel->item(i, 1)->setText(QString("%L1").arg(Display[*its].summary["Total"].PnlSettle, 0, 'f', 0));
		MonitorPageSummayTableModel->item(i, 2)->setText(QString("%L1").arg(Display[*its].summary["Total"].PnlClose, 0, 'f', 0));
		temp.sprintf("%0.2lf%%", Display[*its].summary["Total"].PnlClose / double(Display[*its].summary["Total"].Position) * 100);
		MonitorPageSummayTableModel->item(i, 3)->setText(temp);
		MonitorPageSummayTableModel->item(i, 4)->setText(QString("%L1").arg(Display[*its].summary["Total"].Position, 0, 'f', 0));
		MonitorPageSummayTableModel->item(i, 5)->setText(QString("%L1").arg(Display[*its].summary["Total"].NetPosition, 0, 'f', 0));
		MonitorPageSummayTableModel->item(i, 6)->setText(QString("%L1").arg(Display[*its].summary["Commondity"].Position, 0, 'f', 0));
		MonitorPageSummayTableModel->item(i, 7)->setText(QString("%L1").arg(Display[*its].summary["Financial"].Position, 0, 'f', 0));
		MonitorPageSummayTableModel->item(i, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 3)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 4)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 5)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 6)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
		MonitorPageSummayTableModel->item(i, 7)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ

		if (Display[*its].summary["Total"].PnlSettle > 0)
			MonitorPageSummayTableModel->item(i, 1)->setForeground(QBrush(QColor(255, 0, 0)));
		else if (Display[*its].summary["Total"].PnlSettle < 0)
			MonitorPageSummayTableModel->item(i, 1)->setForeground(QBrush(QColor(0, 255, 0)));
		else
			MonitorPageSummayTableModel->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));

		if (Display[*its].summary["Total"].PnlClose > 0)
			MonitorPageSummayTableModel->item(i, 2)->setForeground(QBrush(QColor(255, 0, 0)));
		else if (Display[*its].summary["Total"].PnlClose < 0)
			MonitorPageSummayTableModel->item(i, 2)->setForeground(QBrush(QColor(0, 255, 0)));
		else
			MonitorPageSummayTableModel->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));
		//summarytable ��������
		total_monitor.PnlClose += Display[*its].summary["Total"].PnlClose;
		total_monitor.PnlSettle += Display[*its].summary["Total"].PnlSettle;
		total_monitor.Position += Display[*its].summary["Total"].Position;
		total_monitor.NetPosition += Display[*its].summary["Total"].NetPosition;
		total_monitor.CommondityPosition += Display[*its].summary["Commondity"].Position;
		total_monitor.FinancialPosition += Display[*its].summary["Financial"].Position;

		//plotbar��������
		total_monitor.PnlAgriculture += Display[*its].summary["Agriculture"].PnlClose;
		total_monitor.PnlBlack += Display[*its].summary["Black"].PnlClose;
		total_monitor.PnlEnergy += Display[*its].summary["Energy"].PnlClose;
		total_monitor.PnlBaseMetal += Display[*its].summary["BaseMetal"].PnlClose;
		total_monitor.PnlPreciousMetal += Display[*its].summary["PreciousMetal"].PnlClose;
		total_monitor.PnlFinancial += Display[*its].summary["Financial"].PnlClose;
		i++;
	}
	MonitorPageSummayTableModel->item(0, 0)->setText(QString::fromLocal8Bit("�ϼ�"));//��Ʒ����
	MonitorPageSummayTableModel->item(0, 1)->setText(QString("%L1").arg(total_monitor.PnlSettle, 0, 'f', 0));
	MonitorPageSummayTableModel->item(0, 2)->setText(QString("%L1").arg(total_monitor.PnlClose, 0, 'f', 0));
	temp.sprintf("%0.2lf%%", total_monitor.PnlClose / double(total_monitor.Position) * 100);
	MonitorPageSummayTableModel->item(0, 3)->setText(temp);
	MonitorPageSummayTableModel->item(0, 4)->setText(QString("%L1").arg(total_monitor.Position, 0, 'f', 0));
	MonitorPageSummayTableModel->item(0, 5)->setText(QString("%L1").arg(total_monitor.NetPosition, 0, 'f', 0));
	MonitorPageSummayTableModel->item(0, 6)->setText(QString("%L1").arg(total_monitor.CommondityPosition, 0, 'f', 0));
	MonitorPageSummayTableModel->item(0, 7)->setText(QString("%L1").arg(total_monitor.FinancialPosition, 0, 'f', 0));
	MonitorPageSummayTableModel->item(0, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 1)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 2)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 3)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 4)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 5)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 6)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	MonitorPageSummayTableModel->item(0, 7)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
	//�� �������̼ۼ������ӯ�� �� ���ս���ۼ������ӯ��������ʾ���ݣ�����������ʾ��ͬ��ɫ
	if (total_monitor.PnlSettle > 0)
		MonitorPageSummayTableModel->item(0, 1)->setForeground(QBrush(QColor(255, 0, 0)));
	else if (total_monitor.PnlSettle < 0)
		MonitorPageSummayTableModel->item(0, 1)->setForeground(QBrush(QColor(0, 255, 0)));
	else
		MonitorPageSummayTableModel->item(0, 1)->setForeground(QBrush(QColor(200, 200, 200)));

	if (total_monitor.PnlClose > 0)
		MonitorPageSummayTableModel->item(0, 2)->setForeground(QBrush(QColor(255, 0, 0)));
	else if (total_monitor.PnlClose < 0)
		MonitorPageSummayTableModel->item(0, 2)->setForeground(QBrush(QColor(0, 255, 0)));
	else
		MonitorPageSummayTableModel->item(0, 2)->setForeground(QBrush(QColor(200, 200, 200)));
	//���������̼ۼ��������ӯ���Ӵ�
	QFont font1("Black", 10, 75);
	MonitorPageSummayTableModel->item(0, 1)->setFont(font1);
	MonitorPageSummayTableModel->item(0, 2)->setFont(font1);

	MonitorPageSummayTable->setModel(MonitorPageSummayTableModel);

	
	plotdata[now_min] = total_monitor.PnlClose;
	

	//��ӯ������ͼ����
	UpdateMonitorPageTotalPnLPlot();


	//ʵʱ����ӯ��
	UpdateMonitorPageKindPnLPlot();


	//��Ʒ��λ��״ͼ
	UpdateMonitorPagePositionPlot();
}
void QtGuiSurveillanceMulti::UpdateMonitorPageTotalPnLPlot()
{
	//��ӯ������ͼ����
	
	QVector<double> xx(sizeofplot+1), yy(sizeofplot+1);
	QVector<QTime>zz(sizeofplot+1);
	for (int i = 0; i < sizeofplot; ++i)
	{
		xx[i] = startTime + i * 60;
		zz[i] = start_time.addSecs(i * 60);
	}
	double max_yy = 0;
	double min_yy = 0;
	for (int i = 0; i < sizeofplot; ++i)
	{

		yy[i] = plotdata[zz[i]];
		if (yy[i] > max_yy)
			max_yy = yy[i];
		else if (yy[i] < min_yy)
			min_yy = yy[i];
	}
	MonitorPageTotalPnLPlot->graph(0)->setData(xx, yy);
	MonitorPageTotalPnLPlot->xAxis->setRange(xx[0], xx[sizeofplot-1]);
	MonitorPageTotalPnLPlot->yAxis->setRange(min_yy, max_yy);
	MonitorPageTotalPnLPlot->xAxis->setTickLabelColor(Qt::white);
	MonitorPageTotalPnLPlot->xAxis->setLabelColor(Qt::white);
	MonitorPageTotalPnLPlot->yAxis->setTickLabelColor(Qt::white);
	MonitorPageTotalPnLPlot->yAxis->setLabelColor(Qt::white);
	MonitorPageTotalPnLPlot->graph(0)->setPen(QPen(QColor(70, 110, 150, 200))); // line color blue for first graph
	MonitorPageTotalPnLPlot->graph(0)->setBrush(QBrush(QColor(20 + 200 / 4.0 * 1, 70 * (1.6 - 1 / 4.0), 150, 150))); // first graph will be filled with translucent blue
	MonitorPageTotalPnLPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
	MonitorPageTotalPnLPlot->replot();
}
void QtGuiSurveillanceMulti::UpdateMonitorPageKindPnLPlot()
{
	//ʵʱ����ӯ��
	regen->setName("Regenerative");
	regen->setPen(QPen(QColor(0, 168, 255, 150)));
	regen->setBrush(QColor(0, 168, 255, 150));
	// prepare x axis with country labels:
	MonitorPageKindPnLPlot->xAxis->setTickLabelRotation(60);
	MonitorPageKindPnLPlot->xAxis->setSubTicks(false);
	MonitorPageKindPnLPlot->xAxis->setTickLength(0, 4);
	MonitorPageKindPnLPlot->xAxis->setRange(0, 7);
	MonitorPageKindPnLPlot->xAxis->setBasePen(QPen(Qt::white));
	MonitorPageKindPnLPlot->xAxis->setTickPen(QPen(Qt::white));
	MonitorPageKindPnLPlot->xAxis->grid()->setVisible(false);
	MonitorPageKindPnLPlot->xAxis->setTickLabelColor(Qt::white);
	MonitorPageKindPnLPlot->xAxis->setLabelColor(Qt::white);
	// prepare y axis:
	double max_bar_y = 0, min_bar_y = 0;
	max_bar_y = max(max_bar_y, total_monitor.PnlAgriculture); min_bar_y = min(min_bar_y, total_monitor.PnlAgriculture);
	max_bar_y = max(max_bar_y, total_monitor.PnlBlack); min_bar_y = min(min_bar_y, total_monitor.PnlBlack);
	max_bar_y = max(max_bar_y, total_monitor.PnlBaseMetal); min_bar_y = min(min_bar_y, total_monitor.PnlBaseMetal);
	max_bar_y = max(max_bar_y, total_monitor.PnlEnergy); min_bar_y = min(min_bar_y, total_monitor.PnlEnergy);
	max_bar_y = max(max_bar_y, total_monitor.PnlPreciousMetal); min_bar_y = min(min_bar_y, total_monitor.PnlPreciousMetal);
	max_bar_y = max(max_bar_y, total_monitor.PnlFinancial); min_bar_y = min(min_bar_y, total_monitor.PnlFinancial);
	MonitorPageKindPnLPlot->yAxis->setRange(min_bar_y*1.2, max_bar_y*1.2);
	MonitorPageKindPnLPlot->yAxis->setPadding(5); // a bit more space to the left border
	MonitorPageKindPnLPlot->yAxis->setBasePen(QPen(Qt::white));
	MonitorPageKindPnLPlot->yAxis->setTickPen(QPen(Qt::white));
	MonitorPageKindPnLPlot->yAxis->setSubTickPen(QPen(Qt::white));
	MonitorPageKindPnLPlot->yAxis->grid()->setSubGridVisible(false);
	MonitorPageKindPnLPlot->yAxis->setTickLabelColor(Qt::white);
	MonitorPageKindPnLPlot->yAxis->setLabelColor(Qt::white);
	// Add data:
	QVector<double>  regenData;
	QVector<double> ticks;
	ticks << 1 << 2 << 3 << 4 << 5 << 6;
	regenData << total_monitor.PnlAgriculture << total_monitor.PnlBlack << total_monitor.PnlBaseMetal << total_monitor.PnlEnergy << total_monitor.PnlPreciousMetal << total_monitor.PnlFinancial;
	regen->setData(ticks, regenData);
	MonitorPageKindPnLPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	MonitorPageKindPnLPlot->replot();
}
void QtGuiSurveillanceMulti::UpdateMonitorPagePositionPlot()
{
	//��Ʒ��λ��״ͼ
	Agriculture->moveAbove(Financial);
	Black->moveAbove(Agriculture);
	BaseMetal->moveAbove(Black);
	Energy->moveAbove(BaseMetal);
	PreciousMetal->moveAbove(Energy);
	QColor FinancialColor = QColor(30, 144, 200, 200);
	QColor AgriculturelColor = QColor(84, 255, 159, 150);
	QColor BlackColor = QColor(255, 246, 143, 150);
	QColor BaseMetalColor = QColor(255, 106, 106, 150);
	QColor EnergyColor = QColor(255, 20, 147, 150);
	QColor PreciousMetalColor = QColor(224, 102, 255, 150);
	Financial->setName("Financial");
	Financial->setPen(QPen(FinancialColor));
	Financial->setBrush(FinancialColor);
	Agriculture->setName("Agriculture");
	Agriculture->setPen(QPen(AgriculturelColor));
	Agriculture->setBrush(AgriculturelColor);
	Black->setName("Black");
	Black->setPen(QPen(BlackColor));
	Black->setBrush(BlackColor);
	BaseMetal->setName("BaseMetal");
	BaseMetal->setPen(QPen(BaseMetalColor));
	BaseMetal->setBrush(BaseMetalColor);
	Energy->setName("Energy");
	Energy->setPen(QPen(EnergyColor));
	Energy->setBrush(EnergyColor);
	PreciousMetal->setName("PreciousMetal");
	PreciousMetal->setPen(QPen(PreciousMetalColor));
	PreciousMetal->setBrush(PreciousMetalColor);

	// Add data:
	QVector<double> AgricultureData, BlackData, BaseMetalData, EnergyData, PreciousMetalData, FinancialData;
	QVector<QString> positionlabels; QVector<double> positionticks;
	double max_bar_yy = 0, min_bar_yy = 0;
	int i = 1;
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{
		AgricultureData << Display[*its].summary["Agriculture"].Position;
		BlackData << Display[*its].summary["Black"].Position;
		BaseMetalData << Display[*its].summary["BaseMetal"].Position;
		PreciousMetalData << Display[*its].summary["PreciousMetal"].Position;
		EnergyData << Display[*its].summary["Energy"].Position;
		FinancialData << Display[*its].summary["Financial"].Position;
		max_bar_yy = max(max_bar_yy, Display[*its].summary["Total"].Position);
		positionticks << i;
		positionlabels << QString::fromLocal8Bit(its->c_str());
		i++;
	}
	Agriculture->setData(positionticks, AgricultureData);
	Black->setData(positionticks, BlackData);
	BaseMetal->setData(positionticks, BaseMetalData);
	Energy->setData(positionticks, EnergyData);
	PreciousMetal->setData(positionticks, PreciousMetalData);
	Financial->setData(positionticks, FinancialData);

	// prepare x axis with country labels:

	MonitorPagePositionPlot->xAxis->setTickLabelRotation(60);
	MonitorPagePositionPlot->xAxis->setSubTicks(false);
	MonitorPagePositionPlot->xAxis->setTickLength(0, 4);
	MonitorPagePositionPlot->xAxis->setRange(0, accounts.size() + 2);
	MonitorPagePositionPlot->xAxis->setBasePen(QPen(Qt::white));
	MonitorPagePositionPlot->xAxis->setTickPen(QPen(Qt::white));
	MonitorPagePositionPlot->xAxis->grid()->setVisible(false);
	MonitorPagePositionPlot->xAxis->setTickLabelColor(Qt::white);
	MonitorPagePositionPlot->xAxis->setLabelColor(Qt::white);
	// prepare y axis:
	MonitorPagePositionPlot->yAxis->setRange(min_bar_yy, max_bar_yy*1.2);
	MonitorPagePositionPlot->yAxis->setPadding(5); // a bit more space to the left border
	MonitorPagePositionPlot->yAxis->setBasePen(QPen(Qt::white));
	MonitorPagePositionPlot->yAxis->setTickPen(QPen(Qt::white));
	MonitorPagePositionPlot->yAxis->setSubTickPen(QPen(Qt::white));
	MonitorPagePositionPlot->yAxis->grid()->setSubGridVisible(false);
	MonitorPagePositionPlot->yAxis->setTickLabelColor(Qt::white);
	MonitorPagePositionPlot->yAxis->setLabelColor(Qt::white);
	MonitorPagePositionPlot->legend->setVisible(true);
	MonitorPagePositionPlot->legend->setBrush(QColor(50, 50, 50));
	MonitorPagePositionPlot->legend->setBorderPen(Qt::NoPen);
	QFont legendFont = font();
	legendFont.setPointSize(8);
	MonitorPagePositionPlot->legend->setFont(legendFont);
	MonitorPagePositionPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
	MonitorPagePositionPlot->replot();
}
void QtGuiSurveillanceMulti::UpdateAccountPages()
{
	/// ******************************************************** �����˺ŵı������***********************************************************///
	int k = 0;
	for (list<string>::iterator its = accounts.begin(); its != accounts.end(); its++)
	{
		
		Display[*its].plotdata[now_min] = Display[*its].summary["Total"].PnlClose;

		if (Display[*its].hEventUi)//���ֲ���Ϣ��ѯ���ʱ�ٽ��и�����ʾ
		{
			///���³ֱֲ�����ʾ����
			int i = 0;
			for (auto it = Display[*its].position1.begin(); it != Display[*its].position1.end(); it++)
			{
				
				p_tablemodel_position[k]->item(i, 0)->setText(QString::fromStdString(it->first.ID));											//ID
				p_tablemodel_position[k]->item(i, 1)->setText(QString::fromStdString(Display[*its].position1[it->first].KIND));					//KIND
				p_tablemodel_position[k]->item(i, 2)->setText(QString::number(Display[*its].position1[it->first].NumPosition));					//NumPosition
				p_tablemodel_position[k]->item(i, 3)->setText(QString::number(Display[*its].position1[it->first].YesNumPosition));				//NumPosition
				p_tablemodel_position[k]->item(i, 4)->setText(QString::number(Display[*its].position1[it->first].TodayNumPosition));			//NumPosition
				p_tablemodel_position[k]->item(i, 5)->setText(QString::number(Display[*its].position1[it->first].LastPrice));
				p_tablemodel_position[k]->item(i, 6)->setText(QString::number(Display[*its].position1[it->first].T1CLose));
				p_tablemodel_position[k]->item(i, 7)->setText(QString::number(Display[*its].position1[it->first].T1Settle));
				p_tablemodel_position[k]->item(i, 8)->setText(QString::number(Display[*its].position1[it->first].Multiplie));
				p_tablemodel_position[k]->item(i, 9)->setText(QString("%L1").arg(double(Display[*its].position1[it->first].Position), 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 10)->setText(QString("%L1").arg(double(Display[*its].position1[it->first].NetPosition), 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 11)->setText(QString("%L1").arg(Display[*its].position1[it->first].PnlSettle, 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 12)->setText(QString("%L1").arg(Display[*its].position1[it->first].PnlClose, 0, 'f', 0));

				p_tablemodel_position[k]->item(i, 13)->setText(QString("%L1").arg(Display[*its].position1[it->first].HoldingPnlSettle, 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 14)->setText(QString("%L1").arg(Display[*its].position1[it->first].HoldingPnlClose, 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 15)->setText(QString("%L1").arg(Display[*its].position1[it->first].TradingPnlSettle, 0, 'f', 0));
				p_tablemodel_position[k]->item(i, 16)->setText(QString("%L1").arg(Display[*its].position1[it->first].TradingPnlClose, 0, 'f', 0));
				QString pctsettle; pctsettle.sprintf("%.2lf%%", Display[*its].position1[it->first].PctSettle * 100);
				p_tablemodel_position[k]->item(i, 17)->setText(pctsettle);
				QString pctclose; pctclose.sprintf("%.2lf%%", Display[*its].position1[it->first].PctClose * 100);
				p_tablemodel_position[k]->item(i, 18)->setText(pctclose);
				p_tablemodel_position[k]->item(i, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
				p_tablemodel_position[k]->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 3)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 4)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 5)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 6)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 7)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 8)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 9)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 10)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 11)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 12)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 13)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 14)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 15)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 16)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 17)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_position[k]->item(i, 18)->setForeground(QBrush(QColor(200, 200, 200)));
				// �������̼�ӯ����ɫ��
				if (Display[*its].position1[it->first].PnlSettle > 0)
					p_tablemodel_position[k]->item(i, 11)->setForeground(QBrush(QColor(255, 0, 0)));
				else if (Display[*its].position1[it->first].PnlSettle < 0)
					p_tablemodel_position[k]->item(i, 11)->setForeground(QBrush(QColor(0, 255, 0)));
				else
					p_tablemodel_position[k]->item(i, 11)->setForeground(QBrush(QColor(200, 200, 200)));
				// ���ý����ӯ����ɫ��
				if (Display[*its].position1[it->first].PnlClose > 0)
					p_tablemodel_position[k]->item(i, 12)->setForeground(QBrush(QColor(255, 0, 0)));
				else if (Display[*its].position1[it->first].PnlClose < 0)
					p_tablemodel_position[k]->item(i, 12)->setForeground(QBrush(QColor(0, 255, 0)));
				else
					p_tablemodel_position[k]->item(i, 12)->setForeground(QBrush(QColor(200, 200, 200)));

				p_tablemodel_position[k]->item(i, 0)->setTextAlignment(Qt::AlignCenter);//�����ַ�λ��
				i++;
			}
			///���»��ܱ�����ʾ����
			i = 0;
			for (auto it = Display[*its].summary.begin(); it != Display[*its].summary.end(); it++)
			{
				p_tablemodel_summary[k]->item(i, 0)->setText(QString::fromStdString(it->first));
				p_tablemodel_summary[k]->item(i, 1)->setText(QString("%L1").arg(Display[*its].summary[it->first].PnlSettle, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 2)->setText(QString("%L1").arg(Display[*its].summary[it->first].PnlClose, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 3)->setText(QString("%L1").arg(Display[*its].summary[it->first].HoldingPnlSettle, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 4)->setText(QString("%L1").arg(Display[*its].summary[it->first].HoldingPnlClose, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 5)->setText(QString("%L1").arg(Display[*its].summary[it->first].TradingPnlSettle, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 6)->setText(QString("%L1").arg(Display[*its].summary[it->first].TradingPnlClose, 0, 'f', 0));

				p_tablemodel_summary[k]->item(i, 7)->setText(QString("%L1").arg(Display[*its].summary[it->first].Position, 0, 'f', 0));
				p_tablemodel_summary[k]->item(i, 8)->setText(QString("%L1").arg(Display[*its].summary[it->first].NetPosition, 0, 'f', 0));
				QString pctreturn; pctreturn.sprintf("%.2lf%%", Display[*its].summary[it->first].ReturnOnPosition * 100);
				p_tablemodel_summary[k]->item(i, 9)->setText(pctreturn);

				p_tablemodel_summary[k]->item(i, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
				p_tablemodel_summary[k]->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 3)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 4)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 5)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 6)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 7)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 8)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_summary[k]->item(i, 9)->setForeground(QBrush(QColor(200, 200, 200)));
				//�������̼�ӯ����ɫ��
				if (Display[*its].summary[it->first].PnlClose > 0)
					p_tablemodel_summary[k]->item(i, 2)->setForeground(QBrush(QColor(255, 0, 0)));
				else if (Display[*its].summary[it->first].PnlClose < 0)
					p_tablemodel_summary[k]->item(i, 2)->setForeground(QBrush(QColor(0, 255, 0)));
				else
					p_tablemodel_summary[k]->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));

				//���ý����ӯ����ɫ��
				if (Display[*its].summary[it->first].PnlSettle > 0)
					p_tablemodel_summary[k]->item(i, 1)->setForeground(QBrush(QColor(255, 0, 0)));
				else if (Display[*its].summary[it->first].PnlSettle < 0)
					p_tablemodel_summary[k]->item(i, 1)->setForeground(QBrush(QColor(0, 255, 0)));
				else
					p_tablemodel_summary[k]->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));
				if (it->first == "Total")
				{
					QFont font("Black", 10, 75);
					p_tablemodel_summary[k]->item(i, 1)->setFont(font);
					p_tablemodel_summary[k]->item(i, 2)->setFont(font);
				}
				p_tablemodel_summary[k]->item(i, 0)->setTextAlignment(Qt::AlignCenter);//�����ַ�λ��
				i++;


			}
			///���½��׼�¼������ʾ����
			i = 0;
			for (list<tradesummary2>::iterator it = Display[*its].transaction.begin(); it != Display[*its].transaction.end(); it++)
			{
				p_tablemodel_trade[k]->item(i, 0)->setText(QString::fromStdString(it->code));
				p_tablemodel_trade[k]->item(i, 1)->setText(QString::fromStdString(it->Direction));
				p_tablemodel_trade[k]->item(i, 2)->setText(QString::fromStdString(it->OffsetFlag));
				p_tablemodel_trade[k]->item(i, 3)->setText(QString::number(it->price));
				p_tablemodel_trade[k]->item(i, 4)->setText(QString::number(it->num));


				p_tablemodel_trade[k]->item(i, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
				p_tablemodel_trade[k]->item(i, 1)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_trade[k]->item(i, 2)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_trade[k]->item(i, 3)->setForeground(QBrush(QColor(200, 200, 200)));
				p_tablemodel_trade[k]->item(i, 4)->setForeground(QBrush(QColor(200, 200, 200)));

				p_tablemodel_trade[k]->item(i, 0)->setTextAlignment(Qt::AlignCenter);//�����ַ�λ��
				i++;
			}
			///�����˻���Ϣ������ʾ����
			p_tablemodel_account[k]->item(0, 0)->setText(QString("%L1").arg(Display[*its].account.StaticEquity, 0, 'f', 0));
			p_tablemodel_account[k]->item(0, 1)->setText(QString("%L1").arg(Display[*its].account.CloseProfit, 0, 'f', 0));
			p_tablemodel_account[k]->item(0, 2)->setText(QString("%L1").arg(Display[*its].account.PositionProfit, 0, 'f', 0));
			p_tablemodel_account[k]->item(0, 3)->setText(QString("%L1").arg(Display[*its].account.DynamicEquity, 0, 'f', 0));
			p_tablemodel_account[k]->item(0, 4)->setText(QString("%L1").arg(Display[*its].account.CurrMargin, 0, 'f', 0));
			p_tablemodel_account[k]->item(0, 5)->setText(QString("%L1").arg(Display[*its].account.AvailableAmount, 0, 'f', 0));
			QString temp; temp.sprintf("%0.2lf%%", Display[*its].account.RiskDegree * 100);
			p_tablemodel_account[k]->item(0, 6)->setText(temp);

			p_tablemodel_account[k]->item(0, 0)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 1)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 2)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 3)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 4)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 5)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 6)->setForeground(QBrush(QColor(200, 200, 200)));//�����ַ���ɫ
			p_tablemodel_account[k]->item(0, 0)->setTextAlignment(Qt::AlignCenter);			  //�����ַ�λ��

			///�����˻��ڻ�ͼ

			QVector<double> x(sizeofplot+1), y(sizeofplot+1);
			QVector<QTime>z(sizeofplot+1);
			for (int i = 0; i < sizeofplot; ++i)
			{
				x[i] = startTime + i * 60;
				z[i] = start_time.addSecs(i * 60);
			}
			double max_y = 0;
			double min_y = 0;
			for (int i = 0; i < sizeofplot; ++i)
			{

				y[i] = Display[*its].plotdata[z[i]];
				if (y[i] > max_y)
					max_y = y[i];
				else if (y[i] < min_y)
					min_y = y[i];
			}
			P_AccountPlot[k]->graph(0)->setData(x, y);
			P_AccountPlot[k]->xAxis->setRange(x[0], x[sizeofplot-1]);
			P_AccountPlot[k]->yAxis->setRange(min_y, max_y);
			P_AccountPlot[k]->xAxis->setTickLabelColor(Qt::white);
			P_AccountPlot[k]->xAxis->setLabelColor(Qt::white);
			P_AccountPlot[k]->yAxis->setTickLabelColor(Qt::white);
			P_AccountPlot[k]->yAxis->setLabelColor(Qt::white);
			P_AccountPlot[k]->graph(0)->setPen(QPen(QColor(70, 110, 150, 200))); // line color blue for first graph
			P_AccountPlot[k]->graph(0)->setBrush(QBrush(QColor(20 + 200 / 4.0 * 1, 70 * (1.6 - 1 / 4.0), 150, 150))); // first graph will be filled with translucent blue
			P_AccountPlot[k]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
			P_AccountPlot[k]->replot();
		}
		///������־����
		QStringList list_log;
		for (list<string>::iterator it = Display[*its].log.begin(); it != Display[*its].log.end(); it++)
		{
			list_log << QString::fromStdString(*it);
		}
		p_listviewmodel[k]->setStringList(list_log);
		P_AccountLogListview[k]->setModel(p_listviewmodel[k]);
		QFont list_font;
		list_font.setPointSize(15);
		list_font.setBold(true);
		list_font.setWeight(50);
		P_AccountLogListview[k]->setFont(list_font);
		P_AccountHoldingTable[k]->setModel(p_tablemodel_position[k]);
		P_AccountTradingTable[k]->setModel(p_tablemodel_trade[k]);
		P_AccountCapitalTable[k]->setModel(p_tablemodel_account[k]);
		P_AccountSummaryTable[k]->setModel(p_tablemodel_summary[k]);
		k++;
	}


}
