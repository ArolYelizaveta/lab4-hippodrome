// UI.cpp
#include "../includes/UI.h"
#include <iostream>
#include <limits>
#include <string>

UI::UI() : dbManager("hippodrome.db"), businessLogic(dbManager), currentUserId(0), isAuthenticated(false) {}

UI::~UI() {
    dbManager.disconnect();
}

void UI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void UI::waitForEnter() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

void UI::showLoginMenu() {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "   СИСТЕМА УПРАВЛЕНИЯ ИППОДРОМОМ\n";
    std::cout << "========================================\n\n";
    std::cout << "АВТОРИЗАЦИЯ\n";
    std::cout << "----------------------------------------\n";

    std::string username, password;
    std::cout << "Логин: ";
    std::cin >> username;
    std::cout << "Пароль: ";
    std::cin >> password;

    std::string role;
    int foreign_id;

    if (dbManager.authenticate(username, password, role, foreign_id)) {
        isAuthenticated = true;
        currentRole = role;
        currentUserId = foreign_id;
        currentUsername = username;

        std::cout << "\nДобро пожаловать, " << username << "!\n";
        std::cout << "Роль: " << (role == "owner" ? "Владелец" : "Жокей") << "\n";
        waitForEnter();
    } else {
        std::cout << "\nОшибка: Неверный логин или пароль!\n";
        waitForEnter();
    }
}

void UI::showMainMenu() {
    if (!isAuthenticated) {
        showLoginMenu();
        if (!isAuthenticated) return;
    }

    while (isAuthenticated) {
        clearScreen();
        std::cout << "========================================\n";
        std::cout << "   ГЛАВНОЕ МЕНЮ\n";
        std::cout << "========================================\n";
        std::cout << "Пользователь: " << currentUsername << " ("
                  << (currentRole == "owner" ? "Владелец" : "Жокей") << ")\n";
        std::cout << "----------------------------------------\n";

        if (currentRole == "owner") {
            std::cout << "1. Просмотр моих лошадей и забегов\n";
            std::cout << "2. Информация о лучшей лошади\n";
            std::cout << "3. Забеги за указанный период\n";
            std::cout << "4. Добавить новую лошадь\n";
            std::cout << "5. Обновить данные лошади\n";
            std::cout << "6. Удалить лошадь\n";
            std::cout << "7. Добавить забег\n";
            std::cout << "8. Распределить призовые\n";
        } else {
            std::cout << "1. Мои забеги\n";
            std::cout << "2. Забеги за указанный период\n";
            std::cout << "3. Информация о самом активном жокее\n";
            std::cout << "4. Добавить забег\n";
            std::cout << "5. Распределить призовые\n";
        }

        std::cout << "0. Выйти\n";
        std::cout << "----------------------------------------\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Неверный ввод!\n";
            waitForEnter();
            continue;
        }

        if (currentRole == "owner") {
            switch (choice) {
                case 1: showOwnerHorses(); break;
                case 2: showBestHorseInfo(); break;
                case 3: showRacesByPeriod(); break;
                case 4: addNewHorse(); break;
                case 5: updateHorseInfo(); break;
                case 6: deleteHorse(); break;
                case 7: addNewRace(); break;
                case 8: distributePrize(); break;
                case 0: isAuthenticated = false; break;
                default: std::cout << "Неверный выбор!\n"; waitForEnter();
            }
        } else {
            switch (choice) {
                case 1: showJockeyOwnRaces(); break;
                case 2: showJockeyRacesByPeriod(); break;
                case 3: showMostActiveJockey(); break;
                case 4: addNewRace(); break;
                case 5: distributePrize(); break;
                case 0: isAuthenticated = false; break;
                default: std::cout << "Неверный выбор!\n"; waitForEnter();
            }
        }
    }
}

void UI::showOwnerHorses() {
    clearScreen();
    std::cout << "МОИ ЛОШАДИ И ЗАБЕГИ\n";
    std::cout << "========================================\n";

    const auto results = dbManager.getOwnerHorsesWithRaces(currentUserId);

    const std::vector<std::string> headers = {"Кличка", "Возраст", "Стаж", "Цена", "Дата забега", "Забег №", "Место", "Жокей"};
    BusinessLogic::printTable(results, headers);

    waitForEnter();
}

void UI::showBestHorseInfo() {
    clearScreen();
    std::cout << "ЛУЧШАЯ ЛОШАДЬ (максимум побед)\n";
    std::cout << "========================================\n";

    const auto results = dbManager.getBestHorseInfo();

    if (!results.empty() && !results[0].empty()) {
        std::cout << "ID лошади: " << results[0][0] << "\n";
        std::cout << "Кличка: " << results[0][1] << "\n";
        std::cout << "Возраст: " << results[0][2] << "\n";
        std::cout << "Стаж участия: " << results[0][3] << " лет\n";
        std::cout << "Цена: " << results[0][4] << " руб.\n";
        std::cout << "Владелец: " << results[0][5] << "\n";
        if (results[0].size() > 6) {
            std::cout << "Забеги (дата - жокей): " << results[0][6] << "\n";
        }
    } else {
        std::cout << "Нет данных о победителях.\n";
    }

    waitForEnter();
}

void UI::showMostActiveJockey() {
    clearScreen();
    std::cout << "САМЫЙ АКТИВНЫЙ ЖОКЕЙ\n";
    std::cout << "========================================\n";

    const auto results = dbManager.getMostActiveJockey();

    if (!results.empty() && !results[0].empty()) {
        std::cout << "ID жокея: " << results[0][0] << "\n";
        std::cout << "Фамилия: " << results[0][1] << "\n";
        std::cout << "Стаж: " << results[0][2] << " лет\n";
        std::cout << "Год рождения: " << results[0][3] << "\n";
        std::cout << "Адрес: " << results[0][4] << "\n";
        if (results[0].size() > 5) {
            std::cout << "Всего забегов: " << results[0][5] << "\n";
        }
    } else {
        std::cout << "Нет данных о забегах.\n";
    }

    waitForEnter();
}

void UI::showJockeyOwnRaces() {
    clearScreen();
    std::cout << "МОИ ЗАБЕГИ\n";
    std::cout << "========================================\n";

    const auto results = dbManager.getJockeyRaces(currentUserId);

    const std::vector<std::string> headers = {"Дата", "Забег №", "Место", "Лошадь"};
    BusinessLogic::printTable(results, headers);

    waitForEnter();
}

void UI::showRacesByPeriod() {
    clearScreen();
    std::cout << "ЗАБЕГИ ЗА ПЕРИОД\n";
    std::cout << "========================================\n";

    std::string start_date, end_date;
    std::cout << "Введите начальную дату (ГГГГ-ММ-ДД): ";
    std::cin >> start_date;
    std::cout << "Введите конечную дату (ГГГГ-ММ-ДД): ";
    std::cin >> end_date;

    const auto results = dbManager.getRacesByPeriod(start_date, end_date);

    const std::vector<std::string> headers = {"Дата", "Забег №", "Место", "Лошадь", "Жокей"};
    BusinessLogic::printTable(results, headers);

    waitForEnter();
}

void UI::showJockeyRacesByPeriod() const {
    clearScreen();
    std::cout << "МОИ ЗАБЕГИ ЗА ПЕРИОД\n";
    std::cout << "========================================\n";

    std::string start_date, end_date;
    std::cout << "Введите начальную дату (ГГГГ-ММ-ДД): ";
    std::cin >> start_date;
    std::cout << "Введите конечную дату (ГГГГ-ММ-ДД): ";
    std::cin >> end_date;

    std::string error;
    const auto results = businessLogic.getJockeyRacesByPeriod(currentUserId, start_date, end_date, error);

    if (error != "OK") {
        std::cout << error << "\n";
    } else {
        const std::vector<std::string> headers = {"Дата", "Забег №", "Место", "Лошадь"};
        BusinessLogic::printTable(results, headers);
    }

    waitForEnter();
}

void UI::addNewHorse() {
    if (currentRole != "owner") {
        std::cout << "Доступно только владельцам!\n";
        waitForEnter();
        return;
    }

    clearScreen();
    std::cout << "ДОБАВЛЕНИЕ НОВОЙ ЛОШАДИ\n";
    std::cout << "========================================\n";

    Horse horse;
    horse.owner_id = currentUserId;

    std::cout << "Кличка: ";
    std::cin.ignore();
    std::getline(std::cin, horse.name);
    std::cout << "Возраст: ";
    std::cin >> horse.age;
    std::cout << "Стаж участия (лет): ";
    std::cin >> horse.experience_years;
    std::cout << "Цена: ";
    std::cin >> horse.price;

    if (dbManager.addHorse(horse)) {
        std::cout << "\nЛошадь успешно добавлена!\n";
    } else {
        std::cout << "\nОшибка при добавлении лошади!\n";
    }

    waitForEnter();
}

void UI::updateHorseInfo() {
    if (currentRole != "owner") {
        std::cout << "Доступно только владельцам!\n";
        waitForEnter();
        return;
    }

    clearScreen();
    std::cout << "ОБНОВЛЕНИЕ ДАННЫХ ЛОШАДИ\n";
    std::cout << "========================================\n";

    int horse_id;
    std::cout << "Введите ID лошади: ";
    std::cin >> horse_id;

    Horse horse;
    horse.owner_id = currentUserId;

    std::cout << "Новая кличка: ";
    std::cin.ignore();
    std::getline(std::cin, horse.name);
    std::cout << "Новый возраст: ";
    std::cin >> horse.age;
    std::cout << "Новый стаж: ";
    std::cin >> horse.experience_years;
    std::cout << "Новая цена: ";
    std::cin >> horse.price;

    if (dbManager.updateHorse(horse_id, horse)) {
        std::cout << "\nДанные лошади успешно обновлены!\n";
    } else {
        std::cout << "\nОшибка при обновлении данных!\n";
    }

    waitForEnter();
}

void UI::deleteHorse() {
    if (currentRole != "owner") {
        std::cout << "Доступно только владельцам!\n";
        waitForEnter();
        return;
    }

    clearScreen();
    std::cout << "УДАЛЕНИЕ ЛОШАДИ\n";
    std::cout << "========================================\n";

    int horse_id;
    std::cout << "Введите ID лошади для удаления: ";
    std::cin >> horse_id;

    std::cout << "Вы уверены? (y/n): ";
    char confirm;
    std::cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        if (dbManager.deleteHorse(horse_id)) {
            std::cout << "\nЛошадь успешно удалена!\n";
        } else {
            std::cout << "\nОшибка при удалении лошади!\n";
        }
    }

    waitForEnter();
}

void UI::addNewRace() {
    clearScreen();
    std::cout << "ДОБАВЛЕНИЕ НОВОГО ЗАБЕГА\n";
    std::cout << "========================================\n";

    Race race;
    std::cout << "Дата (ГГГГ-ММ-ДД): ";
    std::cin >> race.date;
    std::cout << "Номер забега: ";
    std::cin >> race.race_number;
    std::cout << "Занятое место: ";
    std::cin >> race.place;
    std::cout << "ID лошади: ";
    std::cin >> race.horse_id;
    std::cout << "ID жокея: ";
    std::cin >> race.jockey_id;

    std::string error;
    if (businessLogic.validateRaceData(race.horse_id, race.jockey_id, error)) {
        if (dbManager.addRace(race)) {
            std::cout << "\nЗабег успешно добавлен!\n";
        } else {
            std::cout << "\nОшибка при добавлении забега!\n";
        }
    } else {
        std::cout << "\n" << error << "\n";
    }

    waitForEnter();
}

void UI::distributePrize() const {
    clearScreen();
    std::cout << "РАСПРЕДЕЛЕНИЕ ПРИЗОВОГО ФОНДА\n";
    std::cout << "========================================\n";

    double prize_fund;
    std::cout << "Введите размер призового фонда: ";
    std::cin >> prize_fund;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "\nОшибка: Некорректный ввод!\n";
        waitForEnter();
        return;
    }

    std::string result;

    businessLogic.distributePrizeForLastRace(prize_fund, result);
    std::cout << "\n" << result << "\n";

    waitForEnter();
}

void UI::run() {
    while (true) {
        showMainMenu();
        if (!isAuthenticated) {
            char choice;
            std::cout << "Выход из программы? (y/n): ";
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                break;
            }
            showLoginMenu();
        }
    }
}