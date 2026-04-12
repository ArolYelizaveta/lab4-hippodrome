// BusinessLogic.cpp
#include "../includes/BusinessLogic.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

BusinessLogic::BusinessLogic(DatabaseManager& db) : dbManager(db) {}

bool BusinessLogic::validateRaceData(const int horse_id, const int jockey_id, std::string& error_message) const {
    if (!dbManager.horseExists(horse_id)) {
        error_message = "Ошибка: Лошадь с ID " + std::to_string(horse_id) + " не найдена!";
        return false;
    }

    if (!dbManager.jockeyExists(jockey_id)) {
        error_message = "Ошибка: Жокей с ID " + std::to_string(jockey_id) + " не найден!";
        return false;
    }

    error_message = "OK";
    return true;
}

std::vector<std::vector<std::string>> BusinessLogic::getJockeyRacesByPeriod(const int jockey_id,
                                                                              const std::string& start_date,
                                                                              const std::string& end_date,
                                                                              std::string& error_message) const {
    if (start_date > end_date) {
        error_message = "Ошибка: Начальная дата не может быть позже конечной!";
        return {};
    }

    if (!dbManager.jockeyExists(jockey_id)) {
        error_message = "Ошибка: Жокей не найден!";
        return {};
    }

    error_message = "OK";
    return dbManager.getJockeyRacesByPeriod(jockey_id, start_date, end_date);
}

void BusinessLogic::printTable(const std::vector<std::vector<std::string>>& data,
                               const std::vector<std::string>& headers) {
    if (data.empty()) {
        std::cout << "\nНет данных для отображения.\n";
        return;
    }

    std::vector<size_t> col_widths(headers.size());
    for (size_t i = 0; i < headers.size(); i++) {
        col_widths[i] = headers[i].length();
    }
    
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size() && i < col_widths.size(); i++) {
            col_widths[i] = std::max(col_widths[i], row[i].length());
        }
    }

    std::cout << "\n";
    for (size_t i = 0; i < headers.size(); i++) {
        std::cout << std::left << std::setw(static_cast<int>(col_widths[i]) + 2) << headers[i];
    }
    std::cout << "\n";

    for (size_t i = 0; i < headers.size(); i++) {
        std::cout << std::string(col_widths[i] + 2, '-');
    }
    std::cout << "\n";

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size() && i < col_widths.size(); i++) {
            std::cout << std::left << std::setw(static_cast<int>(col_widths[i]) + 2) << row[i];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}