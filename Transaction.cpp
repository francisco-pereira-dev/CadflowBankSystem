#include "pch.h"
#include "Transaction.h"
#include <chrono>
#include <stdexcept>

Transaction::Transaction(const std::string& transactionType, double transactionAmount)
    : type(transactionType), amount(transactionAmount) {
    
    // Defensive check: guarantees a non-positive amount can never reach this
    // object's state, even though Account already validates before constructing
    // it - each class protects its own invariants (OOP best practice).
    if (transactionAmount <= 0.0) {
        throw std::invalid_argument("O montante da transacao deve ser estritamente positivo.");
    }

    // <chrono> is used instead of time(nullptr) for a type-safe, portable capture of the creation instant.
    auto now = std::chrono::system_clock::now();
    timestamp = std::chrono::system_clock::to_time_t(now);
}

const std::string& Transaction::GetType() const {
    return type;
}

double Transaction::GetAmount() const {
    return amount;
}

std::time_t Transaction::GetTimestamp() const {
    return timestamp;
}

void Transaction::SetTimestamp(std::time_t ts) {
	timestamp = ts;
}
