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

    bool ensureConnection();

public:
    explicit DatabaseManager(std::string path);
    ~DatabaseManager();

    void disconnect();

    bool authenticate(const std::string& username, const std::string& password,
                      std::string& role, int& foreign_id);

    std::vector<std::vector<std::string>> getOwnerHorsesWithRaces(int owner_id);

    std::vector<std::vector<std::string>> getBestHorseInfo();

    std::vector<std::vector<std::string>> getJockeyRaces(int jockey_id);

    std::vector<std::vector<std::string>> getMostActiveJockey();

    std::vector<std::vector<std::string>> getRacesByPeriod(const std::string& start_date,
                                                            const std::string& end_date);

    std::vector<std::vector<std::string>> getJockeyRacesByPeriod(int jockey_id,
                                                                   const std::string& start_date,
                                                                   const std::string& end_date);

    bool addHorse(const Horse& horse);

    bool updateHorse(int horse_id, const Horse& horse);

    bool deleteHorse(int horse_id);

    bool addRace(const Race& race);

    bool updateRace(int race_id, const Race& race);

    bool deleteRace(int race_id);

    bool validateHorseJockey(int horse_id, int jockey_id);

    bool distributePrizeMoney(double prize_fund);

    bool horseExists(int horse_id);

    bool jockeyExists(int jockey_id);

    int getLastRaceId();
};

#endif