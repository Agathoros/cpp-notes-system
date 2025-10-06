#ifndef NOTE_H
#define NOTE_H

#include <string>
#include <ctime>
#include <iostream>

class Note {
private:
    std::string title;
    std::string content;
    std::string category;
    time_t creationTime;

public:
    // Конструкторы
    Note();
    Note(const std::string& title, const std::string& content, const std::string& category = "Общее");
    
    // Геттеры
    std::string getTitle() const;
    std::string getContent() const;
    std::string getCategory() const;
    time_t getCreationTime() const;
    std::string getFormattedCreationTime() const;
    
    // Сеттеры
    void setTitle(const std::string& title);
    void setContent(const std::string& content);
    void setCategory(const std::string& category);
    
    // Методы
    void display() const;
    bool contains(const std::string& searchTerm) const;
    
    // Операторы
    bool operator==(const Note& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Note& note);
};

#endif // NOTE_H
