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

   private:
    std::map<std::string, double> _rateDB;
};

#endif /* BITCOINEXCHANGE_HPP */
