#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_addr()
#include <unistd.h>    // For close()

#include "category_initalizer.h"

#define MAX_USERS 100
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_QUESTIONS 10
#define MAX_CATEGORIES 6
#define MAX_OPTIONS 4
#define MAX_ROOMS 10
#define MAX_PLAYERS_PER_ROOM 2

// ANSI escape codes for colored text
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_QUESTION "\x1b[35m"   // Magenta color for questions
#define ANSI_COLOR_CORRECT "\x1b[32m"    // Green color for correct answers
#define ANSI_COLOR_INCORRECT "\x1b[31m"  // Red color for incorrect answers

typedef struct {
    char question[256];
    char options[MAX_OPTIONS][64];
    int correct_option;
} Question;

typedef struct {
    char username[MAX_USERNAME];
    unsigned long password_hash;
    int score;
    int in_room;
} User;

typedef struct {
    int players[MAX_PLAYERS_PER_ROOM];
    int num_players;
    int game_started;
} Room;

typedef struct {
    char name[64];
    int question_indices[MAX_QUESTIONS];
    int num_questions;
} Category;

User users[MAX_USERS];
Room rooms[MAX_ROOMS];
Question questions[MAX_QUESTIONS * MAX_CATEGORIES];

int num_users = 0;
int num_rooms = 0;

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
#define SERVER_IP "127.0.0.1" // Modify with your server's IP address
#define SERVER_PORT 2000      // Modify with your server's port

int connect_to_server() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sockfd; // Return the connected socket descriptor
}
void save_users() {
    FILE *file = fopen("users.txt", "w");
    if (file == NULL) {
        printf("Error opening users file.\n");
        return;
    }

    for (int i = 0; i < num_users; i++) {
        fprintf(file, "%s %lu %d %d\n", users[i].username, users[i].password_hash, users[i].score, users[i].in_room);
    }

    fclose(file);
}

void load_users() {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening users file.\n");
        return;
    }

    while (fscanf(file, "%s %lu %d %d\n", users[num_users].username, &users[num_users].password_hash, &users[num_users].score, &users[num_users].in_room) != EOF) {
        num_users++;
    }

    fclose(file);
}

void displayUserScore(char *username) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Your score: %d\n", users[i].score);
            return;
        }
    }
    printf("User not found.\n");
}

void displayCategoryQuestions(int categoryIndex) {
    extern Category categories[MAX_CATEGORIES];  

    if (categoryIndex < 0 || categoryIndex >= MAX_CATEGORIES) {
        printf("Invalid category index.\n");
        return;
    }

    printf("Category: %s\n", categories[categoryIndex].name);
    printf("Number of Questions: %d\n", categories[categoryIndex].num_questions);

    int num_questions = categories[categoryIndex].num_questions;
    int *question_indices = categories[categoryIndex].question_indices;

    if (num_questions > 0) {
        for (int i = 0; i < num_questions; i++) {
            int questionIndex = question_indices[i];
            Question current_question = questions[questionIndex];

            printf("Question %d:\n", i + 1);
            printf("  %s\n", current_question.question);
            printf("  Options:\n");
            for (int j = 0; j < MAX_OPTIONS; j++) {
                printf("    %d. %s\n", j + 1, current_question.options[j]);
            }
            printf("  Correct Option: %d\n", current_question.correct_option);
        }
    } else {
        printf("No questions available for this category.\n");
    }
}

void films(int difficulty) {
    displayCategoryQuestions(0);
}

void sports(int difficulty) {
    displayCategoryQuestions(1);
}

void science(int difficulty) {
    displayCategoryQuestions(2);
}

void history(int difficulty) {
    displayCategoryQuestions(3);
}

void art(int difficulty) {
    displayCategoryQuestions(4);
}

void cs(int difficulty) {
    displayCategoryQuestions(5);
}

void startGame() {
    printf("Welcome to the Quiz Game!\n");

    int categoryChoice;
    do {
        printf("Choose a category:\n");
        printf("1. Films\n2. Sports\n3. Science\n4. History\n5. Art\n6. Computer Science\n");
        printf("Enter the number of your chosen category (1-6): ");
        scanf("%d", &categoryChoice);

        if (categoryChoice < 1 || categoryChoice > 6) {
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
        } else {
            quiz(categoryChoice - 1); // Pass category index to quiz function
        }
    } while (categoryChoice >= 1 && categoryChoice <= 6);
}

void signup() {
    if (num_users >= MAX_USERS) {
        printf("User limit reached. Signup failed.\n");
        return;
    }

    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("Enter your desired username: ");
    scanf("%s", username);

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Username already taken. Signup failed.\n");
            return;
        }
    }

    printf("Enter your desired password: ");
    scanf("%s", password);

    unsigned long password_hash = hash(password);

    strcpy(users[num_users].username, username);
    users[num_users].password_hash = password_hash;
    users[num_users].score = 0;
    users[num_users].in_room = -1;
    num_users++;

    save_users();

    printf("Signup successful!\n");
}

void create_room(int player_index) {
    if (num_rooms >= MAX_ROOMS) {
        printf("Maximum number of rooms reached. Cannot create more rooms.\n");
        return;
    }

    rooms[num_rooms].players[0] = player_index;
    rooms[num_rooms].num_players = 1;
    rooms[num_rooms].game_started = 0;

    users[player_index].in_room = num_rooms;

    printf("Room created. You are in Room %d.\n", num_rooms + 1);
    printf("Waiting for other players to join...\n");

    num_rooms++;
}

void join_room(int player_index) {
    printf("Available Rooms:\n");
    for (int i = 0; i < num_rooms; i++) {
        if (rooms[i].num_players < MAX_PLAYERS_PER_ROOM && rooms[i].game_started == 0) {
            printf("Room %d\n", i + 1);
        }
    }

    int room_choice;
    printf("Enter the number of the room you want to join: ");
    scanf("%d", &room_choice);

    room_choice--;
    if (room_choice < 0 || room_choice >= num_rooms || rooms[room_choice].num_players >= MAX_PLAYERS_PER_ROOM || rooms[room_choice].game_started == 1) {
        printf("Invalid room choice. Joining failed.\n");
        return;
    }

    rooms[room_choice].players[rooms[room_choice].num_players] = player_index;
    rooms[room_choice].num_players++;

    users[player_index].in_room = room_choice;

    printf("Joined Room %d.\n", room_choice + 1);
}

int checkAnswer(Question q, int user_answer) {
    return (user_answer == q.correct_option);
}

void displayQuestion(Question q) {
    printf("%s\n", q.question);
    for (int i = 0; i < MAX_OPTIONS; i++) {
        printf("%d. %s\n", i + 1, q.options[i]);
    }
}

void displayColoredQuestion(Question q) {
    printf("%s%s%s\n", ANSI_COLOR_QUESTION, q.question, ANSI_COLOR_RESET);

    int indices[MAX_OPTIONS];
    for (int i = 0; i < MAX_OPTIONS; i++) {
        indices[i] = i;
    }

    for (int i = MAX_OPTIONS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < MAX_OPTIONS; i++) {
        printf("%d. %s\n", i + 1, q.options[indices[i]]);
    }
}

void quiz(int categoryIndex) {
    extern Category categories[MAX_CATEGORIES];  

    if (categoryIndex < 0 || categoryIndex >= MAX_CATEGORIES) {
        printf("Invalid category index.\n");
        return;
    }

    int num_questions = categories[categoryIndex].num_questions;
    int *question_indices = categories[categoryIndex].question_indices;

    int score = 0;
    int time_up = 0;
    printf("Welcome to the Quiz Game!\n");

    time_t start_time, current_time;
    double elapsed_seconds;

    time(&start_time);
    for (int i = 0; i < num_questions; i++) {
        int random_index = rand() % num_questions;
        int questionIndex = question_indices[random_index];
        Question current_question = questions[questionIndex];

        displayColoredQuestion(current_question);

        int user_answer;
        printf("Enter your answer (1-4): ");
        scanf("%d", &user_answer);

        time(&current_time);
        elapsed_seconds = difftime(current_time, start_time);
        if (elapsed_seconds >= 30.0) {
            printf("Time's up! Quiz stopped. Your final score: %d/%d\n", score, num_questions);
            time_up = 1;
            break;
        }

        if (user_answer >= 1 && user_answer <= MAX_OPTIONS) {
            if (checkAnswer(current_question, user_answer)) {
                printf("%sCorrect!%s\n", ANSI_COLOR_CORRECT, ANSI_COLOR_RESET);
                score++;
            } else {
                printf("%sIncorrect!%s\n", ANSI_COLOR_INCORRECT, ANSI_COLOR_RESET);
            }
        } else {
            printf("Invalid input. Skipping question.\n");
        }

        // Remove the question index from the list
        for (int j = random_index; j < num_questions - 1; j++) {
            question_indices[j] = question_indices[j + 1];
        }
        question_indices[num_questions - 1] = -1; // Mark the last index as invalid or unused
        num_questions--;
    }

    if (!time_up) {
        printf("Quiz finished! Your final score: %d/%d\n", score, categories[categoryIndex].num_questions);
    }
}

void login() {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    printf("Enter your username: ");
    scanf("%s", username);

    printf("Enter your password: ");
    scanf("%s", password);

    unsigned long entered_password_hash = hash(password);

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].password_hash == entered_password_hash) {
            printf("Login successful!\n");
            printf("Welcome, %s!\n", username);
            while (1) {
                printf("What would you like to do?\n");
                printf("1. Create a Room\n2. Join a Room\n3. Start Quiz\n4. View High Scores\n5. Logout\n");
                int choice;
                printf("Enter your choice: ");
                scanf("%d", &choice);

                switch (choice) {
                    case 1:
                        create_room(i); // Assuming you want to create a room here
                        break;
                    case 2:
                        join_room(i); // Assuming you want to join a room here
                        break;
                    case 3:
                        startGame();
                        break;
                    case 4:
                        // View High Scores logic here
                        break;
                    case 5:
                        printf("Logging out...\n");
                        return;
                    default:
                        printf("Invalid choice. Please enter a valid option.\n");
                        break;
                }
            }
        }
    }

    printf("Invalid username or password.\n");
}

int main() {
    int server_socket = connect_to_server();
    srand(time(NULL)); 

    load_users(); 

    Category categories[MAX_CATEGORIES]; 
    initializeCategory(categories); // Pass the categories array and its size

    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                signup();
                break;
            case 2:
                login();
                break;
            case 3:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number from 1 to 3.\n");
        }
    } while (choice != 3);

    return 0;
}

