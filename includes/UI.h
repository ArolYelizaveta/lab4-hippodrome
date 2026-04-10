// UI.h
#ifndef UI_H
#define UI_H

#include "DatabaseManager.h"
#include "BusinessLogic.h"
#include <string>

class UI {
    DatabaseManager dbManager;
    BusinessLogic businessLogic;

    std::string currentRole;
    int currentUserId;
    std::string currentUsername;

    bool isAuthenticated;

    static void clearScreen();
    static void waitForEnter();

    void showLoginMenu();
    void showMainMenu();

    void showOwnerHorses();
    void showBestHorseInfo();
    void showRacesByPeriod();
    void addNewHorse();
    void updateHorseInfo();
    void deleteHorse();

    void showJockeyOwnRaces();
    void showJockeyRacesByPeriod() const;
    void showMostActiveJockey();

    void addNewRace();
    void distributePrize() const;
    
public:
    UI();
    ~UI();
    
    void run();
};

#endif