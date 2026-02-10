// Улучшенный поиск заметок
#include "notes_manager.h"

class AdvancedSearch {
public:
    static std::vector<Note> searchByDateRange(const NotesManager& manager, 
                                               time_t start, time_t end);
    static std::vector<Note> searchByTags(const NotesManager& manager, 
                                          const std::vector<std::string>& tags);
};
