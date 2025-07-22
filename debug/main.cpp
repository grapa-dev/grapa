#include "GrapaDB.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== ROW TABLE INDEX CORRUPTION BUG DEMO (C++) ===\n\n";

    /*
    // Create a new GrapaDB instance (in-memory or temp file)
    GrapaDB db;
    const char* dbFile = "row_bug_demo.db";
    u64 firstTree = 0;
    if (db.Create(dbFile, GrapaDB::RTABLE_TREE, firstTree)) {
        std::cerr << "Failed to create ROW table DB file\n";
        return 1;
    }

    // Add fields: name (STR), age (INT), city (STR)
    GrapaDBTable table;
    db.CreateTable(firstTree, GrapaDB::RTABLE_TREE, 1, table);
    db.CreateTableField(table, 1, "name", "STR", "VAR", 0, 0);
    db.CreateTableField(table, 2, "age", "INT", "FIX", 4, 0);
    db.CreateTableField(table, 3, "city", "STR", "VAR", 0, 0);

    // Add first record (user1)
    db.SetRecordField(table, "user1", 1, "Alice");
    db.SetRecordField(table, "user1", 2, "25");
    db.SetRecordField(table, "user1", 3, "New York");

    // Retrieve first record
    std::string name1, age1, city1;
    db.GetRecordField(table, "user1", 1, name1);
    db.GetRecordField(table, "user1", 2, age1);
    db.GetRecordField(table, "user1", 3, city1);
    std::cout << "user1: " << name1 << ", " << age1 << ", " << city1 << "\n";

    // Add second record (user2)
    db.SetRecordField(table, "user2", 1, "Bob");
    db.SetRecordField(table, "user2", 2, "30");
    db.SetRecordField(table, "user2", 3, "Los Angeles");

    // Retrieve both records
    std::string name1_again, name2;
    db.GetRecordField(table, "user1", 1, name1_again);
    db.GetRecordField(table, "user2", 1, name2);
    std::cout << "user1 (after user2): " << name1_again << ", user2: " << name2 << "\n";

    // Add third record (user3)
    db.SetRecordField(table, "user3", 1, "Charlie");
    db.SetRecordField(table, "user3", 2, "35");
    db.SetRecordField(table, "user3", 3, "Chicago");

    // Retrieve all records (user1 should now fail if bug is present)
    std::string name1_bug, name2_bug, name3_bug;
    db.GetRecordField(table, "user1", 1, name1_bug);
    db.GetRecordField(table, "user2", 1, name2_bug);
    db.GetRecordField(table, "user3", 1, name3_bug);
    std::cout << "user1 (after user3 - BUG): " << name1_bug << "\n";
    std::cout << "user2 (after user3): " << name2_bug << "\n";
    std::cout << "user3 (after user3): " << name3_bug << "\n";

    // Show debug output
    std::cout << "\nROW table debug output (showing corruption):\n";
    db.DumpTree(firstTree, nullptr); // Or use a custom debug print if available

    std::cout << "\n=== BUG DEMONSTRATION SUMMARY ===\n";
    std::cout << "ROW table: First record becomes unreadable after adding third record (if bug present)\n";
    std::cout << "Workaround: Use COL tables instead of ROW tables\n";
     */
    std::cout << "Demo completed\n";
    return 0;
} 
