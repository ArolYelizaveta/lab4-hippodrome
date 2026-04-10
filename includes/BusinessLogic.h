// BusinessLogic.h
#ifndef BUSINESSLOGIC_H
#define BUSINESSLOGIC_H

#include "DatabaseManager.h"
#include <string>
#include <vector>

class BusinessLogic {
    DatabaseManager& dbManager;
    
public:
    explicit BusinessLogic(DatabaseManager& db);

    bool validateRaceData(int horse_id, int jockey_id, std::string& error_message) const;

    bool distributePrizeForLastRace(double prize_fund, std::string& result_message) const;

    std::vector<std::vector<std::string>> getJockeyRacesByPeriod(int jockey_id, 
                                                                   const std::string& start_date,
                                                                   const std::string& end_date,
                                                                   std::string& error_message) const;

    static void printTable(const std::vector<std::vector<std::string>>& data,
                           const std::vector<std::string>& headers);
};

#endif