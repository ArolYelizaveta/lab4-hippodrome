#include "../includes/DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <utility>

DatabaseManager::DatabaseManager(std::string path) : db(nullptr), db_path(std::move(path)) {}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::ensureConnection() {
    if (db) {
        return true;
    }
    const int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc) {
        std::cerr << "Ошибка открытия БД: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
        return false;
    }
    return true;
}

void DatabaseManager::disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

int DatabaseManager::callback(void* data, int argc, char** argv, char** azColName) {
    auto* rows = static_cast<std::vector<std::vector<std::string>>*>(data);
    std::vector<std::string> row;
    for (int i = 0; i < argc; i++) {
        row.emplace_back(argv[i] ? argv[i] : "NULL");
    }
    rows->push_back(row);
    return 0;
}

bool DatabaseManager::authenticate(const std::string& username, const std::string& password,
                                   std::string& role, int& foreign_id) {
    if (!ensureConnection()) return false;

    const std::string sql = "SELECT role, jockey_id, owner_id FROM Users WHERE username = '" + username +
                      "' AND password_hash = '" + password + "'";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    if (results.empty()) {
        return false;
    }

    role = results[0][0];
    if (role == "jockey") {
        foreign_id = results[0][1].empty() ? -1 : std::stoi(results[0][1]);
    } else {
        foreign_id = results[0][2].empty() ? -1 : std::stoi(results[0][2]);
    }

    return true;
}

std::vector<std::vector<std::string>> DatabaseManager::getOwnerHorsesWithRaces(int owner_id) {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT h.name, h.age, h.experience_years, h.price, "
        "r.date, r.race_number, r.place, j.last_name "
        "FROM Horses h "
        "LEFT JOIN Races r ON h.horse_id = r.horse_id "
        "LEFT JOIN Jockeys j ON r.jockey_id = j.jockey_id "
        "WHERE h.owner_id = " + std::to_string(owner_id) + " "
        "ORDER BY h.name, r.date";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

std::vector<std::vector<std::string>> DatabaseManager::getBestHorseInfo() {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT h.horse_id, h.name, h.age, h.experience_years, h.price, o.last_name as owner, "
        "GROUP_CONCAT(r.date || ' (' || j.last_name || ')') as races "
        "FROM Horses h "
        "JOIN Owners o ON h.owner_id = o.owner_id "
        "JOIN Races r ON h.horse_id = r.horse_id "
        "JOIN Jockeys j ON r.jockey_id = j.jockey_id "
        "WHERE r.place = 1 "
        "GROUP BY h.horse_id "
        "ORDER BY COUNT(*) DESC LIMIT 1";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

std::vector<std::vector<std::string>> DatabaseManager::getMostActiveJockey() {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT j.jockey_id, j.last_name, j.experience_years, j.birth_year, j.address, "
        "COUNT(r.race_id) as total_races "
        "FROM Jockeys j "
        "JOIN Races r ON j.jockey_id = r.jockey_id "
        "GROUP BY j.jockey_id "
        "ORDER BY total_races DESC LIMIT 1";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

std::vector<std::vector<std::string>> DatabaseManager::getJockeyRaces(int jockey_id) {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT r.date, r.race_number, r.place, h.name as horse_name "
        "FROM Races r "
        "JOIN Horses h ON r.horse_id = h.horse_id "
        "WHERE r.jockey_id = " + std::to_string(jockey_id) + " "
        "ORDER BY r.date DESC";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

std::vector<std::vector<std::string>> DatabaseManager::getRacesByPeriod(const std::string& start_date,
                                                                         const std::string& end_date) {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT r.date, r.race_number, r.place, h.name as horse_name, j.last_name as jockey "
        "FROM Races r "
        "JOIN Horses h ON r.horse_id = h.horse_id "
        "JOIN Jockeys j ON r.jockey_id = j.jockey_id "
        "WHERE r.date BETWEEN '" + start_date + "' AND '" + end_date + "' "
        "ORDER BY r.date, r.race_number";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

std::vector<std::vector<std::string>> DatabaseManager::getJockeyRacesByPeriod(int jockey_id,
                                                                               const std::string& start_date,
                                                                               const std::string& end_date) {
    if (!ensureConnection()) return {};

    const std::string sql =
        "SELECT r.date, r.race_number, r.place, h.name as horse_name "
        "FROM Races r "
        "JOIN Horses h ON r.horse_id = h.horse_id "
        "WHERE r.jockey_id = " + std::to_string(jockey_id) + " "
        "AND r.date BETWEEN '" + start_date + "' AND '" + end_date + "' "
        "ORDER BY r.date";

    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return results;
}

bool DatabaseManager::horseExists(int horse_id) {
    if (!ensureConnection()) return false;

    const std::string sql = "SELECT 1 FROM Horses WHERE horse_id = " + std::to_string(horse_id);
    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return !results.empty();
}

bool DatabaseManager::jockeyExists(int jockey_id) {
    if (!ensureConnection()) return false;

    const std::string sql = "SELECT 1 FROM Jockeys WHERE jockey_id = " + std::to_string(jockey_id);
    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    return !results.empty();
}

bool DatabaseManager::validateHorseJockey(int horse_id, int jockey_id) {
    return horseExists(horse_id) && jockeyExists(jockey_id);
}

bool DatabaseManager::addRace(const Race& race) {
    if (!ensureConnection()) return false;

    if (!validateHorseJockey(race.horse_id, race.jockey_id)) {
        return false;
    }

    const std::string sql = "INSERT INTO Races (date, race_number, place, horse_id, jockey_id) VALUES ('" +
                      race.date + "', " + std::to_string(race.race_number) + ", " +
                      std::to_string(race.place) + ", " + std::to_string(race.horse_id) + ", " +
                      std::to_string(race.jockey_id) + ")";

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::addHorse(const Horse& horse) {
    if (!ensureConnection()) return false;

    const std::string sql = "INSERT INTO Horses (name, age, experience_years, price, owner_id) VALUES ('" +
                      horse.name + "', " + std::to_string(horse.age) + ", " +
                      std::to_string(horse.experience_years) + ", " + std::to_string(horse.price) + ", " +
                      std::to_string(horse.owner_id) + ")";

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::updateHorse(int horse_id, const Horse& horse) {
    if (!ensureConnection()) return false;

    const std::string sql = "UPDATE Horses SET name = '" + horse.name + "', age = " + std::to_string(horse.age) +
                      ", experience_years = " + std::to_string(horse.experience_years) +
                      ", price = " + std::to_string(horse.price) +
                      ", owner_id = " + std::to_string(horse.owner_id) +
                      " WHERE horse_id = " + std::to_string(horse_id);

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::deleteHorse(int horse_id) {
    if (!ensureConnection()) return false;

    const std::string sql = "DELETE FROM Horses WHERE horse_id = " + std::to_string(horse_id);

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::updateRace(int race_id, const Race& race) {
    if (!ensureConnection()) return false;

    const std::string sql = "UPDATE Races SET date = '" + race.date + "', race_number = " + std::to_string(race.race_number) +
                      ", place = " + std::to_string(race.place) +
                      ", horse_id = " + std::to_string(race.horse_id) +
                      ", jockey_id = " + std::to_string(race.jockey_id) +
                      " WHERE race_id = " + std::to_string(race_id);

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::deleteRace(int race_id) {
    if (!ensureConnection()) return false;

    const std::string sql = "DELETE FROM Races WHERE race_id = " + std::to_string(race_id);

    char* errMsg = nullptr;
    const int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int DatabaseManager::getLastRaceId() {
    if (!ensureConnection()) return 0;

    const std::string sql = "SELECT MAX(race_id) FROM Races";
    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);

    if (results.empty() || results[0][0] == "NULL") {
        return 0;
    }
    return std::stoi(results[0][0]);
}

bool DatabaseManager::distributePrizeMoney(double prize_fund) {
    if (!ensureConnection()) return false;
    
    const int last_race_id = getLastRaceId();
    if (last_race_id == 0) {
        return false;
    }

    const std::string sql = "SELECT horse_id, place FROM Races WHERE race_id = " + std::to_string(last_race_id) +
                      " AND place IN (1, 2, 3) ORDER BY place";
    
    std::vector<std::vector<std::string>> results;
    char* errMsg = nullptr;
    sqlite3_exec(db, sql.c_str(), callback, &results, &errMsg);
    
    if (results.size() < 3) {
        return false;
    }

    const double prizes[] = {prize_fund * 0.5, prize_fund * 0.3, prize_fund * 0.2};

    const std::string createTable =
        "CREATE TABLE IF NOT EXISTS PrizeDistributions ("
        "distribution_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "race_id INTEGER NOT NULL,"
        "horse_id INTEGER NOT NULL,"
        "place INTEGER NOT NULL,"
        "prize_amount REAL NOT NULL,"
        "FOREIGN KEY(race_id) REFERENCES Races(race_id),"
        "FOREIGN KEY(horse_id) REFERENCES Horses(horse_id))";
    
    sqlite3_exec(db, createTable.c_str(), nullptr, nullptr, &errMsg);

    for (int i = 0; i < 3; i++) {
        std::string insertSql = "INSERT INTO PrizeDistributions (race_id, horse_id, place, prize_amount) VALUES (" +
                                std::to_string(last_race_id) + ", " + results[i][0] + ", " +
                                results[i][1] + ", " + std::to_string(prizes[i]) + ")";
        sqlite3_exec(db, insertSql.c_str(), nullptr, nullptr, &errMsg);
    }
    return true;
}