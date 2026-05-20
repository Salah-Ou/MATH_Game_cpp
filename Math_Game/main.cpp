#include "utils.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <limits>
#include <thread>

using namespace std;

string name;
void Adding();
void Subtracting();
void multiply();
void division();

/* Must be called after every cin >> that precedes getInputWithTimeout.
    Flushes the leftover '\n' so getline inside getInputWithTimeout works correctly.
*/
void flushCin() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ── Levels ────────────────────────────────────────────────────────────────────
int user_answer = -1 ;

void Adding() {
    Adding : 
    clearScreen();
    int score = 0;
    gotoxy(26,4); cout << GREEN << "---------------------------------------------------------------------------";

    for (int i = 0; i < 5; i++) {
        int number1 = rand() % 50 + 1;
        int number2 = rand() % 50 + 1;
        cout << WHITE << "\n\n\t\t\t\t\t Question " << i + 1 << " :: " << number1 << " + " << number2 << " = " << flush ;

        bool answered = getInputWithTimeout(user_answer, 15,"LEVEL 1 : Adding Quiz");

        if (!answered) {
            cout << RED << "\n\n\t\t\t\t\t\t\t==) Time's up! Correct answer : " << number1 + number2 ;
        } else if (user_answer == number1 + number2) {
            cout << GREEN << "\n\t\t\t\t\t\t\t==) Correct Answer!";
            score++;
        } else {
            cout << RED << "\n\t\t\t\t\t\t\t==) Wrong! Correct answer : " << number1 + number2 ;
        }
    }

    gotoxy(48,26); cout << WHITE << "Your Score : "
         << (score < 5 ? RED : GREEN) << score << "/5 ( " << (score * 100) / 5 << "% )" ;

    if (score == 5) {
        gotoxy(40,28); cout << WHITE << "Continue to Level 2? (1 = yes / 0 = no) : ";
        int go; cin >> go; flushCin();
        if (go == 1) Subtracting();
    } else {
        gotoxy(47,28); cout << RED << "Sorry!,you failed Level 1." ;
        gotoxy(42,30); cout << WHITE << "Play again ? (1 = yes / 0 = no) : ";
        int go; cin >> go; flushCin();
        if (go == 1) goto Adding ;
    }
    gotoxy(36,32); cout << "Thanks for playing! Press any key to continue..."; Pause();
}

void Subtracting() {
    Subtracting :
    clearScreen();
    int score = 0;
    gotoxy(29,4); cout << GREEN << "---------------------------------------------------------------------------";
    for (int i = 0; i < 5; i++) {
        int number1 = rand() % 50 + 1;
        int number2 = rand() % 50 + 1;
        cout << WHITE << "\n\n\t\t\t\t\t\t Question " << i + 1 << " :: " << number1 << " - " << number2 << " = " << flush;

        bool answered = getInputWithTimeout(user_answer, 20,"LEVEL 2 : Subtracting Quiz");

        if (!answered) {
            cout << RED << "\n\t\t\t\t\t\t\t\t==) Time's up! Correct answer : " << number1 - number2 ;
        } else if (user_answer == number1 - number2) {
            cout << GREEN << "\n\t\t\t\t\t\t\t\t==) Correct Answer!";
            score++;
        } else {
            cout << RED << "\n\t\t\t\t\t\t\t\t==) Wrong! Correct answer : " << number1 - number2 ;
        }
    }

    gotoxy(55,26); cout << WHITE << "Your Score : "
         << (score < 5 ? RED : GREEN) << score << "/5 ( " << (score * 100) / 5 << "% )" ;

    if (score == 5) {
        gotoxy(47,28); cout << WHITE << "Continue to Level 3? (1=yes / 0=no): ";
        int go; cin >> go; flushCin();
        if (go == 1) multiply();
    } else {
        gotoxy(54,28); cout << RED << " Sorry,you failed Level 2." ;
        gotoxy(50,30); cout << WHITE << "Play again? (1 = yes / 0 = no): ";
        int go; cin >> go; flushCin();
        if (go == 1) goto Subtracting ;
    }
    gotoxy(43,32); cout << "Thanks for playing! Press any key to continue..."; Pause();
}

void multiply() {
    multiply : 
    clearScreen();
    int score = 0;
    gotoxy(30,4); cout << GREEN << "----------------------------------------------------------------------------";

    for (int i = 0; i < 5; i++) {
        int number1 = rand() % 40 + 1;
        int number2 = rand() % 40 + 1;
        cout << WHITE << "\n\n\t\t\t\t\t\t Question " << i + 1 << " :: " << number1 << " * " << number2 << " = " << flush;

        bool answered = getInputWithTimeout(user_answer, 25,"LEVEL 3 : Multiplication Quiz");

        if (!answered) {
            cout << RED << "\n\t\t\t\t\t\t\t\t==) Time's up! Correct answer : " << number1 * number2 ;
        } else if (user_answer == number1 * number2) {
            cout << GREEN << "\n\t\t\t\t\t\t\t\t==) Correct Answer!";
            score++;
        } else {
            cout << RED << "\n\t\t\t\t\t\t\t\t==) Wrong! Correct answer : " << number1 * number2 ;
        }
    }

    gotoxy(55,26); cout << WHITE << "Your Score : "
         << (score < 5 ? RED : GREEN) << score << "/5 ( " << (score * 100) / 5 << "% )" ;

    if (score == 5) {
        gotoxy(49,28); cout << WHITE << "Continue to Level 4? (1 = yes / 0 = no): ";
        int go; cin >> go; flushCin();
        if (go == 1) division();
    } else {
        gotoxy(54,28); cout << RED << "Sorry,you failed Level 3." ;
        gotoxy(52,30); cout << WHITE << "Play again? (1=yes / 0=no) : ";
        int go; cin >> go; flushCin();
        if (go == 1) goto multiply ;
    }
    gotoxy(44,32); cout << "Thanks for playing! Press any key to continue..."; Pause();
}

void division() {
    division : 
    clearScreen();
    int score = 0;
    gotoxy(29,4); cout << GREEN << "--------------------------------------------------------------------------";
    gotoxy(24,6); cout << "\033[1;4;31mNote :" << DEFAULT << GRAY << "(All division questions will have integer answers" 
                        << RED << " & " << GRAY << "number1 = number2 * multiple)" ;

    for (int i = 0; i < 5; i++) {
        int number2 = rand() % 15 + 1;          
        int number1 = number2 * (rand() % 15 + 1); 
        if(number2 != 0){
            cout << WHITE << "\n\n\t\t\t\t\t\t Question " << i + 1 << " :: " << number1 << " / " << number2 << " = " << flush;
    
            bool answered = getInputWithTimeout(user_answer, 30,"LEVEL 4 : Division Quiz");
    
            if (!answered) {
                cout << RED << "\n\t\t\t\t\t\t\t\t==) Time's up! Correct answer : " << number1 / number2 ;
            } else if (user_answer == number1 / number2) {
                cout << GREEN << "\n\t\t\t\t\t\t\t\t==) Correct Answer!";
                score++;
            } else {
                cout << RED << "\n\t\t\t\t\t\t\t\t==) Wrong! Correct answer : " << number1 / number2 ;
            }
        }  
    }

    gotoxy(55,28); cout << WHITE << "Your Score : "
         << (score < 5 ? RED : GREEN) << score << "/5 ( " << (score * 100) / 5 << "% )" ;

    if (score == 5) {
        gotoxy(43,30); cout << "🎉 Congratulations " << name << "! You passed all 4 levels!" << WHITE ;
    } else {
        gotoxy(54,30); cout << RED << "Sorry,you failed Level 4." ;
        gotoxy(53,32); cout << WHITE << "Play again? (1 = yes / 0 = no): ";
        int go; cin >> go; flushCin();
        if (go == 1) goto division ;
    }
    gotoxy(46,32); cout << "Thanks for playing! Press any key to Exit..."; Pause();
}

// ── Entry point ───────────────────────────────────────────────────────────────

int main() {
    clearScreen();
    srand((unsigned int)time(NULL));

    startClock();
    gotoxy(40, 3); cout << ORANGE << "Welcome To MATH QUIZ GAME!" << WHITE ;
    gotoxy(38, 4); cout << "==============================";
    gotoxy(42, 5); cout << "Enter Your Name : ";
    cin >> name;
    flushCin();  
    gotoxy(38, 6); cout << "==============================";
    gotoxy(47, 7); cout << "Hello " << name << "!";
    gotoxy(38, 8); cout << "=============================="<< endl << endl ;

    for (int i = 5; i >= 0; i--) {
        cout << GREEN << "\r\t\t\t\t\t Get ready in: " << RED << i << GREEN << " seconds." << WHITE << flush;
        mySleep(800);
    }
    cout << "\n";

    Adding();
    // Subtracting();
    // multiply();
    // division(); 
    stopClock();
    return 0;
}