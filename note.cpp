#include "note.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

// Конструктор по умолчанию
Note::Note() : title(""), content(""), category("Общее") {
    creationTime = time(nullptr);
}

// Конструктор с параметрами
Note::Note(const std::string& title, const std::string& content, const std::string& category) 
    : title(title), content(content), category(category) {
    creationTime = time(nullptr);
}

// Геттеры
std::string Note::getTitle() const {
    return title;
}

std::string Note::getContent() const {
    return content;
}

std::string Note::getCategory() const {
    return category;
}

time_t Note::getCreationTime() const {
    return creationTime;
}

std::string Note::getFormattedCreationTime() const {
    char buffer[100];
    struct tm* timeinfo = localtime(&creationTime);
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", timeinfo);
    return std::string(buffer);
}

// Сеттеры
void Note::setTitle(const std::string& title) {
    this->title = title;
}

void Note::setContent(const std::string& content) {
    this->content = content;
}

void Note::setCategory(const std::string& category) {
    this->category = category;
}

// Методы
void Note::display() const {
    std::cout << "=== " << title << " ===" << std::endl;
    std::cout << "Категория: " << category << std::endl;
    std::cout << "Создано: " << getFormattedCreationTime() << std::endl;
    std::cout << "Содержимое:" << std::endl;
    std::cout << content << std::endl;
    std::cout << "========================" << std::endl;
}

// Вспомогательная функция для преобразования в нижний регистр (только для латиницы)
std::string toLowerCaseLatin(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](unsigned char c) { 
                       // Преобразуем только латинские буквы
                       if (c >= 'A' && c <= 'Z') {
                           return static_cast<char>(c + 32);
                       }
                       return static_cast<char>(c);
                   });
    return result;
}

bool Note::contains(const std::string& searchTerm) const {
    // Для кириллицы используем точный поиск
    if (title.find(searchTerm) != std::string::npos || 
        content.find(searchTerm) != std::string::npos) {
        return true;
    }
    
    // Для латиницы используем поиск без учёта регистра
    std::string lowerTitle = toLowerCaseLatin(title);
    std::string lowerContent = toLowerCaseLatin(content);
    std::string lowerSearchTerm = toLowerCaseLatin(searchTerm);
    
    return lowerTitle.find(lowerSearchTerm) != std::string::npos || 
           lowerContent.find(lowerSearchTerm) != std::string::npos;
}

// Операторы
bool Note::operator==(const Note& other) const {
    return title == other.title && content == other.content && category == other.category;
}

std::ostream& operator<<(std::ostream& os, const Note& note) {
    os << note.title << "|" << note.content << "|" << note.category << "|" << note.creationTime;
    return os;
}
