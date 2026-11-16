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
    size_t nlen = strlen(name), plen = strlen(phone);
    if (nlen == 0 || nlen >= NAME_LEN || plen == 0 || plen >= PHONE_LEN) {
        fprintf(stderr, "invalid contact: name 1-%d chars, phone 1-%d chars\n", NAME_LEN - 1, PHONE_LEN - 1);
        return NULL;
    }
    Contact *c = malloc(sizeof(Contact));
    if (c == NULL)
        return NULL;
    memcpy(c->name, name, nlen + 1);
    memcpy(c->phone, phone, plen + 1);
    return c;
}

static int book_add(Book *b, const char *name, const char *phone)
{
    if (b->count >= MAX_CONTACTS) {
        fprintf(stderr, "contact book is full (%d)\n", MAX_CONTACTS);
        return -1;
    }
    Contact *c = contact_new(name, phone);
    if (c == NULL)
        return -1;
    b->items[b->count++] = c;
    return 0;
}

static void book_free(Book *b)
{
    for (int i = 0; i < b->count; i++)
        free(b->items[i]);
    b->count = 0;
}

static int book_load(Book *b, const char *path)
{
    char line[128];
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return -1;
    while (fgets(line, sizeof(line), f)) {
        const char *name = strtok(line, ";\n");
        const char *phone = strtok(NULL, ";\n");
        if (name == NULL || phone == NULL)
            continue;
        if (book_add(b, name, phone) != 0)
            break;
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
    printf("%s: %s\n", c->name, c->phone);
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
    int found = 0;
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
            printf("deleted: %s\n", c->name);
            free(c);
            for (int j = i; j < b->count - 1; j++)
                b->items[j] = b->items[j + 1];
            b->count--;
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

    int rc = 0;
    if (strcmp(cmd, "list") == 0) {
        book_list(&book);
    } else if (strcmp(cmd, "find") == 0 && argc == 4) {
        book_find(&book, argv[3]);
    } else if (strcmp(cmd, "add") == 0 && argc == 5) {
        if (book_add(&book, argv[3], argv[4]) == 0) {
            book_save(&book, path);
            printf("added: %s\n", argv[3]);
        } else {
            rc = 1;
        }
    } else if (strcmp(cmd, "del") == 0 && argc == 4) {
        if (book_del(&book, argv[3]) == 0)
            book_save(&book, path);
    } else {
        fprintf(stderr, "unknown command\n");
        rc = 2;
    }
    book_free(&book);
    return rc;
}
