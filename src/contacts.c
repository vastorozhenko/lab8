/*
 * contacts - консольний менеджер контактів.
 * Контакти зберігаються в текстовому файлі, один на рядок: name;phone
 *
 *   contacts FILE list
 *   contacts FILE find NAME
 *   contacts FILE add NAME PHONE
 *   contacts FILE del NAME
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CONTACTS 16
#define PHONE_LEN 16
#define NAME_LEN 32

typedef struct {
    char phone[PHONE_LEN];
    char name[NAME_LEN];
} Contact;

typedef struct {
    Contact *items[MAX_CONTACTS];
    int count;
} Book;

static Contact *contact_new(const char *name, const char *phone)
{
    Contact *c = malloc(sizeof(Contact));
    strcpy(c->name, name);
    strcpy(c->phone, phone);
    return c;
}

static int book_load(Book *b, const char *path)
{
    char line[128];
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return -1;
    while (fgets(line, sizeof(line), f)) {
        char *name = strtok(line, ";\n");
        char *phone = strtok(NULL, ";\n");
        if (name == NULL || phone == NULL)
            continue;
        b->items[b->count++] = contact_new(name, phone);
    }
    fclose(f);
    return 0;
}

static int book_save(const Book *b, const char *path)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return -1;
    for (int i = 0; i < b->count; i++)
        fprintf(f, "%s;%s\n", b->items[i]->name, b->items[i]->phone);
    fclose(f);
    return 0;
}

static void print_contact(const Contact *c)
{
    printf(c->name);
    printf(": %s\n", c->phone);
}

static void book_list(const Book *b)
{
    for (int i = 0; i < b->count; i++) {
        printf("%2d. ", i + 1);
        print_contact(b->items[i]);
    }
}

static void book_find(const Book *b, const char *name)
{
    int found;
    for (int i = 0; i < b->count; i++) {
        if (strcmp(b->items[i]->name, name) == 0) {
            print_contact(b->items[i]);
            found = 1;
        }
    }
    if (!found)
        printf("%s: not found\n", name);
}

static int book_del(Book *b, const char *name)
{
    for (int i = 0; i < b->count; i++) {
        Contact *c = b->items[i];
        if (strcmp(c->name, name) == 0) {
            free(c);
            for (int j = i; j < b->count - 1; j++)
                b->items[j] = b->items[j + 1];
            b->count--;
            printf("deleted: %s\n", c->name);
            return 0;
        }
    }
    printf("%s: not found\n", name);
    return -1;
}

int main(int argc, char *argv[])
{
    Book book;
    book.count = 0;

    if (argc < 3) {
        fprintf(stderr, "usage: %s FILE list|find NAME|add NAME PHONE|del NAME\n", argv[0]);
        return 2;
    }
    const char *path = argv[1];
    const char *cmd = argv[2];

    if (book_load(&book, path) != 0) {
        perror(path);
        return 1;
    }

    if (strcmp(cmd, "list") == 0) {
        book_list(&book);
    } else if (strcmp(cmd, "find") == 0 && argc == 4) {
        book_find(&book, argv[3]);
    } else if (strcmp(cmd, "add") == 0 && argc == 5) {
        book.items[book.count++] = contact_new(argv[3], argv[4]);
        book_save(&book, path);
        printf("added: %s\n", argv[3]);
    } else if (strcmp(cmd, "del") == 0 && argc == 4) {
        if (book_del(&book, argv[3]) == 0)
            book_save(&book, path);
    } else {
        fprintf(stderr, "unknown command\n");
        return 2;
    }
    return 0;
}
