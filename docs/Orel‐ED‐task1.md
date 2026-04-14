## Problem Statement (from Lab #3, Option 16 "Book")

**Option 16. "Book"**

**Exercise 2.3**
Table "Book": 
`id`, `title`, `author` (last name; first name), `year of publication`, `publisher`, `cost price`, `price`, `profit`.

**Exercise 2.4** 
Execute the following queries:
- Display data on books by authors whose last name begins with the letter "K".
- Using the 'alter' statement, add additional columns, one of which is 'type_id' (an integer type and contains book genre category IDs).
- Create a 'category' table ('id', 'cat_name', 'cat_description').
- Display data on all books in the following format: book ID, title, publication year, and book category name.
- Calculate the number of products using 'count' if the price is 28.32 rubles.
- Total book profit using 'sum' if the publication year is > 2017.
- Maximum and minimum book prices using 'max' and 'min'.
- Use the 'inner join' statement to display full information about books and genres for the genre = "Biographies and Memoirs".

---

## Use Case Diagram

<img width="581" height="551" alt="UseCaseDiagram drawio" src="https://github.com/user-attachments/assets/4098e548-136c-4809-a107-ffb5d69ed63b" />

*Description:*
Actors: **Administrator** (manages the book catalog and genres, views statistics) and **User** (views books, searches by author).

Use cases: Login, View book list, Search by author, Add book, Edit book, Delete book, View statistics, Manage genres.

### Text script for the "Search books by author" option
1. The user logs in (authentication).
2. The system determines the "User" role.
3. The user selects the "Search by author" menu item.
4. The system prompts for the initial letters of the author's last name.
5. The user enters "O" (or any wildcard).
6. The system executes the SQL query `SELECT * FROM books WHERE author LIKE 'O%'`.
7. The system displays a list of books (title, author, year, price, genre).
8. The script ends.

---

## Activity Diagrams

### Authentication ("Log in")

<img width="551" height="229" alt="LogIn drawio" src="https://github.com/user-attachments/assets/c34560b2-8ea7-444d-b73f-22bffba2eee1" />

*Description:*
The user enters their login and password → the system checks them in the database → if correct, the main menu opens with the appropriate rights; if incorrect, an error message is displayed and the user is prompted to reenter.

### Operations on the main entity ("CRUD book")

<img width="791" height="271" alt="CRUD drawio" src="https://github.com/user-attachments/assets/0b234257-2786-45f2-a9b5-4181189437b6" />

*Description:*
The administrator selects an action (create, read, update, delete). Each action has its own sequence of steps. For example, for creation – data entry, verification, adding to the database; for deletion – confirmation before deletion.

---

## Class Diagrams

### Authentication

<img width="451" height="164" alt="Authentication drawio" src="https://github.com/user-attachments/assets/29c60cc4-a40f-461a-9479-7042b48b4094" />

*Description:*
The `User` class contains the attributes: `id`, `username`, `password_hash`, `role`.
The `AuthManager` class provides the `authenticate(username, password)` method, which returns a `User` object or `null`. Relationship – usage.

### Main entity (book and genre)

<img width="409" height="346" alt="ClassBook drawio" src="https://github.com/user-attachments/assets/f6746519-5ca9-4ca4-95c4-bae32bfbba55" />

*Description:*
The `Book` class describes a book: `id`, `title`, `author`, `year`, `publisher`, `cost`, `price`, `profit`.
The `Genre` class describes a genre: `id`, `name`, `description`.
Relationship – association: a book has a reference to a genre (`genre_id`).
The `BookManager` class encapsulates business logic (methods `addBook()`, `updateBook()`, `deleteBook()`, `findByAuthor()`, `getStatistics()`).

---

## Sequence Diagrams

### Authentication

<img width="421" height="301" alt="SequenceAuth drawio" src="https://github.com/user-attachments/assets/be2550e0-064e-4986-82e8-3cac8afe6e13" />

*Description:*
Interaction: User → `AuthController` → `Database` → return result. `AuthController` creates a `User` object upon success.

### Operation on the main entity (adding a book)

<img width="421" height="331" alt="SequenceAdd drawio" src="https://github.com/user-attachments/assets/c40d0f3a-ad22-4e7b-a736-d832ef7bd36e" />

*Description:*
Administrator fills in data → `BookController` → validation → `Database` → insert record → response.

---

## Component Diagram

<img width="681" height="381" alt="Component drawio" src="https://github.com/user-attachments/assets/0364d6ba-178d-47bb-b40d-b792794e2307" />

*Description:* 
Project structure: 
- Source files (`src/`) and header files (`includes/`). 
- Object files and the executable (`build/`, `bin/`). 
- External libraries (`libsqlite3`). 
- Database (`books.db`). 
- Documentation (`README.md`, `wiki`). 
- Build files (`CMakeLists.txt` or `Makefile`). 
- CI/CD configuration (`.github/workflows/ci.yml`). 

Components are linked by dependencies; for example, the executable depends on object files and the SQLite library.