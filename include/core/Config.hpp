#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <stdexcept>

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    void loadFromFile(const std::string& filename);
    void set(const std::string& key, const std::string& value);

    /// Безопасные геттеры с проверкой типа
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;

    /// Проверка наличия ключа
    bool has(const std::string& key) const;

    /// Получение всех ключей
    std::vector<std::string> getKeys() const;

private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::unordered_map<std::string, std::string> values;
};

#endif // CONFIG_HPP
