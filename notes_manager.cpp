#include "notes_manager.h"
#include <algorithm>
#include <iostream>
#include <sstream>

NotesManager::NotesManager(const std::string& filename) : filename(filename) {
    loadFromFile();
}

void NotesManager::addNote(const Note& note) {
    notes.push_back(note);
}

void NotesManager::addNote(const std::string& title, const std::string& content, const std::string& category) {
    notes.emplace_back(title, content, category);
}

bool NotesManager::removeNote(int index) {
    if (index >= 0 && index < static_cast<int>(notes.size())) {
        notes.erase(notes.begin() + index);
        return true;
    }
    return false;
}

bool NotesManager::editNote(int index, const std::string& title, const std::string& content, const std::string& category) {
    if (index >= 0 && index < static_cast<int>(notes.size())) {
        notes[index].setTitle(title);
        notes[index].setContent(content);
        notes[index].setCategory(category);
        return true;
    }
    return false;
}

std::vector<Note> NotesManager::searchNotes(const std::string& searchTerm) const {
    std::vector<Note> results;
    for (const auto& note : notes) {
        if (note.contains(searchTerm)) {
            results.push_back(note);
        }
    }
    return results;
}

std::vector<Note> NotesManager::getNotesByCategory(const std::string& category) const {
    std::vector<Note> results;
    for (const auto& note : notes) {
        if (note.getCategory() == category) {
            results.push_back(note);
        }
    }
    return results;
}

std::vector<std::string> NotesManager::getAllCategories() const {
    std::vector<std::string> categories;
    for (const auto& note : notes) {
        std::string category = note.getCategory();
        if (std::find(categories.begin(), categories.end(), category) == categories.end()) {
            categories.push_back(category);
        }
    }
    return categories;
}

void NotesManager::displayAllNotes() const {
    if (notes.empty()) {
        std::cout << "Заметок нет." << std::endl;
        return;
    }
    
    std::cout << "=== ВСЕ ЗАМЕТКИ ===" << std::endl;
    for (size_t i = 0; i < notes.size(); ++i) {
        std::cout << "[" << i << "] " << notes[i].getTitle() 
                  << " (" << notes[i].getCategory() << ")" << std::endl;
    }
}

void NotesManager::displayNotes(const std::vector<Note>& notesToDisplay) const {
    if (notesToDisplay.empty()) {
        std::cout << "Заметки не найдены." << std::endl;
        return;
    }
    
    for (size_t i = 0; i < notesToDisplay.size(); ++i) {
        std::cout << "[" << i << "] " << notesToDisplay[i].getTitle() 
                  << " (" << notesToDisplay[i].getCategory() << ")" << std::endl;
    }
}

void NotesManager::displayNote(int index) const {
    if (index >= 0 && index < static_cast<int>(notes.size())) {
        notes[index].display();
    } else {
        std::cout << "Неверный индекс заметки." << std::endl;
    }
}

bool NotesManager::saveToFile() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& note : notes) {
        file << note << std::endl;
    }
    
    file.close();
    return true;
}

bool NotesManager::loadFromFile() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false; // Файл не существует - это нормально для нового проекта
    }
    
    notes.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string title, content, category, timeStr;
        
        std::getline(iss, title, '|');
        std::getline(iss, content, '|');
        std::getline(iss, category, '|');
        std::getline(iss, timeStr, '|');
        
        if (!title.empty() && !content.empty()) {
            Note note(title, content, category);
            // Восстанавливаем время создания
            time_t creationTime = static_cast<time_t>(std::stoll(timeStr));
            // Примечание: в реальном проекте нужно добавить метод для установки времени
            notes.push_back(note);
        }
    }
    
    file.close();
    return true;
}

int NotesManager::getNotesCount() const {
    return static_cast<int>(notes.size());
}

bool NotesManager::isEmpty() const {
    return notes.empty();
}

Note NotesManager::getNote(int index) const {
    if (index >= 0 && index < static_cast<int>(notes.size())) {
        return notes[index];
    }
    return Note(); // Возвращаем пустую заметку
}
