# Grapa SQL Integration Design

## Overview

This document outlines the design for adding SQL syntax to Grapa's language, making SQL a first-class part of the Grapa grammar. This will enable powerful database querying capabilities that integrate seamlessly with GrapaDBX and the unified path system.

## Vision

**Goal**: Make SQL a native part of Grapa's language, allowing developers to write SQL queries directly in Grapa scripts that seamlessly integrate with GrapaDBX databases through the unified path system.

**Example Usage**:
```grapa
/* SQL becomes native Grapa syntax */
f = $file();
f.cd("my_database");

/* Direct SQL queries */
users = SELECT * FROM users WHERE age > 25 ORDER BY name;
admin_count = SELECT COUNT(*) FROM users WHERE role = 'admin';

/* SQL with GrapaDBX enhanced features */
high_salary = SELECT name, salary FROM employees 
              WHERE salary > 50000 
              ORDER BY salary DESC 
              LIMIT 10;

/* SQL with batch operations */
INSERT INTO users (name, age, email) VALUES 
  ('John Doe', 30, 'john@example.com'),
  ('Jane Smith', 25, 'jane@example.com');

/* SQL with transaction support */
BEGIN TRANSACTION;
UPDATE users SET last_login = NOW() WHERE id = 123;
INSERT INTO login_log (user_id, timestamp) VALUES (123, NOW());
COMMIT;
```

## Architecture Overview

### **1. Grammar Extension**
Extend `lib/grapa/$grapa.grc` to include SQL syntax as native Grapa grammar rules.

### **2. SQL Parser Integration**
Integrate SQL parsing with Grapa's existing BNF grammar system using `$RULE`.

### **3. GrapaDBX Integration**
Connect SQL queries to GrapaDBX's enhanced features (batch operations, transactions, etc.).

### **4. Unified Path System Integration**
Leverage the unified path system to make SQL work seamlessly across file system and database contexts.

## **CRITICAL GAPS FOR SQL IMPLEMENTATION**

### **Foundation Gaps (Search Functionality)**

The following gaps in GrapaDBX search functionality are **critical prerequisites** for SQL implementation:

#### **1. ❌ No Multiple Results Collection**
**Current State**: `SearchDb` only positions cursor at first match
**SQL Requirement**: SELECT queries need to return multiple records
**Missing**: `SearchDbAll()`, `CollectMatches()` methods

#### **2. ❌ No Result Set Management**
**Current State**: Single cursor positioning only
**SQL Requirement**: SQL result sets need navigation (Next, Previous, First, Last)
**Missing**: `GrapaDBXResultSet` class with iteration methods

#### **3. ❌ No Search Result Iteration**
**Current State**: No built-in iteration through matches
**SQL Requirement**: SQL cursors need to iterate through result sets
**Missing**: `SearchDbNext()`, `SearchDbPrevious()` methods

#### **4. ❌ Limited Index Selection Strategy**
**Current State**: Uses "first matching index" only
**SQL Requirement**: SQL query optimizer needs intelligent index selection
**Missing**: `SelectBestIndex()`, cost-based optimization

### **SQL-Specific Gaps**

#### **5. ❌ Missing SQL Grammar Rules in BNF**
**Current State**: No SQL syntax rules in `$grapa.grc`
**Missing Grammar Rules**:
```grapa
@global["$sql_statement"]
    = rule <$select_statement> {@<sql_select,{$1}>}
    | <$insert_statement> {@<sql_insert,{$1}>}
    | <$update_statement> {@<sql_update,{$1}>}
    | <$delete_statement> {@<sql_delete,{$1}>}
    | <$create_statement> {@<sql_create,{$1}>}
    | <$drop_statement> {@<sql_drop,{$1}>}
    ;

@global["$select_statement"]
    = rule SELECT <$select_list> FROM <$table_reference> <$where_clause>? <$order_clause>? <$limit_clause>? {@<create_select,{$2,$4,$5,$6,$7}>}
    ;

@global["$where_clause"]
    = rule WHERE <$condition> {@<create_where,{$2}>}
    ;

@global["$condition"]
    = rule <$column_name> <$operator> <$value> {@<create_condition,{$1,$2,$3}>}
    | <$condition> AND <$condition> {@<create_and,{$1,$3}>}
    | <$condition> OR <$condition> {@<create_or,{$1,$3}>}
    | '(' <$condition> ')' {@<create_grouped,{$2}>}
    ;
```

#### **6. ❌ Missing SQL Execution Functions**
**Current State**: No SQL execution functions in language
**Missing Functions**:
```cpp
// Need to add to GrapaLibRule.cpp:
GrapaError GrapaLibraryRuleSqlSelectEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
GrapaError GrapaLibraryRuleSqlInsertEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
GrapaError GrapaLibraryRuleSqlUpdateEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
GrapaError GrapaLibraryRuleSqlDeleteEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
GrapaError GrapaLibraryRuleSqlCreateEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
GrapaError GrapaLibraryRuleSqlDropEvent::Run(GrapaScriptExec* vScriptExec, GrapaRuleEvent* b, GrapaRuleEvent* p);
```

#### **7. ❌ Missing SQL Query Parser**
**Current State**: No SQL query parsing infrastructure
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlParser {
    GrapaError ParseSelect(const GrapaCHAR& sql, GrapaSqlSelect& select);
    GrapaError ParseWhere(const GrapaCHAR& condition, GrapaSqlCondition& condition);
    GrapaError ParseJoin(const GrapaCHAR& joinClause, GrapaSqlJoin& join);
    GrapaError ParseOrderBy(const GrapaCHAR& orderClause, GrapaSqlOrderBy& orderBy);
    GrapaError ParseGroupBy(const GrapaCHAR& groupClause, GrapaSqlGroupBy& groupBy);
};
```

#### **8. ❌ Missing SQL Query Optimizer**
**Current State**: No query optimization for complex SQL operations
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlOptimizer {
    GrapaError OptimizeQuery(GrapaSqlQuery& query);
    GrapaError SelectBestIndex(const GrapaSqlQuery& query, u64& bestIndexRef);
    GrapaError PlanJoinOrder(const GrapaSqlQuery& query, std::vector<GrapaSqlJoin>& joins);
    GrapaError EstimateQueryCost(const GrapaSqlQuery& query, double& cost);
};
```

#### **9. ❌ Missing SQL Result Set Management**
**Current State**: No comprehensive result set management for SQL queries
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlResultSet {
    std::vector<GrapaCursor> mResults;
    size_t mCurrentIndex;
    GrapaError Next();
    GrapaError Previous();
    GrapaError First();
    GrapaError Last();
    size_t Count();
    GrapaError GetColumn(const GrapaCHAR& columnName, GrapaBYTE& value);
    GrapaError GetRow(GrapaDBXFieldValueArray& row);
};
```

#### **10. ❌ Missing SQL Aggregation Functions**
**Current State**: No SQL aggregation support
**Missing Functions**:
```cpp
// Need to implement:
GrapaError GrapaSqlAggregator::Count(const GrapaSqlQuery& query, u64& result);
GrapaError GrapaSqlAggregator::Sum(const GrapaSqlQuery& query, const GrapaCHAR& column, GrapaBYTE& result);
GrapaError GrapaSqlAggregator::Avg(const GrapaSqlQuery& query, const GrapaCHAR& column, GrapaBYTE& result);
GrapaError GrapaSqlAggregator::Min(const GrapaSqlQuery& query, const GrapaCHAR& column, GrapaBYTE& result);
GrapaError GrapaSqlAggregator::Max(const GrapaSqlQuery& query, const GrapaCHAR& column, GrapaBYTE& result);
```

#### **11. ❌ Missing SQL JOIN Support**
**Current State**: No multi-table join operations
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlJoiner {
    GrapaError InnerJoin(const GrapaSqlQuery& query, GrapaSqlResultSet& result);
    GrapaError LeftJoin(const GrapaSqlQuery& query, GrapaSqlResultSet& result);
    GrapaError RightJoin(const GrapaSqlQuery& query, GrapaSqlResultSet& result);
    GrapaError FullJoin(const GrapaSqlQuery& query, GrapaSqlResultSet& result);
    GrapaError CrossJoin(const GrapaSqlQuery& query, GrapaSqlResultSet& result);
};
```

#### **12. ❌ Missing SQL Transaction Support**
**Current State**: No SQL transaction management
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlTransaction {
    GrapaError Begin();
    GrapaError Commit();
    GrapaError Rollback();
    GrapaError Savepoint(const GrapaCHAR& name);
    GrapaError RollbackToSavepoint(const GrapaCHAR& name);
};
```

#### **13. ❌ Missing SQL Schema Management**
**Current State**: No SQL DDL (Data Definition Language) support
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlSchema {
    GrapaError CreateTable(const GrapaCHAR& tableName, const GrapaDBXFieldArray& schema);
    GrapaError AlterTable(const GrapaCHAR& tableName, const GrapaCHAR& operation, const GrapaDBXField& field);
    GrapaError DropTable(const GrapaCHAR& tableName);
    GrapaError CreateIndex(const GrapaCHAR& tableName, const GrapaCHAR& indexName, const GrapaDU64Array& columns);
    GrapaError DropIndex(const GrapaCHAR& tableName, const GrapaCHAR& indexName);
};
```

#### **14. ❌ Missing SQL Prepared Statements**
**Current State**: No prepared statement support for performance
**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlPreparedStatement {
    GrapaError Prepare(const GrapaCHAR& sql);
    GrapaError BindParameter(u32 index, const GrapaBYTE& value);
    GrapaError Execute(GrapaSqlResultSet& result);
    GrapaError Close();
};
```

### **🆕 CRITICAL: Metadata and Functionality Gaps for Dictionary Fields and Indexes**

#### **15. ❌ Missing Field Metadata for Constraints**
**Current State**: `GrapaDBXField` struct has no constraint metadata
**Current Field Structure**:
```cpp
struct GrapaDBXField {
    u8 mType;
    u8 mStore;
    u8 mTreeType;
    u8 mReserved[5];
    u64 mId;
    u64 mRef;
    u64 mNameId;
    u64 mNameRef;
    u64 mDictOffset;
    u64 mDictSize;
    u64 mSize;
    u64 mGrow;
    u64 mTableRef;
    u64 mFormulaRef;
    u8 mFormulaType;
    u8 mReserved2[7];
    // ❌ MISSING: Constraint metadata
};
```

**Missing Field Metadata**:
```cpp
// Need to add to GrapaDBXField:
u8 mConstraints;           // Bit flags for constraints (UNIQUE, NOT_NULL, etc.)
u8 mDefaultValueType;      // Type of default value
u64 mDefaultValueRef;      // Reference to default value data
u64 mCheckConstraintRef;   // Reference to check constraint expression
u64 mForeignKeyRef;        // Reference to foreign key definition
u8 mIndexed;              // Whether field is indexed
u8 mAutoIncrement;        // Auto-increment flag
u64 mSequenceRef;         // Reference to sequence for auto-increment
```

#### **16. ❌ Missing Unique Constraint Support**
**Current State**: No unique constraint implementation
**SQL Requirement**: `CREATE TABLE users (id INT UNIQUE, email STR UNIQUE)`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXUniqueConstraint {
    GrapaError CreateUniqueConstraint(GrapaDBXTable& table, u64 fieldId);
    GrapaError ValidateUniqueConstraint(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& value);
    GrapaError DropUniqueConstraint(GrapaDBXTable& table, u64 fieldId);
    GrapaError CheckUniqueViolation(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& value);
};
```

#### **17. ❌ Missing NOT NULL Constraint Support**
**Current State**: No NOT NULL constraint implementation
**SQL Requirement**: `CREATE TABLE users (id INT NOT NULL, name STR NOT NULL)`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXNotNullConstraint {
    GrapaError CreateNotNullConstraint(GrapaDBXTable& table, u64 fieldId);
    GrapaError ValidateNotNullConstraint(const GrapaBYTE& value);
    GrapaError DropNotNullConstraint(GrapaDBXTable& table, u64 fieldId);
};
```

#### **18. ❌ Missing Check Constraint Support**
**Current State**: No check constraint implementation
**SQL Requirement**: `CREATE TABLE users (age INT CHECK (age >= 0))`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXCheckConstraint {
    GrapaError CreateCheckConstraint(GrapaDBXTable& table, u64 fieldId, const GrapaCHAR& expression);
    GrapaError ValidateCheckConstraint(const GrapaCHAR& expression, const GrapaBYTE& value);
    GrapaError DropCheckConstraint(GrapaDBXTable& table, u64 fieldId);
    GrapaError EvaluateCheckExpression(const GrapaCHAR& expression, const GrapaDBXFieldValueArray& values);
};
```

#### **19. ❌ Missing Default Value Support**
**Current State**: No default value implementation
**SQL Requirement**: `CREATE TABLE users (created_at TIME DEFAULT NOW())`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXDefaultValue {
    GrapaError SetDefaultValue(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& defaultValue);
    GrapaError GetDefaultValue(GrapaDBXTable& table, u64 fieldId, GrapaBYTE& defaultValue);
    GrapaError ClearDefaultValue(GrapaDBXTable& table, u64 fieldId);
    GrapaError EvaluateDefaultExpression(const GrapaCHAR& expression, GrapaBYTE& result);
};
```

#### **20. ❌ Missing Foreign Key Support**
**Current State**: No foreign key implementation
**SQL Requirement**: `CREATE TABLE orders (user_id INT REFERENCES users(id))`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXForeignKey {
    GrapaError CreateForeignKey(GrapaDBXTable& table, u64 fieldId, const GrapaCHAR& refTable, u64 refFieldId);
    GrapaError ValidateForeignKey(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& value);
    GrapaError DropForeignKey(GrapaDBXTable& table, u64 fieldId);
    GrapaError CascadeDelete(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& value);
    GrapaError CascadeUpdate(GrapaDBXTable& table, u64 fieldId, const GrapaBYTE& oldValue, const GrapaBYTE& newValue);
};
```

#### **21. ❌ Missing Auto-Increment Support**
**Current State**: No auto-increment implementation
**SQL Requirement**: `CREATE TABLE users (id INT AUTO_INCREMENT)`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXAutoIncrement {
    GrapaError CreateAutoIncrement(GrapaDBXTable& table, u64 fieldId, u64 startValue = 1, u64 increment = 1);
    GrapaError GetNextAutoIncrementValue(GrapaDBXTable& table, u64 fieldId, u64& nextValue);
    GrapaError ResetAutoIncrement(GrapaDBXTable& table, u64 fieldId, u64 newValue);
    GrapaError DropAutoIncrement(GrapaDBXTable& table, u64 fieldId);
};
```

#### **22. ❌ Missing Enhanced Index Metadata**
**Current State**: `GrapaDBXIndex` has minimal metadata
**Current Index Structure**:
```cpp
class GrapaDBXIndex {
    GrapaDBXTable mTable;
    u64 mId;
    u64 mRef;
    // ❌ MISSING: Enhanced index metadata
};
```

**Missing Index Metadata**:
```cpp
// Need to add to GrapaDBXIndex:
u8 mIndexType;            // UNIQUE, PRIMARY, SECONDARY, etc.
u8 mIndexMethod;          // BTREE, HASH, etc.
u8 mSortOrder;            // ASC, DESC
u64 mCardinality;         // Number of unique values
u64 mSelectivity;         // Index selectivity (0-100)
u64 mLastUpdated;         // Timestamp of last update
u64 mStatisticsRef;       // Reference to index statistics
u8 mIsActive;             // Whether index is active
u8 mIsUnique;             // Whether index enforces uniqueness
u64 mConstraintRef;       // Reference to associated constraint
```

#### **23. ❌ Missing Index Statistics and Optimization**
**Current State**: No index statistics for query optimization
**SQL Requirement**: Query optimizer needs index statistics
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXIndexStatistics {
    GrapaError UpdateIndexStatistics(GrapaDBXIndex& index);
    GrapaError GetIndexCardinality(GrapaDBXIndex& index, u64& cardinality);
    GrapaError GetIndexSelectivity(GrapaDBXIndex& index, double& selectivity);
    GrapaError AnalyzeIndex(GrapaDBXIndex& index);
    GrapaError EstimateIndexCost(GrapaDBXIndex& index, const GrapaSqlQuery& query, double& cost);
};
```

#### **24. ❌ Missing Composite Index Support**
**Current State**: Limited to single-field indexes
**SQL Requirement**: `CREATE INDEX idx_name_email ON users(name, email)`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXCompositeIndex {
    GrapaError CreateCompositeIndex(GrapaDBXTable& table, const GrapaCHAR& indexName, const GrapaDU64Array& fieldIds);
    GrapaError ValidateCompositeKey(GrapaDBXTable& table, const GrapaDBXIndex& index, const GrapaDBXFieldValueArray& values);
    GrapaError GetCompositeIndexFields(const GrapaDBXIndex& index, GrapaDU64Array& fieldIds);
    GrapaError OptimizeCompositeIndex(GrapaDBXIndex& index, const GrapaSqlQuery& query);
};
```

#### **25. ❌ Missing Partial Index Support**
**Current State**: No partial index implementation
**SQL Requirement**: `CREATE INDEX idx_active_users ON users(email) WHERE active = 1`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXPartialIndex {
    GrapaError CreatePartialIndex(GrapaDBXTable& table, const GrapaCHAR& indexName, const GrapaDU64Array& fieldIds, const GrapaCHAR& whereClause);
    GrapaError EvaluatePartialIndexCondition(const GrapaCHAR& whereClause, const GrapaDBXFieldValueArray& values);
    GrapaError ValidatePartialIndexEntry(GrapaDBXIndex& index, const GrapaDBXFieldValueArray& values);
};
```

#### **26. ❌ Missing Index Maintenance and Rebuilding**
**Current State**: No index maintenance utilities
**SQL Requirement**: `REINDEX users`, `ANALYZE users`
**Missing Components**:
```cpp
// Need to implement:
class GrapaDBXIndexMaintenance {
    GrapaError RebuildIndex(GrapaDBXIndex& index);
    GrapaError AnalyzeTable(GrapaDBXTable& table);
    GrapaError UpdateIndexStatistics(GrapaDBXIndex& index);
    GrapaError CompactIndex(GrapaDBXIndex& index);
    GrapaError ValidateIndexIntegrity(GrapaDBXIndex& index);
};
```

### **🆕 CRITICAL: SQL DDL (Data Definition Language) Gaps**

#### **27. ❌ Missing CREATE TABLE with Constraints**
**Current State**: No SQL CREATE TABLE with constraint syntax
**SQL Requirement**: 
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    email STR UNIQUE NOT NULL,
    name STR NOT NULL,
    age INT CHECK (age >= 0),
    created_at TIME DEFAULT NOW()
);
```

**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlDDL {
    GrapaError CreateTableWithConstraints(const GrapaCHAR& tableName, const GrapaSqlTableDefinition& definition);
    GrapaError ParseTableDefinition(const GrapaCHAR& ddl, GrapaSqlTableDefinition& definition);
    GrapaError ValidateTableConstraints(GrapaDBXTable& table, const GrapaDBXFieldValueArray& values);
    GrapaError ApplyTableConstraints(GrapaDBXTable& table);
};
```

#### **28. ❌ Missing ALTER TABLE Support**
**Current State**: No ALTER TABLE implementation
**SQL Requirement**: 
```sql
ALTER TABLE users ADD COLUMN phone STR;
ALTER TABLE users ADD CONSTRAINT uk_email UNIQUE(email);
ALTER TABLE users DROP CONSTRAINT uk_email;
```

**Missing Components**:
```cpp
// Need to implement:
class GrapaSqlAlterTable {
    GrapaError AddColumn(const GrapaCHAR& tableName, const GrapaSqlColumnDefinition& column);
    GrapaError DropColumn(const GrapaCHAR& tableName, const GrapaCHAR& columnName);
    GrapaError AddConstraint(const GrapaCHAR& tableName, const GrapaSqlConstraint& constraint);
    GrapaError DropConstraint(const GrapaCHAR& tableName, const GrapaCHAR& constraintName);
    GrapaError ModifyColumn(const GrapaCHAR& tableName, const GrapaCHAR& columnName, const GrapaSqlColumnDefinition& newDefinition);
};
```

### **Summary of Metadata and Functionality Gaps**

The analysis reveals **14 additional critical gaps** in metadata and functionality beyond the original 14 SQL gaps:

#### **Field Metadata Gaps (6 gaps)**
15. **Missing Field Metadata for Constraints**: No constraint metadata in `GrapaDBXField`
16. **Missing Unique Constraint Support**: No unique constraint implementation
17. **Missing NOT NULL Constraint Support**: No NOT NULL constraint implementation
18. **Missing Check Constraint Support**: No check constraint implementation
19. **Missing Default Value Support**: No default value implementation
20. **Missing Foreign Key Support**: No foreign key implementation
21. **Missing Auto-Increment Support**: No auto-increment implementation

#### **Index Metadata Gaps (5 gaps)**
22. **Missing Enhanced Index Metadata**: Minimal metadata in `GrapaDBXIndex`
23. **Missing Index Statistics and Optimization**: No index statistics for query optimization
24. **Missing Composite Index Support**: Limited to single-field indexes
25. **Missing Partial Index Support**: No partial index implementation
26. **Missing Index Maintenance and Rebuilding**: No index maintenance utilities

#### **SQL DDL Gaps (2 gaps)**
27. **Missing CREATE TABLE with Constraints**: No SQL CREATE TABLE with constraint syntax
28. **Missing ALTER TABLE Support**: No ALTER TABLE implementation

**Total Gaps for Full SQL Implementation: 28 gaps** (14 original + 14 metadata/functionality gaps)

This comprehensive analysis shows that implementing full SQL support requires addressing not just the search functionality and SQL syntax gaps, but also significant metadata and constraint management capabilities that are currently missing from the GrapaDBX implementation.

## **Implementation Priority**

### **Phase 1: Foundation (Critical Prerequisites)** 🔥 **HIGHEST PRIORITY**
1. **Fix Search Functionality Gaps**
   - [ ] Implement `SearchDbAll()` for multiple results collection
   - [ ] Implement `SearchDbNext()` for result iteration
   - [ ] Create `GrapaDBXResultSet` class for result management
   - [ ] Add enhanced index selection (`SelectBestIndex()`)

### **Phase 2: SQL Grammar and Parser** 🔥 **HIGH PRIORITY**
2. **Add SQL Grammar Rules**
   - [ ] Add SQL statement rules to `$grapa.grc`
   - [ ] Implement SQL execution functions in `GrapaLibRule.cpp`
   - [ ] Create SQL parser infrastructure

### **Phase 3: SQL Core Functionality** ⏳ **MEDIUM PRIORITY**
3. **Implement Core SQL Features**
   - [ ] SQL query optimizer
   - [ ] SQL result set management
   - [ ] Basic aggregation functions (COUNT, SUM, AVG, MIN, MAX)

### **Phase 4: Advanced SQL Features** ⏳ **LOW PRIORITY**
4. **Advanced SQL Capabilities**
   - [ ] JOIN operations (INNER, LEFT, RIGHT, FULL, CROSS)
   - [ ] Transaction support (BEGIN, COMMIT, ROLLBACK)
   - [ ] DDL operations (CREATE, ALTER, DROP)
   - [ ] Prepared statements for performance

## **Integration Points**

### **SQL → GrapaDBX Integration**
```cpp
// SQL SELECT would use enhanced search functionality:
GrapaError ExecuteSqlSelect(const GrapaCHAR& sql, GrapaSqlResultSet& results)
{
    // Parse SQL query
    GrapaSqlQuery query = ParseSqlQuery(sql);
    
    // Use enhanced search functionality
    return SearchDbAll(query.cursor, query.table, query.fieldList, results);
}
```

### **SQL → Unified Path System Integration**
```grapa
// SQL works seamlessly with unified path system:
f = $file();
f.cd("database://my_db");

// SQL queries work on current context
users = SELECT * FROM users WHERE active = 1;
```

## **Testing Strategy**

### **Foundation Testing**
```cpp
// Test search functionality prerequisites
GrapaError TestSearchFoundation() {
    // Test multiple results collection
    // Test result set iteration
    // Test index optimization
}
```

### **SQL Grammar Testing**
```grapa
// Test SQL grammar rules
test_sql_grammar = op() {
    // Test SELECT statement parsing
    // Test WHERE clause parsing
    // Test JOIN clause parsing
    // Test ORDER BY parsing
};
```

### **SQL Execution Testing**
```cpp
// Test SQL execution
GrapaError TestSqlExecution() {
    // Test SELECT query execution
    // Test INSERT query execution
    // Test UPDATE query execution
    // Test DELETE query execution
}
```

## **Performance Considerations**

### **Expected Performance**
- **Simple SELECT**: ~1-5ms (with index optimization)
- **Complex JOIN**: ~10-50ms (depending on table sizes)
- **Aggregation**: ~5-20ms (depending on data size)
- **Transaction**: ~1-10ms (depending on operation count)

### **Optimization Opportunities**
- **Index-based queries**: Leverage GrapaDBX enhanced indexing
- **Batch operations**: Use GrapaDBX batch capabilities
- **Caching**: Implement query result caching
- **Parallel processing**: Use Grapa's built-in parallelism

## **Conclusion**

The SQL implementation requires addressing **14 major gaps** across two categories:

### **Critical Foundation Gaps (4 gaps)**
These must be resolved first as they are prerequisites for SQL functionality:
1. Multiple results collection
2. Result set management  
3. Search result iteration
4. Enhanced index selection

### **SQL-Specific Gaps (10 gaps)**
These are specific to SQL implementation:
5. SQL grammar rules
6. SQL execution functions
7. SQL query parser
8. SQL query optimizer
9. SQL result set management
10. SQL aggregation functions
11. SQL JOIN support
12. SQL transaction support
13. SQL schema management
14. SQL prepared statements

**Implementation Strategy**: Address foundation gaps first, then implement SQL grammar and parser, followed by core SQL functionality, and finally advanced SQL features.

**Timeline**: Foundation gaps (Phase 1) are critical blockers and should be prioritized before any SQL implementation can begin. 

## Grammar Design

### **1. Extending the Command Grammar**

Add SQL statements to the existing `$command` rule in `lib/grapa/$grapa.grc`:

```grapa
@global["$command"]
    = rule '{' <$command_list> ';' '}' {@<scope,{$2}>}
    | $SYSID("INCLUDE") <$comp> {@<include,{$2}>}
    | include <$comp> {@<include,{$2}>}
    | $SYSID("RESET") {@<reset,{}>}
    | reset {@<reset,{}>}
    | if '(' <$comp> ')' <$command> ';' <$else> {@<if,{$3,$5,$7}>}
    | if '(' <$comp> ')' <$command> <$else> {@<if,{$3,$5,$6}>}
    | if '(' <$comp> ')' <$command> {@<if,{$3,$5}>}
    | while '(' <$comp> ')' <$command> {@<while,{$3,$5}>}
    | switch '(' <$comp> ')' '{' <$case_list> '}' {@<switch,{$3,$6}>}
    | break {@<break,{}>}
    | return '(' <$comp> ')' {@<return,{$3}>}
    | exit {@<exit,{}>}
    | <sql_statement>  /* NEW: SQL statements */
    | <custom_command>
    | <$litname> '=' <$comp> {@<assign,{$1,$3}>}
    | <$comp> '=' <$comp> {@<assign,{$1,$3}>}
    | <$comp>
    ;
```

### **2. SQL Statement Grammar**

Define comprehensive SQL grammar rules:

```grapa
/* SQL Statement Types */
@global["$sql_statement"]
    = rule <$sql_select> {@<sql_select_exec,{$1}>}
    | <$sql_insert> {@<sql_insert_exec,{$1}>}
    | <$sql_update> {@<sql_update_exec,{$1}>}
    | <$sql_delete> {@<sql_delete_exec,{$1}>}
    | <$sql_create> {@<sql_create_exec,{$1}>}
    | <$sql_alter> {@<sql_alter_exec,{$1}>}
    | <$sql_drop> {@<sql_drop_exec,{$1}>}
    | <$sql_transaction> {@<sql_transaction_exec,{$1}>}
    ;

/* SELECT Statement */
@global["$sql_select"]
    = rule SELECT <$sql_select_list> FROM <$sql_table_reference> 
         <$sql_where_clause> <$sql_order_clause> <$sql_limit_clause> 
         {@<sql_select_ast,{$2,$4,$5,$6,$7}>}
    | SELECT <$sql_select_list> FROM <$sql_table_reference> 
         <$sql_where_clause> <$sql_order_clause> 
         {@<sql_select_ast,{$2,$4,$5,$6,null}>}
    | SELECT <$sql_select_list> FROM <$sql_table_reference> 
         <$sql_where_clause> 
         {@<sql_select_ast,{$2,$4,$5,null,null}>}
    | SELECT <$sql_select_list> FROM <$sql_table_reference> 
         {@<sql_select_ast,{$2,$4,null,null,null}>}
    ;

/* SELECT List */
@global["$sql_select_list"]
    = rule '*' {@<sql_select_all,{}>}
    | <$sql_column_list> {@<sql_select_columns,{$1}>}
    ;

/* Column List */
@global["$sql_column_list"]
    = rule <$sql_column> ',' <$sql_column_list> {@<sql_column_list_append,{$1,$3}>}
    | <$sql_column> {@<sql_column_list_single,{$1}>}
    ;

/* Column */
@global["$sql_column"]
    = rule <$sql_column_name> AS <$sql_column_alias> {@<sql_column_aliased,{$1,$3}>}
    | <$sql_column_name> {@<sql_column_simple,{$1}>}
    | <$sql_function_call> AS <$sql_column_alias> {@<sql_function_aliased,{$1,$3}>}
    | <$sql_function_call> {@<sql_function_simple,{$1}>}
    ;

/* Table Reference */
@global["$sql_table_reference"]
    = rule <$sql_table_name> AS <$sql_table_alias> {@<sql_table_aliased,{$1,$3}>}
    | <$sql_table_name> {@<sql_table_simple,{$1}>}
    ;

/* WHERE Clause */
@global["$sql_where_clause"]
    = rule WHERE <$sql_condition> {@<sql_where_condition,{$2}>}
    | {@<sql_where_none,{}>}
    ;

/* Conditions */
@global["$sql_condition"]
    = rule <$sql_condition> AND <$sql_condition> {@<sql_and_condition,{$1,$3}>}
    | <$sql_condition> OR <$sql_condition> {@<sql_or_condition,{$1,$3}>}
    | '(' <$sql_condition> ')' {@<sql_parenthesized_condition,{$2}>}
    | <$sql_comparison> {@<sql_simple_condition,{$1}>}
    ;

/* Comparisons */
@global["$sql_comparison"]
    = rule <$sql_column_name> '=' <$sql_value> {@<sql_eq_comparison,{$1,$3}>}
    | <$sql_column_name> '!=' <$sql_value> {@<sql_neq_comparison,{$1,$3}>}
    | <$sql_column_name> '>' <$sql_value> {@<sql_gt_comparison,{$1,$3}>}
    | <$sql_column_name> '<' <$sql_value> {@<sql_lt_comparison,{$1,$3}>}
    | <$sql_column_name> '>=' <$sql_value> {@<sql_gte_comparison,{$1,$3}>}
    | <$sql_column_name> '<=' <$sql_value> {@<sql_lte_comparison,{$1,$3}>}
    | <$sql_column_name> LIKE <$sql_value> {@<sql_like_comparison,{$1,$3}>}
    | <$sql_column_name> IN '(' <$sql_value_list> ')' {@<sql_in_comparison,{$1,$4}>}
    | <$sql_column_name> IS NULL {@<sql_is_null_comparison,{$1}>}
    | <$sql_column_name> IS NOT NULL {@<sql_is_not_null_comparison,{$1}>}
    ;

/* ORDER BY Clause */
@global["$sql_order_clause"]
    = rule ORDER BY <$sql_order_list> {@<sql_order_by,{$3}>}
    | {@<sql_order_none,{}>}
    ;

/* ORDER List */
@global["$sql_order_list"]
    = rule <$sql_order_item> ',' <$sql_order_list> {@<sql_order_list_append,{$1,$3}>}
    | <$sql_order_item> {@<sql_order_list_single,{$1}>}
    ;

/* ORDER Item */
@global["$sql_order_item"]
    = rule <$sql_column_name> ASC {@<sql_order_asc,{$1}>}
    | <$sql_column_name> DESC {@<sql_order_desc,{$1}>}
    | <$sql_column_name> {@<sql_order_default,{$1}>}
    ;

/* LIMIT Clause */
@global["$sql_limit_clause"]
    = rule LIMIT <$sql_number> {@<sql_limit,{$2}>}
    | {@<sql_limit_none,{}>}
    ;

/* INSERT Statement */
@global["$sql_insert"]
    = rule INSERT INTO <$sql_table_name> '(' <$sql_column_list> ')' 
         VALUES '(' <$sql_value_list> ')' {@<sql_insert_values,{$3,$5,$8}>}
    | INSERT INTO <$sql_table_name> VALUES '(' <$sql_value_list> ')' 
         {@<sql_insert_values_implicit,{$3,$6}>}
    ;

/* UPDATE Statement */
@global["$sql_update"]
    = rule UPDATE <$sql_table_name> SET <$sql_set_list> 
         <$sql_where_clause> {@<sql_update,{$2,$4,$5}>}
    ;

/* DELETE Statement */
@global["$sql_delete"]
    = rule DELETE FROM <$sql_table_name> <$sql_where_clause> {@<sql_delete,{$3,$4}>}
    ;

/* Transaction Statements */
@global["$sql_transaction"]
    = rule BEGIN TRANSACTION {@<sql_begin_transaction,{}>}
    | COMMIT {@<sql_commit,{}>}
    | ROLLBACK {@<sql_rollback,{}>}
    ;

/* Basic Elements */
@global["$sql_column_name"]
    = rule $ID {@<sql_column_name_id,{$1}>}
    ;

@global["$sql_table_name"]
    = rule $ID {@<sql_table_name_id,{$1}>}
    ;

@global["$sql_column_alias"]
    = rule $ID {@<sql_column_alias_id,{$1}>}
    ;

@global["$sql_table_alias"]
    = rule $ID {@<sql_table_alias_id,{$1}>}
    ;

@global["$sql_value"]
    = rule $STR {@<sql_value_string,{$1}>}
    | $INT {@<sql_value_int,{$1}>}
    | $FLOAT {@<sql_value_float,{$1}>}
    | $ID {@<sql_value_identifier,{$1}>}
    ;

@global["$sql_number"]
    = rule $INT {@<sql_number_int,{$1}>}
    | $FLOAT {@<sql_number_float,{$1}>}
    ;

@global["$sql_value_list"]
    = rule <$sql_value> ',' <$sql_value_list> {@<sql_value_list_append,{$1,$3}>}
    | <$sql_value> {@<sql_value_list_single,{$1}>}
    ;

@global["$sql_set_list"]
    = rule <$sql_set_item> ',' <$sql_set_list> {@<sql_set_list_append,{$1,$3}>}
    | <$sql_set_item> {@<sql_set_list_single,{$1}>}
    ;

@global["$sql_set_item"]
    = rule <$sql_column_name> '=' <$sql_value> {@<sql_set_item_pair,{$1,$3}>}
    ;

@global["$sql_function_call"]
    = rule COUNT '(' '*' ')' {@<sql_function_count_all,{}>}
    | COUNT '(' <$sql_column_name> ')' {@<sql_function_count,{$3}>}
    | SUM '(' <$sql_column_name> ')' {@<sql_function_sum,{$3}>}
    | AVG '(' <$sql_column_name> ')' {@<sql_function_avg,{$3}>}
    | MAX '(' <$sql_column_name> ')' {@<sql_function_max,{$3}>}
    | MIN '(' <$sql_column_name> ')' {@<sql_function_min,{$3}>}
    | NOW '(' ')' {@<sql_function_now,{}>}
    ;
```

## SQL Execution Engine

### **1. SQL AST (Abstract Syntax Tree)**

Create SQL-specific AST nodes that integrate with Grapa's execution system:

```grapa
/* SQL AST Node Types */
@global["$sql_ast_select"]
    = class {
        select_list = @<"select_list",{@<this>}>;
        table_reference = @<"table_reference",{@<this>}>;
        where_clause = @<"where_clause",{@<this>}>;
        order_clause = @<"order_clause",{@<this>}>;
        limit_clause = @<"limit_clause",{@<this>}>;
        execute = @<"sql_select_execute",{@<this>}>;
    };

@global["$sql_ast_insert"]
    = class {
        table_name = @<"table_name",{@<this>}>;
        columns = @<"columns",{@<this>}>;
        values = @<"values",{@<this>}>;
        execute = @<"sql_insert_execute",{@<this>}>;
    };

@global["$sql_ast_update"]
    = class {
        table_name = @<"table_name",{@<this>}>;
        set_list = @<"set_list",{@<this>}>;
        where_clause = @<"where_clause",{@<this>}>;
        execute = @<"sql_update_execute",{@<this>}>;
    };

@global["$sql_ast_delete"]
    = class {
        table_name = @<"table_name",{@<this>}>;
        where_clause = @<"where_clause",{@<this>}>;
        execute = @<"sql_delete_execute",{@<this>}>;
    };
```

### **2. SQL Execution Functions**

Implement SQL execution functions that integrate with GrapaDBX:

```grapa
/* SQL Execution Functions */
@global["sql_select_execute"]
    = op(ast) {
        /* Get current database context from $file() */
        f = $file();
        
        /* Parse table reference to get database path */
        table_path = ast.table_reference;
        
        /* Navigate to table */
        f.cd(table_path);
        
        /* Build query using GrapaDBX features */
        query = {
            "type": "select",
            "columns": ast.select_list,
            "where": ast.where_clause,
            "order": ast.order_clause,
            "limit": ast.limit_clause
        };
        
        /* Execute using GrapaDBX's enhanced query engine */
        result = f.query(query);
        
        result;
    };

@global["sql_insert_execute"]
    = op(ast) {
        f = $file();
        f.cd(ast.table_name);
        
        /* Use GrapaDBX batch operations for efficiency */
        if (ast.columns) {
            /* Explicit columns specified */
            for (i = 0; i < ast.values.len(); i += 1;) {
                value_set = ast.values[i];
                f.setBatch(value_set.key, value_set.values);
            }
        } else {
            /* Implicit columns - use all fields */
            for (i = 0; i < ast.values.len(); i += 1;) {
                value_set = ast.values[i];
                f.setBatchMultiple(value_set);
            }
        }
        
        /* Return affected row count */
        ast.values.len();
    };

@global["sql_update_execute"]
    = op(ast) {
        f = $file();
        f.cd(ast.table_name);
        
        /* Build update query */
        update_data = {};
        for (i = 0; i < ast.set_list.len(); i += 1;) {
            set_item = ast.set_list[i];
            update_data[set_item.column] = set_item.value;
        }
        
        /* Use GrapaDBX's enhanced update capabilities */
        if (ast.where_clause) {
            /* Update with WHERE clause */
            affected = f.updateBatch(ast.where_clause, update_data);
        } else {
            /* Update all records */
            affected = f.updateBatchAll(update_data);
        }
        
        affected;
    };

@global["sql_delete_execute"]
    = op(ast) {
        f = $file();
        f.cd(ast.table_name);
        
        /* Use GrapaDBX's enhanced delete capabilities */
        if (ast.where_clause) {
            /* Delete with WHERE clause */
            affected = f.deleteBatch(ast.where_clause);
        } else {
            /* Delete all records */
            affected = f.deleteBatchAll();
        }
        
        affected;
    };
```

## GrapaDBX Integration

### **1. Enhanced Query Engine**

Extend GrapaDBX with SQL-aware query capabilities:

```cpp
// GrapaDBX SQL Query Engine
class GrapaDBXSQLQueryEngine {
public:
    // Execute SQL queries
    virtual GrapaError ExecuteSelect(const GrapaDBXSelectQuery& query, GrapaDBXResultSet& results);
    virtual GrapaError ExecuteInsert(const GrapaDBXInsertQuery& query, u64& affectedRows);
    virtual GrapaError ExecuteUpdate(const GrapaDBXUpdateQuery& query, u64& affectedRows);
    virtual GrapaError ExecuteDelete(const GrapaDBXDeleteQuery& query, u64& affectedRows);
    
    // Query optimization
    virtual GrapaError OptimizeQuery(GrapaDBXQuery& query);
    virtual GrapaError SelectBestIndex(const GrapaDBXQuery& query, GrapaDBXIndex*& bestIndex);
    
    // Transaction support
    virtual GrapaError ExecuteInTransaction(u64 transactionId, const GrapaDBXQuery& query);
};

// Query structures
struct GrapaDBXSelectQuery {
    GrapaArray<GrapaCHAR> selectColumns;
    GrapaCHAR tableName;
    GrapaDBXWhereClause whereClause;
    GrapaDBXOrderClause orderClause;
    GrapaDBXLimitClause limitClause;
};

struct GrapaDBXWhereClause {
    enum ConditionType { AND, OR, COMPARISON };
    ConditionType type;
    GrapaDBXComparison comparison;
    GrapaArray<GrapaDBXWhereClause> subConditions;
};
```

### **2. SQL-Aware Indexing**

Leverage GrapaDBX's enhanced indexing for SQL optimization:

```cpp
// SQL-aware index selection
class GrapaDBXSQLIndexOptimizer {
public:
    // Select best index for SQL query
    virtual GrapaDBXIndex* SelectBestIndex(const GrapaDBXSelectQuery& query);
    
    // Analyze query plan
    virtual GrapaError AnalyzeQueryPlan(const GrapaDBXQuery& query, GrapaDBXQueryPlan& plan);
    
    // Suggest indexes for query optimization
    virtual GrapaError SuggestIndexes(const GrapaDBXQuery& query, GrapaArray<GrapaDBXIndexSuggestion>& suggestions);
};
```

### **3. Batch Operations Integration**

Use GrapaDBX's batch operations for SQL efficiency:

```cpp
// SQL batch operations
class GrapaDBXSQLBatchOperations {
public:
    // Batch INSERT using GrapaDBX's SetBatchMultiple
    virtual GrapaError BatchInsert(const GrapaDBXInsertQuery& query);
    
    // Batch UPDATE using GrapaDBX's enhanced update capabilities
    virtual GrapaError BatchUpdate(const GrapaDBXUpdateQuery& query);
    
    // Batch DELETE using GrapaDBX's enhanced delete capabilities
    virtual GrapaError BatchDelete(const GrapaDBXDeleteQuery& query);
};
```

## Unified Path System Integration

### **1. SQL with Unified Paths**

Enable SQL queries across the unified path system:

```grapa
/* SQL works seamlessly with unified paths */
f = $file();

/* SQL on file system database */
f.cd("project/database/users");
users = SELECT * FROM . WHERE age > 25;  /* Query current location */

/* SQL across mixed paths */
f.cd("project/database");
results = SELECT u.name, d.name as dept 
          FROM users/u JOIN departments/d 
          WHERE u.dept_id = d.id;

/* SQL with transaction support */
f.cd("project/database");
BEGIN TRANSACTION;
UPDATE users SET last_login = NOW() WHERE id = 123;
INSERT INTO login_log (user_id, timestamp) VALUES (123, NOW());
COMMIT;
```

### **2. Context-Aware SQL**

SQL execution respects the current navigation context:

```grapa
/* SQL context awareness */
f = $file();

/* Navigate to specific database */
f.cd("my_database/users");

/* SQL automatically uses current context */
active_users = SELECT * FROM . WHERE status = 'active';

/* Navigate to different database */
f.cd("../analytics/metrics");

/* SQL uses new context */
daily_stats = SELECT date, COUNT(*) as count FROM . WHERE date >= '2024-01-01';
```

## Implementation Strategy

### **Phase 1: Basic SQL Grammar**
1. **Extend Grammar**: Add basic SQL grammar rules to `$grapa.grc`
2. **AST Creation**: Implement SQL AST nodes
3. **Basic Execution**: Simple SELECT, INSERT, UPDATE, DELETE
4. **GrapaDBX Integration**: Connect to GrapaDBX's basic operations

### **Phase 2: Advanced SQL Features**
1. **Complex Queries**: JOINs, subqueries, aggregations
2. **Query Optimization**: Index selection, query planning
3. **Transaction Support**: BEGIN, COMMIT, ROLLBACK
4. **Performance Monitoring**: Query statistics and optimization

### **Phase 3: Enhanced Integration**
1. **Unified Path Support**: SQL across file system and databases
2. **Batch Operations**: Leverage GrapaDBX's batch capabilities
3. **Advanced Indexing**: Use GrapaDBX's enhanced indexing
4. **Performance Tuning**: Query optimization and caching

### **Phase 4: Advanced Features**
1. **Stored Procedures**: SQL procedures in Grapa
2. **Views**: Virtual tables and views
3. **Triggers**: Automatic actions on data changes
4. **Advanced Analytics**: Window functions, CTEs

## Benefits

### **1. Developer Experience**
- **Familiar Syntax**: SQL is widely known and understood
- **Powerful Queries**: Complex data operations in simple syntax
- **Integration**: Seamless integration with existing Grapa code
- **Productivity**: Rapid database operations and queries

### **2. Performance**
- **Optimized Execution**: Leverage GrapaDBX's enhanced features
- **Batch Operations**: Efficient bulk operations
- **Index Optimization**: Automatic index selection
- **Caching**: Intelligent query result caching

### **3. Flexibility**
- **Unified Paths**: SQL works across file system and databases
- **Transaction Support**: ACID compliance with simple syntax
- **Extensibility**: Easy to add new SQL features
- **Compatibility**: Works with existing GrapaDB databases

### **4. Power**
- **Complex Queries**: JOINs, aggregations, subqueries
- **Data Analysis**: Advanced analytics and reporting
- **ETL Operations**: Data transformation and loading
- **Real-time Queries**: Live data access and updates

## Example Usage Scenarios

### **1. Data Analysis**
```grapa
/* Complex data analysis with SQL */
f = $file();
f.cd("analytics_database");

/* Sales analysis */
sales_report = SELECT 
    product_category,
    SUM(sales_amount) as total_sales,
    COUNT(*) as transaction_count,
    AVG(sales_amount) as avg_sale
FROM sales 
WHERE sale_date >= '2024-01-01'
GROUP BY product_category
ORDER BY total_sales DESC;
```

### **2. User Management**
```grapa
/* User management with transactions */
f = $file();
f.cd("user_database");

BEGIN TRANSACTION;

/* Create new user */
INSERT INTO users (username, email, created_date) 
VALUES ('john_doe', 'john@example.com', NOW());

/* Get the new user's ID */
new_user_id = SELECT id FROM users WHERE username = 'john_doe';

/* Create user profile */
INSERT INTO user_profiles (user_id, full_name, bio) 
VALUES (new_user_id, 'John Doe', 'Software Developer');

COMMIT;
```

### **3. File System Integration**
```grapa
/* SQL across file system and databases */
f = $file();

/* Query file system metadata */
file_stats = SELECT 
    file_extension,
    COUNT(*) as file_count,
    SUM(file_size) as total_size
FROM /documents 
WHERE file_size > 1024
GROUP BY file_extension;

/* Cross-reference with database */
user_files = SELECT 
    u.username,
    COUNT(f.file_name) as file_count
FROM users/u JOIN /documents/f ON u.username = f.owner
GROUP BY u.username;
```

## Success Criteria

### **1. Grammar Integration**
- ✅ SQL syntax is native part of Grapa grammar
- ✅ No conflicts with existing Grapa syntax
- ✅ Extensible for future SQL features
- ✅ Proper precedence and associativity

### **2. GrapaDBX Integration**
- ✅ Leverages all GrapaDBX enhanced features
- ✅ Uses batch operations for efficiency
- ✅ Supports transactions and rollback
- ✅ Utilizes enhanced indexing

### **3. Unified Path Support**
- ✅ SQL works across file system and databases
- ✅ Context-aware query execution
- ✅ Seamless navigation integration
- ✅ Mixed path queries

### **4. Performance**
- ✅ Query optimization and planning
- ✅ Index selection and usage
- ✅ Batch operation efficiency
- ✅ Caching and prefetching

### **5. Developer Experience**
- ✅ Familiar SQL syntax
- ✅ Rich error messages and debugging
- ✅ Query plan visualization
- ✅ Performance monitoring

## Summary

Adding SQL syntax to Grapa's language will create a powerful, integrated database querying system that leverages GrapaDBX's enhanced features while maintaining the flexibility of the unified path system. This will make Grapa an even more compelling platform for data processing, analysis, and application development.

The SQL integration will be implemented as a natural extension of Grapa's grammar system, ensuring seamless integration with existing code while providing powerful new capabilities for database operations. 

## RAW Field Support

### **Current RAW Field Implementation**

RAW fields are a crucial part of Grapa's data type system and must be fully supported in the SQL integration. The current implementation provides:

#### **1. RAW Field Storage**
- **Type**: `GrapaTokenType::RAW`
- **Storage**: Supports all storage types (`STORE_FIX`, `STORE_VAR`, `STORE_PAR`)
- **Special Handling**: RAW fields store an additional type byte to preserve the original data type
- **Conversion**: Most data types can be converted to and from RAW

#### **2. RAW Field Characteristics**
```grapa
/* RAW field creation */
f.mkfield("binary_data", "RAW", "VAR");     /* Variable size binary data */
f.mkfield("fixed_data", "RAW", "FIX", 16);  /* Fixed size binary data */
f.mkfield("large_data", "RAW", "PAR");      /* Partitioned binary data */

/* RAW field usage */
f.set("record1", "hello".raw(), "binary_data");  /* Store raw bytes */
f.set("record1", (0x6869).raw(), "binary_data"); /* Store hex data */

/* RAW field retrieval */
raw_data = f.get("record1", "binary_data");
hex_string = raw_data.hex();  /* Convert to hex string */
string_value = raw_data.str(); /* Convert to string */
int_value = raw_data.int();   /* Convert to integer */
```

#### **3. RAW Field Comparison**
- **Current Behavior**: RAW fields are compared byte-by-byte
- **Type Preservation**: RAW fields preserve the original data type for proper comparison
- **Special Handling**: Uses `ToDbType()` and `FromDbType()` for type conversion

### **SQL Integration with RAW Fields**

#### **1. RAW Field Grammar Support**
```grapa
/* SQL grammar rules for RAW fields */
@global["$sql_raw_value"]
    = rule $STR '.raw()' {@<sql_raw_from_string,{$1}>}
    | $INT '.raw()' {@<sql_raw_from_int,{$1}>}
    | '0x' $STR {@<sql_raw_from_hex,{$2}>}
    | $RAW {@<sql_raw_literal,{$1}>}
    ;

@global["$sql_raw_comparison"]
    = rule <$sql_column_name> '=' <$sql_raw_value> {@<sql_raw_eq_comparison,{$1,$3}>}
    | <$sql_column_name> '!=' <$sql_raw_value> {@<sql_raw_neq_comparison,{$1,$3}>}
    | <$sql_column_name> '>' <$sql_raw_value> {@<sql_raw_gt_comparison,{$1,$3}>}
    | <$sql_column_name> '<' <$sql_raw_value> {@<sql_raw_lt_comparison,{$1,$3}>}
    | <$sql_column_name> '>=' <$sql_raw_value> {@<sql_raw_gte_comparison,{$1,$3}>}
    | <$sql_column_name> '<=' <$sql_raw_value> {@<sql_raw_lte_comparison,{$1,$3}>}
    ;
```

#### **2. RAW Field SQL Examples**
```grapa
/* RAW field SQL operations */
f = $file();
f.cd("binary_database");

/* Create table with RAW fields */
CREATE TABLE binary_data (
    id INT,
    name STR,
    binary_content RAW,
    metadata RAW
);

/* Insert RAW data */
INSERT INTO binary_data (id, name, binary_content, metadata) VALUES 
    (1, 'file1', 'hello'.raw(), (0x6869).raw()),
    (2, 'file2', 'world'.raw(), (0x776f).raw());

/* Query RAW data */
SELECT * FROM binary_data WHERE binary_content = 'hello'.raw();
SELECT * FROM binary_data WHERE metadata > (0x5000).raw();

/* RAW field functions */
SELECT id, name, binary_content.hex() as hex_content FROM binary_data;
SELECT id, name, binary_content.str() as string_content FROM binary_data;
SELECT id, name, binary_content.int() as int_content FROM binary_data;
```

#### **3. RAW Field Comparison in SQL**
```grapa
/* RAW field comparison examples */
/* Exact match */
SELECT * FROM binary_data WHERE binary_content = 'hello'.raw();

/* Range comparison */
SELECT * FROM binary_data WHERE metadata >= (0x1000).raw() AND metadata <= (0xFFFF).raw();

/* Pattern matching (hex patterns) */
SELECT * FROM binary_data WHERE binary_content LIKE '0x68%';  /* Starts with 'h' */

/* Type-aware comparison */
SELECT * FROM binary_data WHERE binary_content.str() = 'hello';  /* Convert to string first */
SELECT * FROM binary_data WHERE binary_content.int() > 1000;     /* Convert to int first */
```

#### **4. RAW Field Functions in SQL**
```grapa
/* RAW field functions */
SELECT 
    id,
    name,
    binary_content,
    binary_content.hex() as hex_representation,
    binary_content.str() as string_representation,
    binary_content.int() as integer_representation,
    binary_content.len() as byte_length
FROM binary_data;

/* RAW field aggregations */
SELECT 
    COUNT(*) as total_records,
    AVG(binary_content.len()) as avg_size,
    MAX(binary_content.len()) as max_size,
    MIN(binary_content.len()) as min_size
FROM binary_data;
```

#### **5. RAW Field Storage Optimization**
```grapa
/* RAW field storage types in SQL */
CREATE TABLE optimized_binary (
    id INT,
    small_data RAW FIX 16,      /* Fixed size for small data */
    medium_data RAW VAR,        /* Variable size for medium data */
    large_data RAW PAR          /* Partitioned for large data */
);

/* Insert with different storage types */
INSERT INTO optimized_binary (id, small_data, medium_data, large_data) VALUES 
    (1, (0x1234).raw(), 'medium'.raw(), 'large_binary_data'.raw());
```

### **GrapaDBX RAW Field Integration**

#### **1. Enhanced RAW Field Support**
```cpp
// GrapaDBX RAW field comparison
class GrapaDBXRawComparison : public GrapaDBXComparison {
public:
    virtual GrapaError Compare(const GrapaDBXComparisonContext& context, ComparisonResult& result) override {
        // Byte-by-byte comparison for RAW fields
        GrapaBYTE& left = context.leftValue;
        GrapaBYTE& right = context.rightValue;
        
        // Preserve type information for RAW fields
        u8 leftType = left.ToDbType();
        u8 rightType = right.ToDbType();
        
        // Compare raw bytes
        result = CompareRawBytes(left, right);
        
        // If bytes are equal, compare types
        if (result == COMPARISON_EQUAL) {
            result = (leftType < rightType) ? COMPARISON_LESS_THAN : 
                     (leftType > rightType) ? COMPARISON_GREATER_THAN : 
                     COMPARISON_EQUAL;
        }
        
        return 0;
    }
    
private:
    ComparisonResult CompareRawBytes(const GrapaBYTE& left, const GrapaBYTE& right) {
        // Byte-by-byte comparison
        u64 minLen = (left.mLength < right.mLength) ? left.mLength : right.mLength;
        
        for (u64 i = 0; i < minLen; i++) {
            if (left.mBytes[i] < right.mBytes[i]) return COMPARISON_LESS_THAN;
            if (left.mBytes[i] > right.mBytes[i]) return COMPARISON_GREATER_THAN;
        }
        
        // If all bytes match up to minLen, compare lengths
        if (left.mLength < right.mLength) return COMPARISON_LESS_THAN;
        if (left.mLength > right.mLength) return COMPARISON_GREATER_THAN;
        return COMPARISON_EQUAL;
    }
};
```

#### **2. RAW Field Batch Operations**
```cpp
// GrapaDBX RAW field batch operations
class GrapaDBXRawBatchOperations {
public:
    // Batch insert RAW data
    virtual GrapaError BatchInsertRaw(const GrapaDBXInsertQuery& query) {
        for (const auto& value : query.values) {
            if (value.type == GrapaTokenType::RAW) {
                // Handle RAW field insertion with type preservation
                GrapaBYTE rawData = value.ToRaw();
                rawData.mToken = value.originalType;  // Preserve original type
                InsertRawField(query.tableName, value.fieldName, rawData);
            }
        }
        return 0;
    }
    
    // Batch update RAW data
    virtual GrapaError BatchUpdateRaw(const GrapaDBXUpdateQuery& query) {
        for (const auto& setItem : query.setList) {
            if (setItem.value.type == GrapaTokenType::RAW) {
                // Handle RAW field updates with type preservation
                GrapaBYTE rawData = setItem.value.ToRaw();
                rawData.mToken = setItem.value.originalType;
                UpdateRawField(query.tableName, setItem.fieldName, rawData, query.whereClause);
            }
        }
        return 0;
    }
};
```

#### **3. RAW Field Indexing**
```cpp
// GrapaDBX RAW field indexing
class GrapaDBXRawIndexing {
public:
    // Create index on RAW field
    virtual GrapaError CreateRawIndex(const GrapaCHAR& tableName, const GrapaCHAR& fieldName) {
        // RAW fields can be indexed for efficient comparison
        // Index stores both the raw bytes and the original type
        return CreateIndex(tableName, fieldName, INDEX_TYPE_RAW);
    }
    
    // Search using RAW field index
    virtual GrapaError SearchRawIndex(const GrapaCHAR& tableName, const GrapaCHAR& fieldName, 
                                     const GrapaBYTE& searchValue, GrapaDBXResultSet& results) {
        // Use byte-by-byte comparison for RAW field searches
        return SearchIndex(tableName, fieldName, searchValue, results, COMPARISON_MODE_RAW);
    }
};
```

### **RAW Field SQL Functions**

#### **1. RAW Conversion Functions**
```grapa
/* RAW conversion functions in SQL */
SELECT 
    id,
    name,
    binary_content,
    RAW_TO_HEX(binary_content) as hex_string,
    RAW_TO_STR(binary_content) as string_value,
    RAW_TO_INT(binary_content) as integer_value,
    RAW_LENGTH(binary_content) as byte_count
FROM binary_data;

/* RAW creation functions */
SELECT 
    id,
    name,
    STR_TO_RAW('hello') as from_string,
    INT_TO_RAW(12345) as from_int,
    HEX_TO_RAW('0x6869') as from_hex
FROM binary_data;
```

#### **2. RAW Pattern Matching**
```grapa
/* RAW pattern matching */
SELECT * FROM binary_data WHERE RAW_STARTS_WITH(binary_content, 'hello'.raw());
SELECT * FROM binary_data WHERE RAW_ENDS_WITH(binary_content, 'world'.raw());
SELECT * FROM binary_data WHERE RAW_CONTAINS(binary_content, 'test'.raw());
SELECT * FROM binary_data WHERE RAW_MATCHES(binary_content, '0x68%');  /* Hex pattern */
```

#### **3. RAW Aggregation Functions**
```grapa
/* RAW aggregation functions */
SELECT 
    COUNT(*) as total_records,
    AVG(RAW_LENGTH(binary_content)) as avg_size,
    MAX(RAW_LENGTH(binary_content)) as max_size,
    MIN(RAW_LENGTH(binary_content)) as min_size,
    SUM(RAW_LENGTH(binary_content)) as total_size
FROM binary_data;

/* RAW statistical functions */
SELECT 
    RAW_MEDIAN(binary_content) as median_size,
    RAW_STDDEV(RAW_LENGTH(binary_content)) as size_stddev,
    RAW_PERCENTILE(RAW_LENGTH(binary_content), 90) as size_90th_percentile
FROM binary_data;
```

### **RAW Field Performance Considerations**

#### **1. Storage Optimization**
- **Fixed Size**: Use `RAW FIX` for small, predictable binary data
- **Variable Size**: Use `RAW VAR` for medium-sized variable binary data
- **Partitioned**: Use `RAW PAR` for large binary data requiring partial updates

#### **2. Indexing Strategy**
- **Byte-by-Byte Indexing**: RAW fields can be indexed for efficient comparison
- **Type-Aware Indexing**: Preserve original data type for proper comparison
- **Selective Indexing**: Only index RAW fields that are frequently queried

#### **3. Query Optimization**
- **Type Conversion**: Convert RAW to appropriate type before comparison when possible
- **Pattern Matching**: Use hex patterns for efficient RAW field searches
- **Batch Operations**: Use batch operations for large RAW field updates

### **RAW Field Compatibility**

#### **1. Backward Compatibility**
- **Existing RAW Fields**: All existing RAW field functionality preserved
- **Type Preservation**: Original data types maintained in RAW fields
- **Conversion Methods**: All existing conversion methods (`.hex()`, `.str()`, `.int()`) work in SQL

#### **2. Migration Path**
- **Automatic Detection**: SQL parser automatically detects RAW field types
- **Type Inference**: RAW field types inferred from existing schema
- **Gradual Migration**: Existing RAW field code continues to work

#### **3. Enhanced Features**
- **SQL Integration**: RAW fields work seamlessly with SQL syntax
- **Advanced Functions**: New SQL functions for RAW field manipulation
- **Performance**: Leverages GrapaDBX's enhanced features for better performance

### **Summary**

RAW fields are fully supported in the SQL integration with:

1. **Complete Grammar Support**: RAW field syntax integrated into SQL grammar
2. **Type Preservation**: Original data types preserved in RAW fields
3. **Enhanced Functions**: New SQL functions for RAW field manipulation
4. **Performance Optimization**: Leverages GrapaDBX's enhanced features
5. **Backward Compatibility**: All existing RAW field functionality preserved
6. **Advanced Features**: Pattern matching, aggregation, and conversion functions

This ensures that RAW fields work exactly as they do now, while gaining the benefits of SQL syntax and GrapaDBX's enhanced capabilities. 

## Unicode Support

### **Current Unicode Implementation**

Grapa already has strong Unicode support that must be preserved and enhanced in the SQL integration:

#### **1. Unicode String Storage**
- **Encoding**: UTF-8/UTF-16 support for string fields
- **Storage**: Unicode strings stored as `GrapaTokenType::STR`
- **Comparison**: Unicode-aware string comparison
- **Functions**: Unicode-aware string manipulation functions

#### **2. Unicode Characteristics**
```grapa
/* Unicode string handling in Grapa */
f.mkfield("unicode_text", "STR", "VAR");

/* Store Unicode strings */
f.set("record1", "Hello 世界", "unicode_text");  /* Mixed ASCII and Unicode */
f.set("record2", "Привет мир", "unicode_text");  /* Cyrillic */
f.set("record3", "こんにちは世界", "unicode_text");  /* Japanese */
f.set("record4", "안녕하세요", "unicode_text");  /* Korean */

/* Retrieve Unicode strings */
unicode_text = f.get("record1", "unicode_text");
text_length = unicode_text.len();  /* Character count, not byte count */
```

### **SQL Integration with Unicode**

#### **1. Unicode SQL Grammar Support**
```grapa
/* SQL grammar rules for Unicode strings */
@global["$sql_unicode_string"]
    = rule $STR {@<sql_unicode_literal,{$1}>}
    | 'U' $STR {@<sql_unicode_escape,{$2}>}
    | 'N' $STR {@<sql_unicode_national,{$2}>}
    ;

@global["$sql_unicode_comparison"]
    = rule <$sql_column_name> '=' <$sql_unicode_string> {@<sql_unicode_eq_comparison,{$1,$3}>}
    | <$sql_column_name> '!=' <$sql_unicode_string> {@<sql_unicode_neq_comparison,{$1,$3}>}
    | <$sql_column_name> 'LIKE' <$sql_unicode_string> {@<sql_unicode_like_comparison,{$1,$3}>}
    | <$sql_column_name> 'ILIKE' <$sql_unicode_string> {@<sql_unicode_ilike_comparison,{$1,$3}>}
    ;
```

#### **2. Unicode SQL Examples**
```grapa
/* Unicode field SQL operations */
f = $file();
f.cd("unicode_database");

/* Create table with Unicode fields */
CREATE TABLE unicode_data (
    id INT,
    name STR,
    greeting STR,
    description STR
);

/* Insert Unicode data */
INSERT INTO unicode_data (id, name, greeting, description) VALUES 
    (1, 'Alice', 'Hello 世界', 'Welcome to our システム'),
    (2, 'Bob', 'Привет мир', 'Добро пожаловать'),
    (3, 'Charlie', 'こんにちは世界', 'システムへようこそ'),
    (4, 'David', '안녕하세요', '환영합니다');

/* Query Unicode data */
SELECT * FROM unicode_data WHERE greeting = 'Hello 世界';
SELECT * FROM unicode_data WHERE greeting LIKE '%世界%';
SELECT * FROM unicode_data WHERE greeting ILIKE '%мир%';  /* Case-insensitive */

/* Unicode functions */
SELECT id, name, greeting, greeting.len() as char_count FROM unicode_data;
SELECT id, name, greeting.upper() as upper_greeting FROM unicode_data;
SELECT id, name, greeting.lower() as lower_greeting FROM unicode_data;
```

#### **3. Unicode Comparison Modes**
```grapa
/* Unicode comparison modes */
/* Exact match with Unicode normalization */
SELECT * FROM unicode_data WHERE greeting = 'Hello 世界';

/* Case-insensitive Unicode comparison */
SELECT * FROM unicode_data WHERE greeting ILIKE '%мир%';

/* Unicode pattern matching */
SELECT * FROM unicode_data WHERE greeting LIKE '%世界%';

/* Unicode-aware sorting */
SELECT * FROM unicode_data ORDER BY greeting COLLATE unicode;

/* Unicode normalization */
SELECT id, name, UNICODE_NORMALIZE(greeting, 'NFC') as normalized_greeting FROM unicode_data;
```

#### **4. Unicode Functions in SQL**
```grapa
/* Unicode functions */
SELECT 
    id,
    name,
    greeting,
    greeting.len() as character_count,
    greeting.byte_len() as byte_count,
    greeting.upper() as uppercase,
    greeting.lower() as lowercase,
    UNICODE_NORMALIZE(greeting, 'NFC') as normalized,
    UNICODE_NORMALIZE(greeting, 'NFD') as decomposed
FROM unicode_data;

/* Unicode aggregation functions */
SELECT 
    COUNT(*) as total_records,
    AVG(greeting.len()) as avg_char_length,
    MAX(greeting.len()) as max_char_length,
    MIN(greeting.len()) as min_char_length
FROM unicode_data;

/* Unicode character functions */
SELECT 
    id,
    name,
    greeting,
    UNICODE_CHAR(greeting, 0) as first_char,
    UNICODE_CHAR(greeting, -1) as last_char,
    UNICODE_SUBSTRING(greeting, 1, 3) as substring_1_3
FROM unicode_data;
```

## Regex Searching

### **Regex Search Implementation**

#### **1. Regex Grammar Support**
```grapa
/* SQL grammar rules for regex searching */
@global["$sql_regex_pattern"]
    = rule 'REGEXP' $STR {@<sql_regex_pattern,{$2}>}
    | '~' $STR {@<sql_regex_pattern,{$2}>}
    | '!~' $STR {@<sql_regex_not_pattern,{$2}>}
    ;

@global["$sql_regex_comparison"]
    = rule <$sql_column_name> 'REGEXP' $STR {@<sql_regex_comparison,{$1,$3}>}
    | <$sql_column_name> '~' $STR {@<sql_regex_comparison,{$1,$3}>}
    | <$sql_column_name> '!~' $STR {@<sql_regex_not_comparison,{$1,$3}>}
    | <$sql_column_name> 'REGEXP' 'i' $STR {@<sql_regex_case_insensitive,{$1,$4}>}
    ;
```

#### **2. Regex SQL Examples**
```grapa
/* Regex searching in SQL */
f = $file();
f.cd("regex_database");

/* Create table for regex testing */
CREATE TABLE regex_data (
    id INT,
    name STR,
    email STR,
    phone STR,
    description STR
);

/* Insert test data */
INSERT INTO regex_data (id, name, email, phone, description) VALUES 
    (1, 'Alice Smith', 'alice@example.com', '555-123-4567', 'Software engineer'),
    (2, 'Bob Johnson', 'bob.johnson@test.org', '555-987-6543', 'Data scientist'),
    (3, 'Charlie Brown', 'charlie@company.co.uk', '+1-555-111-2222', 'Product manager'),
    (4, 'David Wilson', 'david.wilson@startup.io', '555-555-5555', 'UX designer');

/* Basic regex searching */
SELECT * FROM regex_data WHERE email REGEXP '@example\.com$';
SELECT * FROM regex_data WHERE phone REGEXP '^555-';
SELECT * FROM regex_data WHERE name REGEXP '^[A-Z][a-z]+ [A-Z][a-z]+$';

/* Case-insensitive regex */
SELECT * FROM regex_data WHERE email REGEXP 'i' '^[a-z]+@';
SELECT * FROM regex_data WHERE description REGEXP 'i' 'engineer|scientist';

/* Negated regex */
SELECT * FROM regex_data WHERE email !~ '@example\.com$';
SELECT * FROM regex_data WHERE phone !~ '^555-';

/* Complex regex patterns */
SELECT * FROM regex_data WHERE email REGEXP '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$';
SELECT * FROM regex_data WHERE phone REGEXP '^(\+?1-)?555-\d{3}-\d{4}$';
SELECT * FROM regex_data WHERE name REGEXP '^[A-Z][a-z]+ [A-Z][a-z]+$';
```

#### **3. Regex Functions in SQL**
```grapa
/* Regex functions */
SELECT 
    id,
    name,
    email,
    REGEXP_MATCH(email, '^([^@]+)@([^@]+)$') as email_parts,
    REGEXP_REPLACE(phone, '^555-', '1-555-') as formatted_phone,
    REGEXP_EXTRACT(description, '([A-Za-z]+)') as first_word
FROM regex_data;

/* Regex with capture groups */
SELECT 
    id,
    name,
    email,
    REGEXP_CAPTURE(email, '^([^@]+)@([^@]+)$', 1) as username,
    REGEXP_CAPTURE(email, '^([^@]+)@([^@]+)$', 2) as domain
FROM regex_data;

/* Regex validation */
SELECT 
    id,
    name,
    email,
    REGEXP_TEST(email, '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$') as valid_email,
    REGEXP_TEST(phone, '^(\+?1-)?555-\d{3}-\d{4}$') as valid_phone
FROM regex_data;
```

#### **4. Advanced Regex Features**
```grapa
/* Advanced regex features */
/* Unicode-aware regex */
SELECT * FROM regex_data WHERE name REGEXP '\\p{Han}';  /* Chinese characters */
SELECT * FROM regex_data WHERE name REGEXP '\\p{Cyrillic}';  /* Cyrillic characters */

/* Regex with flags */
SELECT * FROM regex_data WHERE email REGEXP 'i' '^[a-z]+@';  /* Case-insensitive */
SELECT * FROM regex_data WHERE description REGEXP 'm' '^engineer$';  /* Multiline */

/* Regex aggregation */
SELECT 
    COUNT(*) as total_records,
    COUNT(CASE WHEN email REGEXP '^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$' THEN 1 END) as valid_emails,
    COUNT(CASE WHEN phone REGEXP '^(\+?1-)?555-\d{3}-\d{4}$' THEN 1 END) as valid_phones
FROM regex_data;
```

## JSON/XML Field Querying

### **JSON Field Support**

#### **1. JSON Field Implementation**
```grapa
/* JSON field support in SQL */
f = $file();
f.cd("json_database");

/* Create table with JSON fields */
CREATE TABLE json_data (
    id INT,
    name STR,
    metadata JSON,
    settings JSON,
    profile JSON
);

/* Insert JSON data */
INSERT INTO json_data (id, name, metadata, settings, profile) VALUES 
    (1, 'Alice', '{"age": 30, "city": "New York", "skills": ["Python", "SQL", "JavaScript"]}', '{"theme": "dark", "notifications": true}', '{"bio": "Software engineer", "website": "alice.dev"}'),
    (2, 'Bob', '{"age": 25, "city": "San Francisco", "skills": ["Java", "Kotlin", "Android"]}', '{"theme": "light", "notifications": false}', '{"bio": "Mobile developer", "github": "bobdev"}'),
    (3, 'Charlie', '{"age": 35, "city": "London", "skills": ["C++", "Rust", "Go"]}', '{"theme": "auto", "notifications": true}', '{"bio": "Systems programmer", "linkedin": "charlie-sys"}');

/* Query JSON fields */
SELECT * FROM json_data WHERE metadata->>'city' = 'New York';
SELECT * FROM json_data WHERE metadata->>'age' > 30;
SELECT * FROM json_data WHERE metadata->'skills' ? 'Python';
SELECT * FROM json_data WHERE settings->>'theme' = 'dark';
```

#### **2. JSON Path Queries**
```grapa
/* JSON path queries */
/* Simple path access */
SELECT id, name, metadata->>'city' as city FROM json_data;
SELECT id, name, metadata->>'age' as age FROM json_data;

/* Nested path access */
SELECT id, name, profile->>'bio' as bio FROM json_data;
SELECT id, name, profile->>'website' as website FROM json_data;

/* Array access */
SELECT id, name, metadata->'skills'->0 as first_skill FROM json_data;
SELECT id, name, metadata->'skills'->1 as second_skill FROM json_data;

/* Array contains */
SELECT * FROM json_data WHERE metadata->'skills' ? 'Python';
SELECT * FROM json_data WHERE metadata->'skills' ? 'Java';

/* JSON functions */
SELECT 
    id,
    name,
    metadata,
    JSON_EXTRACT(metadata, '$.age') as age,
    JSON_EXTRACT(metadata, '$.city') as city,
    JSON_EXTRACT(metadata, '$.skills[0]') as first_skill,
    JSON_LENGTH(metadata->'skills') as skill_count
FROM json_data;
```

#### **3. JSON Aggregation and Functions**
```grapa
/* JSON aggregation functions */
SELECT 
    COUNT(*) as total_records,
    AVG(CAST(metadata->>'age' AS INT)) as avg_age,
    JSON_AGG(metadata->>'city') as all_cities,
    JSON_OBJECT_AGG(name, metadata->>'age') as name_age_map
FROM json_data;

/* JSON manipulation functions */
SELECT 
    id,
    name,
    metadata,
    JSON_SET(metadata, '$.country', 'USA') as metadata_with_country,
    JSON_REMOVE(metadata, '$.age') as metadata_without_age,
    JSON_MERGE(metadata, '{"department": "Engineering"}') as metadata_with_dept
FROM json_data;

/* JSON validation */
SELECT 
    id,
    name,
    metadata,
    JSON_VALID(metadata) as is_valid_json,
    JSON_TYPE(metadata->>'age') as age_type,
    JSON_TYPE(metadata->'skills') as skills_type
FROM json_data;
```

### **XML Field Support**

#### **1. XML Field Implementation**
```grapa
/* XML field support in SQL */
f = $file();
f.cd("xml_database");

/* Create table with XML fields */
CREATE TABLE xml_data (
    id INT,
    name STR,
    config XML,
    document XML,
    schema XML
);

/* Insert XML data */
INSERT INTO xml_data (id, name, config, document, schema) VALUES 
    (1, 'Alice', '<config><theme>dark</theme><notifications>true</notifications></config>', '<user><name>Alice</name><role>admin</role><permissions><read>true</read><write>true</write></permissions></user>', '<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"><xs:element name="user"><xs:complexType><xs:sequence><xs:element name="name" type="xs:string"/><xs:element name="role" type="xs:string"/></xs:sequence></xs:complexType></xs:element></xs:schema>'),
    (2, 'Bob', '<config><theme>light</theme><notifications>false</notifications></config>', '<user><name>Bob</name><role>user</role><permissions><read>true</read><write>false</write></permissions></user>', '<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema"><xs:element name="user"><xs:complexType><xs:sequence><xs:element name="name" type="xs:string"/><xs:element name="role" type="xs:string"/></xs:sequence></xs:complexType></xs:element></xs:schema>');

/* Query XML fields */
SELECT * FROM xml_data WHERE config->>'theme' = 'dark';
SELECT * FROM xml_data WHERE document->>'role' = 'admin';
SELECT * FROM xml_data WHERE document->'permissions'->>'write' = 'true';
```

#### **2. XPath Queries**
```grapa
/* XPath queries for XML fields */
/* Simple XPath */
SELECT id, name, XPATH(document, '/user/name') as user_name FROM xml_data;
SELECT id, name, XPATH(document, '/user/role') as user_role FROM xml_data;

/* XPath with conditions */
SELECT * FROM xml_data WHERE XPATH(document, '/user/role') = 'admin';
SELECT * FROM xml_data WHERE XPATH(document, '/user/permissions/write') = 'true';

/* XPath functions */
SELECT 
    id,
    name,
    document,
    XPATH(document, '/user/name') as name_xpath,
    XPATH(document, '/user/role') as role_xpath,
    XPATH_COUNT(document, '/user/permissions/*') as permission_count
FROM xml_data;

/* XML validation */
SELECT 
    id,
    name,
    document,
    XML_VALID(document) as is_valid_xml,
    XML_VALIDATE(document, schema) as validates_against_schema
FROM xml_data;
```

#### **3. XML Functions and Aggregation**
```grapa
/* XML functions and aggregation */
SELECT 
    id,
    name,
    document,
    XML_EXTRACT(document, '/user/name') as extracted_name,
    XML_EXTRACT(document, '/user/role') as extracted_role,
    XML_LENGTH(document) as xml_length
FROM xml_data;

/* XML aggregation */
SELECT 
    COUNT(*) as total_records,
    XML_AGG(document) as all_documents,
    XPATH_AGG(document, '/user/role') as all_roles
FROM xml_data;

/* XML transformation */
SELECT 
    id,
    name,
    document,
    XML_TRANSFORM(document, 'stylesheet.xsl') as transformed_document,
    XML_TO_JSON(document) as json_version
FROM xml_data;
```

### **GrapaDBX Implementation for Unicode, Regex, and JSON/XML**

#### **1. Unicode Support in GrapaDBX**
```cpp
// Unicode support in GrapaDBX
class GrapaDBXUnicodeSupport {
public:
    // Unicode-aware string comparison
    virtual GrapaError CompareUnicode(const GrapaCHAR& left, const GrapaCHAR& right, s8& result) {
        // Use Unicode-aware comparison
        result = UnicodeCompare(left, right);
        return 0;
    }
    
    // Unicode normalization
    virtual GrapaError NormalizeUnicode(const GrapaCHAR& input, GrapaCHAR& output, const GrapaCHAR& form) {
        // Normalize Unicode string (NFC, NFD, NFKC, NFKD)
        return UnicodeNormalize(input, output, form);
    }
    
    // Unicode character count
    virtual u64 UnicodeCharCount(const GrapaCHAR& str) {
        // Count Unicode characters (not bytes)
        return UnicodeLength(str);
    }
    
private:
    s8 UnicodeCompare(const GrapaCHAR& left, const GrapaCHAR& right);
    GrapaError UnicodeNormalize(const GrapaCHAR& input, GrapaCHAR& output, const GrapaCHAR& form);
    u64 UnicodeLength(const GrapaCHAR& str);
};
```

#### **2. Regex Support in GrapaDBX**
```cpp
// Regex support in GrapaDBX
class GrapaDBXRegexSupport {
public:
    // Regex pattern matching
    virtual GrapaError RegexMatch(const GrapaCHAR& text, const GrapaCHAR& pattern, bool& matches) {
        // Use PCRE2 or similar regex engine
        return RegexEngine::Match(text, pattern, matches);
    }
    
    // Regex with flags
    virtual GrapaError RegexMatchWithFlags(const GrapaCHAR& text, const GrapaCHAR& pattern, 
                                          const GrapaCHAR& flags, bool& matches) {
        // Support case-insensitive, multiline, etc.
        return RegexEngine::MatchWithFlags(text, pattern, flags, matches);
    }
    
    // Regex capture groups
    virtual GrapaError RegexCapture(const GrapaCHAR& text, const GrapaCHAR& pattern, 
                                   GrapaArray<GrapaCHAR>& captures) {
        // Extract capture groups
        return RegexEngine::Capture(text, pattern, captures);
    }
    
    // Regex replacement
    virtual GrapaError RegexReplace(const GrapaCHAR& text, const GrapaCHAR& pattern, 
                                   const GrapaCHAR& replacement, GrapaCHAR& result) {
        // Replace matched patterns
        return RegexEngine::Replace(text, pattern, replacement, result);
    }
};
```

#### **3. JSON Support in GrapaDBX**
```cpp
// JSON support in GrapaDBX
class GrapaDBXJsonSupport {
public:
    // JSON path queries
    virtual GrapaError JsonPathQuery(const GrapaCHAR& json, const GrapaCHAR& path, GrapaCHAR& result) {
        // Execute JSON path query
        return JsonEngine::PathQuery(json, path, result);
    }
    
    // JSON validation
    virtual GrapaError JsonValidate(const GrapaCHAR& json, bool& isValid) {
        // Validate JSON syntax
        return JsonEngine::Validate(json, isValid);
    }
    
    // JSON extraction
    virtual GrapaError JsonExtract(const GrapaCHAR& json, const GrapaCHAR& path, GrapaValue& result) {
        // Extract value from JSON path
        return JsonEngine::Extract(json, path, result);
    }
    
    // JSON manipulation
    virtual GrapaError JsonSet(const GrapaCHAR& json, const GrapaCHAR& path, 
                              const GrapaValue& value, GrapaCHAR& result) {
        // Set value at JSON path
        return JsonEngine::Set(json, path, value, result);
    }
};
```

#### **4. XML Support in GrapaDBX**
```cpp
// XML support in GrapaDBX
class GrapaDBXXmlSupport {
public:
    // XPath queries
    virtual GrapaError XPathQuery(const GrapaCHAR& xml, const GrapaCHAR& xpath, GrapaArray<GrapaCHAR>& results) {
        // Execute XPath query
        return XmlEngine::XPathQuery(xml, xpath, results);
    }
    
    // XML validation
    virtual GrapaError XmlValidate(const GrapaCHAR& xml, bool& isValid) {
        // Validate XML syntax
        return XmlEngine::Validate(xml, isValid);
    }
    
    // XML extraction
    virtual GrapaError XmlExtract(const GrapaCHAR& xml, const GrapaCHAR& xpath, GrapaCHAR& result) {
        // Extract value from XPath
        return XmlEngine::Extract(xml, xpath, result);
    }
    
    // XML transformation
    virtual GrapaError XmlTransform(const GrapaCHAR& xml, const GrapaCHAR& stylesheet, GrapaCHAR& result) {
        // Transform XML with XSLT
        return XmlEngine::Transform(xml, stylesheet, result);
    }
};
```

### **Performance Considerations**

#### **1. Unicode Performance**
- **Normalization Caching**: Cache normalized Unicode strings
- **Collation Tables**: Pre-computed Unicode collation tables
- **Character Set Optimization**: Optimize for common Unicode ranges

#### **2. Regex Performance**
- **Pattern Compilation**: Compile regex patterns once, reuse
- **Index Integration**: Use regex indexes for common patterns
- **Early Termination**: Stop matching when possible

#### **3. JSON/XML Performance**
- **Path Indexing**: Index common JSON/XML paths
- **Lazy Parsing**: Parse only when needed
- **Caching**: Cache parsed JSON/XML structures

### **Summary**

The enhanced SQL integration now supports:

1. **Unicode Support**: Full Unicode string handling with normalization, collation, and functions
2. **Regex Searching**: Powerful regex pattern matching with flags and capture groups
3. **JSON Field Querying**: JSON path queries, validation, and manipulation
4. **XML Field Querying**: XPath queries, validation, and transformation
5. **Performance Optimization**: Efficient implementations for all features
6. **Backward Compatibility**: All existing functionality preserved

These features make GrapaDBX competitive with modern database systems while maintaining Grapa's unique capabilities. 