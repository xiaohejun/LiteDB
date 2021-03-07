#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "util.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum { 
    PREPARE_SUCCESS, 
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum { 
    EXECUTE_SUCCESS, 
    EXECUTE_TABLE_FULL 
} ExecuteResult;

typedef enum {
    STATEMENT_INSERT, 
    STATEMENT_SELECT 
} StatementType;

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct {
    StatementType type;
    Row rowToInsert;
} Statement;

const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / sizeof(Row);
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
    uint32_t rowNums;
    void *pages[TABLE_MAX_PAGES];
} Table;

typedef struct {
    char *buffer;
    size_t buffLen;
    ssize_t inputLen;
} InputBuffer;

InputBuffer* NewInputBuffer()
{
    InputBuffer *input = malloc(sizeof(sizeof(InputBuffer)));
    input->buffer = NULL;
    input->buffLen = 0;
    input->inputLen = 0;
    return input;
}

void DeleteInputBuffer(InputBuffer *input)
{
    free(input->buffer);
    free(input);
}


void SerializeRow(Row *src, void *dest)
{
    SERIALIZE_MEMCPY_ATTR(dest, src, id);
    SERIALIZE_MEMCPY_ATTR(dest, src, username);
    SERIALIZE_MEMCPY_ATTR(dest, src, email);
}

void DeserializeRow(void *src, Row *dest)
{
    DESERIALIZE_MEMCPY_ATTR(dest, src, id);
    DESERIALIZE_MEMCPY_ATTR(dest, src, username);
    DESERIALIZE_MEMCPY_ATTR(dest, src, email);
}

void* RowSlot(Table *table, uint32_t rowNum)
{
    uint32_t pageNum = rowNum / ROWS_PER_PAGE;
    void *page = table->pages[pageNum];
    if (page == NULL) {
        table->pages[pageNum] = malloc(PAGE_SIZE);
        page = table->pages[pageNum];
    }
    uint32_t rowOffset = rowNum % ROWS_PER_PAGE;
    uint32_t byteOffset = rowOffset * sizeof(Row);
    return page + byteOffset;
}

Table* NewTable()
{
    Table *table = malloc(sizeof(Table));
    table->rowNums = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        table->pages[i] = NULL;
    }
    return table;
}

void FreeTable(Table *table)
{
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i) {
        free(table->pages[i]);
    }
    free(table);
}

void PrintPrompt()
{
    printf("LiteDB > ");
}

MetaCommandResult DoMetaCommand(InputBuffer *input, Table *table)
{
    if (strcmp(input->buffer, ".exit") == 0) {
        DeleteInputBuffer(input);
        free(table);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

PrepareResult PrepareStatement(InputBuffer *input, Statement *statement)
{
    if (strncmp(input->buffer, "insert", strlen("insert")) == 0) {
        statement->type = STATEMENT_INSERT;
        int argsAssign = sscanf(input->buffer, "insert %u %s %s", 
            &statement->rowToInsert.id,
            statement->rowToInsert.username, 
            statement->rowToInsert.email);
        if (argsAssign < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }

    if (strncmp(input->buffer, "select", strlen("select")) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void ReadInput(InputBuffer *input)
{
    ssize_t readBytes = getline(&(input->buffer), &(input->buffLen), stdin);
    if (readBytes <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    
    // 忽略结尾的行'\n'字符
    input->inputLen = readBytes - 1;
    input->buffer[readBytes - 1] = '\0';
}

void PrintRow(Row *row)
{
    printf("(%u, %s, %s)\n", row->id, row->username, row->email);
}

ExecuteResult ExecuteInsert(Statement *statement, Table *table)
{
    if (table->rowNums >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Row *rowToInsert = &(statement->rowToInsert);
    SerializeRow(rowToInsert, RowSlot(table, table->rowNums));
    table->rowNums += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult ExecuteSelect(Statement *statement, Table *table)
{
    Row row;
    for (uint32_t i = 0; i < table->rowNums; ++i) {
        DeserializeRow(RowSlot(table, i), &row);
        PrintRow(&row);
    }
    return EXECUTE_SUCCESS;
}

ExecuteResult ExecuteStatement(Statement *statement, Table *table)
{
    switch (statement->type) {
        case STATEMENT_INSERT:
            return ExecuteInsert(statement, table);
        case STATEMENT_SELECT:
            return ExecuteSelect(statement, table);
        default:
            break;
    }
}

bool IsMetaCommand(InputBuffer *input)
{
    return input->buffer[0] == '.';
}

int main()
{
    Table* table = NewTable();
    InputBuffer *input = NewInputBuffer();
    while (true) {
        PrintPrompt();
        ReadInput(input);
        if (IsMetaCommand(input)) {
            if (DoMetaCommand(input, table) == META_COMMAND_UNRECOGNIZED_COMMAND) {
                printf("Unrecognized command '%s'\n", input->buffer);
                continue;
            }
        }
        Statement statement;
        switch (PrepareStatement(input, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement.\n");
            	continue;
            case PREPARE_UNRECOGNIZED_STATEMENT:
                printf("Unrecognized command '%s'\n", input->buffer);
                continue;
            default:
                break;
        }

        switch (ExecuteStatement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Executed.\n");
                break;
            case EXECUTE_TABLE_FULL:
                printf("Error: Table full.\n");
                break;
            default:
                break;
        }
    }
    return 0;
}