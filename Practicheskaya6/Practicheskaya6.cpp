#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <windows.h>
#include <Windows.h> 
#pragma execution_character_set("utf-8")
using namespace std;

class Hero { //Базовый класс героя (инкапсуляция: поля private)
private:
    string name;
    int hp;
    int attack;
    int defense;
    bool blocking = false;   // true если герой в защите

public:
    Hero(string name, int hp, int attack, int defense)
        : name(name), hp(hp), attack(attack), defense(defense) {
        if (hp <= 0 || attack < 0 || defense < 0)
            throw invalid_argument("Неверные характеристики героя");
    }
    virtual ~Hero() {}

    string getName() const { return name; }// геттеры и сеттер
    int getHp() const { return hp; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    bool isAlive() const { return hp > 0; }
    void setName(string newName) { name = newName; }
    void setBlocking(bool value) { blocking = value; }

    virtual int calcDamage(const Hero& enemy) const {
        int dmg = attack - enemy.defense;
        return dmg < 1 ? 1 : dmg;
    }
    virtual string getInfo() const = 0;

    void attackEnemy(Hero& enemy) { // Атака. Враг передаётся по ссылке, чтобы изменился оригинал
        int dmg = calcDamage(enemy);
        if (enemy.blocking) {
            dmg = dmg / 2;   // защита уменьшает урон вдвое
            if (dmg < 1) dmg = 1;
            enemy.blocking = false;
            cout << enemy.name << " блокирует часть удара!" << endl;
        }
        enemy.hp -= dmg;
        if (enemy.hp < 0) enemy.hp = 0;
        cout << name << " атакует и наносит " << dmg << " урона." << endl;
    }

    void defend() {
        blocking = true;
        cout << name << " встаёт в защиту (следующий удар слабее вдвое)." << endl;
    }
};

class Warrior : public Hero { //Наследники
public:
    Warrior() : Hero("Воин", 140, 15, 10) {}
    string getInfo() const override { return "Воин   - HP 140, атака 15, броня 10 (обычный урон)"; }
};

class Archer : public Hero {
public:
    Archer() : Hero("Лучник", 100, 20, 5) {}
    int calcDamage(const Hero& enemy) const override { // игнорирует половину брони
        int dmg = getAttack() - enemy.getDefense() / 2;
        return dmg < 1 ? 1 : dmg;
    }
    string getInfo() const override { return "Лучник - HP 100, атака 20, броня 5 (игнорирует половину брони)"; }
};

class Mage : public Hero {
public:
    Mage() : Hero("Маг", 80, 25, 2) {}
    int calcDamage(const Hero&) const override {   // игнорирует всю броню
        return getAttack();
    }
    string getInfo() const override { return "Маг    - HP 80, атака 25, броня 2 (игнорирует всю броню)"; }
};

unique_ptr<Hero> createHero(int id) {
    if (id == 1) return make_unique<Warrior>();
    if (id == 2) return make_unique<Archer>();
    if (id == 3) return make_unique<Mage>();
    throw invalid_argument("Такого героя нет");
}

int readInt(string text, int min, int max) {
    while (true) {
        cout << text;
        string line;
        if (!getline(cin, line)) throw runtime_error("Ввод завершён");
        try {
            int value = stoi(line);
            if (value < min || value > max) throw out_of_range("вне диапазона");
            return value;
        }
        catch (const exception&) {
            cout << "Ошибка! Введите число от " << min << " до " << max << "." << endl;
        }
    }
}

void showHeroes() {
    for (int i = 1; i <= 3; i++)
        cout << i << " - " << createHero(i)->getInfo() << endl;
}

bool battle(Hero& player, Hero& enemy) { // Бой: true - победил игрок
    bool enemyDefended = false;
    while (player.isAlive() && enemy.isAlive()) {
        cout << "\n" << player.getName() << ": " << player.getHp() << " HP   |   "
            << enemy.getName() << ": " << enemy.getHp() << " HP" << endl;
        int action = readInt("Ваш ход (1 - атака, 2 - защита): ", 1, 2);
        player.setBlocking(false);
        if (action == 1) player.attackEnemy(enemy);
        else player.defend();
        if (!enemy.isAlive()) break;

        // ход соперника: при HP < 30% защищается (не два раза подряд), иначе атакует
        enemy.setBlocking(false);
        if (enemy.getHp() < 30 && !enemyDefended) {
            enemy.defend();
            enemyDefended = true;
        }
        else {
            enemy.attackEnemy(player);
            enemyDefended = false;
        }
    }
    return player.isAlive();
}

int main() {
    SetConsoleCP(65001); // устанавливаем кодировку входящего потока данных на UTF-8 
    SetConsoleOutputCP(65001); // устанавливаем кодировку исходящего потока данных на UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    try {
        cout << "АРЕНА ГЕРОЕВ" << endl;
        while (true) {
            cout << "\n1 - Выбрать героя\n0 - Выход" << endl;
            if (readInt("Ваш выбор: ", 0, 1) == 0) {
                cout << "До встречи!" << endl;
                break;
            }
            cout << "\nВыберите героя:" << endl;
            showHeroes();
            int heroId = readInt("Ваш герой: ", 1, 3);

            cout << "\nВыберите соперника:" << endl;
            showHeroes();
            int enemyId = readInt("Соперник: ", 1, 3);

            unique_ptr<Hero> player = createHero(heroId);
            unique_ptr<Hero> enemy = createHero(enemyId);
            player->setName(player->getName() + " (вы)");
            enemy->setName(enemy->getName() + " (соперник)");

            if (battle(*player, *enemy)) {
                cout << "\nПОБЕДА! Вы выиграли бой!" << endl;
            }
            else {
                cout << "\nВы проиграли сражение. Игра окончена." << endl;
                break;
            }
        }
    }
    catch (const exception& e) {
        cout << "Программа завершена: " << e.what() << endl;
    }
    return 0;
}