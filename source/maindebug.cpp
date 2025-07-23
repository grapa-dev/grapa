#define DEBUG_BTREE_INVESTIGATION
#include "grapa/GrapaBtree.h"
#include "grapa/GrapaFileIO.h"
#include <iostream>
#include <string>
#include <vector>

class BTreeTestHarness {
private:
    GrapaBtree btree;
    GrapaFileIO fileIO;
    u64 rootTree;
    
public:
    BTreeTestHarness() : rootTree(0) {
        // Initialize the file object first
        // This is the key step that was missing - we need to set the file object
        // before calling any BTree operations
        btree.SetFile(&fileIO);
        
        // Enable debug output to see what's happening
        #ifdef DEBUG_BTREE_INVESTIGATION
        printf("[DEBUG] BTreeTestHarness: File object set to fileIO\n");
        #endif
    }
    
    bool Initialize() {
        std::cout << "=== BTree Test Harness Initialization ===\n";
        
        // Step 1: Create the BTree file (this initializes the file structure)
        // This follows the same pattern as GrapaDB::Create() -> GrapaBtree::Create()
        GrapaError err = btree.Create("test_btree.db");
        if (err) {
            std::cerr << "Failed to create BTree file, error: " << err << "\n";
            return false;
        }
        std::cout << "BTree file created successfully\n";
        
        // Step 2: Create a new tree for our test data
        // This creates the root tree that will hold our test data
        if (btree.NewTree(rootTree, GrapaBtree::SU64_TREE, 0, GrapaBtree::NODE_WIDTH)) {
            std::cerr << "Failed to create new tree\n";
            return false;
        }
        std::cout << "Root tree created at " << rootTree << "\n";
        
        // Step 3: Set up root tree as the file's first tree
        // This tells the BTree which tree is the main root tree
        if (btree.SetFileTree(rootTree, true)) {
            std::cerr << "Failed to set file tree\n";
            return false;
        }
        
        std::cout << "BTree initialized successfully with root tree at " << rootTree << "\n";
        
        // Enable debug output to see what's happening
        #ifdef DEBUG_BTREE_INVESTIGATION
        printf("[DEBUG] BTreeTestHarness: Initialization complete, root tree at %llu\n", rootTree);
        #endif
        
        return true;
    }
    
    bool RunBasicTests() {
        std::cout << "=== Running Basic BTree Tests ===" << std::endl;
        
        // Test 1: Simple insertion and retrieval
        std::cout << "\n--- Test 1: Simple Insert/Retrieve ---" << std::endl;
        
        // Create cursor for key=10, value=1000
        // Pattern: Set(treeRef, valueType, key, value)
        GrapaCursor keyCursor;
        keyCursor.Set(rootTree, GrapaBtree::SU64_ITEM, 10, 1000);
        
        printf("[DEBUG] Inserting key=10, value=1000\n");
        if (btree.Insert(keyCursor) == 0) {
            std::cout << "✓ Insert successful" << std::endl;
        } else {
            std::cout << "✗ Insert failed" << std::endl;
            return false;
        }
        
        // Try to retrieve the value
        GrapaCursor searchCursor;
        searchCursor.Set(rootTree, GrapaBtree::SU64_ITEM, 10, 0);
        
        printf("[DEBUG] Searching for key=10\n");
        if (btree.Search(searchCursor) == 0) {
            std::cout << "✓ Search successful, found value=" << searchCursor.mValue << std::endl;
        } else {
            std::cout << "✗ Search failed" << std::endl;
            return false;
        }
        
        // Test 2: Try a second insertion
        std::cout << "\n--- Test 2: Second Insert/Retrieve ---" << std::endl;
        
        GrapaCursor keyCursor2;
        keyCursor2.Set(rootTree, GrapaBtree::SU64_ITEM, 20, 2000);
        
        printf("[DEBUG] Inserting key=20, value=2000\n");
        if (btree.Insert(keyCursor2) == 0) {
            std::cout << "✓ Second insert successful" << std::endl;
        } else {
            std::cout << "✗ Second insert failed" << std::endl;
            return false;
        }
        
        // Retrieve the second value
        GrapaCursor searchCursor2;
        searchCursor2.Set(rootTree, GrapaBtree::SU64_ITEM, 20, 0);
        
        printf("[DEBUG] Retrieving key=20\n");
        if (btree.Search(searchCursor2) == 0) {
            std::cout << "✓ Second retrieve successful: key=" << searchCursor2.mKey << ", value=" << searchCursor2.mValue << std::endl;
        } else {
            std::cout << "✗ Second retrieve failed" << std::endl;
            return false;
        }
        
        // Test 3: Verify first value is still there
        std::cout << "\n--- Test 3: Verify First Value Still Exists ---" << std::endl;
        GrapaCursor searchCursor3;
        searchCursor3.Set(rootTree, GrapaBtree::SU64_ITEM, 10, 0);
        
        printf("[DEBUG] Re-retrieving key=10\n");
        if (btree.Search(searchCursor3) == 0) {
            std::cout << "✓ First value still exists: key=" << searchCursor3.mKey << ", value=" << searchCursor3.mValue << std::endl;
        } else {
            std::cout << "✗ First value lost!" << std::endl;
            return false;
        }
        
        std::cout << "\n=== All Basic Tests Passed! ===" << std::endl;
        return true;
    }
    
    void TestNodeSplits() {
        std::cout << "\n=== Test 2: Node Split Operations ===\n";
        
        // Clear existing data
        btree.ClearFile();
        btree.SetFileTree(rootTree, true);
        
        // Insert enough items to force node splits (NODE_WIDTH=3, so 4+ items should split)
        std::cout << "Inserting items to force node splits...\n";
        std::vector<std::pair<u64, u64>> testData = {
            {5, 500}, {15, 1500}, {25, 2500}, {35, 3500},  // Should cause first split
            {10, 1000}, {20, 2000}, {30, 3000}, {40, 4000} // Should cause more splits
        };
        
        for (const auto& item : testData) {
            std::cout << "Inserting " << item.first << " -> " << item.second << "\n";
            InsertItem(item.first, item.second);
        }
        
        // Verify all items still exist
        std::cout << "\nVerifying all items after splits...\n";
        for (const auto& item : testData) {
            VerifyItem(item.first, item.second);
        }
    }
    
    void TestDeleteEdgeCases() {
        std::cout << "\n=== Test 3: Delete Edge Cases ===\n";
        
        // Clear existing data
        btree.ClearFile();
        btree.SetFileTree(rootTree, true);
        
        // Insert items to create a multi-node tree
        std::cout << "Creating multi-node tree...\n";
        std::vector<u64> keys = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
        for (u64 key : keys) {
            InsertItem(key, key * 100);
        }
        
        // Test 1: Delete first item
        std::cout << "\nDeleting first item (5)\n";
        DeleteItem(5);
        VerifyItemMissing(5);
        for (size_t i = 1; i < keys.size(); i++) {
            VerifyItem(keys[i], keys[i] * 100);
        }
        
        // Test 2: Delete last item
        std::cout << "\nDeleting last item (50)\n";
        DeleteItem(50);
        VerifyItemMissing(50);
        for (size_t i = 1; i < keys.size() - 1; i++) {
            VerifyItem(keys[i], keys[i] * 100);
        }
        
        // Test 3: Delete middle item
        std::cout << "\nDeleting middle item (25)\n";
        DeleteItem(25);
        VerifyItemMissing(25);
        
        // Test 4: Delete all remaining items
        std::cout << "\nDeleting all remaining items...\n";
        for (size_t i = 1; i < keys.size() - 1; i++) {
            if (keys[i] != 25) { // Skip the one we already deleted
                DeleteItem(keys[i]);
                VerifyItemMissing(keys[i]);
            }
        }
        
        // Test 5: Reinsert items after complete deletion
        std::cout << "\nReinserting items after complete deletion...\n";
        for (u64 key : keys) {
            InsertItem(key, key * 1000); // Different values to distinguish
        }
        
        // Verify all reinserted items
        for (u64 key : keys) {
            VerifyItem(key, key * 1000);
        }
    }
    
    void TestLeafMoveBugScenario() {
        std::cout << "\n=== Test 4: Leaf Move Bug Scenario (Original Bug) ===\n";
        
        // Clear existing data
        btree.ClearFile();
        btree.SetFileTree(rootTree, true);
        
        // This test recreates the scenario that triggered the original bug:
        // 1. Insert items to fill a node
        // 2. Delete an item that causes leaf shifting
        // 3. Verify no data corruption
        
        std::cout << "Inserting items to fill a node (NODE_WIDTH=3)...\n";
        InsertItem(10, 100);
        InsertItem(20, 200);
        InsertItem(30, 300);
        
        std::cout << "Verifying initial state...\n";
        VerifyItem(10, 100);
        VerifyItem(20, 200);
        VerifyItem(30, 300);
        
        // Delete first item - this should trigger the leaf move logic we fixed
        std::cout << "\nDeleting first item (10) - this triggers leaf move logic\n";
        DeleteItem(10);
        
        std::cout << "Verifying after deletion...\n";
        VerifyItemMissing(10);
        VerifyItem(20, 200);
        VerifyItem(30, 300);
        
        // Delete second item - test the leaf move logic again
        std::cout << "\nDeleting second item (20) - testing leaf move again\n";
        DeleteItem(20);
        
        std::cout << "Verifying after second deletion...\n";
        VerifyItemMissing(10);
        VerifyItemMissing(20);
        VerifyItem(30, 300);
        
        // Insert new items to test the fixed logic
        std::cout << "\nInserting new items to test fixed logic...\n";
        InsertItem(15, 150);
        InsertItem(25, 250);
        
        std::cout << "Final verification...\n";
        VerifyItem(15, 150);
        VerifyItem(25, 250);
        VerifyItem(30, 300);
        VerifyItemMissing(10);
        VerifyItemMissing(20);
    }
    
    void TestStressOperations() {
        std::cout << "\n=== Test 5: Stress Operations ===\n";
        
        // Clear existing data
        btree.ClearFile();
        btree.SetFileTree(rootTree, true);
        
        // Insert many items
        std::cout << "Inserting 50 items...\n";
        for (u64 i = 1; i <= 50; i++) {
            InsertItem(i, i * 1000);
        }
        
        // Verify all items
        std::cout << "Verifying all 50 items...\n";
        for (u64 i = 1; i <= 50; i++) {
            VerifyItem(i, i * 1000);
        }
        
        // Delete every other item
        std::cout << "\nDeleting every other item...\n";
        for (u64 i = 2; i <= 50; i += 2) {
            DeleteItem(i);
        }
        
        // Verify remaining items
        std::cout << "Verifying remaining items...\n";
        for (u64 i = 1; i <= 50; i++) {
            if (i % 2 == 1) {
                VerifyItem(i, i * 1000);
            } else {
                VerifyItemMissing(i);
            }
        }
        
        // Insert new items
        std::cout << "\nInserting new items...\n";
        for (u64 i = 51; i <= 75; i++) {
            InsertItem(i, i * 2000);
        }
        
        // Final verification
        std::cout << "Final verification...\n";
        for (u64 i = 1; i <= 75; i++) {
            if (i % 2 == 1 && i <= 50) {
                VerifyItem(i, i * 1000);
            } else if (i >= 51) {
                VerifyItem(i, i * 2000);
            } else {
                VerifyItemMissing(i);
            }
        }
    }
    
private:
    void InsertItem(u64 key, u64 value) {
        GrapaCursor cursor;
        cursor.Set(rootTree, GrapaBtree::SU64_ITEM, key, value);
        
        GrapaError err = btree.Insert(cursor);
        if (err) {
            std::cerr << "Failed to insert item " << key << " -> " << value << " (error: " << err << ")\n";
        }
    }
    
    void DeleteItem(u64 key) {
        GrapaCursor cursor;
        cursor.Set(rootTree, GrapaBtree::SU64_ITEM, key);
        
        GrapaError err = btree.Delete(cursor);
        if (err) {
            std::cerr << "Failed to delete item " << key << " (error: " << err << ")\n";
        }
    }
    
    void VerifyItem(u64 key, u64 expectedValue) {
        GrapaCursor cursor;
        cursor.Set(rootTree, GrapaBtree::SU64_ITEM, key);
        
        GrapaError err = btree.Search(cursor);
        if (err) {
            std::cerr << "ERROR: Item " << key << " not found (error: " << err << ")\n";
            return;
        }
        
        if (cursor.mValue != expectedValue) {
            std::cerr << "ERROR: Item " << key << " has wrong value. Expected: " << expectedValue << ", Got: " << cursor.mValue << "\n";
        } else {
            std::cout << "✓ Item " << key << " -> " << cursor.mValue << " (correct)\n";
        }
    }
    
    void VerifyItemMissing(u64 key) {
        GrapaCursor cursor;
        cursor.Set(rootTree, GrapaBtree::SU64_ITEM, key);
        
        GrapaError err = btree.Search(cursor);
        if (err == 0) {
            std::cerr << "ERROR: Item " << key << " should not exist but was found with value " << cursor.mValue << "\n";
        } else {
            std::cout << "✓ Item " << key << " correctly not found\n";
        }
    }
};

int main() {
    std::cout << "=== BTree Core Functionality Test Harness ===" << std::endl;
    std::cout << "This harness validates the core BTree implementation in isolation." << std::endl;
    
    BTreeTestHarness harness;
    
    // Initialize the BTree system
    if (!harness.Initialize()) {
        std::cerr << "Failed to initialize BTree system. Exiting." << std::endl;
        return 1;
    }
    
    std::cout << "\nBTree system initialized successfully!" << std::endl;
    
    // Run all tests
    bool basicTestsPassed = harness.RunBasicTests();
    if (!basicTestsPassed) {
        std::cerr << "\nBasic tests failed. Stopping further tests." << std::endl;
        return 1;
    }
    
    // Only run additional tests if basic tests pass
    harness.TestNodeSplits();
    harness.TestDeleteEdgeCases();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    return 0;
}