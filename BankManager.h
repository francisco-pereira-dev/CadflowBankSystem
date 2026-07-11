#pragma once
#include <vector>
#include <deque>
#include <afx.h>
#include <fstream>
#include <sstream>
#include <string>
#include "Account.h"

/**
 * @class BankManager
 * @brief Owns and coordinates every Account in the system: creation, lookup, transactions, transfers, deletion and file persistence.
 *
 * This is the single source of truth for account data. UI dialogs never touch
 * accounts directly - they always go through this class, so every state
 * mutation flows through one place and is (best-effort) persisted to
 * cadflow_data.txt immediately after.
 *
 * @warning GetAccount()/CreateAccount() return raw Account* pointers into the
 * internal m_accounts container. std::deque guarantees these pointers survive
 * push_back(), but they are still invalidated by DeleteAccount() (erase) for
 * the erased element and any element after it. Callers must not hold onto an
 * Account* across a call that can mutate m_accounts.
 */
class BankManager
{
private:
	// std::deque instead of std::vector: push_back never invalidates pointers/references
	// to already-existing elements (only iterators), which protects the Account*
	// handed out by GetAccount/CreateAccount from dangling when another account is created.
	std::deque<Account> m_accounts;

	/**
	 * @brief Serializes every account and its full transaction history to cadflow_data.txt.
	 *
	 * Writes to a temporary file first and only replaces the real file on
	 * success, so an interrupted write can never leave the database half-written.
	 */
	void SaveToFile();

	/**
	 * @brief Rebuilds m_accounts from cadflow_data.txt at startup.
	 *
	 * Parses one account record at a time; a malformed record is skipped
	 * (with a warning) instead of throwing, so a corrupted file degrades
	 * gracefully rather than crashing the application on launch.
	 */
	void LoadFromFile();

public:
	/**
	 * @brief Constructs the manager and immediately loads any persisted accounts from disk.
	 */
	BankManager();

	/**
	 * @brief Creates a new account with an auto-assigned account number.
	 * @param ownerName The owner's name, as typed in the UI (Unicode CString).
	 * @return Account* Pointer to the newly created account, so the caller can read its generated ID/balance immediately. Valid until the next call that mutates m_accounts (see class warning).
	 */
	Account* CreateAccount(const CString& ownerName);

	/**
	 * @brief Looks up an account by its number.
	 * @param accountNumber The account number to search for.
	 * @return Account* Pointer to the matching account, or nullptr if none exists. Valid until the next call that mutates m_accounts (see class warning).
	 */
	Account* GetAccount(int accountNumber);

	/**
	 * @brief Applies a deposit or withdrawal to an account and persists the result.
	 * @param accountNumber The account to operate on.
	 * @param type 0 = deposit, 1 = withdrawal (withdrawal includes the 1% fee, see Account::withdrawWithFee).
	 * @param amount The amount requested by the user (before any fee).
	 * @return true if the transaction was accepted and applied, false otherwise (account not found, invalid amount, or insufficient balance).
	 */
	bool ProcessTransaction(int accountNumber, int type, double amount);

	/**
	 * @brief Permanently removes an account from the system and persists the change.
	 * @param accountNumber The account to remove.
	 * @return true if the account existed and was removed, false if no account matched.
	 */
	bool DeleteAccount(int accountNumber);

	/**
	 * @brief Moves funds between two accounts as a single logical operation.
	 *
	 * No fee is applied here (unlike a direct withdrawal): the exact amount
	 * requested leaves fromAccount and arrives at toAccount. If the deposit
	 * into toAccount fails after the withdrawal already succeeded, the
	 * withdrawn amount is manually redeposited into fromAccount to avoid
	 * leaving the source account short.
	 *
	 * @param fromAccountId The source account number.
	 * @param toAccountId The destination account number.
	 * @param amount The amount to move.
	 * @return true if the transfer completed successfully, false otherwise (invalid amount, account not found, or insufficient balance).
	 */
	bool TransferTo(int fromAccountId, int toAccountId, double amount);

	/**
	 * @brief Exposes every account for read-only iteration (e.g. populating the accounts table).
	 * @return const std::deque<Account>& Constant reference - callers can read but never mutate accounts through this.
	 */
	const std::deque<Account>& GetAllAccounts() const;
};
