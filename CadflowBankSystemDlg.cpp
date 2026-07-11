
// CadflowBankSystemDlg.cpp: arquivo de implementacao
//

#include "pch.h"
#include "framework.h"
#include "CadflowBankSystem.h"
#include "CadflowBankSystemDlg.h"
#include "afxdialogex.h"
#include "CreateAccountDlg.h"
#include "TransactionDlg.h"
#include "AccountDetailsDlg.h"
#include "TransferDlg.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog box used for the application's About screen

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Suporte DDX/DDV

// Implementacao
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCadflowBankSystemDlg dialog box



CCadflowBankSystemDlg::CCadflowBankSystemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CADFLOWBANKSYSTEM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCadflowBankSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACCOUNTS,           m_listaContas);
	DDX_Control(pDX, IDC_TXT_CONTAS_ATIVAS,       m_txtTituloContas);
	// DDX_Control (not just GetDlgItem) is required for every button below:
	// SetFaceColor/SetTextColor on a CMFCButton only take effect through the bound C++ member.
	DDX_Control(pDX, IDC_BTN_CREATE_ACCOUNT,      m_btnCreate);
	DDX_Control(pDX, IDC_BTN_PROCESS_TRANSACTION, m_btnTransaction);
	DDX_Control(pDX, IDC_BTN_VIEW_DETAILS,        m_btnDetails);
	DDX_Control(pDX, IDC_BTN_DELETE_ACCOUNT,      m_btnDelete);
	DDX_Control(pDX, IDC_BTN_TRANSFER,            m_btnTransfer);
}

BEGIN_MESSAGE_MAP(CCadflowBankSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CREATE_ACCOUNT,       &CCadflowBankSystemDlg::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_BTN_PROCESS_TRANSACTION,  &CCadflowBankSystemDlg::OnBnClickedBtnTransaction)
	ON_BN_CLICKED(IDC_BTN_VIEW_DETAILS,         &CCadflowBankSystemDlg::OnBnClickedBtnDetails)
	ON_BN_CLICKED(IDC_BTN_DELETE_ACCOUNT,       &CCadflowBankSystemDlg::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_TRANSFER,             &CCadflowBankSystemDlg::OnBnClickedBtnTransfer)
END_MESSAGE_MAP()


BOOL CCadflowBankSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(L"Sistema Bancario - Dashboard Principal");

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_brBranco.CreateSolidBrush(RGB(255, 255, 255));

	// -40 logical units renders at roughly 30pt on screen.
	m_fontTitulo.CreateFont(
		-40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	// Applied to the DDX-bound member directly - more reliable than a fresh GetDlgItem() lookup.
	m_txtTituloContas.SetFont(&m_fontTitulo);

	// Windows' visual theming otherwise overrides SetFaceColor on CMFCButton,
	// silently ignoring the custom blue/white styling below.
	CMFCButton::EnableWindowsTheming(FALSE);
	const COLORREF corAzul   = RGB(0, 80, 200);
	const COLORREF corBranco = RGB(255, 255, 255);

	m_btnCreate.SetFaceColor(corAzul, TRUE);
	m_btnCreate.SetTextColor(corBranco);
	m_btnCreate.SetImage(IDB_BITMAP4);

	m_btnTransaction.SetFaceColor(corAzul, TRUE);
	m_btnTransaction.SetTextColor(corBranco);
	m_btnTransaction.SetImage(IDB_BITMAP5);

	m_btnDetails.SetFaceColor(corAzul, TRUE);
	m_btnDetails.SetTextColor(corBranco);
	m_btnDetails.SetImage(IDB_BITMAP6);

	m_btnDelete.SetFaceColor(corAzul, TRUE);
	m_btnDelete.SetTextColor(corBranco);
	m_btnDelete.SetImage(IDB_BITMAP10);

	m_btnTransfer.SetFaceColor(corAzul, TRUE);
	m_btnTransfer.SetTextColor(corBranco);
	m_btnTransfer.SetImage(IDB_BITMAP16);

	// -20 logical units renders at roughly 15pt on screen.
	m_fontBotao.CreateFont(
		-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
	);
	m_btnCreate.SetFont(&m_fontBotao);
	m_btnTransaction.SetFont(&m_fontBotao);
	m_btnDetails.SetFont(&m_fontBotao);
	m_btnDelete.SetFont(&m_fontBotao);
	m_btnTransfer.SetFont(&m_fontBotao);

	// 24x24 matches user.bmp's native resolution; anything smaller would crop the icon.
	m_imgListContas.Create(24, 24, ILC_COLOR32 | ILC_MASK, 1, 0);
	CBitmap bmpUser;
	bmpUser.LoadBitmap(IDB_BITMAP3);
	m_imgListContas.Add(&bmpUser, RGB(255, 255, 255));
	m_listaContas.SetImageList(&m_imgListContas, LVSIL_SMALL);

	m_listaContas.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	// LVS_SHOWSELALWAYS keeps the selection highlight visible even when the
	// table loses keyboard focus (e.g. while a modal dialog is open on top of it).
	m_listaContas.ModifyStyle(0, LVS_SHOWSELALWAYS);

	// Column widths are computed from the table's actual on-screen width
	// (not hardcoded), so the layout still looks correct at other DPI/resolutions.
	CRect rcLista;
	m_listaContas.GetClientRect(&rcLista);
	int nLargura = rcLista.Width();

	// 34px reserved for the icon (24px icon + 10px margin); the remaining
	// width is split evenly across the 3 columns.
	const int nIconPad = 34;
	int nTexto = (nLargura - nIconPad) / 3;
	int nCol0  = nTexto + nIconPad; // Account number: text space + icon.
	int nCol1  = nTexto;            // Owner: text space only.
	int nCol2  = nLargura - nCol0 - nCol1; // Balance: absorbs the rounding remainder.

	m_listaContas.InsertColumn(0, L"N\u00BA Conta",  LVCFMT_LEFT, nCol0);
	m_listaContas.InsertColumn(1, L"Titular",        LVCFMT_LEFT, nCol1);
	m_listaContas.InsertColumn(2, L"Saldo Atual",    LVCFMT_LEFT, nCol2);

	UpdateAccountList();

	return TRUE;
}

void CCadflowBankSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, the code below is needed to
// paint the icon. For MFC applications using the document/view model, this
// is done automatically by the framework.

void CCadflowBankSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centers the icon within the client rect, regardless of the window's current size.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to get the cursor to display while the user
// drags the minimized window.
HCURSOR CCadflowBankSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// Manual digit-by-digit formatting (rather than std::locale) so the output is
// guaranteed to use European conventions ('.' thousands, ',' decimal) regardless
// of the end-user's system locale, and stays correct even for very large
// (billions-scale) balances that std::locale grouping can mishandle inconsistently.
CString CCadflowBankSystemDlg::FormatCurrency(double dValor) const
{
	CString strBase;
	strBase.Format(L"%.2f", dValor);

	int nPontoIdx = strBase.Find(L'.');
	CString strParteInteira = strBase.Left(nPontoIdx);
	CString strParteDecimal = strBase.Mid(nPontoIdx + 1);

	// Sign is stripped and re-applied at the end, so the grouping loop below
	// only ever has to deal with digits.
	bool bNegativo = false;
	if (!strParteInteira.IsEmpty() && strParteInteira[0] == L'-')
	{
		bNegativo = true;
		strParteInteira = strParteInteira.Mid(1);
	}

	// Inserts a '.' every 3 digits counting from the right (thousands groups),
	// never before the first digit and never after the last.
	CString strComPontos;
	int nLen = strParteInteira.GetLength();
	for (int i = 0; i < nLen; ++i)
	{
		strComPontos += strParteInteira[i];
		int nRestantes = nLen - i - 1;
		if (nRestantes > 0 && (nRestantes % 3) == 0)
		{
			strComPontos += L'.';
		}
	}

	CString strResultado;
	strResultado.Format(L"%s%s,%s \u20AC", bNegativo ? L"-" : L"", strComPontos, strParteDecimal);
	return strResultado;
}

// Full rebuild (DeleteAllItems + re-InsertItem for every account) rather than
// incremental diffing: simpler and safe given the current account volumes,
// but it is why nSelectAccount exists below - a full rebuild would otherwise
// silently drop the user's current selection on every single refresh.
void CCadflowBankSystemDlg::UpdateAccountList(int nSelectAccount)
{
	m_listaContas.DeleteAllItems();

	const std::deque<Account>& contas = m_bankManager.GetAllAccounts();

	for (int i = 0; i < (int)contas.size(); ++i)
	{
		const Account& conta = contas[i];

		CString strNum;
		strNum.Format(L"    %d", conta.getAccountNumber()); // Leading spaces reserve room for the user icon drawn in the same column.
		LVITEM lvi = {};
		lvi.mask    = LVIF_TEXT | LVIF_IMAGE;
		lvi.iItem   = i;
		lvi.iSubItem = 0;
		lvi.pszText = strNum.GetBuffer();
		lvi.iImage  = 0;
		int nIndex = m_listaContas.InsertItem(&lvi);
		strNum.ReleaseBuffer(); // Must pair every GetBuffer() call, or the CString is left in an invalid locked state.

		// Re-selects the account the caller asked to keep in focus (e.g. the
		// one just created/modified), since the DeleteAllItems() above cleared any prior selection.
		if (conta.getAccountNumber() == nSelectAccount)
		{
			m_listaContas.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			m_listaContas.EnsureVisible(nIndex, FALSE);
		}

		// Account::getOwnerName() returns a narrow std::string; CStringA/CString
		// bridges it back into the Unicode UI world.
		CStringA strNomeA(conta.getOwnerName().c_str());
		m_listaContas.SetItemText(i, 1, CString(strNomeA));

		CString strSaldo = FormatCurrency(conta.getBalance());
		m_listaContas.SetItemText(i, 2, strSaldo);
	}
}

void CCadflowBankSystemDlg::OnBnClickedBtnCreate()
{
	CCreateAccountDlg dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		// pNovaConta is only read once, immediately, to extract the ID -
		// per BankManager's contract, this pointer is not held onto afterwards.
		Account* pNovaConta = m_bankManager.CreateAccount(dlg.GetOwnerName());
		UpdateAccountList(pNovaConta != nullptr ? pNovaConta->getAccountNumber() : -1);
	}
}

void CCadflowBankSystemDlg::OnBnClickedBtnTransaction()
{
	int nSel = m_listaContas.GetNextItem(-1, LVNI_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox(L"Seleciona uma conta na lista antes de processar uma transacao.", MB_ICONWARNING | MB_OK);
		return;
	}

	CString strNum = m_listaContas.GetItemText(nSel, 0);

	CTransactionDlg dlg(this);
	dlg.m_strAccountNum = strNum;

	// Balance is read and copied into the dialog *before* DoModal(), because
	// pContaAtiva is a raw pointer into BankManager's storage - it must not be
	// dereferenced after the modal loop returns, in case anything mutated accounts in between.
	Account* pContaAtiva = m_bankManager.GetAccount(_ttoi(strNum));
	if (pContaAtiva != nullptr)
	{
		dlg.m_dCurrentBalance = pContaAtiva->getBalance();
	}

	if (dlg.DoModal() == IDOK)
	{
		int nConta = _ttoi(strNum);

		bool ok = m_bankManager.ProcessTransaction(nConta, dlg.GetTransactionType(), dlg.GetAmount());

		if (!ok)
		{
			// Withdrawal failures get a more specific message, since the real
			// cause is often the 1% fee pushing the total just over the balance -
			// not obvious from the raw numbers the user typed.
			if (dlg.GetTransactionType() == 1) // 1 = Withdrawal in the combo box
			{
				AfxMessageBox(L"Transacao recusada. O saldo disponivel pode nao cobrir o montante pedido mais a taxa de 1% sobre levantamentos.", MB_ICONERROR | MB_OK);
			}
			else
			{
				AfxMessageBox(L"Transacao recusada. Verifica o saldo disponivel e o valor inserido.", MB_ICONERROR | MB_OK);
			}
		}

		// Refreshed even on failure, so the table always reflects the engine's
		// real current state (defensive: covers any future case where a "failed"
		// transaction still partially mutated data).
		UpdateAccountList(nConta);
	}
}

void CCadflowBankSystemDlg::OnBnClickedBtnDetails()
{
	int nSel = m_listaContas.GetNextItem(-1, LVNI_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox(L"Seleciona uma conta na lista para ver os seus detalhes.", MB_ICONWARNING | MB_OK);
		return;
	}

	CString strNum = m_listaContas.GetItemText(nSel, 0);
	int nConta = _ttoi(strNum);
	Account* pConta = m_bankManager.GetAccount(nConta);

	// Defensive guard only: a row visible in the table always corresponds to a
	// live account at this point, since no mutation happens between the table
	// read above and this lookup.
	if (pConta == nullptr) return;

	// Everything the details dialog needs is copied out into plain CString/vector
	// members *before* DoModal(), so the dialog never needs to touch pConta or
	// BankManager itself - it stays a pure read-only view (see AccountDetailsDlg.h).
	CAccountDetailsDlg dlg(this);
	CStringA strNomeA(pConta->getOwnerName().c_str());
	dlg.m_strOwnerName     = CString(strNomeA);
	dlg.m_strAccountNumber = strNum;
	dlg.m_strBalance = FormatCurrency(pConta->getBalance());
	dlg.m_transactionHistory = pConta->getTransactionHistory();

	dlg.DoModal();
}

void CCadflowBankSystemDlg::OnBnClickedBtnDelete()
{
	int nSel = m_listaContas.GetNextItem(-1, LVNI_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox(L"Selecione uma conta na lista.", MB_ICONWARNING | MB_OK);
		return;
	}

	CString strNum = m_listaContas.GetItemText(nSel, 0);
	int nConta = _ttoi(strNum);

	// Deletion is irreversible (no undo/trash), so a confirmation prompt is
	// mandatory before it reaches BankManager.
	int resposta = AfxMessageBox(L"Tem a certeza que deseja apagar esta conta?", MB_YESNO | MB_ICONWARNING);
	if (resposta == IDYES)
	{
		bool sucesso = m_bankManager.DeleteAccount(nConta);
		if (!sucesso)
		{
			AfxMessageBox(L"Erro ao eliminar a conta.", MB_ICONERROR | MB_OK);
			return;
		}

		// No account to re-select after a successful delete, so the default (-1 = none) applies.
		UpdateAccountList();
	}
}

void CCadflowBankSystemDlg::OnBnClickedBtnTransfer()
{
	int nSel = m_listaContas.GetNextItem(-1, LVNI_SELECTED);
	if (nSel == -1)
	{
		AfxMessageBox(L"Selecione a conta de origem na lista.", MB_ICONWARNING | MB_OK);
		return;
	}

	CString strNum = m_listaContas.GetItemText(nSel, 0);

	// BankManager is passed in (not copied) so TransferDlg can populate its
	// destination combo box from the live account list without this dialog
	// having to pre-fetch and hand over a snapshot.
	TransferDlg dlg(this, &m_bankManager);
	dlg.m_strSourceAccountNum = strNum;

	if (dlg.DoModal() == IDOK)
	{
		int nContaOrigem = _ttoi(strNum);
		int nContaDestino = dlg.GetDestAccountNumber();
		double dAmount = dlg.GetAmount();

		// TransferTo() is atomic (with manual rollback on partial failure) -
		// see BankManager.cpp for details.
		bool sucesso = m_bankManager.TransferTo(nContaOrigem, nContaDestino, dAmount);

		if (!sucesso)
		{
			AfxMessageBox(L"Erro ao processar a transferencia.", MB_ICONERROR | MB_OK);
		}

		// Re-selects the source account even on failure, since that is still
		// the account the user was working with.
		UpdateAccountList(nContaOrigem);
	}
}

// Repaints static text/background per-control, since MFC's default dialog
// theme cannot express "one title blue, everything else black" without this hook.
HBRUSH CCadflowBankSystemDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkColor(RGB(255, 255, 255));

		// Only the "Contas Ativas" title gets the corporate blue; every other label stays black.
		if (pWnd && pWnd->GetDlgCtrlID() == IDC_TXT_CONTAS_ATIVAS)
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

