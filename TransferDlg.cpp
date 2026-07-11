// TransferDlg.cpp : implementation file
//

#include "pch.h"
#include "CadflowBankSystem.h"
#include "afxdialogex.h"
#include "afxbutton.h"
#include "TransferDlg.h"
#include "BankManager.h"


// TransferDlg dialog box

IMPLEMENT_DYNAMIC(TransferDlg, CDialogEx)

TransferDlg::TransferDlg(CWnd* pParent /*=nullptr*/, BankManager* pBankManager /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TRANSFER, pParent), m_pBankManager(pBankManager),
	  m_amount(0.0), m_nDestAccountNumber(-1)
{

}

TransferDlg::~TransferDlg()
{
}

void TransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// Bindings for the visual controls
	DDX_Control(pDX, IDC_COMBO_TRANSFER_DEST,     m_comboDestino);
	DDX_Text(pDX,    IDC_EDIT_TRANSFER_AMOUNT,    m_strAmountInput);
	// Buttons - without DDX_Control, SetFaceColor is ignored by Windows theming.
	DDX_Control(pDX, IDC_BTN_CONFIRM_TRANSFER,    m_btnConfirm);
	DDX_Control(pDX, IDCANCEL,                    m_btnCancelar);
	// Title static control - bound so SetFont can be applied via the C++ member.
	DDX_Control(pDX, IDC_TXT_PROCESSAR_TRANSACAO, m_txtTitulo);
}


BEGIN_MESSAGE_MAP(TransferDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CONFIRM_TRANSFER, &TransferDlg::OnBnClickedBtnConfirmTransfer)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// TransferDlg message handlers

BOOL TransferDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(L"Sistema Bancario - Transferir Fundos");

	m_brBranco.CreateSolidBrush(RGB(255, 255, 255));
	m_brushCinza.CreateSolidBrush(RGB(240, 240, 240));

	// -28 logical units renders at roughly 21pt on screen - matches TransactionDlg.
	m_fontTitulo.CreateFont(
		-28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	m_txtTitulo.SetFont(&m_fontTitulo);

	// -20 logical units renders at roughly 15pt on screen.
	m_fontBotao.CreateFont(
		-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);

	// -19 logical units renders at roughly 14pt on screen.
	m_fontLabels.CreateFont(
		-19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	GetDlgItem(IDC_TXT_LBL_TRANSFER_SOURCE)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_TXT_LBL_TRANSFER_DEST)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_TXT_LBL_TRANSFER_AMOUNT)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_EDIT_TRANSFER_SOURCE)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_EDIT_TRANSFER_AMOUNT)->SetFont(&m_fontLabels);
	GetDlgItem(IDC_COMBO_TRANSFER_DEST)->SetFont(&m_fontLabels);

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

	// Source account field is locked - its number comes from the row the user
	// had selected on the dashboard, never typed manually here.
	CEdit* pEditOrigem = (CEdit*)GetDlgItem(IDC_EDIT_TRANSFER_SOURCE);
	pEditOrigem->SetReadOnly(TRUE);

	m_strSourceAccountNum.TrimLeft();
	SetDlgItemText(IDC_EDIT_TRANSFER_SOURCE, m_strSourceAccountNum);

	// Populates the destination combo with every account except the source one.
	if (m_pBankManager != nullptr && m_comboDestino.GetSafeHwnd() != NULL)
	{
		int nContaOrigem = _ttoi(m_strSourceAccountNum);
		const std::deque<Account>& contas = m_pBankManager->GetAllAccounts();

		for (const auto& conta : contas)
		{
			if (conta.getAccountNumber() == nContaOrigem) continue; // Cannot transfer to the source account itself.

			CStringA strNomeA(conta.getOwnerName().c_str());
			CString strItem;
			strItem.Format(L"%d - %s", conta.getAccountNumber(), CString(strNomeA));

			int nIdx = m_comboDestino.AddString(strItem);
			m_comboDestino.SetItemData(nIdx, conta.getAccountNumber());
		}

		// No destination accounts exist: surface this directly in the combo
		// and disable it, instead of letting the user pick nothing and fail later in OnOK().
		if (m_comboDestino.GetCount() == 0)
		{
			m_comboDestino.AddString(L"Nao foram encontradas contas.");
			m_comboDestino.SetCurSel(0);
			m_comboDestino.EnableWindow(FALSE);
		}
	}
	else
	{
		AfxMessageBox(L"Aviso de Interface: O CComboBox de destino falhou a carregar.", MB_ICONERROR | MB_OK);
	}

	return TRUE;
}

void TransferDlg::OnBnClickedBtnConfirmTransfer()
{
	// Routes the click through the same validated close path used for Enter/OnOK, instead of duplicating logic here.
	OnOK();
}

void TransferDlg::OnOK()
{
	// No real destination accounts available: block before running any other
	// validation, since GetItemData() below would read meaningless placeholder data otherwise.
	if (!m_comboDestino.IsWindowEnabled())
	{
		AfxMessageBox(L"Nao existem contas disponiveis para transferencia.", MB_ICONWARNING | MB_OK);
		return;
	}

	if (!UpdateData(TRUE))
	{
		AfxMessageBox(L"Erro: Falha no Data Exchange. Verifica o ID do campo montante.", MB_ICONERROR | MB_OK);
		return;
	}

	int nIdxDestino = m_comboDestino.GetCurSel();
	if (nIdxDestino == CB_ERR)
	{
		AfxMessageBox(L"Selecione a conta de destino.", MB_ICONWARNING | MB_OK);
		return;
	}

	if (m_strAmountInput.IsEmpty())
	{
		AfxMessageBox(L"Indique o montante a transferir.", MB_ICONWARNING | MB_OK);
		return;
	}

	m_amount = _ttof(m_strAmountInput);
	if (m_amount <= 0.0)
	{
		AfxMessageBox(L"Por favor, insira um montante valido e positivo.", MB_ICONWARNING | MB_OK);
		return;
	}

	// The combo box display text is "id - name"; the real account number was
	// stashed via SetItemData when the list was populated, since it cannot be parsed back reliably from the display string.
	m_nDestAccountNumber = (int)m_comboDestino.GetItemData(nIdxDestino);

	int nContaOrigem = _ttoi(m_strSourceAccountNum);
	if (nContaOrigem == m_nDestAccountNumber)
	{
		AfxMessageBox(L"Nao pode transferir para a mesma conta.", MB_ICONWARNING | MB_OK);
		return;
	}

	// This balance check is a UX pre-check only - BankManager::TransferTo()
	// re-validates independently and is the real source of truth.
	if (m_pBankManager != nullptr)
	{
		Account* pContaOrigem = m_pBankManager->GetAccount(nContaOrigem);
		if (pContaOrigem != nullptr && m_amount > pContaOrigem->getBalance())
		{
			AfxMessageBox(L"Saldo insuficiente na conta de origem.", MB_ICONWARNING | MB_OK);
			return;
		}
	}

	CDialogEx::OnOK();
}

int TransferDlg::GetDestAccountNumber() const
{
	return m_nDestAccountNumber;
}

double TransferDlg::GetAmount() const
{
	return m_amount;
}

// Repaints static text/background per-control, since MFC's default dialog
// theme cannot express "readonly field grey, title blue, rest black" without this hook.
HBRUSH TransferDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// Grey background signals to the user that the source-account field is locked/non-editable.
	if (nCtlColor == CTLCOLOR_EDIT && pWnd &&
		pWnd->GetDlgCtrlID() == IDC_EDIT_TRANSFER_SOURCE)
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
