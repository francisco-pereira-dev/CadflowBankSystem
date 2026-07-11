#include "pch.h"
#include "CadflowBankSystem.h"
#include "AccountDetailsDlg.h"
#include "afxbutton.h"

IMPLEMENT_DYNAMIC(CAccountDetailsDlg, CDialogEx)

CAccountDetailsDlg::CAccountDetailsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ACCOUNT_DETAILS, pParent)
{
}

CAccountDetailsDlg::~CAccountDetailsDlg()
{
}

void CAccountDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TRANSACTIONS, m_listTransactions);
	// DDX_Control (not just GetDlgItem) is required here: SetFaceColor/SetTextColor
	// on a CMFCButton only take effect through the bound C++ member, not the raw HWND.
	DDX_Control(pDX, IDC_BTN_CLOSE_DETAILS, m_btnClose);
}

BEGIN_MESSAGE_MAP(CAccountDetailsDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLOSE_DETAILS, &CAccountDetailsDlg::OnBnClickedBtnClose)
END_MESSAGE_MAP()

BOOL CAccountDetailsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(L"Sistema Bancario - Detalhes da Conta");

	m_brBranco.CreateSolidBrush(RGB(255, 255, 255));

	m_fontTitulos.CreateFont(
		-28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	m_fontNormal.CreateFont(
		-22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	// Deliberately oversized (~45pt) so the balance is the visual focal point of the screen.
	m_fontSaldoGrande.CreateFont(
		-60, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	m_fontBotao.CreateFont(
		-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	if (CWnd* pWnd = GetDlgItem(IDC_TXT_TITLE_INFO)) pWnd->SetFont(&m_fontTitulos);
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_TITLE_HISTORY)) pWnd->SetFont(&m_fontTitulos);
	
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_LBL_OWNER)) pWnd->SetFont(&m_fontNormal);
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_LBL_ACC_NUM)) pWnd->SetFont(&m_fontNormal);
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_DETAIL_NAME)) pWnd->SetFont(&m_fontNormal);
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_DETAIL_NUM)) pWnd->SetFont(&m_fontNormal);
	
	// "Saldo Atual:" label reuses the title font (larger) so it visually anchors
	// the balance figure below it; centered to stay aligned regardless of control width.
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_LBL_BALANCE_TITLE)) {
		pWnd->SetFont(&m_fontTitulos);
		pWnd->ModifyStyle(SS_TYPEMASK, SS_CENTER);
	}
	
	// Centered so very large or very small balances (billions vs. a few euros)
	// stay visually balanced inside the control instead of drifting to one side.
	if (CWnd* pWnd = GetDlgItem(IDC_TXT_DETAIL_BALANCE)) {
		pWnd->SetFont(&m_fontSaldoGrande);
		pWnd->ModifyStyle(SS_TYPEMASK, SS_CENTER);
	}

	// Flat "borderless" style makes this read as a link/back-action rather than
	// a primary action button, distinguishing it from the blue CTAs on other screens.
	m_btnClose.SetFont(&m_fontBotao);
	m_btnClose.SetFaceColor(RGB(255, 255, 255), TRUE);
	m_btnClose.SetTextColor(RGB(0, 80, 200));
	m_btnClose.m_bTransparent = TRUE;
	m_btnClose.m_nFlatStyle = CMFCButton::BUTTONSTYLE_NOBORDERS;
	m_btnClose.SetImage(IDB_BITMAP14);

	// FormatCurrency() (caller side) still produces the literal word "EUR" as a
	// fallback separator; swapped here for the actual currency symbol before display.
	m_strBalance.Replace(L"EUR ", L"\u20AC ");

	// All three fields are plain SetDlgItemText: no DDX_Text binding exists for
	// them, since this dialog only ever displays data, it never writes it back.
	SetDlgItemText(IDC_TXT_DETAIL_NAME,    m_strOwnerName);
	SetDlgItemText(IDC_TXT_DETAIL_NUM,     m_strAccountNumber);
	SetDlgItemText(IDC_TXT_DETAIL_BALANCE, m_strBalance);

	// 24x24 matches the native bitmap resolution - anything smaller would crop the icon.
	m_imgListTrans.Create(24, 24, ILC_COLOR32 | ILC_MASK, 2, 0);
	CBitmap bmpDown; // Index 0: green arrow, shown for deposits.
	bmpDown.LoadBitmap(IDB_BITMAP12);
	m_imgListTrans.Add(&bmpDown, RGB(255, 255, 255));
	CBitmap bmpUp; // Index 1: red arrow, shown for withdrawals.
	bmpUp.LoadBitmap(IDB_BITMAP13);
	m_imgListTrans.Add(&bmpUp, RGB(255, 255, 255));
	m_listTransactions.SetImageList(&m_imgListTrans, LVSIL_SMALL);

	m_listTransactions.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	// Columns split roughly evenly (33/33/34%); 25px is reserved upfront so the
	// vertical scrollbar (which appears once the history overflows) never clips column 3.
	CRect rectLista;
	m_listTransactions.GetClientRect(&rectLista);
	int nLarguraTotal = rectLista.Width() - 25;
	int nCol1 = (nLarguraTotal * 33) / 100;
	int nCol2 = (nLarguraTotal * 33) / 100;
	int nCol3 = nLarguraTotal - nCol1 - nCol2; // Absorbs the rounding remainder so columns always total 100%.

	m_listTransactions.InsertColumn(0, L"Tipo",      LVCFMT_LEFT, nCol1);
	m_listTransactions.InsertColumn(1, L"Montante",  LVCFMT_LEFT, nCol2);
	m_listTransactions.InsertColumn(2, L"Data/Hora", LVCFMT_LEFT, nCol3);

	// m_transactionHistory is a snapshot copy handed over by the caller (see header),
	// so iterating and formatting it here can never mutate the engine's real data.
	for (size_t i = 0; i < m_transactionHistory.size(); ++i)
	{
		const Transaction& trans = m_transactionHistory[i];

		// trans.GetType() stores the raw internal string ("Deposito"/"Levantamento...");
		// leading spaces reserve visual room for the icon drawn in the same column.
		bool bDeposito = (trans.GetType() == "Deposito");
		CString strType = bDeposito ? L"    Deposito" : L"    Levantamento";

		int nIcone = bDeposito ? 0 : 1; // Must match the index order used when building m_imgListTrans above.

		LVITEM lvi = {};
		lvi.mask     = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem    = (int)i;
		lvi.iSubItem = 0;
		lvi.pszText  = strType.GetBuffer();
		lvi.iImage   = nIcone;
		m_listTransactions.InsertItem(&lvi);
		strType.ReleaseBuffer(); // Must pair every GetBuffer() call, or the CString is left in an invalid locked state.

		CString strAmount;
		strAmount.Format(L"\u20AC %.2f", trans.GetAmount());
		m_listTransactions.SetItemText((int)i, 1, strAmount);

		// Transaction stores a raw time_t; CTime::Format converts it to a
		// human-readable local date/time for display only.
		CTime transTime(trans.GetTimestamp());
		CString strTime = transTime.Format(L"%Y-%m-%d %H:%M");
		m_listTransactions.SetItemText((int)i, 2, strTime);
	}

	return TRUE;
}

// IDCANCEL (not IDOK) is used deliberately: this is a read-only view, so closing
// it must never be mistaken for confirming/submitting anything back to the caller.
void CAccountDetailsDlg::OnBnClickedBtnClose()
{
	EndDialog(IDCANCEL);
}

// Repaints static text/background per-control, since MFC's default dialog
// theme cannot express "some labels blue, some black" without this hook.
HBRUSH CAccountDetailsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));

		// Only the identity/emphasis controls get the corporate blue; every other
		// label stays black to keep the blue reserved for what matters most.
		int nID = pWnd ? pWnd->GetDlgCtrlID() : 0;
		if (nID == IDC_TXT_TITLE_INFO || 
		    nID == IDC_TXT_TITLE_HISTORY || 
		    nID == IDC_TXT_DETAIL_BALANCE ||
		    nID == IDC_TXT_DETAIL_NAME ||
		    nID == IDC_TXT_DETAIL_NUM ||
		    nID == IDC_TXT_LBL_BALANCE_TITLE)
		{
			pDC->SetTextColor(RGB(0, 80, 200));
		}
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));
		}

		return (HBRUSH)m_brBranco.GetSafeHandle();
	}

	return hbr;
}
