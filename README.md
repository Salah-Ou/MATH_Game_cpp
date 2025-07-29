# Math Quiz Game
## Overview 
The Math Quiz Game is a console-based application that tests and improves your basic arithmetic skills through timed quizzes across four difficulty levels. The game progressively challenges players with addition, subtraction, multiplication, and division problems.

## Features 
### 1. Game Levels 
1. <strong> Level 1 : Addition </strong> (20 seconds per question).
2. <strong> Level 2 : Subtraction </strong> (15 seconds per question).
3. <strong> Level 3 : Multiplication </strong> (15 seconds per question).
4. <strong> Level 4 : Division </strong> (22 seconds per question).

### 2. Game  Mechanics
- <strong> Timed Questions : </strong> Each question must be answered within a specific time limit.
- <strong> Score Tracking : </strong> Players earn points for correct answers.
- <strong> Progression System : </strong> Advance to next level only by scoring 100%.
- <strong> Retry Option : </strong> Failed levels can be retried.
- <strong> Visual Feedback : </strong> Color-coded responses (green/red) for correct/incorrect answers.
- <strong> Sound Effects : </strong> Audio feedback for time warnings and transitions.

### 3. Technical Features
- Real-time clock display.
- Console color formatting.
- Input validation.
- Random number generation for problems.
- Timeout handling for unanswered questions.

## How To Play 
### 1. Installation :
    - Compile the code using a C++ compiler (tested on Windows).
    - Requires Windows.h and conio.h libraries.

### 2. Starting the Game :
    - Run the executable.
    - Enter your name when prompted.
    - The game will begin with a countdown.

### 3. Gameplay :
    - Solve each math problem within the time limit.
    - Type your answer and press Enter.
    - Earn points for correct answers.
    - Progress through levels by scoring perfectly (5/5).

### 4. Controls :
    - Numeric input for answers.
    - Enter key to submit answer.
    - '1' to continue/retry.
    - '0' to exit.

## Code Structure
- `main() : ` Entry point, handles player introduction.
- `Adding() : ` Level 1 - Addition problems.
- `Subtracting() : ` Level 2 - Subtraction problems
- `multiply() : ` Level 3 - Multiplication problems.
- `division() : ` Level 4 - Division problems.
- `Time() : ` Displays current time.
- `getInputWithTimeout() : ` Handles timed input.

## Customization Options
- You can modify :
    - Time limits by changing the timeout values.
    - Difficulty by adjusting the random number ranges.
    - Visual style by editing the color codes.
    - Sound effects by modifying Beep() parameters.


