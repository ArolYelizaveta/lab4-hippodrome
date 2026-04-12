// test_businesslogic.cpp
#include <gtest/gtest.h>
#include "../includes/BusinessLogic.h"
#include "../includes/DatabaseManager.h"
#include <sqlite3.h>
#include <cstdio>
#include <unistd.h>

class BusinessLogicTest : public ::testing::Test {
protected:
    DatabaseManager* dbManager;
    BusinessLogic* businessLogic;
    char temp_db[256];

    void SetUp() override {
        snprintf(temp_db, sizeof(temp_db), "/tmp/test_db_%d.db", getpid());

        sqlite3* db;
        sqlite3_open(temp_db, &db);

        const char* create_users = "CREATE TABLE Users (username TEXT PRIMARY KEY, password_hash TEXT, role TEXT, jockey_id INTEGER, owner_id INTEGER);";
        const char* create_owners = "CREATE TABLE Owners (owner_id INTEGER PRIMARY KEY, last_name TEXT, first_name TEXT, phone TEXT, address TEXT);";
        const char* create_jockeys = "CREATE TABLE Jockeys (jockey_id INTEGER PRIMARY KEY, last_name TEXT, experience_years INTEGER, birth_year INTEGER, address TEXT);";
        const char* create_horses = "CREATE TABLE Horses (horse_id INTEGER PRIMARY KEY, name TEXT, age INTEGER, experience_years INTEGER, price REAL, owner_id INTEGER);";
        const char* create_races = "CREATE TABLE Races (race_id INTEGER PRIMARY KEY, date TEXT, race_number INTEGER, place INTEGER, horse_id INTEGER, jockey_id INTEGER);";

        char* errMsg = nullptr;
        sqlite3_exec(db, create_users, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, create_owners, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, create_jockeys, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, create_horses, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, create_races, nullptr, nullptr, &errMsg);

        const char* insert_jockeys = "INSERT INTO Jockeys VALUES (1, 'Petrov', 5, 1990, 'Saint Petersburg');"
                                      "INSERT INTO Jockeys VALUES (2, 'Sidorov', 3, 1995, 'Moscow');";
        const char* insert_horses = "INSERT INTO Horses VALUES (1, 'Thunder', 5, 3, 10000, 1);"
                                     "INSERT INTO Horses VALUES (2, 'Lightning', 4, 2, 15000, 1);"
                                     "INSERT INTO Horses VALUES (3, 'Storm', 6, 4, 20000, 2);";
        const char* insert_races = "INSERT INTO Races VALUES (1, '2024-01-15', 1, 1, 1, 1);"
                                    "INSERT INTO Races VALUES (2, '2024-01-15', 1, 2, 2, 2);"
                                    "INSERT INTO Races VALUES (3, '2024-01-15', 1, 3, 3, 1);"
                                    "INSERT INTO Races VALUES (4, '2024-01-20', 2, 1, 2, 1);"
                                    "INSERT INTO Races VALUES (5, '2024-01-20', 2, 2, 1, 2);"
                                    "INSERT INTO Races VALUES (6, '2024-01-20', 2, 3, 3, 2);"
                                    "INSERT INTO Races VALUES (7, '2024-02-10', 3, 1, 1, 1);"
                                    "INSERT INTO Races VALUES (8, '2024-02-10', 3, 2, 2, 2);"
                                    "INSERT INTO Races VALUES (9, '2024-02-10', 3, 3, 3, 1);";

        sqlite3_exec(db, insert_jockeys, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, insert_horses, nullptr, nullptr, &errMsg);
        sqlite3_exec(db, insert_races, nullptr, nullptr, &errMsg);

        sqlite3_close(db);

        dbManager = new DatabaseManager(temp_db);
        businessLogic = new BusinessLogic(*dbManager);
    }

    void TearDown() override {
        delete businessLogic;
        delete dbManager;
        std::remove(temp_db);
    }
};

TEST_F(BusinessLogicTest, ValidateRaceDataValid) {
    std::string error;
    bool result = businessLogic->validateRaceData(1, 1, error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, "OK");
}

TEST_F(BusinessLogicTest, ValidateRaceDataValidSecondHorse) {
    std::string error;
    bool result = businessLogic->validateRaceData(2, 2, error);
    EXPECT_TRUE(result);
    EXPECT_EQ(error, "OK");
}

TEST_F(BusinessLogicTest, ValidateRaceDataInvalidHorse) {
    std::string error;
    bool result = businessLogic->validateRaceData(999, 1, error);
    EXPECT_FALSE(result);
    EXPECT_NE(error.find("Лошадь"), std::string::npos);
}

TEST_F(BusinessLogicTest, ValidateRaceDataInvalidJockey) {
    std::string error;
    bool result = businessLogic->validateRaceData(1, 999, error);
    EXPECT_FALSE(result);
    EXPECT_NE(error.find("Жокей"), std::string::npos);
}

TEST_F(BusinessLogicTest, ValidateRaceDataBothInvalid) {
    std::string error;
    bool result = businessLogic->validateRaceData(999, 999, error);
    EXPECT_FALSE(result);
    EXPECT_NE(error.find("Лошадь"), std::string::npos);
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodValid) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(1, "2024-01-01", "2024-01-31", error);
    EXPECT_EQ(error, "OK");
    EXPECT_EQ(results.size(), 3);
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodEmptyResult) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(1, "2025-01-01", "2025-01-31", error);
    EXPECT_EQ(error, "OK");
    EXPECT_TRUE(results.empty());
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodInvalidDates) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(1, "2024-01-20", "2024-01-10", error);
    EXPECT_TRUE(results.empty());
    EXPECT_NE(error.find("Начальная дата"), std::string::npos);
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodInvalidJockey) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(999, "2024-01-01", "2024-01-31", error);
    EXPECT_TRUE(results.empty());
    EXPECT_NE(error.find("Жокей не найден"), std::string::npos);
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodSecondJockey) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(2, "2024-01-01", "2024-01-31", error);
    EXPECT_EQ(error, "OK");
    EXPECT_EQ(results.size(), 3);
}

TEST_F(BusinessLogicTest, GetJockeyRacesByPeriodExactDates) {
    std::string error;
    auto results = businessLogic->getJockeyRacesByPeriod(1, "2024-01-20", "2024-01-20", error);
    EXPECT_EQ(error, "OK");
    EXPECT_EQ(results.size(), 1);
}

TEST_F(BusinessLogicTest, PrintTableEmptyData) {
    std::vector<std::vector<std::string>> empty_data;
    std::vector<std::string> headers = {"Test"};
    testing::internal::CaptureStdout();
    BusinessLogic::printTable(empty_data, headers);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Нет данных"), std::string::npos);
}

TEST_F(BusinessLogicTest, HorseExistsTrue) {
    EXPECT_TRUE(dbManager->horseExists(1));
    EXPECT_TRUE(dbManager->horseExists(2));
    EXPECT_TRUE(dbManager->horseExists(3));
}

TEST_F(BusinessLogicTest, HorseExistsFalse) {
    EXPECT_FALSE(dbManager->horseExists(999));
}

TEST_F(BusinessLogicTest, JockeyExistsTrue) {
    EXPECT_TRUE(dbManager->jockeyExists(1));
    EXPECT_TRUE(dbManager->jockeyExists(2));
}

TEST_F(BusinessLogicTest, JockeyExistsFalse) {
    EXPECT_FALSE(dbManager->jockeyExists(999));
}