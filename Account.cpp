#include "pch.h"
#include "Account.h"

// Static counter initialization for automatic, sequential account numbering.
// Starts at 10000 so generated IDs look like realistic bank account numbers.
int Account::nextAccountNumber = 10000;

int Account::peekNextAccountNumber() {
    return nextAccountNumber;
}

Account::Account(const std::string& owner) 
    : ownerName(owner), balance(0.0) {
    
    // Auto-assigns the sequential number and increments the static counter.
    // The account number is never user-settable through this path (security: prevents ID collisions/spoofing).
    accountNumber = nextAccountNumber++;
}

bool Account::deposit(double amount) {
    // Edge case: amount must be strictly positive.
    // Rejected before touching balance, so an invalid call never mutates state.
    if (amount <= 0.0) {
        return false;
    }

    // Balance is updated optimistically, before the transaction record is created.
    balance += amount;

    try {
        transactionHistory.push_back(Transaction("Deposito", amount));
    } catch (...) {
        // Rollback: if recording history fails (e.g. std::bad_alloc), undo the
        // balance change so the account never drifts from its transaction log.
        balance -= amount;
        return false;
    }

    return true;
}

bool Account::withdraw(double amount) {
    // Edge case: amount must be strictly positive (blocks nonsensical/negative-amount calls).
    if (amount <= 0.0) {
        return false;
    }

    // Edge case: block the withdrawal outright if it would overdraw the account.
    // No fee applies here - this path is shared with TransferTo, which must move
    // the exact requested amount between accounts without any deduction.
    if (amount > balance) {
        return false;
    }

    balance -= amount;

    try {
        transactionHistory.push_back(Transaction("Levantamento", amount));
    } catch (...) {
        // Rollback: undo the balance change if the history insert throws,
        // so a failed withdrawal never leaves the account short of its own record.
        balance += amount;
        return false;
    }

    return true;
}

bool Account::withdrawWithFee(double amount, double feeRate) {
    // Edge case: amount must be strictly positive.
    if (amount <= 0.0) {
        return false;
    }

    // The fee is charged on top of the requested amount, not deducted from it -
    // the user always receives exactly `amount`; the account pays amount + fee.
    double totalCost = amount * (1.0 + feeRate);

    // Overdraft check uses totalCost (not amount), otherwise a withdrawal could
    // be approved here and still push the balance negative once the fee is applied.
    if (totalCost > balance) {
        return false;
    }

    balance -= totalCost;

    // The recorded transaction amount is totalCost (not the requested amount), so the
    // history/statement reflects the real money that left the account, fee included.
    try {
        transactionHistory.push_back(Transaction("Levantamento (c/ taxa 1%)", totalCost));
    } catch (...) {
        // Rollback: undo the balance deduction if the history insert throws.
        balance += totalCost;
        return false;
    }

    return true;
}

int Account::getAccountNumber() const {
    return accountNumber;
}

const std::string& Account::getOwnerName() const {
    return ownerName;
}

double Account::getBalance() const {
    return balance;
}

const std::vector<Transaction>& Account::getTransactionHistory() const {
    return transactionHistory;
}

// I/O reconstruction methods below bypass all business validation on purpose:
// they exist solely to replay state that was already validated once, when the
// account was first persisted to cadflow_data.txt.
void Account::SetAccountNumber(int id) {
	accountNumber = id;
}

void Account::SetBalance(double newBalance) {
	balance = newBalance;
}

void Account::LoadTransaction(const Transaction& t) {
	transactionHistory.push_back(t);
}

void Account::SetNextAccountNumber(int id) {
	nextAccountNumber = id;
}
