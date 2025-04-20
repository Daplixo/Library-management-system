#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TITLE_LEN 100
#define MAX_AUTHOUR_LEN 50
#define FILENAME "library_data.dat"

struct book
{
    char title[MAX_TITLE_LEN];
    char authour[MAX_AUTHOUR_LEN];
    int book_id;
    struct book *next;
};

typedef struct book Book;

void saveLibraryToFile(Book *head)
{
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL)
    {
        printf("\nError: Unable to save library data! Check file permissions.\n");
        return;
    }

    int count = 0;
    Book *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    if (fwrite(&count, sizeof(int), 1, file) != 1)
    {
        printf("\nError writing count to file!\n");
        fclose(file);
        return;
    }

    current = head;
    while (current != NULL)
    {
        if (fwrite(current, sizeof(Book) - sizeof(Book *), 1, file) != 1)
        {
            printf("\nError writing book data to file!\n");
            fclose(file);
            return;
        }
        current = current->next;
    }

    fclose(file);
}

Book *loadLibraryFromFile()
{
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL)
    {
        return NULL;
    }

    int count;
    if (fread(&count, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return NULL;
    }

    if (count <= 0)
    {
        fclose(file);
        return NULL;
    }

    Book *head = NULL;
    Book *tail = NULL;

    for (int i = 0; i < count; i++)
    {
        Book *newBook = (Book *)malloc(sizeof(Book));
        if (newBook == NULL)
        {
            fclose(file);
            return head;
        }

        if (fread(newBook, sizeof(Book) - sizeof(Book *), 1, file) != 1)
        {
            free(newBook);
            fclose(file);
            return head;
        }

        newBook->next = NULL;

        if (head == NULL)
        {
            head = newBook;
            tail = newBook;
        }
        else
        {
            tail->next = newBook;
            tail = newBook;
        }
    }

    fclose(file);
    return head;
}

void addBook(Book **head)
{
    Book *newBook = (Book *)malloc(sizeof(Book));
    if (newBook == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("Enter the title of the book: ");
    scanf(" %[^\n]", newBook->title);
    printf("Enter the Authour name: ");
    scanf(" %[^\n]", newBook->authour);
    printf("Enter the Book Id: ");
    scanf("%d", &newBook->book_id);

    newBook->next = NULL;

    if (*head == NULL)
    {
        *head = newBook;
    }
    else
    {
        Book *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newBook;
    }

    saveLibraryToFile(*head);
    printf("\nBook added successfully and saved to file!\n");
}

void displayBooks(Book *head)
{
    if (head == NULL)
    {
        printf("Library is empty!\n");
        return;
    }

    printf("\nLibrary Books:\n");
    Book *current = head;

    while (current != NULL)
    {
        printf("Book ID: %d\n", current->book_id);
        printf("Book Title: %s\n", current->title);
        printf("Book Authour: %s\n", current->authour);
        printf("\n");
        current = current->next;
    }
}

void searchBook(Book *head, int bookID)
{
    Book *current = head;
    int found = 0;

    while (current != NULL)
    {
        if (current->book_id == bookID)
        {
            printf("\nBook Found: \n");
            printf("Book ID: %d\n", current->book_id);
            printf("Book Title: %s\n", current->title);
            printf("Book Authour: %s\n", current->authour);
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("\nBook does not exist!\n");
    }
}

void removeBook(Book **head, int bookID)
{
    if (*head == NULL)
    {
        printf("\nLibrary is empty!\n");
        return;
    }

    Book *current = *head;
    Book *previous = NULL;

    if (current != NULL && current->book_id == bookID)
    {
        *head = current->next;
        free(current);
        saveLibraryToFile(*head);
        printf("\nBook with ID %d removed successfully and changes saved!\n", bookID);
        return;
    }

    while (current != NULL && current->book_id != bookID)
    {
        previous = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("\nBook with ID %d not found!\n", bookID);
        return;
    }

    previous->next = current->next;
    free(current);
    saveLibraryToFile(*head);
    printf("\nBook with ID %d removed successfully and changes saved!\n", bookID);
}

void freeLibrary(Book *head)
{
    Book *current = head;
    Book *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

void displayMenu()
{
    printf("\n==================================\n");
    printf("    LIBRARY MANAGEMENT SYSTEM    \n");
    printf("==================================\n");
    printf("1. Add books to the library\n");
    printf("2. Display books in the library\n");
    printf("3. Search books by ID\n");
    printf("4. Remove a book by ID\n");
    printf("5. Exit\n");
    printf("==================================\n");
    printf("Enter your choice: ");
}

int main()
{
    Book *library = NULL;
    int choice, bookID;

    library = loadLibraryFromFile();
    if (library != NULL)
    {
        int count = 0;
        Book *temp = library;
        while (temp != NULL)
        {
            count++;
            temp = temp->next;
        }
        printf("Library data loaded successfully! (%d books found)\n", count);
    }

    while (1)
    {
        displayMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addBook(&library);
            break;

        case 2:
            displayBooks(library);
            break;

        case 3:
            printf("\nEnter Book Id to search: ");
            scanf("%d", &bookID);
            searchBook(library, bookID);
            break;

        case 4:
            printf("\nEnter Book Id to remove: ");
            scanf("%d", &bookID);
            removeBook(&library, bookID);
            break;

        case 5:
            freeLibrary(library);
            printf("\nExiting the program. Goodbye!\n");
            return 0;

        default:
            printf("\nInvalid choice! Please try again\n");
        }
    }

    return 0;
}