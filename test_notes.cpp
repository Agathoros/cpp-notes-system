#include <gtest/gtest.h>
#include "note.h"
#include "notes_manager.h"

// Тесты для класса Note
class NoteTest : public ::testing::Test {
protected:
    void SetUp() override {
        testNote = Note("Тестовая заметка", "Содержимое заметки", "Тест");
    }
    
    Note testNote;
};

TEST_F(NoteTest, ConstructorWithParameters) {
    EXPECT_EQ(testNote.getTitle(), "Тестовая заметка");
    EXPECT_EQ(testNote.getContent(), "Содержимое заметки");
    EXPECT_EQ(testNote.getCategory(), "Тест");
    EXPECT_GT(testNote.getCreationTime(), 0);
}

TEST_F(NoteTest, DefaultConstructor) {
    Note emptyNote;
    EXPECT_EQ(emptyNote.getTitle(), "");
    EXPECT_EQ(emptyNote.getContent(), "");
    EXPECT_EQ(emptyNote.getCategory(), "Общее");
    EXPECT_GT(emptyNote.getCreationTime(), 0);
}

TEST_F(NoteTest, SettersAndGetters) {
    testNote.setTitle("Новый заголовок");
    testNote.setContent("Новое содержимое");
    testNote.setCategory("Новая категория");
    
    EXPECT_EQ(testNote.getTitle(), "Новый заголовок");
    EXPECT_EQ(testNote.getContent(), "Новое содержимое");
    EXPECT_EQ(testNote.getCategory(), "Новая категория");
}

TEST_F(NoteTest, ContainsSearch) {
    EXPECT_TRUE(testNote.contains("Тестовая"));
    EXPECT_TRUE(testNote.contains("заметка"));
    EXPECT_TRUE(testNote.contains("Содержимое"));
    EXPECT_FALSE(testNote.contains("Несуществующий"));
}

TEST_F(NoteTest, ContainsSearchCaseInsensitiveLatin) {
    Note latinNote("Test Note", "This is test content", "Test");
    EXPECT_TRUE(latinNote.contains("test"));
    EXPECT_TRUE(latinNote.contains("TEST"));
    EXPECT_TRUE(latinNote.contains("Test"));
    EXPECT_TRUE(latinNote.contains("content"));
    EXPECT_TRUE(latinNote.contains("CONTENT"));
}

TEST_F(NoteTest, EqualityOperator) {
    Note sameNote("Тестовая заметка", "Содержимое заметки", "Тест");
    Note differentNote("Другая заметка", "Другое содержимое", "Другая");
    
    EXPECT_TRUE(testNote == sameNote);
    EXPECT_FALSE(testNote == differentNote);
}

// Тесты для класса NotesManager
class NotesManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager = new NotesManager("test_notes.txt");
        // Очищаем файл для чистых тестов
        manager->saveToFile();
    }
    
    void TearDown() override {
        delete manager;
    }
    
    NotesManager* manager;
};

TEST_F(NotesManagerTest, AddNote) {
    EXPECT_EQ(manager->getNotesCount(), 0);
    
    manager->addNote("Заголовок", "Содержимое", "Категория");
    EXPECT_EQ(manager->getNotesCount(), 1);
    
    Note note("Заголовок2", "Содержимое2");
    manager->addNote(note);
    EXPECT_EQ(manager->getNotesCount(), 2);
}

TEST_F(NotesManagerTest, RemoveNote) {
    manager->addNote("Заголовок", "Содержимое", "Категория");
    manager->addNote("Заголовок2", "Содержимое2", "Категория2");
    
    EXPECT_TRUE(manager->removeNote(0));
    EXPECT_EQ(manager->getNotesCount(), 1);
    
    EXPECT_FALSE(manager->removeNote(5)); // Неверный индекс
    EXPECT_EQ(manager->getNotesCount(), 1);
}

TEST_F(NotesManagerTest, EditNote) {
    manager->addNote("Заголовок", "Содержимое", "Категория");
    
    EXPECT_TRUE(manager->editNote(0, "Новый заголовок", "Новое содержимое", "Новая категория"));
    
    Note editedNote = manager->getNote(0);
    EXPECT_EQ(editedNote.getTitle(), "Новый заголовок");
    EXPECT_EQ(editedNote.getContent(), "Новое содержимое");
    EXPECT_EQ(editedNote.getCategory(), "Новая категория");
    
    EXPECT_FALSE(manager->editNote(5, "Заголовок", "Содержимое", "Категория")); // Неверный индекс
}

TEST_F(NotesManagerTest, SearchNotes) {
    manager->addNote("Покупки", "Купить молоко и хлеб", "Личное");
    manager->addNote("Работа", "Подготовить презентацию", "Работа");
    manager->addNote("Учеба", "Изучить C++", "Образование");
    
    std::vector<Note> results = manager->searchNotes("молоко");
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].getTitle(), "Покупки");
    
    results = manager->searchNotes("C++");
    EXPECT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].getTitle(), "Учеба");
    
    results = manager->searchNotes("несуществующий");
    EXPECT_EQ(results.size(), 0);
}

TEST_F(NotesManagerTest, GetNotesByCategory) {
    manager->addNote("Заметка1", "Содержимое1", "Работа");
    manager->addNote("Заметка2", "Содержимое2", "Личное");
    manager->addNote("Заметка3", "Содержимое3", "Работа");
    
    std::vector<Note> workNotes = manager->getNotesByCategory("Работа");
    EXPECT_EQ(workNotes.size(), 2);
    
    std::vector<Note> personalNotes = manager->getNotesByCategory("Личное");
    EXPECT_EQ(personalNotes.size(), 1);
    
    std::vector<Note> emptyCategory = manager->getNotesByCategory("Несуществующая");
    EXPECT_EQ(emptyCategory.size(), 0);
}

TEST_F(NotesManagerTest, GetAllCategories) {
    manager->addNote("Заметка1", "Содержимое1", "Работа");
    manager->addNote("Заметка2", "Содержимое2", "Личное");
    manager->addNote("Заметка3", "Содержимое3", "Работа");
    
    std::vector<std::string> categories = manager->getAllCategories();
    EXPECT_EQ(categories.size(), 2);
    
    // Проверяем, что категории уникальны
    bool hasWork = false, hasPersonal = false;
    for (const auto& category : categories) {
        if (category == "Работа") hasWork = true;
        if (category == "Личное") hasPersonal = true;
    }
    EXPECT_TRUE(hasWork);
    EXPECT_TRUE(hasPersonal);
}

TEST_F(NotesManagerTest, IsEmpty) {
    EXPECT_TRUE(manager->isEmpty());
    
    manager->addNote("Заголовок", "Содержимое");
    EXPECT_FALSE(manager->isEmpty());
}

TEST_F(NotesManagerTest, GetNote) {
    manager->addNote("Заголовок", "Содержимое", "Категория");
    
    Note note = manager->getNote(0);
    EXPECT_EQ(note.getTitle(), "Заголовок");
    EXPECT_EQ(note.getContent(), "Содержимое");
    EXPECT_EQ(note.getCategory(), "Категория");
    
    // Тест с неверным индексом
    Note emptyNote = manager->getNote(5);
    EXPECT_EQ(emptyNote.getTitle(), "");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
