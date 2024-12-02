#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

// Function to handle errors
void handle_error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

// Callback function for executing queries
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s: %s\t", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void clear_screen() {
#ifdef _WIN32
    system("cls"); // Windows command to clear the screen
#else
    system("clear"); // Linux/macOS command to clear the screen
#endif
}

void search_by_name(sqlite3 *db) {
    char name[100];
    char sql[200];
    char *err_msg = NULL;

    printf("Enter name to search: ");
    scanf("%s", name);

    // Prepare the SQL query
    sprintf(sql, "SELECT * FROM Contacts WHERE Name LIKE '%%%s%%';", name);

    int rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Search completed.\n");
    }
}

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc;

    // Open database
    rc = sqlite3_open("contacts.db", &db);
    if (rc) {
        handle_error("Can't open database");
    } else {
        printf("Opened database successfully\n");
    }

    // Create table if not exists
    char *sql = "CREATE TABLE IF NOT EXISTS Contacts ("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "Name TEXT NOT NULL,"
                "PhoneNumber TEXT NOT NULL,"
                "DateOfBirth DATE"
                ");";

    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if (rc != SQLITE_OK) {
        handle_error("SQL error");
    } else {
        printf("Table created successfully\n");
    }

    // Menu
    int choice;
    do {
        printf("\nMenu:\n");
        printf("1. Add Contact\n");
        printf("2. View Contacts\n");
        printf("3. Update Contact\n");
        printf("4. Delete Contact\n");
        printf("5. Search by Name\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char name[100], phone[15], dob[20];
                printf("Enter Name: ");
                scanf("%s", name);
                printf("Enter Phone Number: ");
                scanf("%s", phone);
                printf("Enter Date of Birth (YYYY-MM-DD): ");
                scanf("%s", dob);

                char insert_sql[200];
                sprintf(insert_sql, "INSERT INTO Contacts (Name, PhoneNumber, DateOfBirth) VALUES ('%s', '%s', '%s');",
                        name, phone, dob);

                rc = sqlite3_exec(db, insert_sql, callback, 0, &err_msg);
                if (rc != SQLITE_OK) {
                    handle_error("SQL error");
                } else {
                    printf("Contact added successfully\n");
                }
                break;
            }
            case 2: {
                clear_screen();  // Clear the screen
                printf("Contacts:\n");
                char *select_sql = "SELECT * FROM Contacts;";
                rc = sqlite3_exec(db, select_sql, callback, 0, &err_msg);
                if (rc != SQLITE_OK) {
                    handle_error("SQL error");
                }
                break;
            }
            case 3: {
                int id, field_choice;
                char new_value[100];
                char update_sql[200];
                printf("Enter ID of the contact to update: ");
                scanf("%d", &id);

                printf("What do you want to update?\n");
                printf("1. Name\n");
                printf("2. Phone Number\n");
                printf("3. Date of Birth\n");
                printf("Enter your choice: ");
                scanf("%d", &field_choice);

                switch (field_choice) {
                    case 1:
                        printf("Enter new Name: ");
                        scanf("%s", new_value);
                        sprintf(update_sql, "UPDATE Contacts SET Name = '%s' WHERE ID = %d;", new_value, id);
                        break;
                    case 2:
                        printf("Enter new Phone Number: ");
                        scanf("%s", new_value);
                        sprintf(update_sql, "UPDATE Contacts SET PhoneNumber = '%s' WHERE ID = %d;", new_value, id);
                        break;
                    case 3:
                        printf("Enter new Date of Birth (YYYY-MM-DD): ");
                        scanf("%s", new_value);
                        sprintf(update_sql, "UPDATE Contacts SET DateOfBirth = '%s' WHERE ID = %d;", new_value, id);
                        break;
                    default:
                        printf("Invalid choice.\n");
                        continue;
                }

                rc = sqlite3_exec(db, update_sql, callback, 0, &err_msg);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "SQL error: %s\n", err_msg);
                    sqlite3_free(err_msg);
                } else {
                    printf("Contact updated successfully.\n");
                }
                break;
            }
            case 4: {
                int id;
                printf("Enter ID of the contact to delete: ");
                scanf("%d", &id);

                char delete_sql[100];
                sprintf(delete_sql, "DELETE FROM Contacts WHERE ID = %d;", id);

                rc = sqlite3_exec(db, delete_sql, callback, 0, &err_msg);
                if (rc != SQLITE_OK) {
                    handle_error("SQL error");
                } else {
                    printf("Contact deleted successfully\n");
                }
                break;
            }
            case 5:
                search_by_name(db);
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    sqlite3_close(db); // Close database
    return 0;
}

// To compile gcc -o crud.exe crud.c libsqlite3.a -- You might have to install sqlite.
