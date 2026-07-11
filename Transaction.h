#pragma once

#include <string>
#include <ctime>

/**
 * @class Transaction
 * @brief Representa uma unica transacao financeira (ex: Deposito ou Levantamento).
 * 
 * Encapsula os dados da transacao, garantindo a imutabilidade do seu estado
 * apos a criacao. Regista o tipo, o montante e a data/hora exata em que ocorreu.
 */
class Transaction {
private:
    std::string type;      ///< O tipo de transacao (ex: "Deposito" ou "Levantamento")
    double amount;         ///< O montante financeiro envolvido
    std::time_t timestamp; ///< A data e hora exata da ocorrencia da transacao

public:
    /**
     * @brief Construtor completo para inicializar a transacao.
     * 
     * @param transactionType O tipo da transacao.
     * @param transactionAmount O valor da transacao.
     */
    Transaction(const std::string& transactionType, double transactionAmount);

    /**
     * @brief Retorna o tipo de transacao.
     * @return const std::string& Referencia constante para o tipo de transacao.
     */
    const std::string& GetType() const;

    /**
     * @brief Retorna o montante da transacao.
     * @return double O montante financeiro.
     */
    double GetAmount() const;

    /**
     * @brief Retorna a data e hora da transacao.
     * @return std::time_t O timestamp da transacao.
     */
    std::time_t GetTimestamp() const;

    /**
     * @brief Define manualmente o timestamp (usado na reconstrucao a partir de ficheiro).
     * @param ts O valor de timestamp a aplicar.
     */
    void SetTimestamp(std::time_t ts);
};
