#include "FlyweightPattern.h"


class CharacterStyle {
public:
    std::string fontName;
    int fontSize;
    std::string color;

    CharacterStyle(const std::string& font, int size, const std::string& col)
        : fontName(font), fontSize(size), color(col) {
        std::cout << "Створення CharacterStyle: Шрифт=" << fontName
                  << ", Розмір=" << fontSize << ", Колір=" << color << std::endl;
    }

    void display() const {
    }
};

class CharacterStyleFactory {
private:
    std::map<std::string, CharacterStyle*> styles;

    std::string getKey(const std::string& font, int size, const std::string& color) {
        return font + "_" + std::to_string(size) + "_" + color;
    }

public:
    ~CharacterStyleFactory() {
        for (auto const& [key, val] : styles) {
            delete val;
        }
        styles.clear();
        std::cout << "Фабрику CharacterStyleFactory знищено. Всі стилі звільнено." << std::endl;
    }

    CharacterStyle* getStyle(const std::string& font, int size, const std::string& color) {
        std::string key = getKey(font, size, color);
        if (styles.find(key) == styles.end()) {
            std::cout << "Фабрика: Стиль для ключа '" << key << "' не знайдено. Створюю новий." << std::endl;
            styles[key] = new CharacterStyle(font, size, color);
        } else {
            std::cout << "Фабрика: Повторне використання існуючого стилю для ключа '" << key << "'." << std::endl;
        }
        return styles[key];
    }

    int getTotalStylesCreated() const {
        return styles.size();
    }
};

class Character {
public:
    char characterSymbol;
    CharacterStyle* style;

    Character(char symbol, CharacterStyle* s) : characterSymbol(symbol), style(s) {}

    void display() const {
        std::cout << "Символ: '" << characterSymbol
                  << "' (Шрифт: " << style->fontName
                  << ", Розмір: " << style->fontSize
                  << ", Колір: " << style->color << ")" << std::endl;
    }
};

void demonstrateFlyweight() {
    std::cout << "\n=== Демонстрація шаблону Легковаговик ===\n\n";

    CharacterStyleFactory factory;
    std::vector<Character*> document;

    document.push_back(new Character('H', factory.getStyle("Times New Roman", 12, "Чорний")));
    document.push_back(new Character('e', factory.getStyle("Times New Roman", 12, "Чорний")));
    document.push_back(new Character('l', factory.getStyle("Times New Roman", 12, "Чорний")));
    document.push_back(new Character('l', factory.getStyle("Times New Roman", 12, "Чорний")));
    document.push_back(new Character('o', factory.getStyle("Times New Roman", 12, "Чорний")));

    document.push_back(new Character(' ', factory.getStyle("Arial", 10, "Сірий")));

    document.push_back(new Character('W', factory.getStyle("Arial", 16, "Червоний")));
    document.push_back(new Character('o', factory.getStyle("Arial", 16, "Червоний")));
    document.push_back(new Character('r', factory.getStyle("Arial", 16, "Червоний")));
    document.push_back(new Character('l', factory.getStyle("Arial", 16, "Червоний")));
    document.push_back(new Character('d', factory.getStyle("Arial", 16, "Червоний")));

    document.push_back(new Character('!', factory.getStyle("Times New Roman", 12, "Чорний")));

    std::cout << "\n--- Відображення документа ---" << std::endl;
    for (const auto* ch : document) {
        ch->display();
    }

    std::cout << "\nЗагальна кількість створених об'єктів CharacterStyle: " << factory.getTotalStylesCreated() << std::endl;
    std::cout << "Загальна кількість об'єктів Character у документі: " << document.size() << std::endl;

    for (auto* ch : document) {
        delete ch;
    }
    document.clear();

    std::cout << "\n=== Кінець демонстрації Легковаговика ===\n";
}