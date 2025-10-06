#include "notes_manager.h"
#include <iostream>
#include <string>
#include <limits>

void displayMenu() {
    std::cout << "\n=== СИСТЕМА ЗАМЕТОК ===" << std::endl;
    std::cout << "1. Показать все заметки" << std::endl;
    std::cout << "2. Добавить заметку" << std::endl;
    std::cout << "3. Редактировать заметку" << std::endl;
    std::cout << "4. Удалить заметку" << std::endl;
    std::cout << "5. Показать заметку" << std::endl;
    std::cout << "6. Поиск заметок" << std::endl;
    std::cout << "7. Показать заметки по категории" << std::endl;
    std::cout << "8. Показать все категории" << std::endl;
    std::cout << "9. Сохранить заметки" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "Выберите действие: ";
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    clearInput();
    std::getline(std::cin, input);
    return input;
}

int getIntInput(const std::string& prompt) {
    int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Ошибка! Введите число: ";
        clearInput();
    }
    return value;
}

void addNote(NotesManager& manager) {
    std::string title = getInput("Введите заголовок заметки: ");
    std::string content = getInput("Введите содержимое заметки: ");
    std::string category = getInput("Введите категорию (или Enter для 'Общее'): ");
    
    if (category.empty()) {
        category = "Общее";
    }
    
    manager.addNote(title, content, category);
    std::cout << "Заметка добавлена!" << std::endl;
}

void editNote(NotesManager& manager) {
    manager.displayAllNotes();
    if (manager.isEmpty()) {
        return;
    }
    
    int index = getIntInput("Введите номер заметки для редактирования: ");
    if (index < 0 || index >= manager.getNotesCount()) {
        std::cout << "Неверный номер заметки!" << std::endl;
        return;
    }
    
    std::string title = getInput("Введите новый заголовок: ");
    std::string content = getInput("Введите новое содержимое: ");
    std::string category = getInput("Введите новую категорию: ");
    
    if (manager.editNote(index, title, content, category)) {
        std::cout << "Заметка отредактирована!" << std::endl;
    } else {
        std::cout << "Ошибка при редактировании заметки!" << std::endl;
    }
}

void deleteNote(NotesManager& manager) {
    manager.displayAllNotes();
    if (manager.isEmpty()) {
        return;
    }
    
    int index = getIntInput("Введите номер заметки для удаления: ");
    if (manager.removeNote(index)) {
        std::cout << "Заметка удалена!" << std::endl;
    } else {
        std::cout << "Неверный номер заметки!" << std::endl;
    }
}

void showNote(NotesManager& manager) {
    manager.displayAllNotes();
    if (manager.isEmpty()) {
        return;
    }
    
    int index = getIntInput("Введите номер заметки для просмотра: ");
    manager.displayNote(index);
}

void searchNotes(NotesManager& manager) {
    std::string searchTerm = getInput("Введите поисковый запрос: ");
    std::vector<Note> results = manager.searchNotes(searchTerm);
    
    std::cout << "\nРезультаты поиска:" << std::endl;
    manager.displayNotes(results);
}

void showNotesByCategory(NotesManager& manager) {
    std::string category = getInput("Введите категорию: ");
    std::vector<Note> results = manager.getNotesByCategory(category);
    
    std::cout << "\nЗаметки в категории '" << category << "':" << std::endl;
    manager.displayNotes(results);
}

void showAllCategories(NotesManager& manager) {
    std::vector<std::string> categories = manager.getAllCategories();
    
    std::cout << "\nВсе категории:" << std::endl;
    for (const auto& category : categories) {
        std::cout << "- " << category << std::endl;
    }
}

int main() {
    NotesManager manager;
    
    std::cout << "Добро пожаловать в систему заметок!" << std::endl;
    
    int choice;
    do {
        displayMenu();
        choice = getIntInput("");
        
        switch (choice) {
            case 1:
                manager.displayAllNotes();
                break;
            case 2:
                addNote(manager);
                break;
            case 3:
                editNote(manager);
                break;
            case 4:
                deleteNote(manager);
                break;
            case 5:
                showNote(manager);
                break;
            case 6:
                searchNotes(manager);
                break;
            case 7:
                showNotesByCategory(manager);
                break;
            case 8:
                showAllCategories(manager);
                break;
            case 9:
                if (manager.saveToFile()) {
                    std::cout << "Заметки сохранены!" << std::endl;
                } else {
                    std::cout << "Ошибка при сохранении!" << std::endl;
                }
                break;
            case 0:
                std::cout << "До свидания!" << std::endl;
                manager.saveToFile(); // Автосохранение при выходе
                break;
            default:
                std::cout << "Неверный выбор!" << std::endl;
        }
    } while (choice != 0);
    
    return 0;
}
