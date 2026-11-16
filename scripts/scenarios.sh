#!/usr/bin/env bash
# Сценарії запуску для динамічного аналізу: звичайна робота і шкідливі вхідні дані.
# Використання: scenarios.sh "<команда запуску>", наприклад "valgrind --leak-check=full ./contacts"
RUN=$1
DB=$(mktemp)
LONG=$(printf 'A%.0s' {1..64})

scenario() {
    local title=$1; shift
    cp data/contacts.txt "$DB"
    echo "===== scenario: $title"
    $RUN "$DB" "$@" 2>&1
    echo "===== exit code: $?"
    echo
}

scenario "list"                      list
scenario "find existing contact"     find "Olena Koval"
scenario "find missing contact"      find "Nobody"
scenario "add contact"               add "Taras Bondar" "+380661234567"
scenario "delete contact"            del "Ivan Petrenko"
scenario "format string in name"     add '%p.%p.%p.%p' "+380000000000"
scenario "name longer than buffer"   add "$LONG" "+380000000000"
rm -f "$DB"
