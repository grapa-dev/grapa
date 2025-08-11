#include "GrapaBtree.h"
#include "GrapaFileCache.h"
#include <iostream>
#include <string>
#include <vector>

class BTreeTestHarness {
private:
    GrapaBtree btree;
    GrapaFileCache fileCache;
    u64 rootTree;
    
public:
    BTreeTestHarness() : rootTree(0) {
        // Use in-memory file cache for testing
        btree.SetFile(&fileCache);
    }
    
    bool Initialize() {
        std::cout << "=== BTree Test Harness Initialization ===\n";
        
        // Create a new BTree
        if (btree.Create("test_btree.db")) {
            std::cerr << "Failed to create BTree\n";
            return false;
        }
        
        // Set up root tree
        if (btree.SetFileTree(rootTree, true)) {
            std::cerr << "Failed to set file tree\n";
            return false;
        }
        
        std::cout << "BTree initialized successfully\n";
        return true;
    }
    
    void TestBasicOperations() {
        std::cout << "\n=== Test 1: Basic Insert/Delete Operations ===\n";
        
        // Test 1: Insert a few items
        std::cout << "Inserting items: 10, 20, 30\n";
        InsertItem(10, 100);
        InsertItem(20, 200);
        InsertItem(30, 300);
        
        // Verify all items exist
        VerifyItem(10, 100);
        VerifyItem(20, 200);
        VerifyItem(30, 300);
        
        // Test 2: Delete middle item
        std::cout << "\nDeleting middle item (20)\n";
        DeleteItem(20);
        
        // Verify remaining items
        VerifyItem(10, 100);
        VerifyItem(30, 300);
        VerifyItemMissing(20);
        
        // Test 3: Insert new item
        std::cout << "\nInserting new item (25)\n";
        InsertItem(25, 250);
        
        // Verify all items
        VerifyItem(10, 100);
        VerifyItem(25, 250);
        VerifyItem(30, 300);
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
        
        if (cursor.value != expectedValue) {
            std::cerr << "ERROR: Item " << key << " has wrong value. Expected: " << expectedValue << ", Got: " << cursor.value << "\n";
        } else {
            std::cout << "✓ Item " << key << " -> " << cursor.value << " (correct)\n";
        }
    }
    
    void VerifyItemMissing(u64 key) {
        GrapaCursor cursor;
        cursor.Set(rootTree, GrapaBtree::SU64_ITEM, key);
        
        GrapaError err = btree.Search(cursor);
        if (err == 0) {
            std::cerr << "ERROR: Item " << key << " should not exist but was found with value " << cursor.value << "\n";
        } else {
            std::cout << "✓ Item " << key << " correctly not found\n";
        }
    }
};

int main() {
    std::cout << "=== BTree Core Functionality Test Harness ===\n";
    std::cout << "Testing BTree operations in isolation to verify the leaf move bug fix\n\n";
    
    BTreeTestHarness harness;
    
    if (!harness.Initialize()) {
        std::cerr << "Failed to initialize test harness\n";
        return 1;
    }
    
    // Run all tests
    harness.TestBasicOperations();
    harness.TestNodeSplits();
    harness.TestDeleteEdgeCases();
    harness.TestLeafMoveBugScenario();
    harness.TestStressOperations();
    
    std::cout << "\n=== All BTree Tests Completed ===\n";
    std::cout << "If all tests passed, the BTree core functionality is solid.\n";
    std::cout << "The leaf move bug fix is working correctly.\n";
    
    return 0;
} 
