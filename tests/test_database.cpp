#include <gtest/gtest.h>
#include <sqlite3.h>
#include <iostream>
#include <fstream>

class DatabaseTest : public ::testing::Test {
protected:
    sqlite3* db;

    void SetUp() override {
        sqlite3_open(":memory:", &db);
        const char* sql =
            "CREATE TABLE Users (username TEXT PRIMARY KEY, password_hash TEXT, role TEXT, jockey_id INTEGER, owner_id INTEGER);"
            "CREATE TABLE Owners (owner_id INTEGER PRIMARY KEY, last_name TEXT, first_name TEXT, phone TEXT, address TEXT);"
            "CREATE TABLE Jockeys (jockey_id INTEGER PRIMARY KEY, last_name TEXT, experience_years INTEGER, birth_year INTEGER, address TEXT);"
            "CREATE TABLE Horses (horse_id INTEGER PRIMARY KEY, name TEXT, age INTEGER, experience_years INTEGER, price REAL, owner_id INTEGER);"
            "CREATE TABLE Races (race_id INTEGER PRIMARY KEY, date TEXT, race_number INTEGER, place INTEGER, horse_id INTEGER, jockey_id INTEGER);"
            "INSERT INTO Users VALUES ('owner1', 'pass', 'owner', NULL, 1);"
            "INSERT INTO Users VALUES ('jockey1', 'pass', 'jockey', 1, NULL);"
            "INSERT INTO Owners VALUES (1, 'Ivanov', 'Ivan', '123456', 'Moscow');"
            "INSERT INTO Jockeys VALUES (1, 'Petrov', 5, 1990, 'Saint Petersburg');"
            "INSERT INTO Horses VALUES (1, 'Thunder', 5, 3, 10000, 1);"
            "INSERT INTO Races VALUES (1, '2024-01-15', 1, 1, 1, 1);";
        char* errMsg = nullptr;
        sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    }

    void TearDown() override {
        sqlite3_close(db);
    }
};

TEST_F(DatabaseTest, TableExists) {
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='Users'";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    sqlite3_finalize(stmt);
}

TEST_F(DatabaseTest, InsertHorse) {
    const char* sql = "INSERT INTO Horses VALUES (2, 'Lightning', 4, 2, 15000, 1)";
    char* errMsg = nullptr;
    ASSERT_EQ(sqlite3_exec(db, sql, nullptr, nullptr, &errMsg), SQLITE_OK);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM Horses", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    ASSERT_EQ(sqlite3_column_int(stmt, 0), 2);
    sqlite3_finalize(stmt);
}

TEST_F(DatabaseTest, UpdateHorse) {
    const char* sql = "UPDATE Horses SET price = 20000 WHERE horse_id = 1";
    char* errMsg = nullptr;
    ASSERT_EQ(sqlite3_exec(db, sql, nullptr, nullptr, &errMsg), SQLITE_OK);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT price FROM Horses WHERE horse_id = 1", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    ASSERT_EQ(sqlite3_column_double(stmt, 0), 20000);
    sqlite3_finalize(stmt);
}

TEST_F(DatabaseTest, DeleteHorse) {
    const char* sql = "DELETE FROM Horses WHERE horse_id = 1";
    char* errMsg = nullptr;
    ASSERT_EQ(sqlite3_exec(db, sql, nullptr, nullptr, &errMsg), SQLITE_OK);

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM Horses", -1, &stmt, nullptr);
    sqlite3_step(stmt);
    ASSERT_EQ(sqlite3_column_int(stmt, 0), 0);
    sqlite3_finalize(stmt);
}