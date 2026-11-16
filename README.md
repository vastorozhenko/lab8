# lab8: аналіз та виявлення вразливостей у коді

Об'єкт аналізу - `contacts`, консольний менеджер контактів мовою C. Контакти зберігаються в текстовому файлі, один на рядок у форматі `name;phone`.

## Збірка і запуск

Потрібно: Ubuntu 24.04, `gcc`, `make`; для аналізу - `cppcheck`, `flawfinder`, `valgrind`.

```bash
sudo apt install -y build-essential cppcheck flawfinder valgrind
make                      # ./contacts
make asan                 # ./contacts-asan (AddressSanitizer + UndefinedBehaviorSanitizer)
```

```bash
cp data/contacts.txt my.txt
./contacts my.txt list
./contacts my.txt find "Olena Koval"
./contacts my.txt add "Taras Bondar" "+380661234567"
./contacts my.txt del "Ivan Petrenko"
```

## Аналіз

```bash
make analyze STAGE=before     # або STAGE=after після виправлень
bash scripts/compare.sh       # порівняльна таблиця before/after -> logs/comparison.md
```

| Інструмент | Тип | Що перевіряє | Журнал |
|---|---|---|---|
| gcc `-Wall -Wextra -fanalyzer` | статичний | попередження компілятора і міжпроцедурний аналіз шляхів виконання | `gcc-analyzer.log` |
| cppcheck `--enable=all --inconclusive` | статичний | неініціалізовані змінні, витоки, вихід за межі, стиль | `cppcheck.log` |
| flawfinder | статичний | небезпечні функції C (strcpy, printf, fopen...) з рівнем ризику 0-5 | `flawfinder.log` |
| valgrind memcheck | динамічний | недійсні читання/запис, використання неініціалізованих значень, витоки пам'яті | `valgrind.log` |
| AddressSanitizer + UBSan | динамічний | переповнення буферів, use-after-free, витоки, невизначена поведінка | `asan.log` |

Динамічні інструменти запускають програму в сценаріях `scripts/scenarios.sh`: звичайні операції (list, find, add, del), пошук відсутнього контакту, ім'я з форматними специфікаторами `%p` і ім'я довше за буфер.

Журнали: `logs/<етап>/`, підсумок кожного етапу - `summary.txt`.

## Результати аналізу

| ID | Вразливість | CWE | Критичність | Місце (до виправлення) | Хто виявив |
|---|---|---|---|---|---|
| V1 | переповнення буфера в купі: `strcpy` імені і телефону без перевірки довжини | CWE-787, CWE-120 | критична | `contact_new`, рядки 31-32 | flawfinder, valgrind, ASan |
| V2 | форматний рядок: `printf(c->name)` | CWE-134 | висока | `print_contact`, рядок 66 | gcc, flawfinder; ручний тест з `%p` |
| V3 | використання пам'яті після звільнення: ім'я виводиться після `free(c)` | CWE-416 | висока | `book_del`, рядок 100 | gcc, valgrind, ASan |
| V4 | вихід за межі масиву `items[16]`: немає перевірки кількості контактів | CWE-787 | висока | `book_load`, `main` (add) | ручний перегляд коду |
| V5 | неініціалізована змінна `found` | CWE-457 | середня | `book_find`, рядок 87 | cppcheck, gcc -fanalyzer, valgrind |
| V6 | витік пам'яті: контакти не звільняються | CWE-401 | низька | `main` | valgrind, LeakSanitizer |
| V7 | немає перевірки результату `malloc` | CWE-690 | низька | `contact_new`, рядок 30 | ручний перегляд коду |

### Виправлення

| ID | Що змінено |
|---|---|
| V1 | перевірка довжини імені (1-31) і телефону (1-15) до копіювання, `memcpy` з відомою довжиною замість `strcpy` |
| V2 | `printf("%s: %s\n", ...)` - дані користувача не використовуються як форматний рядок |
| V3 | повідомлення виводиться до `free(c)` |
| V4 | `book_add` перевіряє `count < MAX_CONTACTS` при завантаженні і додаванні |
| V5 | `int found = 0` |
| V6 | `book_free` звільняє всі контакти перед виходом |
| V7 | перевірка `malloc` на `NULL` |

Порівняння до і після - `logs/comparison.md`, повні журнали - `logs/before/` і `logs/after/`.

Після виправлення flawfinder залишає лише підказки рівнів 1-2 (`char[]`, `memcpy`, `strlen`, `fopen`): масиви фіксованого розміру і копіювання захищені перевіркою довжини, а файл відкривається за шляхом, який указав сам користувач.

### Рекомендації

- Не використовувати `strcpy`, `strcat`, `sprintf`, `gets`; копіювати лише після перевірки довжини або через `snprintf`.
- Ніколи не передавати дані користувача як форматний рядок; збирати з `-Wformat -Wformat-security -Werror=format-security`.
- Після `free` не звертатися до вказівника (обнуляти його); для структур з власниками пам'яті мати функцію звільнення.
- Ініціалізувати змінні при оголошенні.
- Перевіряти межі масивів і результат `malloc`.
- Запускати статичний аналіз (gcc -fanalyzer, cppcheck, flawfinder) і тести під valgrind/ASan автоматично в CI, збирати релізи з `-D_FORTIFY_SOURCE=2 -fstack-protector-strong`.
