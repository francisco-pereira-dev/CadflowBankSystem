#include "pch.h"
#include "CadflowBankSystem.h"
#include "TransactionDlg.h"
#include "afxbutton.h"
#include <cmath>

IMPLEMENT_DYNAMIC(CTransactionDlg, CDialogEx)

CTransactionDlg::CTransactionDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TRANSACTION, pParent), m_amount(0.0), m_dCurrentBalance(0.0), m_nTransactionType(0)
{
}

CTransactionDlg::~CTransactionDlg()
{
}

void CTransactionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	// Bindings for the visual controls
	DDX_Control(pDX, IDC_COMBO_TRANS_TYPE,          m_comboTransType);
	DDX_Text(pDX,    IDC_EDIT_TRANS_AMOUNT,          m_strAmountInput);
	// Buttons - without DDX_Control, SetFaceColor is ignored by Windows theming.
	DDX_Control(pDX, IDC_BTN_CONFIRM_TRANSACTION,   m_btnConfirm);
	DDX_Control(pDX, IDCANCEL,                      m_btnCancelar);
	DDX_Control(pDX, IDC_BTN_MAX_AMOUNT,            m_btnMaxAmount);
	// Title static control - bound so SetFont can be applied via the C++ member.
	DDX_Control(pDX, IDC_TXT_PROCESSAR_TRANSACAO,   m_txtTitulo);
}

BEGIN_MESSAGE_MAP(CTransactionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONFIRM_TRANSACTION, &CTransactionDlg::OnBnClickedBtnConfirmTransaction)
	ON_BN_CLICKED(IDC_BTN_MAX_AMOUNT, &CTransactionDlg::OnBnClickedBtnMaxAmount)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CTransactionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(L"Sistema Bancario - Processar Transacao");

	m_brBranco.CreateSolidBrush(RGB(255, 255, 255));
	m_brushCinza.CreateSolidBrush(RGB(240, 240, 240));

	// -28 logical units renders at roughly 21pt on screen - matches CreateAccountDlg.
	m_fontTitulo.CreateFont(
		-28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	// Applied to the DDX-bound member directly - more reliable than a fresh GetDlgItem() lookup.
	m_txtTitulo.SetFont(&m_fontTitulo);

	// -20 logical units renders at roughly 15pt on screen - matches CreateAccountDlg.
	m_fontBotao.CreateFont(
		-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	// -19 logical units renders at roughly 14pt on screen - matches CreateAccountDlg.
	m_fontLabels.CreateFont(
		-19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	GetDlgItem(IDC_TXT_LBL_TRANS_ACC)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_TXT_LBL_TRANS_TYPE)->SetFont(&m_fontLabels);
	// Reuses the transaction-type label to warn about the withdrawal fee, since
	// no separate label control exists in the resource layout for this notice.
	SetDlgItemText(IDC_TXT_LBL_TRANS_TYPE, L"Tipo de Transa\u00E7\u00E3o (levantamentos c/ taxa 1%):");
	GetDlgItem(IDC_TXT_LBL_TRANS_AMT)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_EDIT_TRANS_ACCOUNT)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_EDIT_TRANS_AMOUNT)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_COMBO_TRANS_TYPE)->SetFont(&m_fontLabels);

	// Windows' visual theming otherwise overrides SetFaceColor on CMFCButton,
	// silently ignoring the custom blue/white styling below.
	CMFCButton::EnableWindowsTheming(FALSE);

	const COLORREF corAzul   = RGB(0, 80, 200);
	const COLORREF corBranco = RGB(255, 255, 255);
	const COLORREF corPreto  = RGB(0, 0, 0);

	m_btnConfirm.SetFaceColor(corAzul, TRUE);
	m_btnConfirm.SetTextColor(corBranco);
	m_btnConfirm.SetImage(IDB_BITMAP17);
	m_btnConfirm.SetFont(&m_fontBotao);

	m_btnCancelar.SetFaceColor(corBranco, TRUE);
	m_btnCancelar.SetTextColor(corPreto);
	m_btnCancelar.SetImage(IDB_BITMAP9);
	m_btnCancelar.SetFont(&m_fontBotao);

	m_btnMaxAmount.SetFaceColor(corAzul, TRUE);
	m_btnMaxAmount.SetTextColor(corBranco);
	m_btnMaxAmount.SetImage(IDB_BITMAP18);
	m_btnMaxAmount.SetFont(&m_fontBotao);

	// Active account field is locked - its number is supplied by the caller (Dashboard), never typed by the user.
	CEdit* pEditConta = (CEdit*)GetDlgItem(IDC_EDIT_TRANS_ACCOUNT);
	pEditConta->SetReadOnly(TRUE);
	pEditConta->ModifyStyle(ES_CENTER | ES_RIGHT, ES_LEFT); // Forces left alignment to fix odd default spacing on this readonly field.

	// TrimLeft removes the padding spaces the number inherited from the table's icon-margin formatting (see UpdateAccountList).
	m_strAccountNum.TrimLeft();
	SetDlgItemText(IDC_EDIT_TRANS_ACCOUNT, m_strAccountNum);

	if (m_comboTransType.GetSafeHwnd() != NULL)
	{
		m_comboTransType.AddString(L"Deposito");
		m_comboTransType.AddString(L"Levantamento");
		m_comboTransType.SetCurSel(0);
	}
	else
	{
		AfxMessageBox(L"Aviso de Interface: O CComboBox falhou a carregar.", MB_ICONERROR | MB_OK);
	}

	return TRUE;
}

void CTransactionDlg::OnBnClickedBtnConfirmTransaction()
{
	// Routes the click through the same validated close path used for Enter/OnOK, instead of duplicating logic here.
	OnOK();
}

void CTransactionDlg::OnBnClickedBtnMaxAmount()
{
	// Forces the combo to Withdrawal regardless of what was previously selected - this button only makes sense for withdrawals.
	int nIdx = m_comboTransType.FindStringExact(-1, L"Levantamento");
	if (nIdx != CB_ERR)
	{
		m_comboTransType.SetCurSel(nIdx);
	}

	// The full balance already includes the 1% fee that will be added on top
	// at withdrawal time. To fully empty the account, the amount typed here
	// must be balance / 1.01 (the pre-fee requested amount), not the raw balance.
	// floor() truncates down (never rounds up) so that requested*1.01 can never
	// exceed the actual balance due to decimal rounding - see chat history for
	// the bug this fixes (withdrawal rejected even though "Levantar Todo" was used).
	double dValorPedido = floor((m_dCurrentBalance / 1.01) * 100.0) / 100.0;
	CString strSaldo;
	strSaldo.Format(L"%.2f", dValorPedido);
	SetDlgItemText(IDC_EDIT_TRANS_AMOUNT, strSaldo);
}

void CTransactionDlg::OnOK()
{
	if (!UpdateData(TRUE)) {
		AfxMessageBox(L"Erro: Falha no Data Exchange. Verifica o ID do campo montante.", MB_ICONERROR | MB_OK);
		return;
	}

	m_amount = _ttof(m_strAmountInput);

	if (m_amount <= 0.0) {
		AfxMessageBox(L"Por favor, insira um montante valido e positivo.", MB_ICONWARNING | MB_OK);
		return;
	}

	// Amounts above this cap are rejected here in the UI to avoid triggering a
	// known crash further down in afxwin2.inl on overflow-prone conversions.
	const double MAX_MONTANTE = 999999999.99;
	if (m_amount > MAX_MONTANTE) {
		AfxMessageBox(L"Montante demasiado elevado. Limite maximo: 999 999 999.99 EUR.", MB_ICONWARNING | MB_OK);
		return;
	}

	// Captured BEFORE the dialog is destroyed by the base OnOK(): reading the
	// CComboBox after DoModal() returns would hit an invalid HWND and always report 0 (Deposit).
	if (m_comboTransType.GetSafeHwnd() != NULL) {
		m_nTransactionType = m_comboTransType.GetCurSel();
	}

	CDialogEx::OnOK();
}

int CTransactionDlg::GetTransactionType() const
{
	// Returns the value captured in OnOK() before the dialog closed - the live combo box is no longer safe to read at this point.
	return m_nTransactionType;
}

double CTransactionDlg::GetAmount() const
{
	return m_amount;
}

// Repaints static text/background per-control, since MFC's default dialog
// theme cannot express "readonly field grey, title blue, rest black" without this hook.
HBRUSH CTransactionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// Grey background signals to the user that the active-account field is locked/non-editable.
	if (nCtlColor == CTLCOLOR_EDIT && pWnd &&
		pWnd->GetDlgCtrlID() == IDC_EDIT_TRANS_ACCOUNT)
	{
		pDC->SetBkColor(RGB(240, 240, 240));
		pDC->SetTextColor(RGB(80, 80, 80));
		return (HBRUSH)m_brushCinza.GetSafeHandle();
	}

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));

		// Only the dialog title gets the corporate blue; every other label stays black.
		if (pWnd && pWnd->GetDlgCtrlID() == IDC_TXT_PROCESSAR_TRANSACAO)
			pDC->SetTextColor(RGB(0, 80, 200));
		else
			pDC->SetTextColor(RGB(0, 0, 0));

		return (HBRUSH)m_brBranco.GetSafeHandle();
	}

	return hbr;
}
