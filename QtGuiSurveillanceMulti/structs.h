#pragma once
#include "stdafx.h"
struct OrderStruct
{
	TThostFtdcInstrumentIDType code;
	TThostFtdcVolumeType volume;
	TThostFtdcDirectionType direction;
	TThostFtdcCombOffsetFlagType kp;
};
struct AccountInfo
{
	std::list<std::string>  TdAddress;
	std::list<std::string>  MdAddress;
	std::string  UserID;
	std::string  Password;
	std::string  BrokerID;
	std::string  PositionFileHead;
	std::string  PositionFileTail;
	std::string  AccountName;
};
struct  Display_Position_Key
{
	std::string ID;
	std::string Direction;
	//������Ҫ��Ϊmap�ļ���������Ҫ����operator ����
	friend bool operator < (const struct Display_Position_Key &k1, const struct Display_Position_Key &k2) 
	{
		if (k1.ID < k2.ID)        //���Ͱ���������
			return true;
		else if (k1.ID == k2.ID)  //���������ͬ������������������
			return k1.Direction < k2.Direction;
       return false;
	};

};
struct Display_Position
{
	std::string KIND;
	int NumPosition;
	int TodayNumPosition;
	int YesNumPosition;
	double LastPrice;
	double T1CLose;
	double T1Settle;
	int Multiplie;
	int Position;
	double NetPosition;
	double CostClose;
	double CostSettle;
	double PnlSettle;
	double PnlClose;
	double HoldingPnlClose;
	double HoldingPnlSettle;
	double TradingPnlClose;
	double TradingPnlSettle;
	double PctSettle;
	double PctClose;
};
struct Display_Summary
{

	double Position;
	double NetPosition;
	double PnlSettle;
	double PnlClose;
	double HoldingPnlClose;
	double HoldingPnlSettle;
	double TradingPnlClose;
	double TradingPnlSettle;
	double ReturnOnPosition;

};
struct tradesummary2
{
	std::string code;
	std::string OffsetFlag;
	std::string Direction;
	double price;
	int num;

};
struct Display_Account
{
	double StaticEquity;
	double DynamicEquity;
	double CloseProfit;
	double PositionProfit;
	double CurrMargin;
	double AvailableAmount;
	double RiskDegree;

};
struct Display_Monitor
{

	double Position;
	double NetPosition;
	double CommondityPosition;
	double FinancialPosition;
	double PnlSettle;
	double PnlClose;
	double ReturnOnPosition;
	double PnlFinancial;
	double PnlAgriculture;
	double PnlBlack;
	double PnlEnergy;
	double PnlBaseMetal;
	double PnlPreciousMetal;
	std::map<QTime, double>plotdata;
};
struct Display_GUI
{
	std::map<Display_Position_Key, Display_Position>position1;	//�ֲ���Ϣ    <<Ʒ��ID,Ʒ�ֶ��>���ֲ�����>
	std::map<std::string, Display_Position>position;			//�ֲ���Ϣ    <Ʒ��ID���ֲ�����>
	std::map<std::string, Display_Summary>summary;				//����ͳ����Ϣ  <����ID,�����������>
	std::list<tradesummary2>transaction;						//������Ϣ
	Display_Account account;
	std::list<std::string> log;									//CTP������־		
	std::map<QTime, double>plotdata;
	bool hEventUi=false;	
	bool bIsEmpty=false;												//�жϻ���ҳ�ǲ���ȫ��0
};