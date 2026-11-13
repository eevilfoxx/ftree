#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>
#include <memory>

class Config {
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }
    
    void loadFromFile(const std::string& filename);
    void set(const std::string& key, const std::string& value);
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    std::unordered_map<std::string, std::string> values;
};

#endif