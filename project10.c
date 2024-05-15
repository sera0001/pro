#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>   

#include "category_initalizer.h"

#define MAX_USERS 100
#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_QUESTIONS 10
#define MAX_CATEGORIES 6
#define MAX_OPTIONS 4
#define MAX_ROOMS 10
#define MAX_PLAYERS_PER_ROOM 2
# define MIN_PLAYERS_TO_START 3
#define SERVER_IP "127.0.0.1" 
#define SERVER_PORT 2000     



#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_QUESTION "\x1b[35m"   
#define ANSI_COLOR_CORRECT "\x1b[32m"     
#define ANSI_COLOR_INCORRECT "\x1b[31m"  

/*typedef struct {
    char question[256];
    char options[MAX_OPTIONS][64];
    int correct_option;
} Question;*/

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
    int admin;
    
} Room;

/*typedef struct {
    char name[64];
    int question_indices[MAX_QUESTIONS];
    int num_questions;
} Category;*/

User users[MAX_USERS];
Room rooms[MAX_ROOMS];
Question questions[MAX_QUESTIONS * MAX_CATEGORIES];

void quiz(int categoryIndex, int connfd);

int num_users = 0;
int num_rooms = 0;

unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; 

    return hash;
}

int connect_to_server() {
    int sockfd;
    struct sockaddr_in servaddr;

   
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    servaddr.sin_port = htons(SERVER_PORT);

   
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
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
    printf("Category: %s\n", categories[categoryIndex].name);
    printf("Number of Questions: %d\n", categories[categoryIndex].num_questions);

    for (int i = 0; i < categories[categoryIndex].num_questions; i++) {
        Question current_question = categories[categoryIndex].questions[i];
        printf("Question %d:\n", i + 1);
        printf("  %s\n", current_question.question);
        printf("  Options:\n");
        for (int j = 0; j < MAX_OPTIONS; j++) {
            printf("    %d. %s\n", j + 1, current_question.options[j]);
        }
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

void startGame(char* game_data) {
    printf("Welcome to the Quiz Game!\n");

    // Use the game_data here if necessary
    // ...

    int categoryChoice;
    do {
        printf("Choose a category:\n");
        printf("1. Films\n2. Sports\n3. Science\n4. History\n5. Art\n6. Computer Science\n");
        printf("Enter the number of your chosen category (1-6): ");
        scanf("%d", &categoryChoice);

        if (categoryChoice < 1 || categoryChoice > MAX_CATEGORIES) {
            printf("Invalid choice. Please enter a number between 1 and %d.\n", MAX_CATEGORIES);
        } else {
            quiz(categoryChoice - 1 , connfd);
        }
    } while (categoryChoice >= 1 && categoryChoice <= MAX_CATEGORIES);
}

void signup(int connfd) {
    if (num_users >= MAX_USERS) {
        write(connfd, "User limit reached. Signup failed.\n", strlen("User limit reached. Signup failed.\n"));
        return;
    }

    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    // Receive username from client
    read(connfd, username, sizeof(username));

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            write(connfd, "Username already taken. Signup failed.\n", strlen("Username already taken. Signup failed.\n"));
            return;
        }
    }

    // Receive password from client
    read(connfd, password, sizeof(password));

    unsigned long password_hash = hash(password);

    strcpy(users[num_users].username, username);
    users[num_users].password_hash = password_hash;
    users[num_users].score = 0;
    users[num_users].in_room = -1;
    num_users++;

    save_users();

    write(connfd, "Signup successful!\n", strlen("Signup successful!\n"));
}


void create_room(int player_index, int connfd) {
    if (num_rooms >= MAX_ROOMS) {
        write(connfd, "Maximum number of rooms reached. Cannot create more rooms.\n", strlen("Maximum number of rooms reached. Cannot create more rooms.\n"));
        return;
    }

    rooms[num_rooms].players[0] = player_index;
    rooms[num_rooms].num_players = 1;
    rooms[num_rooms].game_started = 0;
    rooms[num_rooms].admin = player_index; 

    users[player_index].in_room = num_rooms;

    char room_message[1024];
    sprintf(room_message, "Room created. You are in Room %d (admin).\n", num_rooms + 1);
    write(connfd, room_message, strlen(room_message));
    write(connfd, "Waiting for other players to join...\n", strlen("Waiting for other players to join...\n"));

    while (rooms[num_rooms].num_players < MIN_PLAYERS_TO_START) {
        sleep(5); 
    }

    write(connfd, "All players have joined. The game can now start!\n", strlen("All players have joined. The game can now start!\n"));

    num_rooms++;
}


void join_room(int player_index, int connfd) {
    char room_list[1024] = "Available Rooms:\n";
    for (int i = 0; i < num_rooms; i++) {
        if (rooms[i].num_players < MAX_PLAYERS_PER_ROOM && rooms[i].game_started == 0) {
            char room_number[50];
            sprintf(room_number, "Room %d\n", i + 1);
            strcat(room_list, room_number);
        }
    }
    write(connfd, room_list, strlen(room_list));

    int room_choice;
    read(connfd, &room_choice, sizeof(room_choice));

    room_choice--;
    if (room_choice < 0 || room_choice >= num_rooms || rooms[room_choice].num_players >= MAX_PLAYERS_PER_ROOM || rooms[room_choice].game_started == 1) {
        write(connfd, "Invalid room choice. Joining failed.\n", strlen("Invalid room choice. Joining failed.\n"));
        return;
    }

    rooms[room_choice].players[rooms[room_choice].num_players] = player_index;
    rooms[room_choice].num_players++;

    users[player_index].in_room = room_choice;

    char join_message[1024];
    sprintf(join_message, "Joined Room %d.\n", room_choice + 1);
    write(connfd, join_message, strlen(join_message));
}

int checkAnswer(Question q, int user_answer) {
    return (user_answer == q.correct_option);
}

void displayQuestion(Question question) {
    printf("Question:\n");
    printf("  %s\n", question.question);
    printf("  Options:\n");
    for (int j = 0; j < MAX_OPTIONS; j++) {
        printf("    %d. %s\n", j + 1, question.options[j]);
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
void displaySingleQuestion(int categoryIndex, int questionIndex) {
    if (categoryIndex < 0 || categoryIndex >= MAX_CATEGORIES) {
        printf("Invalid category index.\n");
        return;
    }

    if (questionIndex < 0 || questionIndex >= categories[categoryIndex].num_questions) {
        printf("Invalid question index.\n");
        return;
    }

    Question current_question = categories[categoryIndex].questions[questionIndex];
    displayQuestion(current_question);
}


void shuffleArray(int *array, int n) {
   
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void quiz(int categoryIndex, int connfd) {
    if (categoryIndex < 0 || categoryIndex >= MAX_CATEGORIES) {
        write(connfd, "Invalid category index.\n", strlen("Invalid category index.\n"));
        return;
    }

    int num_questions = categories[categoryIndex].num_questions;
    int question_indices[num_questions];
    
    for (int i = 0; i < num_questions; i++) {
        question_indices[i] = i;
    }

    srand(time(NULL));
    for (int i = num_questions - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = question_indices[i];
        question_indices[i] = question_indices[j];
        question_indices[j] = temp;
    }

    int score = 0;
    write(connfd, "Welcome to the Quiz Game!\n", strlen("Welcome to the Quiz Game!\n"));

    time_t start_time, current_time;
    double elapsed_seconds;
    time(&start_time); 

    for (int i = 0; i < num_questions; i++) {
        int questionIndex = question_indices[i];
        Question current_question = categories[categoryIndex].questions[questionIndex];

        time(&current_time); 
        elapsed_seconds = difftime(current_time, start_time);
        int remaining_time = 30 - (int)elapsed_seconds;
        char time_message[1024];
        sprintf(time_message, "Time remaining: %d seconds\n", remaining_time);
        write(connfd, time_message, strlen(time_message));

        // Send question to client
        write(connfd, current_question.question_text, strlen(current_question.question_text));

        // Get client's answer
        char client_answer[1024];
        read(connfd, client_answer, sizeof(client_answer));

        // Check client's answer
        if (remaining_time <= 0) {
            char score_message[1024];
            sprintf(score_message, "Time's up! Quiz stopped. Your final score: %d/%d\n", score, num_questions);
            write(connfd, score_message, strlen(score_message));
            return; 
        }

        if (client_answer[0] >= '1' && client_answer[0] <= '4') {
            if (checkAnswer(current_question, client_answer)) {
                write(connfd, "Correct!\n", strlen("Correct!\n"));
                score++;
            } else {
                write(connfd, "Incorrect. Try again!\n", strlen("Incorrect. Try again!\n"));
            }
        } else {
            write(connfd, "Invalid input. Skipping question.\n", strlen("Invalid input. Skipping question.\n"));
        }
    }

    char final_score_message[1024];
    sprintf(final_score_message, "Quiz finished! Your final score: %d/%d\n", score, num_questions);
    write(connfd, final_score_message, strlen(final_score_message));
}


void login(int connfd) {
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];

    // Receive username from client
    read(connfd, username, sizeof(username));

    // Receive password from client
    read(connfd, password, sizeof(password));

    unsigned long entered_password_hash = hash(password);

    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].password_hash == entered_password_hash) {
            write(connfd, "Login successful!\n", strlen("Login successful!\n"));

            while (1) {
                // Receive choice from client
                int choice;
                read(connfd, &choice, sizeof(choice));

                switch (choice) {
                    case 1:
                        create_room(i , connfd); 
                        break;
                    case 2:
                        join_room(i , connfd); 
                        break;
                    case 3:
                        quiz(categoryIndex, connfd); // This function should be adapted for server-client model
                        break;
                    case 4:
                        // View High Scores 
                        break;
                    case 5:
                        write(connfd, "Logging out...\n", strlen("Logging out...\n"));
                        return;
                    default:
                        write(connfd, "Invalid choice. Please enter a valid option.\n", strlen("Invalid choice. Please enter a valid option.\n"));
                        break;
                }
            }
        }
    }

    write(connfd, "Invalid username or password.\n", strlen("Invalid username or password.\n"));
}


int main() {
    int sockfd = connect_to_server();

    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Play as a guest\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // Send choice to server
        write(sockfd, &choice, sizeof(choice));

        switch (choice) {
            case 1:
                signup(sockfd);
                break;
            case 2:
                login(sockfd);
                break;
            case 3:
                // The server should send game data after the connection is established
                char game_data[1024];
                read(sockfd, game_data, sizeof(game_data));

                // Start your game here using the game_data
                startGame(game_data);
                break;
            case 4:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Please enter a number from 1 to 4.\n");
        }
    } while (choice != 4);

    close(sockfd);
    return 0;
}
