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
