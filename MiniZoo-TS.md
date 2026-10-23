# Міні-зоопарк із захищеним зберіганням даних

## Використати

- C++11 - C++17
- MSVC (Visual Studio)
- SQLite (не принципово, але щоб можна було перевірити без розробника, MSSQL можна також)
- будь-яку криптобібліотеку (OpenSSL, Crypto++, Windows CNG…)
- С++ Poco дозволено

## SQL

При старті програма повинна створити БД з такими таблицями:

Приклад

```sql
CREATE TABLE Enclosures (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE Animals (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    species TEXT NOT NULL,
    age INTEGER NOT NULL,
    enclosure_id INTEGER,
    FOREIGN KEY(enclosure_id) REFERENCES Enclosures(id)
);
```

**Enclosures:**

1. Savanna
2. Aquarium
3. Birds

**Animals:**

- Lion
- Tiger
- Shark
- Parrot

Вивести список тварин разом із назвою вольєра. (не обов’язково в С++ але буде бонусом)

```text
Lion   -> Savanna
Tiger  -> Savanna
Shark  -> Aquarium
Parrot -> Birds
```

Перемістити тварину:

```text
Parrot -> Savanna
```

## Частина AES

```cpp
std::vector<uint8_t> Encrypt(
    const std::string& text,
    const std::vector<uint8_t>& key
);

std::string Decrypt(
    const std::vector<uint8_t>& cipher,
    const std::vector<uint8_t>& key
);
```

```sql
CREATE TABLE AnimalNotes
(
    animal_id INTEGER,
    encrypted_note BLOB
);
```

- зашифрувати нотатку AES
- зберегти в БД
- прочитати з БД
- розшифрувати
- вивести на екран

Результат: **не** очікується повноцінний проект з конфігурацією чи параметрами запуску, деякі моенти хардкодом, але щоб CodeReview було не складним забезпечити читабельність коментарями і іменнуваннями.
