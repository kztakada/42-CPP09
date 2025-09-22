#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP
#include <exception>
#include <map>
#include <stdexcept>
#include <string>

class BitcoinExchange {
   public:
    BitcoinExchange();
    BitcoinExchange(std::string const &DataBasePath);
    ~BitcoinExchange();
    BitcoinExchange(BitcoinExchange const &other);
    BitcoinExchange &operator=(BitcoinExchange const &other);

    std::string exchange(const std::string &request);

    std::size_t getRateDBSize() const;

   private:
    std::map<std::string, double> _rateDB;

    // for date validation and formatting
    std::string _toFormattedDate(const std::string &date) const;
    int _toNumberInt(const std::string &numberStr) const;
    bool _isExistedDate(const int year, const int month, const int day) const;

    bool _extractDateAndValue(const std::string &request, std::string &date,
        double &value, const char &separator) const;
    bool _isValidValueString(const std::string &valueStr) const;
    bool _isAllSpaces(const std::string &str) const;

    std::string _doubleToString(double value) const;
};

#endif /* BITCOINEXCHANGE_HPP */
