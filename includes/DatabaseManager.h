#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>

struct Horse {
    int horse_id;
    std::string name;
    int age;
    int experience_years;
    double price;
    int owner_id;
};

struct Jockey {
    int jockey_id;
    std::string last_name;
    int experience_years;
    int birth_year;
    std::string address;
};

struct Owner {
    int owner_id;
    std::string last_name;
    std::string first_name;
    std::string phone;
    std::string address;
};

struct Race {
    int race_id;
    std::string date;
    int race_number;
    int place;
    int horse_id;
    int jockey_id;
};

struct PrizeDistribution {
    int distribution_id;
    int race_id;
    int horse_id;
    int place;
    double prize_amount;
};

class DatabaseManager {
    sqlite3* db;
    std::string db_path;
    
    static int callback(void* data, int argc, char** argv, char** azColName);
    
public:
    explicit DatabaseManager(std::string  path);
    ~DatabaseManager();
    
    bool connect();
    void disconnect();
    bool isConnected() const;

    bool authenticate(const std::string& username, const std::string& password, 
                      std::string& role, int& foreign_id) const;

    std::vector<std::vector<std::string>> getOwnerHorsesWithRaces(int owner_id) const;
    std::vector<std::vector<std::string>> getBestHorseInfo() const;

    std::vector<std::vector<std::string>> getJockeyRaces(int jockey_id) const;
    std::vector<std::vector<std::string>> getMostActiveJockey() const;

    std::vector<std::vector<std::string>> getRacesByPeriod(const std::string& start_date, 
                                                            const std::string& end_date) const;
    std::vector<std::vector<std::string>> getJockeyRacesByPeriod(int jockey_id, 
                                                                   const std::string& start_date,
                                                                   const std::string& end_date) const;

    bool addHorse(const Horse& horse) const;
    bool updateHorse(int horse_id, const Horse& horse) const;
    bool deleteHorse(int horse_id) const;
    
    bool addRace(const Race& race) const;
    bool updateRace(int race_id, const Race& race) const;
    bool deleteRace(int race_id) const;

    bool validateHorseJockey(int horse_id, int jockey_id) const;

    bool distributePrizeMoney(double prize_fund) const;

    bool horseExists(int horse_id) const;
    bool jockeyExists(int jockey_id) const;
    int getLastRaceId() const;
};

#endif