#ifndef NOTES_MANAGER_H
#define NOTES_MANAGER_H

#include "note.h"
#include <vector>
#include <string>
#include <fstream>

class NotesManager {
private:
    std::vector<Note> notes;
    std::string filename;

public:
    NotesManager(const std::string& filename = "notes.txt");
    
    // Основные операции
    void addNote(const Note& note);
    void addNote(const std::string& title, const std::string& content, const std::string& category = "Общее");
    bool removeNote(int index);
    bool editNote(int index, const std::string& title, const std::string& content, const std::string& category);
    
    // Поиск и фильтрация
    std::vector<Note> searchNotes(const std::string& searchTerm) const;
    std::vector<Note> getNotesByCategory(const std::string& category) const;
    std::vector<std::string> getAllCategories() const;
    
    // Отображение
    void displayAllNotes() const;
    void displayNotes(const std::vector<Note>& notesToDisplay) const;
    void displayNote(int index) const;
    
    // Файловые операции
    bool saveToFile() const;
    bool loadFromFile();
    
    // Информация
    int getNotesCount() const;
    bool isEmpty() const;
    Note getNote(int index) const;
};

#endif // NOTES_MANAGER_H
