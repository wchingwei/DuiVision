#include "StdAfx.h"
#include "DuiHandlerMain.h"
#include "registry.h"

//////////////////////////////////////////////////////////////
// CDuiHandlerMain

CDuiHandlerMain::CDuiHandlerMain(void) : CDuiHandler()
{
	m_pDlg = NULL;
	m_uTimerAni = 0;
	m_nAniIndex = 0;
}

CDuiHandlerMain::~CDuiHandlerMain(void)
{
}

// 初始化
void CDuiHandlerMain::OnInit()
{
	// 获取保存的浏览器页签URL
	CRegistryUtil reg(HKEY_CURRENT_USER);
	for(int i=1; i<=20; i++)
	{
		CString strUrlKey;
		strUrlKey.Format(L"url%d", i);
		CString strUrl = reg.GetStringValue(NULL, REG_EXPLORER_SUBKEY, strUrlKey);
		if(!strUrl.IsEmpty())
		{
			m_asUrl.Add(strUrl);
		}
	}

	// 创建页面
	for(int i=0; i<m_asUrl.GetSize(); i++)
	{
	}

	//InsertExplorerTab(1, L"aaa", L"http://www.csdn.net");

	// 启动动画定时器
	m_uTimerAni = DuiSystem::AddDuiTimer(500);
}

// 皮肤消息处理(实现皮肤的保存和获取)
LRESULT CDuiHandlerMain::OnDuiMsgSkin(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(Msg == MSG_GET_SKIN_TYPE)	// 获取Skin类型
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		int nBkType = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE);
		*(int*)wParam = nBkType;
		return TRUE;
	}else
	if(Msg == MSG_GET_SKIN_VALUE)	// 获取Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			*(int*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES);
			return TRUE;
		}else
		if(wParam == BKTYPE_COLOR)
		{
			*(COLORREF*)lParam = reg.GetDWordValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR);
			return TRUE;
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			*(CString*)lParam = reg.GetStringValue(NULL, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE);
			return TRUE;
		}
	}else
	if(Msg == MSG_SET_SKIN_VALUE)	// 设置Skin值
	{
		CRegistryUtil reg(HKEY_CURRENT_USER);
		reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKTYPE, wParam);
		if(wParam == BKTYPE_IMAGE_RESOURCE)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_RES, lParam);
		}else
		if(wParam == BKTYPE_COLOR)
		{
			reg.SetDWordValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKCOLOR, lParam);
		}else
		if(wParam == BKTYPE_IMAGE_FILE)
		{
			CString* pstrImgFile = (CString*)lParam;
			reg.SetStringValue(HKEY_CURRENT_USER, REG_CONFIG_SUBKEY, REG_CONFIG_BKPIC_FILE, *pstrImgFile);
		}
		return TRUE;
	}
	return FALSE;
}


// DUI定时器事件处理
void CDuiHandlerMain::OnTimer(UINT uTimerID, CString strTimerName)
{
    if(uTimerID == m_uTimerAni)
	{
	}
}

// 进程间消息处理
LRESULT CDuiHandlerMain::OnDuiMsgInterprocess(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 命令行参数,可以对命令行参数进行处理,也可以直接显示主窗口
	DUI_INTERPROCESS_MSG* pInterMsg = (DUI_INTERPROCESS_MSG*)lParam;
	CString strCmd = pInterMsg->wInfo;
	if(!strCmd.IsEmpty())
	{
		DuiSystem::DuiMessageBox(NULL, L"执行了命令行参数:" + strCmd);
	}else
	{
		CDlgBase* pDlg = DuiSystem::Instance()->GetDuiDialog(L"dlg_main");
		if(pDlg)
		{
			pDlg->SetForegroundWindow();
			pDlg->ShowWindow(SW_NORMAL);
			pDlg->ShowWindow(SW_SHOW);
			pDlg->BringWindowToTop();
		}
	}
	return TRUE;
}

// 获取Tab页中的浏览器控件
CDuiWebBrowserCtrl* CDuiHandlerMain::GetTabWebControl(CControlBase* pTabCtrl)
{
	if(pTabCtrl == NULL)
	{
		return NULL;
	}

	vector<CControlBase*>* paControls = pTabCtrl->GetControls();
	if(paControls)
	{
		for (size_t i = 0; i < paControls->size(); i++)
		{
			CControlBase* pControl = paControls->at(i);
			if(pControl && pControl->IsClass(CDuiWebBrowserCtrl::GetClassName()))
			{
				return (CDuiWebBrowserCtrl*)pControl;
			}
		}
	}

	return NULL;
}

// 获取Tab页中某个控件ID对应的Tab页信息
TabItemInfo* CDuiHandlerMain::GetTabInfoByWebCtrlId(UINT uID)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		int nCount = pTabCtrl->GetItemCount();
		for(int i=0; i<nCount; i++)
		{
			TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(i);
			if(pTabInfo && pTabInfo->pControl)
			{
				CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebControl(pTabInfo->pControl);
				if(pWebControl && (pWebControl->GetID() == uID))
				{
					return pTabInfo;
				}
			}
		}
	}

	return NULL;
}

// 获取当前页面的浏览器控件
CDuiWebBrowserCtrl* CDuiHandlerMain::GetCurTabWebControl()
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		int nItem = pTabCtrl->GetSelectItem();
		TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nItem);
		if(pTabInfo && pTabInfo->pControl)
		{
			//CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)(pTabInfo->pControl->GetControl(L"webbrowser"));
			CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebControl(pTabInfo->pControl);
			return pWebControl;
		}
	}

	return NULL;
}

// 创建浏览器页面
void CDuiHandlerMain::InsertExplorerTab(int nIndex, CString strTitle, CString strUrl)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		pTabCtrl->LoadTabXml(L"tab_webbrowser");
		int nCount = pTabCtrl->GetItemCount();
		pTabCtrl->SetSelectItem(nCount-1);
		TabItemInfo* pTabInfo = pTabCtrl->GetItemInfo(nCount - 1);
		if(pTabInfo && pTabInfo->pControl)
		{
			pTabInfo->strText = strTitle;
			pTabInfo->nImageIndex = 0;
			//CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)(pTabInfo->pControl->GetControl(L"webbrowser"));
			CDuiWebBrowserCtrl* pWebControl = (CDuiWebBrowserCtrl*)GetTabWebControl(pTabInfo->pControl);
			if(pWebControl)
			{
				pWebControl->Navigate(strUrl);
			}
		}
	}
/*
	CDuiPanel* pControlPanel = (CDuiPanel*)DuiSystem::CreateControlByName(L"div", m_pDlg->GetSafeHwnd(), m_pDuiObject);
	if(pControlPanel)
	{
		// 创建div,并插入tab页
		//pControlPanel->SetName(L"");
		pTabCtrl->AddControl(pControlPanel);
		pControlPanel->SetTitle(strTitle);
		pControlPanel->SetEnableScroll(FALSE);
		pControlPanel->SetPosStr(L"0,27,-0,-0");
		pControlPanel->OnPositionChange();
		CString strId;
		strId.Format(L"url-%d", nIndex);
		pTabCtrl->InsertItem(nIndex, 5000+nIndex, strId, strTitle, L"", 0, pControlPanel);

		// 创建浏览器控件
		CDuiWebBrowserCtrl* pControlWeb = (CDuiWebBrowserCtrl*)DuiSystem::CreateControlByName(L"webbrowser", m_pDlg->GetSafeHwnd(), pControlPanel);
		pControlPanel->AddControl(pControlWeb);
		pControlWeb->SetDelayCreate(true);
		pControlWeb->Navigate(strUrl);
		pControlWeb->SetPosStr(L"0,47,-1,-1");
		
		pControlWeb->OnPositionChange();
	}
*/
}

// 导航到上一页
LRESULT CDuiHandlerMain::OnDuiMsgButtonGoBack(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->GoBack();
	}

	return TRUE;
}

// 导航到下一页
LRESULT CDuiHandlerMain::OnDuiMsgButtonGoForward(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->GoForward();
	}

	return TRUE;
}

// 刷新web页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonRefresh(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->Refresh();
	}

	return TRUE;
}

// 导航到首页
LRESULT CDuiHandlerMain::OnDuiMsgButtonHome(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->Navigate(L"http://www.blueantstudio.net");
	}

	return TRUE;
}

// URL输入框的键盘事件处理
LRESULT CDuiHandlerMain::OnDuiMsgComboUrlKeyDown(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(wParam != VK_RETURN)
	{
		return FALSE;
	}

	CString strUrl = L"";
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if(pUrlCtrl != NULL)
	{
		strUrl = pUrlCtrl->GetEditText();
	}

	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->Navigate(strUrl);
	}

	return TRUE;
}

// URL输入框的事件处理
LRESULT CDuiHandlerMain::OnDuiMsgComboUrl(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if(wParam != SELECT_ITEM)
	{
		return FALSE;
	}

	CString strUrl = L"";
	CDuiComboBox* pUrlCtrl = (CDuiComboBox*)GetControl(_T("combo.url"));
	if(pUrlCtrl != NULL)
	{
		strUrl = pUrlCtrl->GetEditText();
	}

	CDuiWebBrowserCtrl* pWebControl = GetCurTabWebControl();
	if(pWebControl)
	{
		pWebControl->Navigate(strUrl);
	}

	return TRUE;
}

// 新建页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonNewTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 新建web页面
	InsertExplorerTab(-1, L"", L"");

	return TRUE;
}

// 关闭页面
LRESULT CDuiHandlerMain::OnDuiMsgButtonCloseTab(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	if(pTabCtrl)
	{
		// wParam参数表示tab页索引
		pTabCtrl->DeleteItem(wParam);
	}

	return TRUE;
}

#define MAX_URL 256
// 浏览器页面的标题变更事件
LRESULT CDuiHandlerMain::OnDuiMsgWebTitleChange(UINT uID, CString strName, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// 获取页面标题
	CString strTitle = L"";
	DISPPARAMS FAR* pdispparams = (DISPPARAMS FAR*)wParam;
	if(pdispparams && pdispparams->rgvarg[0].vt == VT_BSTR)
	{
		WCHAR szTitle[MAX_URL];
		int len = wcslen(pdispparams->rgvarg[0].bstrVal);               
		wcsncpy(szTitle, pdispparams->rgvarg[0].bstrVal, MAX_URL - 5);
		if(len > MAX_URL - 5)
		{
			wcscat(szTitle, L"...");
		}
		strTitle = szTitle;
	}

	// 找对对应的浏览器控件,并设置对应的tab页签文字
	CDuiTabCtrl* pTabCtrl = (CDuiTabCtrl*)GetControl(_T("tabctrl.main"));
	TabItemInfo* pTabInfo = GetTabInfoByWebCtrlId(uID);
	if(pTabInfo && pTabCtrl)
	{
		pTabInfo->strText = strTitle;
		pTabCtrl->RefreshItems();
	}

	return TRUE;
}
