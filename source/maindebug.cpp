#define DEBUG_BTREE_INVESTIGATION
#include "grapa/GrapaBtree.h"
#include "grapa/GrapaFileIO.h"
#include <iostream>
#include <string>

// Simple test to verify BTree core functionality works
int main() {
    std::cout << "=== Simple BTree Test ===" << std::endl;
    
    GrapaBtree btree;
    GrapaFileIO fileIO;
    
    // Set up the file object
    btree.SetFile(&fileIO);
    
    // Create a simple BTree file
    u64 firstTree;
    GrapaError err = btree.Create("test_simple.db");
    if (err) {
        std::cerr << "Failed to create BTree file, error: " << err << std::endl;
        return 1;
    }
    std::cout << "BTree file created successfully" << std::endl;
    
    // Create a new tree
    if (btree.NewTree(firstTree, GrapaBtree::SU64_TREE, 0, GrapaBtree::NODE_WIDTH)) {
        std::cerr << "Failed to create new tree" << std::endl;
        return 1;
    }
    std::cout << "Tree created at " << firstTree << std::endl;
    
    // Test basic insert and search
    GrapaCursor cursor;
    cursor.Set(firstTree, GrapaBtree::SU64_ITEM, 10, 100);
    
    if (btree.Insert(cursor) == 0) {
        std::cout << "✓ Insert successful" << std::endl;
    } else {
        std::cout << "✗ Insert failed" << std::endl;
        return 1;
    }
    
    // Search for the inserted value
    GrapaCursor searchCursor;
    searchCursor.Set(firstTree, GrapaBtree::SU64_ITEM, 10, 0);
    
    if (btree.Search(searchCursor) == 0) {
        std::cout << "✓ Search successful, found value=" << searchCursor.mValue << std::endl;
    } else {
        std::cout << "✗ Search failed" << std::endl;
        return 1;
    }
    
    std::cout << "=== BTree Core Test Passed! ===" << std::endl;
    return 0;
}