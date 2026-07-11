
// CadflowBankSystemDlg.h: header file
//

#pragma once

// Pulls in the account-management engine (business logic only, no UI coupling).
#include "BankManager.h"
#include "afxdialogex.h"
#include "afxbutton.h"

/**
 * @class CCadflowBankSystemDlg
 * @brief Main dashboard dialog: lists every account and launches all other dialogs (create, transaction, details, delete, transfer).
 *
 * Owns the single BankManager instance for the whole application lifetime.
 * Every button handler here follows the same pattern: read the current
 * selection from m_listaContas, delegate the actual business logic to
 * m_bankManager, then call UpdateAccountList() to refresh the table from
 * the engine's post-operation state.
 */
class CCadflowBankSystemDlg : public CDialogEx
{
// Constructs 
public:
	/**
	 * @brief Constructs the dialog.
	 * @param pParent Optional parent window (unused; this is the app's top-level window).
	 */
	CCadflowBankSystemDlg(CWnd* pParent = nullptr);

// Data Window
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CADFLOWBANKSYSTEM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // Support DDX/DDV

// Implementation
protected:
	HICON m_hIcon;

	/// Business engine - single instance owning every account for the lifetime of this dialog.
	BankManager m_bankManager;

	/// Main table listing every account (number, owner, balance).
	CListCtrl m_listaContas;

	CBrush m_brBranco; ///< White background brush, applied via OnCtlColor.
	CFont m_fontTitulo; ///< Large bold font for the "Contas Ativas" title.
	CStatic m_txtTituloContas; ///< Title control, bound via DDX_Control so SetFont takes effect.

	/// Icon list providing the small user icon shown in the account-number column.
	CImageList m_imgListContas;

	// The 5 dashboard action buttons - bound via DDX_Control (required for SetFaceColor/SetTextColor to work).
	CMFCButton m_btnCreate;
	CMFCButton m_btnTransaction;
	CMFCButton m_btnDetails;
	CMFCButton m_btnDelete;
	CMFCButton m_btnTransfer;

	CFont m_fontBotao; ///< Bold font shared by all 5 dashboard buttons.

	/**
	 * @brief Clears and repopulates m_listaContas from BankManager::GetAllAccounts().
	 * @param nSelectAccount Account number to re-select and scroll into view after the refresh (default -1 = no selection). Needed because a full DeleteAllItems()/InsertItem() cycle would otherwise lose the user's current selection on every table refresh.
	 */
	void UpdateAccountList(int nSelectAccount = -1);

	/**
	 * @brief Formats a raw double balance for display: thousands separator '.', decimal separator ',', trailing euro symbol.
	 * @param dValor The raw balance value.
	 * @return CString The formatted string, e.g. "1.000.000.000,00 €".
	 */
	CString FormatCurrency(double dValor) const;

	// Message map generated functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	/// Opens CCreateAccountDlg; on IDOK, creates the account in the engine and re-selects it in the table.
	afx_msg void OnBnClickedBtnCreate();
	/// Opens CTransactionDlg for the selected account; on IDOK, applies the deposit/withdrawal and refreshes the table.
	afx_msg void OnBnClickedBtnTransaction();
	/// Opens CAccountDetailsDlg (read-only) with the selected account's identity, balance and transaction history.
	afx_msg void OnBnClickedBtnDetails();
	/// Confirms with the user, then permanently removes the selected account from the engine.
	afx_msg void OnBnClickedBtnDelete();
	/// Opens TransferDlg with the selected account as source; on IDOK, moves funds to the chosen destination account.
	afx_msg void OnBnClickedBtnTransfer();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};
