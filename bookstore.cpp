#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <map>
#include <fstream>

using namespace std;

// Book structure
struct Book {
    string title;
    string author;
    string code;
    string genre;
    string review;
    double rating;
    string summary;
    string status; // Available or Rented
    double price;

    Book(string t, string a, string c, string g, string s, double p)
        : title(t), author(a), code(c), genre(g), review(""), rating(0), summary(s), status("Available"), price(p) {}
};

// User structure
struct User {
    string username;
    string password;
    vector<Book> cart;
    vector<Book> wishlist;
    vector<Book> rentedBooks; // For tracking rented books
    double totalSpent;

    User() : username(""), password(""), totalSpent(0) {}
    User(string u, string p) : username(u), password(p), totalSpent(0) {}
};

class Bookstore {
private:
    vector<Book> books;
    map<string, User> users;
    User* loggedInUser;

public:
    Bookstore() : loggedInUser(nullptr) {
        books.emplace_back("The Great Gatsby", "F. Scott Fitzgerald", "BK101", "Fiction", "A classic novel set in the 1920s.", 500.00);
        books.emplace_back("1984", "George Orwell", "BK102", "Dystopian", "A story about totalitarianism and surveillance.", 400.00);
        books.emplace_back("To Kill a Mockingbird", "Harper Lee", "BK103", "Fiction", "A novel about racial injustice.", 300.00);
        books.emplace_back("The Catcher in the Rye", "J.D. Salinger", "BK104", "Fiction", "A story about teenage alienation.", 350.00);
        books.emplace_back("Moby Dick", "Herman Melville", "BK105", "Adventure", "A novel about the obsession with a white whale.", 450.00);
    }

    void clearConsole() {
        #ifdef _WIN32
            system("CLS");
        #else
            system("clear");
        #endif
    }

    void delay(int seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds)); // Corrected usage
    }

    void displayWelcomeMessage() {
        clearConsole();
        cout << endl << endl;

        string welcome1 = "WELCOME TO THE";
        string welcome2 = "BOOKSTORE SYSTEM";

        int width = 50;
        cout << string((width - welcome1.length()) / 2, ' ') << welcome1 << endl;
        cout << string((width - welcome2.length()) / 2, ' ') << welcome2 << endl;
        cout << string(width, '-') << endl;
        cout << "Your one-stop shop for all your reading needs!" << endl;
        cout << string(width, '-') << endl;
        delay(2);
    }

    void registerUser() {
        string username, password;
        cout << endl << "Enter a username: ";
        cin >> username;
        cout << "Enter a password: ";
        cin >> password;

        if (users.find(username) != users.end()) {
            cout << endl << "Username already exists! Please try again.\n";
            return;
        }

        users[username] = User(username, password);
        cout << endl << "Registration successful! Redirecting to login...\n";
        delay(2);
    }

    bool loginUser() {
        string username, password;
        cout << endl << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        auto it = users.find(username);
        if (it != users.end() && it->second.password == password) {
            loggedInUser = &it->second;
            cout << "Login successful! Welcome, " << loggedInUser->username << "!\n";
            return true;
        } else {
            cout << "Invalid credentials. Please try again.\n";
            return false;
        }
    }

    void displayBooks() {
        cout << "\nAvailable Books:\n";
        cout << left << setw(30) << "Title" << setw(20) << "Author" << setw(10) << "Code" 
             << setw(15) << "Genre" << setw(10) << "Price" << setw(10) << "Status" << endl;
        cout << string(100, '-') << endl;

        for (const auto& book : books) {
            cout << left << setw(30) << book.title << setw(20) << book.author << setw(10) << book.code 
                 << setw(15) << book.genre << setw(10) << fixed << setprecision(2) << book.price << setw(10) << book.status << endl;
        }
        cout << endl;
    }

    void addToCart() {
        string code;
        cout << "\nEnter the code of the book you want to add to the cart: ";
        cin >> code;

        for (const auto& book : books) {
            if (book.code == code && book.status == "Available") {
                loggedInUser->cart.push_back(book);
                cout << "Book added to cart!\n";
                return;
            }
        }
        cout << "Book not found or unavailable.\n";
    }

    void viewCart() {
        clearConsole();
        cout << "\nYour Cart:\n";
        cout << left << setw(30) << "Title" << setw(20) << "Author" << setw(10) << "Code" 
             << setw(15) << "Genre" << setw(10) << "Price" << endl;
        cout << string(100, '-') << endl;

        double totalPrice = 0;
        for (const auto& book : loggedInUser->cart) {
            cout << left << setw(30) << book.title << setw(20) << book.author << setw(10) << book.code 
                 << setw(15) << book.genre << setw(10) << fixed << setprecision(2) << book.price << endl;
            totalPrice += book.price;
        }
        cout << "Total Price: " << fixed << setprecision(2) << totalPrice << " Rs\n";
        cout << endl;
    }

    void rentBook() {
        string code;
        cout << "\nEnter the code of the book you want to rent: ";
        cin >> code;

        for (auto& book : books) {
            if (book.code == code && book.status == "Available") {
                book.status = "Rented";
                loggedInUser->rentedBooks.push_back(book);
                loggedInUser->totalSpent += book.price;
                saveRentedBook(); // Save rented book to file
                cout << "Book rented successfully!\n";
                return;
            }
        }
        cout << endl << "Book not found or unavailable.\n";
    }

    void returnBook() {
        string code;
        cout << "\nEnter the code of the book you want to return: ";
        cin >> code;

        for (auto& book : books) {
            if (book.code == code && book.status == "Rented") {
                book.status = "Available";
                cout << "Book returned successfully!\n";
                return;
            }
        }
        cout << endl << "Book not found or was not rented.\n";
    }

    void addReviewAndRating() {
        string code;
        cout << "\nEnter the code of the book you want to review: ";
        cin >> code;

        for (auto& book : books) {
            if (book.code == code) {
                cout << "Enter your review: ";
                cin.ignore(); // clear newline
                getline(cin, book.review);
                cout << "Enter your rating (0-5): ";
                cin >> book.rating;
                saveReview(book); // Save review to file
                cout << "Review and rating added successfully!\n";
                return;
            }
        }
        cout << endl << "Book not found.\n";
    }

    void viewRentedBooks() {
        clearConsole();
        cout << "\nRented Books:\n";
        cout << left << setw(30) << "Title" << setw(20) << "Author" << setw(10) << "Code" 
             << setw(15) << "Genre" << setw(10) << "Price" << endl;
        cout << string(100, '-') << endl;

        for (const auto& book : loggedInUser->rentedBooks) {
            cout << left << setw(30) << book.title << setw(20) << book.author << setw(10) << book.code 
                 << setw(15) << book.genre << setw(10) << fixed << setprecision(2) << book.price << endl;
        }
        cout << endl;
    }

    void viewOrderHistory() {
        clearConsole();
        cout << "\nOrder History:\n";
        cout << left << setw(20) << "Username" << setw(10) << "Book Code" 
             << setw(30) << "Title" << setw(20) << "Author" 
             << setw(10) << "Price" << endl;
        cout << string(100, '-') << endl;

        ifstream inFile("order_history.txt");
        string line;
        while (getline(inFile, line)) {
            cout << line << endl;
        }
        inFile.close();
        cout << endl;
    }

    void payment() {
        cout << "\nProceeding to payment...\n";
        delay(1);
        double totalPrice = 0;

        for (const auto& book : loggedInUser->cart) {
            totalPrice += book.price;
        }

        cout << "Total amount: " << fixed << setprecision(2) << totalPrice << " Rs\n";
        cout << "Payment successful! Thank you for your purchase!\n";

        // Save order history to order_history.txt
        saveOrderHistory(totalPrice);

        loggedInUser->cart.clear(); // Clear the cart after payment
    }

    void saveRentedBook() {
        ofstream outFile("rented_books.txt", ios::app); // Open file in append mode
        if (outFile.is_open()) {
            for (const auto& book : loggedInUser->rentedBooks) {
                outFile << "User Name: " << loggedInUser->username << endl;
                outFile << "Book Code: " << book.code << endl;
                outFile << endl; // Space between different users
            }
            outFile.close();
        } else {
            cout << "Unable to open file for saving rented books.\n";
        }
    }

    void saveReview(const Book& book) {
        ofstream outFile("book_review.txt", ios::app); // Open file in append mode
        if (outFile.is_open()) {
            outFile << "User Name: " << loggedInUser->username << endl;
            outFile << "Book Code: " << book.code << endl;
            outFile << "Review: " << book.review << endl;
            outFile << "Rating: " << book.rating << endl;
            outFile << endl; // Space between different users
            outFile.close();
        } else {
            cout << "Unable to open file for saving book reviews.\n";
        }
    }

    void saveOrderHistory(double totalPrice) {
        ofstream outFile("order_history.txt", ios::app); // Open file in append mode
        if (outFile.is_open()) {
            for (const auto& book : loggedInUser->cart) {
                outFile << "User Name: " << loggedInUser->username << endl;
                outFile << "Book Code: " << book.code << endl;
                outFile << "Price: " << fixed << setprecision(2) << book.price << " Rs" << endl;
                outFile << "Title: " << book.title << endl; // Save title
                outFile << "Author: " << book.author << endl; // Save author
                outFile << endl; // Space between different users
            }
            outFile.close();
        } else {
            cout << "Unable to open file for saving order history.\n";
        }
    }

    void customerCare() {
        cout << "\nCustomer Care:\n";
        cout << "For any inquiries, please enter your query below:\n";
        cout << "Type 'exit' to finish.\n";

        string query;
        cin.ignore(); // Clear newline
        getline(cin, query);
        
        // Log the query to customer_care.txt
        ofstream outFile("customer_care.txt", ios::app); // Open file in append mode
        if (outFile.is_open()) {
            outFile << "User Name: " << loggedInUser->username << endl;
            outFile << "Query: " << query << endl;
            outFile << endl; // Space between different users
            outFile.close();
            cout << "Your query has been recorded. Thank you for contacting customer care!\n";
        } else {
            cout << "Unable to open file for saving customer care queries.\n";
        }

        cout << "For any further inquiries, please contact us at support@bookstore.com\n";
        cout << "You can also call us at +1234567890\n";
        cout << "Our support team is available 24/7.\n";
        pressEnterToContinue();
    }

    void logout() {
        cout << "Logging out...\n";
        delay(1);
        loggedInUser = nullptr; // Reset logged-in user
        clearConsole(); // Clear the console after logging out
    }

    User* getLoggedInUser() {
        return loggedInUser;
    }

    void pressEnterToContinue() {
        cout << "\nPress Enter to continue...";
        cin.ignore(); // clear newline from previous input
        cin.get(); // Wait for user input
    }
};

int main() {
    Bookstore bookstore;
    int choice;

    bookstore.displayWelcomeMessage(); // Display the welcome message at the start

    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << endl << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                bookstore.registerUser();
                bookstore.pressEnterToContinue();
                break;
            case 2:
                if (bookstore.loginUser()) {
                    while (true) {
                        bookstore.clearConsole();
                        cout << "\nUser Menu:\n";
                        cout << "1. Display Books\n";
                        cout << "2. Add to Cart\n";
                        cout << "3. View Cart\n";
                        cout << "4. Rent a Book\n";
                        cout << "5. Return a Book\n";
                        cout << "6. Add Review and Rating\n";
                        cout << "7. View Rented Books\n"; // New option
                        cout << "8. View Order History\n";
                        cout << "9. Payment\n";
                        cout << "10. Customer Care\n"; // Added Customer Care
                        cout << "11. Logout\n";
                        cout << endl << "Enter your choice: ";
                        cin >> choice;

                        switch (choice) {
                            case 1:
                                bookstore.displayBooks();
                                bookstore.pressEnterToContinue();
                                break;
                            case 2:
                                bookstore.addToCart();
                                bookstore.pressEnterToContinue();
                                break;
                            case 3:
                                bookstore.viewCart();
                                bookstore.pressEnterToContinue();
                                break;
                            case 4:
                                bookstore.rentBook();
                                bookstore.pressEnterToContinue();
                                break;
                            case 5:
                                bookstore.returnBook();
                                bookstore.pressEnterToContinue();
                                break;
                            case 6:
                                bookstore.addReviewAndRating();
                                bookstore.pressEnterToContinue();
                                break;
                            case 7:
                                bookstore.viewRentedBooks(); // Call the new method
                                bookstore.pressEnterToContinue();
                                break;
                            case 8:
                                bookstore.viewOrderHistory();
                                bookstore.pressEnterToContinue();
                                break;
                            case 9:
                                bookstore.payment();
                                bookstore.pressEnterToContinue();
                                break;
                            case 10:
                                bookstore.customerCare(); // Call customer care method
                                break;
                            case 11:
                                bookstore.logout(); // Call logout method
                                break; // Return to the user menu
                            default:
                                cout << "Invalid choice. Please try again.\n";
                                bookstore.pressEnterToContinue();
                                break;
                        }

                        // If the user logs out, break out of the user menu loop
                        if (bookstore.getLoggedInUser() == nullptr) {
                            break;
                        }
                    }
                }
                break;
            case 3:
                cout << "Exiting the program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
                bookstore.pressEnterToContinue();
                break;
        }
    }

    return 0;
}