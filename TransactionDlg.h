#pragma once
#include "afxdialogex.h"
#include "afxbutton.h"
#include "resource.h"

/**
 * @class CTransactionDlg
 * @brief Modal dialog for processing a deposit or withdrawal on a given account.
 *
 * The caller must set m_strAccountNum and m_dCurrentBalance before calling
 * DoModal(), so the dialog can display the active account and let the
 * "Levantar Todo" button compute the maximum safely-withdrawable amount.
 * After a successful IDOK, GetTransactionType() and GetAmount() expose the
 * validated results for BankManager::ProcessTransaction().
 */
class CTransactionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTransactionDlg)

public:
	/**
	 * @brief Constructs the dialog.
	 * @param pParent Owner window, or nullptr for the application's main window.
	 */
	CTransactionDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTransactionDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRANSACTION };
#endif

public:
	CString m_strAccountNum;
	CComboBox m_comboTransType;
	double m_amount;

	// Current balance of the active account, filled in by the Dashboard before DoModal().
	double m_dCurrentBalance;

	CString m_strAmountInput;

	// Captured in OnOK() before the dialog is destroyed
	// (after DoModal() returns, m_comboTransType.m_hWnd is NULL and GetCurSel() crashes).
	int m_nTransactionType;

	// Bound via DDX_Control (required for SetFaceColor to take effect).
	CMFCButton m_btnConfirm;
	CMFCButton m_btnCancelar;
	CMFCButton m_btnMaxAmount;

	/**
	 * @brief Returns the transaction type selected by the user.
	 * @return 0 for Deposit, 1 for Withdrawal.
	 */
	int GetTransactionType() const;

	/**
	 * @brief Returns the validated transaction amount.
	 * @return The positive amount typed by the user, in EUR (before any fee is applied).
	 */
	double GetAmount() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/**
	 * @brief Initializes fonts, button styling, and populates the transaction-type combo.
	 * @return TRUE to let the framework set the input focus.
	 */
	virtual BOOL OnInitDialog();

	/**
	 * @brief Validates the typed amount and closes the dialog with IDOK on success.
	 *
	 * Rejects amounts that are not strictly positive or exceed the interface's
	 * overflow-safety cap, then caches the combo box selection before the base
	 * class destroys the dialog.
	 */
	virtual void OnOK();

	// Brush and fonts for the white/blue visual style (mirrors CreateAccountDlg).
	CBrush  m_brBranco;     // Dialog's white background.
	CBrush  m_brushCinza;   // Grey background for the readonly field (active account).
	CFont   m_fontTitulo;   // Large font for the title.
	CFont   m_fontBotao;    // Bold font for button text.
	CFont   m_fontLabels;   // Larger font for labels and inputs.
	CStatic m_txtTitulo;    // Title static control, bound via DDX.

	/**
	 * @brief Paints the readonly account field grey and the title blue.
	 */
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	/**
	 * @brief Routes the "Confirm" button click to OnOK() for validated closing.
	 */
	afx_msg void OnBnClickedBtnConfirmTransaction();

	/**
	 * @brief Selects "Levantamento" and fills the amount field with the
	 * maximum amount that can be withdrawn given the 1% fee and current balance.
	 */
	afx_msg void OnBnClickedBtnMaxAmount();
};
