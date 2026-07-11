#include "pch.h"
#include "BankManager.h"
#include <string>
#include <cstdio>

BankManager::BankManager()
{
	// Loading here (not lazily) means the account list is ready before the
	// first dialog ever asks for it.
	LoadFromFile();
}

Account* BankManager::CreateAccount(const CString& ownerName)
{
	// MFC CString is UTF-16 by default; Account/std::string need a narrow
	// string, so this conversion is mandatory before crossing the UI/engine boundary.
	CStringA strOwnerA(ownerName);
	std::string stdOwnerName(strOwnerA.GetString());

	// emplace_back constructs the Account directly inside the deque - no
	// intermediate temporary object, and (unlike vector) never invalidates
	// pointers to accounts already in the container.
	m_accounts.emplace_back(stdOwnerName);

	SaveToFile();

	// Returned so the caller can immediately read the auto-generated account
	// number/balance without a second lookup. See class-level warning in the
	// header: this pointer is only safe until the next mutating call.
	return &m_accounts.back();
}

Account* BankManager::GetAccount(int accountNumber)
{
	// auto& (not auto): iterating by value here would search through disposable
	// copies and any returned pointer would dangle the moment the loop ends.
	for (auto& account : m_accounts)
	{
		if (account.getAccountNumber() == accountNumber)
		{
			return &account;
		}
	}
	return nullptr;
}

bool BankManager::ProcessTransaction(int accountNumber, int type, double amount)
{
	Account* account = GetAccount(accountNumber);
	
	if (account == nullptr) {
		return false; // Defensive check: UI should never send an unknown account number, but never trust it blindly.
	}

	bool success = false;
	if (type == 0) // Deposit
	{
		success = account->deposit(amount);
	}
	else if (type == 1) // Withdrawal - the 1% fee is applied only on this path, never on TransferTo.
	{
		success = account->withdrawWithFee(amount, 0.01);
	}

	if (success) {
		SaveToFile();
	}

	return success;
}

bool BankManager::DeleteAccount(int accountNumber)
{
	// Single pass: the search and the erase share the same iterator, so the
	// account is never scanned twice for what is logically one lookup.
	for (auto it = m_accounts.begin(); it != m_accounts.end(); ++it)
	{
		if (it->getAccountNumber() == accountNumber)
		{
			m_accounts.erase(it);
			SaveToFile();
			return true;
		}
	}

	return false;
}

bool BankManager::TransferTo(int fromAccountId, int toAccountId, double amount)
{
	if (amount <= 0.0) return false;

	Account* fromAccount = GetAccount(fromAccountId);
	Account* toAccount = GetAccount(toAccountId);

	if (fromAccount == nullptr || toAccount == nullptr) return false;

	// Deliberately calls withdraw() (fee-free), not withdrawWithFee(): a transfer
	// between accounts must move the exact amount, unlike a direct withdrawal.
	if (!fromAccount->withdraw(amount)) return false;

	if (!toAccount->deposit(amount))
	{
		// MANUAL ROLLBACK: the withdrawal already succeeded, so if the deposit
		// into the destination fails, the money must be put back into the
		// source account or it would simply vanish from the system.
		fromAccount->deposit(amount);
		return false;
	}

	SaveToFile();
	return true;
}

void BankManager::LoadFromFile()
{
	std::ifstream inFile("cadflow_data.txt");
	if (!inFile.is_open()) return;

	m_accounts.clear();
	int highestId = 10000;
	int duplicadosIgnorados = 0;
	bool bLinhaCorrompida = false;

	std::string line;
	while (std::getline(inFile, line))
	{
		if (line.empty()) continue;

		// Each record (account line + its transaction lines) is parsed inside
		// its own try/catch: stoi/stod/stoll throw on malformed numeric text,
		// and without this guard a single corrupted line would crash the app
		// on every future launch, before any window even appears.
		try
		{
			std::stringstream ss(line);
			std::string idStr, ownerName, balanceStr;

			std::getline(ss, idStr, '|');
			std::getline(ss, ownerName, '|');
			std::getline(ss, balanceStr, '|');

			if (idStr.empty() || ownerName.empty() || balanceStr.empty()) continue;

			int id = std::stoi(idStr);
			double balance = std::stod(balanceStr);

			// A duplicate ID (manual edit or file corruption) would otherwise load as
			// two distinct in-memory accounts, with the second permanently unreachable
			// through GetAccount() - detected here, before it gets a chance to happen.
			bool bDuplicado = false;
			for (const auto& existente : m_accounts)
			{
				if (existente.getAccountNumber() == id) { bDuplicado = true; break; }
			}

			// Tracks the highest ID seen so the static counter (see Account::nextAccountNumber)
			// can be resumed correctly below, instead of restarting from 10000 every launch.
			if (id > highestId) {
				highestId = id;
			}

			Account acc(ownerName);
			acc.SetAccountNumber(id);
			acc.SetBalance(balance);

			std::string countStr;
			std::getline(inFile, countStr);
			int tCount = countStr.empty() ? 0 : std::stoi(countStr);

			for (int i = 0; i < tCount; ++i)
			{
				std::string tLine;
				if (!std::getline(inFile, tLine)) break; // File truncated mid-record: stop reading this account's history instead of looping on empty lines.
				std::stringstream tSS(tLine);

				std::string tType, tAmountStr, tTimeStr;
				std::getline(tSS, tType, '|');
				std::getline(tSS, tAmountStr, '|');
				std::getline(tSS, tTimeStr, '|');

				if (!tType.empty() && !tAmountStr.empty() && !tTimeStr.empty()) {
					Transaction t(tType, std::stod(tAmountStr));
					t.SetTimestamp(std::stoll(tTimeStr));
					acc.LoadTransaction(t);
				}
			}

			// The transaction lines are always consumed above (even when bDuplicado),
			// so the stream position stays in sync for the next record regardless of outcome.
			if (bDuplicado) {
				++duplicadosIgnorados;
			} else {
				m_accounts.push_back(acc);
			}
		}
		catch (...)
		{
			// Corrupted record (non-numeric field, etc.): skip it and keep loading the rest of the file.
			bLinhaCorrompida = true;
			continue;
		}
	}
	inFile.close();

	// Warnings are shown once, after the full file has been read, rather than
	// per-record, to avoid a wall of message boxes on a badly corrupted file.
	if (bLinhaCorrompida)
	{
		AfxMessageBox(L"Aviso: foram encontrados registos corrompidos em cadflow_data.txt. Esses registos foram ignorados.", MB_ICONWARNING | MB_OK);
	}
	if (duplicadosIgnorados > 0)
	{
		AfxMessageBox(L"Aviso: foram encontradas contas com numero duplicado em cadflow_data.txt. Os duplicados foram ignorados.", MB_ICONWARNING | MB_OK);
	}

	// Resuming from highestId + 1 (rather than always 10001) guarantees a newly
	// created account can never collide with an ID already loaded from file.
	Account::SetNextAccountNumber(highestId > 10000 ? highestId + 1 : 10001);
}

const std::deque<Account>& BankManager::GetAllAccounts() const
{
	return m_accounts;
}

void BankManager::SaveToFile()
{
	// Writing to a temp file first, then swapping it in, means a crash or a
	// full disk mid-write leaves the OLD cadflow_data.txt untouched - opening
	// an ofstream directly on the real file would truncate it immediately,
	// destroying the entire database if the write never completes.
	const char* szTempFile = "cadflow_data.tmp";
	const char* szRealFile = "cadflow_data.txt";

	std::ofstream outFile(szTempFile);
	if (!outFile.is_open()) return;

	for (const auto& acc : m_accounts)
	{
		outFile << acc.getAccountNumber() << "|" << acc.getOwnerName() << "|" << acc.getBalance() << "\n";
		const auto& history = acc.getTransactionHistory();
		outFile << history.size() << "\n";
		for (const auto& t : history)
		{
			outFile << t.GetType() << "|" << t.GetAmount() << "|" << t.GetTimestamp() << "\n";
		}
	}

	bool bEscritaOk = !outFile.fail();
	outFile.close();

	if (bEscritaOk)
	{
		// Windows' CRT rename() fails outright if the destination already exists
		// (unlike POSIX rename), so the old file must be removed first.
		remove(szRealFile);
		rename(szTempFile, szRealFile);
	}
}

