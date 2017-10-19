#include "stdafx.h"
using namespace std;
extern map<string, Display_GUI>Display;
extern logInfo *pLog;
CFTMD::CFTMD()
{

}

CFTMD::~CFTMD()
{
	
}

void CFTMD::Init(AccountInfo account_info, CFTTD* pTdHandler)
{
	// ����һ��CThostFtdcMdApiʵ��
	memset(qh_BrokerID, 0, sizeof(qh_BrokerID));
	memset(qh_MDAddress, 0, sizeof(qh_MDAddress));
	memset(qh_UserID, 0, sizeof(qh_UserID));
	memset(qh_Password, 0, sizeof(qh_Password));
	strcpy_s(qh_BrokerID, account_info.BrokerID.c_str());
	strcpy_s(qh_UserID, account_info.UserID.c_str());
	strcpy_s(qh_Password, account_info.Password.c_str());
	g_pTdHandler = pTdHandler;
	g_AccountInfo = account_info;
	m_pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
	m_pMdApi->RegisterSpi(this);
	for (list<string>::iterator it = account_info.MdAddress.begin(); it != account_info.MdAddress.end(); it++)
	{
		strcpy_s(qh_MDAddress, (*it).c_str());
		m_pMdApi->RegisterFront(qh_MDAddress);
	}
	hEvent = CreateEvent(NULL, false, false, NULL);
	m_pMdApi->Init();
	if (WaitForSingleObject(hEvent, 30000) == WAIT_TIMEOUT)
	{
		printf("***%s*** ����ǰ�˵�¼���� ���߳����˳�\n", g_pTdHandler->g_AccountInfo.AccountName.c_str());
		nowtime = time(NULL); //��ȡ����ʱ��
		localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
		char filepre_tm[18];
		strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
		Display[g_AccountInfo.AccountName].log.push_back(string(filepre_tm)+string(":Market front landed failed !!!"));
	}

		
	
}

void CFTMD::OnFrontConnected()
{
	nowtime = time(NULL); //��ȡ����ʱ��
	localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
	char filepre_tm[18];
	strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
	Display[g_AccountInfo.AccountName].log.push_back(string(filepre_tm) + string(":Market front connected successfully!!!"));
	CThostFtdcReqUserLoginField reqUserLogin;
	memset(&reqUserLogin,0,sizeof(reqUserLogin));
	strcpy_s(reqUserLogin.BrokerID, qh_BrokerID);
	strcpy_s(reqUserLogin.UserID, qh_UserID);
	strcpy_s(reqUserLogin.Password, qh_Password);
	int login = m_pMdApi->ReqUserLogin(&reqUserLogin,1);
}
void CFTMD::OnFrontDisconnected(int nReason)
{
	printf("***%s*** �������ӶϿ� ԭ�����:%d\n", g_pTdHandler->g_AccountInfo.AccountName.c_str(), nReason);
	nowtime = time(NULL); //��ȡ����ʱ��
	localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
	char filepre_tm[18];
	strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
	Display[g_AccountInfo.AccountName].log.push_back(string(filepre_tm) + string(":The market connection is broken !!!"));
}
void CFTMD::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo->ErrorID == 0)
	{
		initSubMD();
		nowtime = time(NULL); //��ȡ����ʱ��
		localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
		char filepre_tm[18];
		strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
		Display[g_AccountInfo.AccountName].log.push_back(string(filepre_tm) + string(":Market front landed successfully!!!"));
		
	}
	else
	{
		nowtime = time(NULL); //��ȡ����ʱ��
		localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
		char filepre_tm[18];
		strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
		Display[g_AccountInfo.AccountName].log.push_back(string(filepre_tm) + string(":Market front landing error!!!"));
		printf("***%s*** ����ǰ�˵�½���� ErrorID=%d ErrorMsg=%s ��ǰ����=%s\n", g_pTdHandler->g_AccountInfo.AccountName.c_str(), pRspInfo->ErrorID, pRspInfo->ErrorMsg, pRspUserLogin->TradingDay);
	}
}

void CFTMD::initSubMD()
{
    //���ݺ�Լ�б�������
	int n_count = g_pTdHandler->Instruments.size();
	char ** codes = new char*[n_count];
	TThostFtdcInstrumentIDType* InstrumentIDs = new TThostFtdcInstrumentIDType[n_count];
	int i = 0;
	for (list<CThostFtdcInstrumentField>::iterator it = g_pTdHandler->Instruments.begin(); it!=g_pTdHandler->Instruments.end(); it++)
	{
		strcpy_s(InstrumentIDs[i], it->InstrumentID);
		codes[i] = InstrumentIDs[i];
		i++;
	}
	m_pMdApi->SubscribeMarketData(codes, n_count);

	delete[] codes;
	delete[] InstrumentIDs;
}

void CFTMD::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		SetEvent(hEvent);
}
void CFTMD::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) 
{
	CThostFtdcDepthMarketDataField *pMD;
	LastDepth[pDepthMarketData->InstrumentID] = *pDepthMarketData;
	pMD = &LastDepth[pDepthMarketData->InstrumentID];
	pMD->LastPrice = (pMD->LastPrice > 10000000.0) ? 0 : pMD->LastPrice;                          ///���¼�
	pMD->OpenPrice = (pMD->OpenPrice > 10000000.0) ? pMD->LastPrice : pMD->OpenPrice;             ///����
	pMD->HighestPrice = (pMD->HighestPrice > 10000000.0) ? pMD->LastPrice : pMD->HighestPrice;    ///��߼�
	pMD->LowestPrice = (pMD->LowestPrice > 10000000.0) ? pMD->LastPrice : pMD->LowestPrice;       ///��ͼ�
	pMD->BidPrice1 = (pMD->BidPrice1 > 10000000.0) ? pMD->LastPrice : pMD->BidPrice1;             ///�����һ
	pMD->AskPrice1 = (pMD->AskPrice1 > 10000000.0) ? pMD->LastPrice : pMD->AskPrice1;             ///������һ
	pMD->AveragePrice = (pMD->AveragePrice > 10000000.0) ? pMD->LastPrice : pMD->AveragePrice;    ///���վ�
	string code ;
	string direction;
	Display_Position_Key key;
	code = string(pDepthMarketData->InstrumentID);

	
	char *p = new char[g_AccountInfo.AccountName.length() + 1]();
	strcpy(p, g_AccountInfo.AccountName.c_str());
	if (Display[g_AccountInfo.AccountName].hEventUi)
	{

		//�ȴ��ֲ���Ϣˢ�½������ٸ������ݣ�����ˢ�³ֲ���Ϣ����ʱ�������Display.position1�����¿�map���ã����±���
		direction = "��";
		key.ID = code;
		key.Direction = direction;
		if (Display[g_AccountInfo.AccountName].position1.find(key) != Display[g_AccountInfo.AccountName].position1.end())
		{
			Display[g_AccountInfo.AccountName].position1[key].LastPrice = pMD->LastPrice;
			Display[g_AccountInfo.AccountName].position1[key].T1CLose = pMD->PreClosePrice;
			Display[g_AccountInfo.AccountName].position1[key].T1Settle = pMD->PreSettlementPrice;
			Display[g_AccountInfo.AccountName].position1[key].PctClose = Display[g_AccountInfo.AccountName].position1[key].LastPrice / Display[g_AccountInfo.AccountName].position1[key].T1CLose - 1;
			Display[g_AccountInfo.AccountName].position1[key].PctSettle = Display[g_AccountInfo.AccountName].position1[key].LastPrice / Display[g_AccountInfo.AccountName].position1[key].T1Settle - 1;
			if (Display[g_AccountInfo.AccountName].position1[key].CostClose < 1)
			{
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose = 0;
				nowtime = time(NULL); //��ȡ����ʱ��
				localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
				char filepre_tm[18];
				strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
				if (local.tm_hour <= 15 && local.tm_hour >= 9)
					bIsCostZero = true;				
				pLog->printLog("(%s) (%s)\n %s��CostCloseΪ%f\n", g_AccountInfo.AccountName.c_str(), filepre_tm, key.ID.c_str(), Display[g_AccountInfo.AccountName].position1[key].CostClose);
			}
			else
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose = (Display[g_AccountInfo.AccountName].position1[key].LastPrice - Display[g_AccountInfo.AccountName].position1[key].CostClose)*Display[g_AccountInfo.AccountName].position1[key].Multiplie*Display[g_AccountInfo.AccountName].position1[key].NumPosition;
			if (Display[g_AccountInfo.AccountName].position1[key].CostSettle < 1)
			{
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle = 0;
				nowtime = time(NULL); //��ȡ����ʱ��
				localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
				if (local.tm_hour <= 15 && local.tm_hour >= 9)
					bIsCostZero = true;
			}
			else
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle = (Display[g_AccountInfo.AccountName].position1[key].LastPrice - Display[g_AccountInfo.AccountName].position1[key].CostSettle)*Display[g_AccountInfo.AccountName].position1[key].Multiplie*Display[g_AccountInfo.AccountName].position1[key].NumPosition;

			Display[g_AccountInfo.AccountName].position1[key].Position = abs(Display[g_AccountInfo.AccountName].position1[key].NumPosition*Display[g_AccountInfo.AccountName].position1[key].T1CLose*Display[g_AccountInfo.AccountName].position1[key].Multiplie);
			Display[g_AccountInfo.AccountName].position1[key].NetPosition = Display[g_AccountInfo.AccountName].position1[key].NumPosition*Display[g_AccountInfo.AccountName].position1[key].T1CLose*Display[g_AccountInfo.AccountName].position1[key].Multiplie;

			Display[g_AccountInfo.AccountName].position1[key].PnlClose = Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose + Display[g_AccountInfo.AccountName].position1[key].TradingPnlClose;
			Display[g_AccountInfo.AccountName].position1[key].PnlSettle = Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle + Display[g_AccountInfo.AccountName].position1[key].TradingPnlSettle;

		}
		direction = "��";
		key.ID = code;
		key.Direction = direction;
		if (Display[g_AccountInfo.AccountName].position1.find(key) != Display[g_AccountInfo.AccountName].position1.end())
		{
			Display[g_AccountInfo.AccountName].position1[key].LastPrice = pMD->LastPrice;
			Display[g_AccountInfo.AccountName].position1[key].T1CLose = pMD->PreClosePrice;
			Display[g_AccountInfo.AccountName].position1[key].T1Settle = pMD->PreSettlementPrice;
			Display[g_AccountInfo.AccountName].position1[key].PctClose = Display[g_AccountInfo.AccountName].position1[key].LastPrice / Display[g_AccountInfo.AccountName].position1[key].T1CLose - 1;
			Display[g_AccountInfo.AccountName].position1[key].PctSettle = Display[g_AccountInfo.AccountName].position1[key].LastPrice / Display[g_AccountInfo.AccountName].position1[key].T1Settle - 1;

			if (Display[g_AccountInfo.AccountName].position1[key].CostClose < 1)
			{
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose = 0;
				nowtime = time(NULL); //��ȡ����ʱ��
				localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
				if (local.tm_hour <= 15 && local.tm_hour >= 9)
					bIsCostZero = true;

			}
			else
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose = (Display[g_AccountInfo.AccountName].position1[key].LastPrice - Display[g_AccountInfo.AccountName].position1[key].CostClose)*Display[g_AccountInfo.AccountName].position1[key].Multiplie*Display[g_AccountInfo.AccountName].position1[key].NumPosition;
			if (Display[g_AccountInfo.AccountName].position1[key].CostSettle < 1)
			{
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle = 0;
				nowtime = time(NULL); //��ȡ����ʱ��
				localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
				if (local.tm_hour <= 15 && local.tm_hour >= 9)
					bIsCostZero = true;
			}
			else
				Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle = (Display[g_AccountInfo.AccountName].position1[key].LastPrice - Display[g_AccountInfo.AccountName].position1[key].CostSettle)*Display[g_AccountInfo.AccountName].position1[key].Multiplie*Display[g_AccountInfo.AccountName].position1[key].NumPosition;

			Display[g_AccountInfo.AccountName].position1[key].Position = abs(Display[g_AccountInfo.AccountName].position1[key].NumPosition*Display[g_AccountInfo.AccountName].position1[key].T1CLose*Display[g_AccountInfo.AccountName].position1[key].Multiplie);
			Display[g_AccountInfo.AccountName].position1[key].NetPosition = Display[g_AccountInfo.AccountName].position1[key].NumPosition*Display[g_AccountInfo.AccountName].position1[key].T1CLose*Display[g_AccountInfo.AccountName].position1[key].Multiplie;

			Display[g_AccountInfo.AccountName].position1[key].PnlClose = Display[g_AccountInfo.AccountName].position1[key].HoldingPnlClose + Display[g_AccountInfo.AccountName].position1[key].TradingPnlClose;
			Display[g_AccountInfo.AccountName].position1[key].PnlSettle = Display[g_AccountInfo.AccountName].position1[key].HoldingPnlSettle + Display[g_AccountInfo.AccountName].position1[key].TradingPnlSettle;
		}
	}
	delete p;
}




double CFTMD::GetLastPrice(string order)
{
	double price = 0.001;
	if (LastDepth.size() == 0)
	{
		ExitThread(0);
	}
	else
		price = LastDepth[order].LastPrice;
	//����δ���嵽�ĺ�Լ���趨�۸�Ϊ0.001��С��Ŀǰ���к�Լ����С�䶯��λ������������Ч��
	if (price > 10000 * 1000)
		price = 0.001;
	else if (price < -10000 * 1000)
		price = 0.001;
	return price;
}
int CFTMD::GetMultiple(string code)
{
	for (list<CThostFtdcInstrumentField>::iterator it = g_pTdHandler->Instruments.begin(); it != g_pTdHandler->Instruments.end(); it++)
	{
		if (strcmp(it->InstrumentID, code.c_str())==0)
		{
			int multi = it->VolumeMultiple;
			return multi;
		}
	}
	bIsMultiZero = true;

	nowtime = time(NULL); //��ȡ����ʱ��
	localtime_s(&local, &nowtime);  //��ȡ��ǰϵͳʱ��
	char filepre_tm[18];
	strftime(filepre_tm, 18, "%Y%m%d_%H%M%S", &local);
	pLog->printLog("(%s) (%s)\n %s��ȡMultiʧ�ܣ���ʱlist<CThostFtdcInstrumentField>����Ϊ%d \n", g_AccountInfo.AccountName.c_str(), filepre_tm, code.c_str(), g_pTdHandler->Instruments.size());
	return 0;
}
double CFTMD::GetTClose(string code)
{
	if (LastDepth.size() == 0)
	{
		ExitThread(0);
	}
	return LastDepth[code].PreClosePrice;
}
double CFTMD::GetTSettle(string code)
{
	if (LastDepth.size() == 0)
	{
		ExitThread(0);
	}
	return LastDepth[code].PreSettlementPrice;
}

double CFTMD::GetVolume(string code)
{
	if (LastDepth.size() == 0)
	{
		ExitThread(0);
	}
	return LastDepth[code].Volume;
}

void CFTMD::AccountLogout()
{
	UnsubscribeMD();
	CThostFtdcUserLogoutField UserLogoutField;
	memset(&UserLogoutField, 0, sizeof(UserLogoutField));
	CThostFtdcUserLogoutField * pUserLogoutField = &UserLogoutField;
	strcpy_s(pUserLogoutField->BrokerID, g_pTdHandler->g_AccountInfo.BrokerID.c_str());
	strcpy_s(pUserLogoutField->UserID, g_pTdHandler->g_AccountInfo.UserID.c_str());
	ResetEvent(hEvent);
	m_pMdApi->ReqUserLogout(pUserLogoutField, 1);
	if (WaitForSingleObject(hEvent, 10000) == WAIT_TIMEOUT)
	{

		printf("***%s*** ����ǰ�˵ǳ�ʧ�� ���߳����˳�\n", g_pTdHandler->g_AccountInfo.AccountName.c_str());
		ExitThread(0);
	}
	else
	{

		printf("***%s*** ����ǰ�˵ǳ�\n", g_pTdHandler->g_AccountInfo.AccountName.c_str());
	}

}

void CFTMD::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pRspInfo->ErrorID == 0)
		SetEvent(hEvent);
}

void CFTMD::UnsubscribeMD()
{
	//ȡ�������ĵ������б�
	int n_count = g_pTdHandler->Instruments.size();
	char ** codes = new char*[n_count];
	TThostFtdcInstrumentIDType* InstrumentIDs = new TThostFtdcInstrumentIDType[n_count];
	int i = 0;
	for (list<CThostFtdcInstrumentField>::iterator it = g_pTdHandler->Instruments.begin(); it != g_pTdHandler->Instruments.end(); it++)
	{
		strcpy_s(InstrumentIDs[i], it->InstrumentID);
		codes[i] = InstrumentIDs[i];
		i++;
	}
	ResetEvent(hEvent);
	m_pMdApi->UnSubscribeMarketData(codes, n_count);
	if (WaitForSingleObject(hEvent, 10000) == WAIT_TIMEOUT)
	{

		printf("***%s*** �˶�����ʧ�� ���߳����˳�\n", g_pTdHandler->g_AccountInfo.AccountName.c_str());
		ExitThread(0);
	}
	else
	{

		printf("***%s*** �˶�����ɹ�\n", g_pTdHandler->g_AccountInfo.AccountName.c_str());
	}
	delete[] codes;
	delete[] InstrumentIDs;
}

void CFTMD::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (bIsLast)
		SetEvent(hEvent);
}