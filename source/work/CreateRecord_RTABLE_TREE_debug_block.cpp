// RTABLE_TREE index insert loop for CreateRecord with defensive delete, single insert, and detailed trace debug output
// Inside the RTABLE_TREE case, in the index insert loop:
tableCursor.Set(indexCursor.mValue, RPTR_ITEM, pCursor.mKey);

// Defensive: delete any stale index entry before inserting
Delete(tableCursor);

// Trace: before insert, print the value being assigned and the expected record reference
if (tableCursor.mValueType == RPTR_ITEM) {
    GrapaCursor expectedRecCursor;
    expectedRecCursor.Set(tableCursor.mTreeRef, RREC_ITEM, tableCursor.mKey);
    GrapaError expectedErr = Search(expectedRecCursor);
    printf("[TRACE][BeforeInsert][RPTR_ITEM] key=%llu value=%llu treeRef=%llu expectedRecValue=%llu expectedRecErr=%lld\n",
        tableCursor.mKey, tableCursor.mValue, tableCursor.mTreeRef, expectedRecCursor.mValue, expectedErr);
}

// Insert the new/updated index entry
GrapaError insertErr = Insert(tableCursor);

// Trace: after insert, print the result and the actual record reference
if (tableCursor.mValueType == RPTR_ITEM) {
    GrapaCursor actualRecCursor = tableCursor;
    GrapaError actualErr = PtrToRec(tableCursor, actualRecCursor);
    printf("[TRACE][AfterInsert][RPTR_ITEM] key=%llu indexValue=%llu actualRecValue=%llu actualRecKey=%llu actualErr=%lld insertErr=%lld\n",
        tableCursor.mKey, tableCursor.mValue, actualRecCursor.mValue, actualRecCursor.mKey, actualErr, insertErr);
}

// Debug output: show the insert and the flat index state
if (tableCursor.mValueType == RPTR_ITEM) {
    printf("[DEBUG][CreateRecord][Insert][RPTR_ITEM] key=%llu value=%llu treeRef=%llu err=%lld\n",
        tableCursor.mKey, tableCursor.mValue, tableCursor.mTreeRef, insertErr);

    // Print flat list of all keys/values in the $KEY index
    GrapaCursor flatCursor;
    flatCursor.Set(tableCursor.mTreeRef);
    GrapaError flatErr = First(flatCursor);
    while (!flatErr) {
        printf("[DEBUG][CreateRecord][IndexFlat] key=%llu value=%llu valueType=%d\n",
            flatCursor.mKey, flatCursor.mValue, flatCursor.mValueType);
        flatErr = Next(flatCursor);
    }
} 