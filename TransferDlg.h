#pragma once
#include "afxdialogex.h"
#include "afxbutton.h"

class BankManager;

/**
 * @class TransferDlg
 * @brief Modal dialog for transferring funds from one account to another.
 *
 * The caller must set m_strSourceAccountNum before calling DoModal() so the
 * source account is displayed (read-only). Requires a valid BankManager*
 * to populate the destination combo box and to pre-check the source
 * account's balance. After a successful IDOK, GetDestAccountNumber() and
 * GetAmount() expose the validated results for BankManager::TransferTo().
 */
class TransferDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TransferDlg)

public:
	/**
	 * @brief Constructs the dialog.
	 * @param pParent Owner window, or nullptr for the application's main window.
	 * @param pBankManager Pointer to the engine instance, used to list destination accounts and pre-check the source balance.
	 */
	TransferDlg(CWnd* pParent = nullptr, BankManager* pBankManager = nullptr);   // standard constructor
	virtual ~TransferDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRANSFER };
#endif

public:
	CString m_strSourceAccountNum;
	CComboBox m_comboDestino;
	CString m_strAmountInput;

	CMFCButton m_btnConfirm;
	CMFCButton m_btnCancelar;

	/**
	 * @brief Returns the validated transfer amount.
	 * @return The positive amount typed by the user, in EUR.
	 */
	double GetAmount() const;

	/**
	 * @brief Returns the destination account number selected by the user.
	 * @return The real account number, recovered via CComboBox::GetItemData() (not parsed from the display text).
	 */
	int GetDestAccountNumber() const;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	/**
	 * @brief Initializes fonts, button styling, and populates the destination-account combo box.
	 * @return TRUE to let the framework set the input focus.
	 */
	virtual BOOL OnInitDialog();

	/**
	 * @brief Validates the destination account and amount, then closes the dialog with IDOK on success.
	 *
	 * Rejects an unselected destination, an empty or non-positive amount,
	 * a destination equal to the source account, and an amount exceeding
	 * the source account's current balance (a UX pre-check only;
	 * BankManager::TransferTo() re-validates independently).
	 */
	virtual void OnOK();

	BankManager* m_pBankManager;
	double m_amount;
	int m_nDestAccountNumber;

	// Brush and fonts for the white/blue visual style (mirrors TransactionDlg).
	CBrush  m_brBranco;
	CBrush  m_brushCinza;
	CFont   m_fontTitulo;
	CFont   m_fontBotao;
	CFont   m_fontLabels;
	CStatic m_txtTitulo;

	/**
	 * @brief Paints the readonly source-account field grey and the title blue.
	 */
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	/**
	 * @brief Routes the "Confirm" button click to OnOK() for validated closing.
	 */
	afx_msg void OnBnClickedBtnConfirmTransfer();
};
