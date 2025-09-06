#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <limits>
#include <string>
using namespace std;

const string bookDataBase = "books.csv";
const string userDataBase = "users.csv";

class Book{
	
	private:
		
		string title;
		string author;
		string isbn;
		bool availability_status;
		
	public:
		
		//Constructor - initializes an object upon calling. 
		Book (string t = "", string a = "", string i = "", bool s = true ) : title(t), author(a), isbn(i),
		availability_status(s) {}
		
		//Setter functions - assigning user value to private variables
		void setTitle(string t) {title = t;}
		void setAuthor (string a) {author = a;}
		void setISBN (string i) {isbn = i;}
		void setStatus (bool s) {availability_status = s;}
		
		//Getter functions - getting value from the private variables
		string getTitle() const {return title;}
		string getAuthor() const {return author;}
		string getISBN() const {return isbn;}
		bool getStatus() const{return availability_status;}
		
		
		//Function that returns all object properties.
		string toCSV() const{ 
			
			return title + "," + author + "," + isbn + "," + (availability_status ? "Available" : "Borrowed");
		}
		
};

class LibraryUser{
	
	private:
		
		string name;
		string id;
		vector <Book> borrowedBooklist;
		
	public:
		
		//Constructor that initializes user details.
		//Initializes with empty values. In the case of Book b, initialized with an empty vector.
		LibraryUser (string n = "", string i = "", Book b = Book()) : name(n), id(i), borrowedBooklist() {}
		
		//Setters
		void setName(string n) {name = n;}
		void setID(string i){id = i;}
		
		//Getters
		string getName() const{return name;}
		string getID() const{return id;}
		
		//Method to let user borrow a book.
		bool borrowBook(Book& book){
			
			//It checks the availability status of the book.
			if (!book.getStatus()){
				cout << "Sorry, the book is already borrowed." << endl;
				return false;
			}
			//If the book is available, it adds the book to the user's borrowed book list and updates the status.			
			borrowedBooklist.push_back(book);
			book.setStatus(false);
			
			return true;
		}
		
		//Method to display all the borrowed books of the user.
		void displayBorrowedBooks() const{
			
			if (borrowedBooklist.empty()){
				
				cout << "No book currently borrowed." << endl;
				return;
			}
			
			for (const auto& book : borrowedBooklist){
				
				cout << book.getTitle() << "  " << book.getAuthor() << "  " << book.getISBN() << endl;
			}
		}
		
		//Method to return a book to the library.
		bool returnBook(const string& title, vector <Book>& books){
			
			//This statement finds a value from the container based from the basis(predicate).
			//Checks if the user has borrowed the book.
			auto userIt = find_if(books.begin(), books.end(), [&title] (const Book& book){
				return book.getTitle() == title;
			});
			
			if(userIt == books.end()){
				cout << "You have not borrowed this book." << endl;
				return false; //User didn't borrow this book.
			} 
			
			auto libraryIt = find_if(books.begin(), books.end(), [&title] (const Book& book){
				return book.getTitle() == title;
			});
			
			if (libraryIt != books.end()){
				libraryIt->setStatus(true);//Updates the book status in the library to available.
			}
			//Removes the book from the user's borrowed book list.
			borrowedBooklist.erase(userIt);
			
			return true;
			
		}

		string returnUserDetails() const{
			string details = name + "," + id + ",";
			
			if (borrowedBooklist.empty()){
				details += "NONE";
			} else{
				for (int i = 0; i < borrowedBooklist.size(); ++i){
					
					details = details + borrowedBooklist[i].getTitle();
					
					if (i < borrowedBooklist.size() - 1){
						details += ";";
					}
					
				}
			}
			
			return details;
		}
};

class Library{
	
	private:
		
		vector <Book> books;
		vector <LibraryUser> users;
		
		//Private method that loads book data from the books database.
		void loadBooksFromFile(){
			
			ifstream bookFile(bookDataBase);
			
			if (!bookFile.is_open()){
				cerr << "Error: Unable to open file." << endl;
				return;
			}
			
			string line;
			getline(bookFile, line); //Skips the header line.
			
				//Reads each line from the file.
				while(getline(bookFile, line)){
					
					size_t pos1 = line.find(',');
					size_t pos2 = line.find(',', pos1 + 1);
					size_t pos3 = line.find(',', pos2 + 1);
					
					//Ensures that all necessary commas are present.
					if (pos1 != string::npos && pos2 != string::npos && pos3 != string::npos){
						
						
						string title = line.substr(0, pos1); 
						string author = line.substr(pos1 + 1, pos2 - pos1 - 1);
						string isbn = line.substr(pos2 + 1, pos3 - pos2 -1);
						string statusStr = line.substr(pos3 + 1);
						bool status = (statusStr == "Available");
						
						books.emplace_back(title, author, isbn, status); //Adds the book to the vector. Uses emplace_back to construct in place.
					}
				}	
				
				bookFile.close(); //Close the file after reading.
		}
		
		//Private method that loads user data from the users database.
		void loadUsersFromFile(){
			
			ifstream userFile(userDataBase);
			
			if(!userFile.is_open()){
				cerr << "Error: Unable to open file." << endl;
				return;
			} 
			
			string line;
			getline(userFile, line); //Skips the header line.
			
			//Reads each line from the file.

			while(getline(userFile, line)){
				
				size_t pos1 = line.find(",");
				if (pos1 == string::npos) continue; // Skip if no comma found
				
				size_t pos2 = line.find(",", pos1 + 1);
				if (pos2 == string::npos) continue; // Skip if second comma not found
					
					string name = line.substr(0, pos1);					
					string id = line.substr(pos1 + 1, pos2 - pos1 - 1);
					
					LibraryUser user(name, id); //Creates a user object with the name and id.
					
					string borrowedBooksStr;
					
					if (pos2 + 1 < line.length()){
						borrowedBooksStr = line.substr(pos2 + 1);
					} else {
						borrowedBooksStr = "NONE";
					}
					
					//Processes the borrowed books string if it's not "NONE" or empty.
					if (borrowedBooksStr != "NONE" && !borrowedBooksStr.empty()){
						
						size_t start = 0;
						size_t end = borrowedBooksStr.find(";");
						
						while (end != string::npos){
							
							string bookTitle = borrowedBooksStr.substr(start, end - start);
							
							// Find the book in the library's main collection
							auto bookIt = find_if(books.begin(), books.end(), [&bookTitle](Book& book){
								return book.getTitle() == bookTitle;
							});
							
							if (bookIt != books.end()){
								bookIt->setStatus(false); // Mark the book as borrowed.
								user.borrowBook(*bookIt);
							}	
							
							start = end + 1;
							end = borrowedBooksStr.find(";", start);
						}
						
						// Handle the last book title after the final semicolon.
						string lastBookTitle = borrowedBooksStr.substr(start);
						if (!lastBookTitle.empty()){
							
							auto bookIt = find_if(books.begin(), books.end(), [&lastBookTitle](Book& book){
								return book.getTitle() == lastBookTitle;
							});
							
							if (bookIt != books.end()){
								bookIt->setStatus(false); // Mark the book as borrowed.
								user.borrowBook(*bookIt);
							}
							
						}												
						
					}
					
					users.emplace_back(user);
				
			}
			
			userFile.close();
		}
	
	public:
		
		//Constructor that initializes by loading the values from the databases.
		Library(const string& booksFile, const string& usersFile){
			
			loadBooksFromFile();
			loadUsersFromFile();
		}
		
		//Method that returns the vector of books.
		vector<Book>& getBooks(){
			return books;
		}
		
		//Method that saves book data into the books database.
		void saveBookFile(){ 

			ofstream saveBook(bookDataBase); 
			
			if (!saveBook){
				
				cerr << "Error: Unable open books file." << endl;
				return;
			}
			
			saveBook << "TITLE,AUTHOR,ISBN,STATUS" << endl;
			 
			for (const auto& book : books){
				saveBook << book.toCSV() << endl;
			}
			
			saveBook.close();
			
			if (saveBook.fail()){
				cerr << "Error: Failed to save book." << endl;
			} else {
				cout << "Book successfully saved." << endl;
			}
		
		}
		
		//Method that saves user data into the users database.
		void saveUserFile(){
			
			
			ofstream saveUser(userDataBase);
			
			if (!saveUser){
				
				cerr << "Error: Unable to open users file." << endl;
				return;
			}
			
			saveUser << "NAME,ID,BORROWED_BOOKS" << endl;
			
			for (const auto& user : users){
				saveUser << user.returnUserDetails() << endl;
			}
			
			saveUser.close();
		}
		
		//Method to add books.
		void addBook(const string& title, const string& author, const string& ISBN){
			
			Book myBook(title, author, ISBN); //Constructor that initializes the properties of an object.
			books.emplace_back(myBook); //the constructed object is then added to the vector.
			saveBookFile();
		}
		
		//Method to remove book by title.
		void removeBook(const string& title){
			
			if (books.empty()){
				cout << "Cannot remove book: Library is empty." << endl;
				return;
			}
			
			//This statement finds a value from the container based from the basis(predicate).
			auto it = find_if(books.begin(), books.end(), [&title] (const Book& book){
				return book.getTitle() == title;
			});
			
			//Removes the book once it's found.
			if (it != books.end()){
				
				books.erase(it);
				saveBookFile();
				cout << "Book successfully removed." << endl;
			} else{
				 cout << "Book not found." << endl;
			}
		}
		
		//Method to add user.
		void addUser(const string& name, const string& id){
			
			LibraryUser user(name, id);
			users.emplace_back(user);
			saveUserFile();
		}
		
		//Method to remove user by id.
		void removeUser(const string& id){
			
			auto it = find_if(users.begin(), users.end(), [&id] (const LibraryUser& user){
				return user.getID() == id;
			});
			
			if (it != users.end()){
				
				users.erase(it);
				saveUserFile();
				cout << "User removed successfully." << endl;
			} else{
				cout << "User not found." << endl;
			}
		}
		
		//Method to display all books in the library.
		void displayBooks() const{
			
			if (books.empty()){
				cout << "No books in the library." << endl;
				return;
			}
			
			for (const auto& book : books){
				cout << book.toCSV() << endl;
			}
		}

		//Method to display only available books.
		void displayAvailableBooks() const{
			
			if (books.empty()){
				cout << "No books in the library." << endl;
				return;
			}

			bool found = false;
			
			for (const auto& book : books){
				if (book.getStatus()){
					cout << book.toCSV() << endl;
					found = true;
				}
			}

			if(!found){
				cout << "No available books at the moment." << endl;
			}
		}
		
		//Method to display all users.
		void displayUsers() const{
			
			if (users.empty()){
				cout << "No registered users." << endl;
				return;
			}
			
			for (const auto& user : users){
				cout << user.returnUserDetails() << endl;
			}
		}
		
		//Method to check if user exists by id. Utilized in login function.
		bool checkUser(const string& userId) const{
			
			for (const auto& user : users){
				
				if (user.getID() == userId){
					return true;
				}
			}
			
			return false;
		}
		
		//Method to get user details by id. Utilized in login function.
		LibraryUser* getUserById(const string& userId) {
			for (auto& user : users) {
				if (user.getID() == userId) {
					return &user;
				}
			}
			return nullptr;
		}

					
};

class UI{
	
	private:
		
		Library library;
		LibraryUser* currentUser;
		
		//Private method that clears the console screen.
		void clearScreen(){
			cout << "\033[2J\033[1;1H"; // ANSI escape codes
		}
		
		//Private method that waits for user to press enter.
		void pressEnterToContinue(){
			cout << "\nPress Enter to continue...";
        	cin.ignore(numeric_limits<streamsize>::max(), '\n');
        	cin.get();
		}
		
	public:
		
		//Constructor that initializes the library with the book and user databases also sets current user to null.
		UI() : library(bookDataBase, userDataBase), currentUser(nullptr) {}
		
		//Method to display the login screen.
		void showLoginScreen(){
			
			clearScreen();
			cout << "====================================\n";
			cout << "     LIBRARY MANAGEMENT SYSTEM\n";
			cout << "====================================\n";
			
			cout << "\n----------LOGIN AS----------\n";
			cout << "1. User\n";
			cout << "2. Librarian\n";
			cout << "3. Exit\n";
		}
	
		//Method to display the user dashboard.
		void showUserScreen(){
			
			clearScreen();
			
			cout << "====================================\n";
			cout << "         USER DASHBOARD\n";
			cout << "====================================\n";
			
			cout << "\n----------WELCOME, USER----------\n";
			cout << "1. Borrow book\n";
			cout << "2. Return book\n";
			cout << "3. Display all books borrowed\n";
			cout << "4. Show available books\n";
			cout << "5. Logout\n";
		}
	
		//Method to display the librarian dashboard.
		void showLibrarianScreen(){
			
			clearScreen();
			
			cout << "====================================\n";
			cout << "         LIBRARIAN DASHBOARD\n";
			cout << "====================================\n";
			
			cout << "\n----------WELCOME, LIBRARIAN----------\n";
			cout << "1. Add book\n";
			cout << "2. Remove book\n";
			cout << "3. Add user\n";
			cout << "4. Remove user\n";
			cout << "5. Display all books\n";
			cout << "6. Display all users\n";
			cout << "7. Logout\n";
		}
		
		//Method to display messages and wait for user to press enter.
		void showMessage(const string& message) {
        cout << "\n>> " << message << "\n";
        pressEnterToContinue();
    }
    
		//Method to get validated number input within a specified range.
		int getNumberInput(const string& prompt, int min, int max){
			
			int choice;
			
			while(true){
				
				cout << prompt;
				cin >> choice;
				
				if (cin.fail() || choice < min || choice > max){
					
				cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between " 
                     << min << " and " << max << endl;
				} else{
					
					cin.ignore();
					return choice;
				}
			}
		}

		//Method to get text input from user.
		string getTextInput(const string& prompt){
			
			cout << prompt;
			string input;
			getline(cin, input);
			return input;
		}

		//Method to handle librarian login.
		bool loginAsLibrarian(){
			
			clearScreen();
			
			cout << "=========================\n";
			cout << "     LIBRARIAN LOGIN\n";
			cout << "=========================\n";
			
			string librarianUserName = getTextInput("Enter your username: ");
			string librarianPassword = getTextInput("Enter your password: ");
			
				if (librarianUserName == "librarian" && librarianPassword == "library123"){
					showMessage("Login successful. Welcome librarian!");
					return true;
				} else{
					showMessage("Incorrect credentials: Unable to login");
					return false;
				}
		}

		//Method to handle user login.
		bool loginAsUser(){
			
			clearScreen();
			
			cout << "=========================\n";
			cout << "        USER LOGIN\n";
			cout << "=========================\n";
			
			string userId = getTextInput("Enter your id: ");
			
			if (library.checkUser(userId)){
				currentUser = library.getUserById(userId);
				showMessage("Login successful: Welcome " + currentUser->getName() + "!");
				return true;
			} else{
				showMessage("User ID is not found. Please try again or contact the librarian for assistance.");
				return false;
			}
		}

		//Method to handle user login attempts and dashboard access.
		void handleUserLogin() {
			while (true) {
				if (loginAsUser()) {
					userDashboard();
					break;
				}
				
				int choice = getNumberInput("\n1. Try again\n2. Exit program\nChoose option: ", 1, 2);
				if (choice == 2) {
					break;
				}
			}
		}			

		//Method to handle librarian login attempts and dashboard access.
		void handleLibrarianLogin(){
			while (true){
				if (loginAsLibrarian()){
					librarianDashboard();
					break;
				}
				
				int choice = getNumberInput("\n1. Try again\n2. Exit program\nChoose option: ", 1, 2);
				if (choice == 2) {
					break;
				}
			}
		}
		
		//User dashboard method.
		void userDashboard(){
			
			bool running = true;
			
			while (running){
				
				showUserScreen();
				int option = getNumberInput("Choose an option: ", 1, 5);
				
				switch (option){
					
					case 1:{
						
						string titleToBorrow = getTextInput("Enter the title of the book to borrow: ");
						
						//Finds the book in the library.
						auto bookIt = find_if(library.getBooks().begin(), library.getBooks().end(), [&titleToBorrow]
						(const Book& book) {
							return book.getTitle() == titleToBorrow;
						});
						
						if (bookIt != library.getBooks().end()){
							
							if (currentUser->borrowBook(*bookIt)){
								showMessage("Book has been borrowed successfully!");
								library.saveBookFile();
								library.saveUserFile();
							} else{
								showMessage("Book is not available for borrowing."); //prints if book is already borrowed or doesn't exist.
							} 
						}
						
						break;
					}
					
					case 2: {
						
						if (currentUser){
							
							string titleToReturn = getTextInput("Enter the title to return: ");
							bool success = currentUser->returnBook(titleToReturn, library.getBooks());
							
							if(success){
								showMessage("Book has been returned successfully!");
								library.saveBookFile();
								library.saveUserFile();
							} else {
								showMessage("Failed to return book.");
							}
						}
						
						break;
					}	
					
					case 3: {
						
						if (currentUser){
							
							clearScreen();
							
							cout << "-------- BORROWED BOOKS --------\n";
							currentUser->displayBorrowedBooks();
							pressEnterToContinue();
							
						}
						break;
					}
					
					case 4: {
						
						cout << "-------- AVAILABLE BOOKS --------\n";
						
						library.displayAvailableBooks();
						pressEnterToContinue();
						break;
					}	
					
					case 5: {
						
						currentUser = nullptr;
						running = false;
						showMessage("Logged out successfully!");
						break;
					}
				}
			}
		}
		
		//Librarian dashboard method.
		void librarianDashboard(){
			
			bool running = true;
			while(running){
				
				showLibrarianScreen();
				int option = getNumberInput("Choose an option: ", 1, 7);
				
				switch(option){
					
					case 1: {
						
						string title = getTextInput("Enter the title: ");
						string author = getTextInput("Enter the name of the author: ");
						string isbn = getTextInput("Enter ISBN number: ");
						
						library.addBook(title, author, isbn);
						showMessage("Book added successfully!");
						
						break;
					}
					
					case 2: {
						
						string titleToRemove = getTextInput("Enter the title of the book to be removed: ");
						library.removeBook(titleToRemove);
						showMessage("Book has been removed in the library.");
						break;
					}
					
					case 3: {
						
						string userName = getTextInput("Enter name: ");
						string userId = getTextInput("Enter id: ");
						
						library.addUser(userName, userId);
						showMessage("User is now registered!");
						break;
					}
					
					case 4: {
						
						string userIdToRemove = getTextInput("Enter user ID to remove: ");						
						library.removeUser(userIdToRemove);
						break;
					}
					
					case 5: {
						
						clearScreen();
						
						cout << "-------- ALL BOOKS --------\n";
						library.displayBooks();
						pressEnterToContinue();
						break;
					}
					
					case 6: {
						
						clearScreen();
						
						cout << "-------- ALL USERS --------\n";
						library.displayUsers();
						pressEnterToContinue();
						break;
					}
					
					case 7: {
						
						running = false;
						showMessage("Logged out successfully!");
						break;
					}
				}
			}
		}
};

int main()
{
	
	UI ui;
	
	ui.showLoginScreen();
	int option = ui.getNumberInput("Choose an option: ", 1, 3);
	
	switch(option){
		
		case 1:
			ui.handleUserLogin();
			break;
			
		case 2:
			ui.handleLibrarianLogin();
			break;
		
		
	}

	return 0;
}