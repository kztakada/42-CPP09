#include "BitcoinExchange.hpp"

#include <fstream>
#include <sstream>

BitcoinExchange::BitcoinExchange() : _rateDB() {}

BitcoinExchange::BitcoinExchange(std::string const &DataBasePath) : _rateDB() {
    // Load the database from the specified path
    std::ifstream dbFile(DataBasePath.c_str());
    if (!dbFile.is_open()) {
        throw std::runtime_error("could not open master database file.");
    }

    std::string line;
    int lineNumber = 1;
    // file format: date,exchange_rate
    while (std::getline(dbFile, line)) {
        std::istringstream ss(line);
        std::string date;
        double rate;
        if (std::getline(ss, date, ',') && ss >> rate) {
            // 残りの文字をチェック
            std::string remaining;
            std::getline(ss, remaining);

            // 残りが空白文字のみかチェック
            bool hasExtraData = false;
            for (std::string::const_iterator it = remaining.begin();
                it != remaining.end(); ++it) {
                if (*it != ' ' && *it != '\t' && *it != '\r') {
                    hasExtraData = true;
                    break;
                }
            }

            if (!hasExtraData) {
                _rateDB[date] = rate;
                lineNumber++;
                continue;
            }
            std::stringstream errss;
            errss << "invalid format in master database file. (line "
                  << lineNumber << ": " << line << ")";
            throw std::runtime_error(errss.str());
        }
        if (line == "date,exchange_rate" || line == "") {
            lineNumber++;
            continue;  // ヘッダー行をスキップ
        }
        std::stringstream errss;
        errss << "invalid format in master database file. (line " << lineNumber
              << ": " << line << ")";
        throw std::runtime_error(errss.str());
    }
    if (_rateDB["date"])
        _rateDB.erase("date");  // remove header if exists
    dbFile.close();

    // TODO:  _rateDB[date]のフォーマットチェックを追加する
}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(BitcoinExchange const &other)
    : _rateDB(other._rateDB) {}

BitcoinExchange &BitcoinExchange::operator=(BitcoinExchange const &other) {
    if (this != &other) {
        _rateDB = other._rateDB;
    }
    return *this;
}

// public member functions
std::string BitcoinExchange::exchange(const std::string &request) {
    return request;
}
