#pragma once

#include <string>
#include <vector>
#include "Transaction.h"

/**
 * @class Account
 * @brief Represents a bank account and its core operations.
 *
 * Strictly follows OOP practices. The account number is generated
 * sequentially and managed internally. Provides safe methods for deposits
 * and withdrawals, with strong error handling for edge cases (active rejection).
 */
class Account {
private:
    int accountNumber;                           ///< Unique identifier for the account
    std::string ownerName;                       ///< Name of the account owner
    double balance;                              ///< Current account balance
    std::vector<Transaction> transactionHistory; ///< Full history of transactions

    static int nextAccountNumber;                ///< Internal static counter used to generate sequential account numbers

public:
    /**
     * @brief Returns the next available account number without incrementing it.
     * @return int The next account number.
     */
    static int peekNextAccountNumber();

    /**
     * @brief Constructs the account. Balance starts at 0.0 and the account number is auto-assigned.
     *
     * @param owner The name of the account owner.
     */
    explicit Account(const std::string& owner);

    /**
     * @brief Adds the amount to the balance and records the transaction.
     *
     * Actively rejects negative or zero values.
     *
     * @param amount The amount to deposit.
     * @return true if the deposit succeeded, false if the amount is invalid.
     */
    bool deposit(double amount);

    /**
     * @brief Deducts the amount from the balance and records the transaction, if there is sufficient balance.
     *
     * Implements strict logic to reject values <= 0 or greater than the available balance.
     *
     * @param amount The amount to withdraw.
     * @return true if the withdrawal succeeded, false otherwise.
     */
    bool withdraw(double amount);

    /**
     * @brief Deducts the amount from the balance, plus a percentage fee, and records the transaction.
     *
     * Used only for direct withdrawals (does not apply to transfers between accounts).
     *
     * @param amount The amount requested by the user (before fee).
     * @param feeRate The fee to apply, as a decimal fraction (e.g. 0.01 for 1%).
     * @return true if the withdrawal succeeded, false if the balance does not cover amount + fee.
     */
    bool withdrawWithFee(double amount, double feeRate);

    /**
     * @brief Returns the account's unique number.
     * @return int The account number.
     */
    int getAccountNumber() const;

    /**
     * @brief Returns the account owner's name.
     * @return const std::string& Constant reference to the owner's name (avoids unnecessary copies).
     */
    const std::string& getOwnerName() const;

    /**
     * @brief Returns the available balance.
     * @return double The current balance.
     */
    double getBalance() const;

    /**
     * @brief Returns the account's full transaction history.
     * @return const std::vector<Transaction>& Constant reference to the transaction history.
     */
    const std::vector<Transaction>& getTransactionHistory() const;

	// Strict setters for engine reconstruction from file
	/**
	 * @brief Overwrites the account number. Used only when rebuilding accounts from cadflow_data.txt.
	 * @param id The account number stored in the file.
	 */
	void SetAccountNumber(int id);

	/**
	 * @brief Overwrites the balance directly, bypassing deposit/withdraw validation. Used only when rebuilding accounts from file.
	 * @param newBalance The balance value stored in the file.
	 */
	void SetBalance(double newBalance);

	/**
	 * @brief Appends an already-built Transaction to the history without re-validating it. Used only when rebuilding accounts from file.
	 * @param t The transaction to append.
	 */
	void LoadTransaction(const Transaction& t);

	/**
	 * @brief Resets the static account-number counter. Used once at startup so new accounts never collide with IDs already present in the file.
	 * @param id The next account number to hand out.
	 */
	static void SetNextAccountNumber(int id);
};
