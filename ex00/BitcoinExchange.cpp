#include "BitcoinExchange.hpp"

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <sstream>

const std::string ERR_BAD_INPUT = "bad input => ";

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
    std::getline(dbFile, line);
    if (!(line == "date,exchange_rate")) {
        throw std::runtime_error("master database first line is not a header.");
    }
    while (std::getline(dbFile, line)) {
        if (line == "") {
            lineNumber++;
            continue;  // skip header line
        }
        std::string date;
        double rate;
        try {
            if (_extractDateAndValue(line, date, rate, ',')) {
                _rateDB[date] = rate;
                lineNumber++;
                continue;
            }
        } catch (std::exception &e) {
        }
        std::stringstream errss;
        errss << "invalid format in master database file. (line " << lineNumber
              << ": " << line << ")";
        throw std::runtime_error(errss.str());
    }
    dbFile.close();
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

// ----------------------------------------------------------------------------
// public member functions
std::string BitcoinExchange::exchange(const std::string &request) {
    if (_rateDB.empty()) {
        throw std::runtime_error("master database is empty.");
    }

    std::string date;
    double value;
    if (!_extractDateAndValue(request, date, value, '|')) {
        throw std::invalid_argument(ERR_BAD_INPUT + request);
    }
    if (value < 0) {
        throw std::invalid_argument("not a positive number.");
    }
    if (value > 1000) {
        throw std::invalid_argument("too large a number.");
    }

    std::map<std::string, double>::const_iterator it =
        _rateDB.lower_bound(date);
    if (it == _rateDB.end()) {
        --it;  // Use the last available date
    } else if (it != _rateDB.begin() && it->first != date) {
        --it;  // Use the closest previous date
    }

    double result = value * it->second;

    std::ostringstream oss;
    oss << date << " => " << _doubleToString(value) << " = "
        << _doubleToString(result);

    return oss.str();
}

std::size_t BitcoinExchange::getRateDBSize() const { return _rateDB.size(); }

// ----------------------------------------------------------------------------
// private member functions
std::string BitcoinExchange::_toFormattedDate(const std::string &date) const {
    // date format: Year-Month-Day
    // return format: YYYY-MM-DD
    std::istringstream ss(date);
    std::ostringstream oss;
    std::string year, month, day;
    if (std::getline(ss, year, '-') && std::getline(ss, month, '-') &&
        std::getline(ss, day)) {
        while (!year.empty() && std::isspace(year[0])) {
            year.erase(0, 1);  // Trim trailing spaces
        }
        int yearInt = _toNumberInt(year);
        int monthInt = _toNumberInt(month);
        while (!day.empty() && std::isspace(day[day.length() - 1])) {
            day.erase(day.length() - 1);  // Trim trailing spaces
        }
        int dayInt = _toNumberInt(day);
        if (!_isExistedDate(yearInt, monthInt, dayInt)) {
            throw std::invalid_argument(ERR_BAD_INPUT + date);
        }
        oss << (yearInt < 1000 ? "0" : "") << (yearInt < 100 ? "0" : "")
            << (yearInt < 10 ? "0" : "") << yearInt << '-'
            << (monthInt < 10 ? "0" : "") << monthInt << '-'
            << (dayInt < 10 ? "0" : "") << dayInt;
    } else {
        throw std::invalid_argument(ERR_BAD_INPUT + date);
    }
    return oss.str();
}

int BitcoinExchange::_toNumberInt(const std::string &numberStr) const {
    if (numberStr[0] == '-') {
        return -1;
    }
    std::string trimmedNumber = numberStr;
    if (trimmedNumber[0] == '+') {
        trimmedNumber = trimmedNumber.substr(1);  // Remove leading '+'
    }
    for (std::size_t i = 0; i < trimmedNumber.length(); ++i) {
        if (!std::isdigit(trimmedNumber[i])) {
            return -1;
        }
    }
    return std::atoi(trimmedNumber.c_str());
}

bool BitcoinExchange::_isExistedDate(
    const int year, const int month, const int day) const {
    // Check for valid date considering leap years
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    static const int daysInMonth[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    // Check for leap year in February
    if (month == 2 &&
        ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        maxDay = 29;
    }
    return day >= 1 && day <= maxDay;
}

bool BitcoinExchange::_extractDateAndValue(const std::string &request,
    std::string &date, double &value, const char &separator) const {
    std::istringstream ss(request);
    std::string valueStr("");
    if (request.find(separator) == std::string::npos) {
        throw std::invalid_argument(ERR_BAD_INPUT + request);
    } else if (request.find(separator) != request.rfind(separator)) {
        throw std::invalid_argument(ERR_BAD_INPUT + request);
    }
    std::string trimmedRequest = request;
    while (!trimmedRequest.empty() && std::isspace(trimmedRequest[0])) {
        trimmedRequest.erase(0, 1);  // Trim leading spaces
    }
    while (!trimmedRequest.empty() &&
           std::isspace(trimmedRequest[trimmedRequest.length() - 1])) {
        trimmedRequest.erase(
            trimmedRequest.length() - 1);  // Trim trailing spaces
    }
    if (trimmedRequest.empty() || trimmedRequest == std::string(1, separator)) {
        throw std::invalid_argument(ERR_BAD_INPUT + request);
    }
    if (std::getline(ss, date, separator) && std::getline(ss, valueStr)) {
        if (date.empty() || valueStr.empty()) {
            throw std::invalid_argument(ERR_BAD_INPUT + request);
        }
        if (_isAllSpaces(date) || _isAllSpaces(valueStr)) {
            throw std::invalid_argument(ERR_BAD_INPUT + request);
        }

        while (!valueStr.empty() && std::isspace(valueStr[0])) {
            valueStr.erase(0, 1);  // Trim leading spaces
        }
        while (!valueStr.empty() &&
               std::isspace(valueStr[valueStr.length() - 1])) {
            valueStr.erase(valueStr.length() - 1);  // Trim trailing spaces
        }
        if (!_isValidValueString(valueStr)) {
            throw std::invalid_argument(ERR_BAD_INPUT + valueStr);
        }

        std::string formattedDate = _toFormattedDate(date);
        date = formattedDate;
        char *endptr = NULL;
        errno = 0;
        value = strtod(valueStr.c_str(), &endptr);
        if (errno != 0 || *endptr != '\0') {
            throw std::invalid_argument(ERR_BAD_INPUT + valueStr);
        }
        return true;
    }
    return false;
}

bool BitcoinExchange::_isValidValueString(const std::string &valueStr) const {
    // Check if the value string is a valid positive number (integer or float)
    if (valueStr.empty()) {
        return false;
    }
    std::string trimmedValue = valueStr;
    if (trimmedValue[0] == '+' || trimmedValue[0] == '-') {
        trimmedValue = trimmedValue.substr(1);  // Remove leading '+'
    }
    if (!std::isdigit(trimmedValue[0])) {
        return false;
    }
    bool hasDecimalPoint = false;
    for (std::size_t i = 0; i < trimmedValue.length(); ++i) {
        if (trimmedValue[i] == '.') {
            if (hasDecimalPoint) {
                return false;  // More than one decimal point
            }
            hasDecimalPoint = true;
        } else if (!std::isdigit(trimmedValue[i])) {
            return false;  // Non-digit character found
        }
    }

    return true;
}

bool BitcoinExchange::_isAllSpaces(const std::string &str) const {
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (!std::isspace(str[i])) {
            return false;
        }
    }
    return true;
}

static bool hasPoint(const std::string &s) {
    return s.find('.') != std::string::npos;
}

static bool isScientific(const std::string &s) {
    return s.find('e') != std::string::npos || s.find('E') != std::string::npos;
}

static bool isSpecialDouble(const double d) {
    return (d != d) || (d == std::numeric_limits<double>::infinity()) ||
           (d == -std::numeric_limits<double>::infinity());
}

std::string BitcoinExchange::_doubleToString(double value) const {
    float f = static_cast<float>(value);
    std::ostringstream oss;
    oss << f;
    std::string s = oss.str();
    if (isSpecialDouble(value)) {
        return s;
    }
    if (isScientific(s)) {
        oss.str("");
        oss << std::fixed << f;
        s = oss.str();
    }
    if (hasPoint(s)) {
        while (s.length() > 0 && s[s.length() - 1] == '0') {
            s = s.substr(0, s.length() - 1);
        }
        if (s[s.length() - 1] == '.') {
            s = s.substr(0, s.length() - 1);
        }
    }
    return s;
}
