#pragma once
#include "afxdialogex.h"
#include "afxbutton.h"
#include "resource.h"
#include <vector>
#include "Transaction.h"

/**
 * @class CAccountDetailsDlg
 * @brief Read-only dialog that displays a single account's identity, balance and full transaction history.
 *
 * This dialog owns no business logic: all data (owner name, account number,
 * formatted balance, transaction history) must be populated by the caller
 * (CCadflowBankSystemDlg) on the public members below, before DoModal() is invoked.
 */
class CAccountDetailsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAccountDetailsDlg)

public:
	/**
	 * @brief Constructs the dialog. Does not populate any data - the caller must set the public members before calling DoModal().
	 * @param pParent Optional parent window.
	 */
	CAccountDetailsDlg(CWnd* pParent = nullptr);

	/**
	 * @brief Destructor.
	 */
	virtual ~CAccountDetailsDlg();

// Data Window
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ACCOUNT_DETAILS };
#endif

public:
	CString m_strOwnerName;     ///< Owner name, already converted to CString by the caller.
	CString m_strAccountNumber; ///< Account number, already formatted as text by the caller.
	CString m_strBalance;       ///< Balance, already formatted (thousands/decimal separators, currency symbol) by the caller.

	/// Copy of the account's transaction history, handed over by value so this
	/// read-only view can never mutate the engine's real data.
	std::vector<Transaction> m_transactionHistory;

private:
	CListCtrl m_listTransactions; ///< Table listing every transaction in m_transactionHistory.

	CBrush m_brBranco; ///< White background brush, applied via OnCtlColor.

	// The 4 fonts below implement the visual hierarchy: section titles, normal
	// labels/values, the oversized balance figure, and the bold button text.
	CFont m_fontTitulos;
	CFont m_fontNormal;
	CFont m_fontSaldoGrande;
	CFont m_fontBotao;

	/// Icons for the transaction table: green arrow (index 0, deposit), red arrow (index 1, withdrawal).
	CImageList m_imgListTrans;

	CMFCButton m_btnClose; ///< "Close" button, bound via DDX_Control so SetFaceColor/SetTextColor take effect.

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Support DDX/DDV
	virtual BOOL OnInitDialog();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnClose();

	DECLARE_MESSAGE_MAP()
};
