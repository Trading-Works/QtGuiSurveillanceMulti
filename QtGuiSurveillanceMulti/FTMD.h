#pragma once
#include "stdafx.h"
class CFTMD : public CThostFtdcMdSpi  
{
public:
	CFTMD();
	virtual ~CFTMD();
	typedef std::map<std::string, CThostFtdcDepthMarketDataField> TYP_QUTOE;
	TYP_QUTOE LastDepth;

	TThostFtdcBrokerIDType qh_BrokerID;
	TThostFtdcAddressType qh_MDAddress;
	TThostFtdcUserIDType qh_UserID;
    TThostFtdcPasswordType qh_Password;


	AccountInfo g_AccountInfo;
	HANDLE hEvent;
	CThostFtdcMdApi *m_pMdApi;

	time_t nowtime;
	struct tm local;
	bool bIsCostZero=false;
	bool bIsMultiZero = false;

    void Init(AccountInfo account_info, CFTTD* pTdHandler);
	void initSubMD();
	
	double GetLastPrice(std::string code);
	double GetVolume(std::string code);
	double GetTClose(std::string code);
	double GetTSettle(std::string code);
	int GetMultiple(std::string code);
	void UnsubscribeMD();
	void AccountLogout();



	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();
	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	void OnFrontDisconnected(int nReason);
	///�������֪ͨ
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//�˶�����֪ͨ
	void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	//�ǳ�֪ͨ
	void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
private:
    CFTTD *g_pTdHandler;
};