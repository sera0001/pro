#include "category_initalizer.h"
#include <string.h>

#define MAX_QUESTIONS 10
#define MAX_CATEGORIES 6
 #define MAX_OPTIONS 4

typedef struct {
    char question[256];
    char options[MAX_OPTIONS][64];
    int correct_option;
} Question;

typedef struct {
    Question questions[MAX_QUESTIONS];
    int num_questions;
} Category;



Category categories[MAX_CATEGORIES];

void initializeCategory(int categoryIndex) {
    switch(categoryIndex) {
        case 0: // Films category
            categories[0].num_questions = 5;
            strcpy(categories[0].questions[0].question, "Who directed the movie 'Inception'?");
            strcpy(categories[0].questions[0].options[0], "Christopher Nolan");
            strcpy(categories[0].questions[0].options[1], "Martin Scorsese");
            strcpy(categories[0].questions[0].options[2], "James Cameron");
            strcpy(categories[0].questions[0].options[3], "Steven Spielberg");
            categories[0].questions[0].correct_option = 1;

            strcpy(categories[0].questions[1].question, "Which actor portrayed the character Tony Stark in the Marvel Cinematic Universe?");
            strcpy(categories[0].questions[1].options[0], "Robert Downey Jr.");
            strcpy(categories[0].questions[1].options[1], "Chris Evans");
            strcpy(categories[0].questions[1].options[2], "Chris Hemsworth");
            strcpy(categories[0].questions[1].options[3], "Mark Ruffalo");
            categories[0].questions[1].correct_option = 1;
            break;

        case 1: // Sports category
            strcpy(categories[1].questions[0].question, "Which athlete holds the record for the fastest 100-meter sprint?");
            strcpy(categories[1].questions[0].options[0], "Usain Bolt");
            strcpy(categories[1].questions[0].options[1], "Carl Lewis");
            strcpy(categories[1].questions[0].options[2], "Michael Johnson");
            strcpy(categories[1].questions[0].options[3], "Jesse Owens");
            categories[1].questions[0].correct_option = 1;

            strcpy(categories[1].questions[1].question, "In tennis, what is the term for a score of zero?");
            strcpy(categories[1].questions[1].options[0], "Love");
            strcpy(categories[1].questions[1].options[1], "Deuce");
            strcpy(categories[1].questions[1].options[2], "Advantage");
            strcpy(categories[1].questions[1].options[3], "Fault");
            categories[1].questions[1].correct_option = 1;
            break;

        case 2: // Science category
            strcpy(categories[2].questions[0].question, "What is the chemical symbol for gold?");
            strcpy(categories[2].questions[0].options[0], "Au");
            strcpy(categories[2].questions[0].options[1], "Ag");
            strcpy(categories[2].questions[0].options[2], "Cu");
            strcpy(categories[2].questions[0].options[3], "Fe");
            categories[2].questions[0].correct_option = 1;

            strcpy(categories[2].questions[1].question, "Which planet is known as the 'Red Planet'?");
            strcpy(categories[2].questions[1].options[0], "Mars");
            strcpy(categories[2].questions[1].options[1], "Venus");
            strcpy(categories[2].questions[1].options[2], "Saturn");
            strcpy(categories[2].questions[1].options[3], "Jupiter");
            categories[2].questions[1].correct_option = 1;
            break;

        case 3: // History category
            strcpy(categories[3].questions[0].question, "Who was the first emperor of Rome?");
            strcpy(categories[3].questions[0].options[0], "Augustus");
            strcpy(categories[3].questions[0].options[1], "Julius Caesar");
            strcpy(categories[3].questions[0].options[2], "Nero");
            strcpy(categories[3].questions[0].options[3], "Constantine");
            categories[3].questions[0].correct_option = 1;

            strcpy(categories[3].questions[1].question, "What year did World War II end?");
            strcpy(categories[3].questions[1].options[0], "1945");
            strcpy(categories[3].questions[1].options[1], "1939");
            strcpy(categories[3].questions[1].options[2], "1918");
            strcpy(categories[3].questions[1].options[3], "1950");
            categories[3].questions[1].correct_option = 1;
            break;

        case 4: // Art category
            strcpy(categories[4].questions[0].question, "Which artist is known for his melting clocks in the painting 'The Persistence of Memory'?");
            strcpy(categories[4].questions[0].options[0], "Salvador Dalí");
            strcpy(categories[4].questions[0].options[1], "Pablo Picasso");
            strcpy(categories[4].questions[0].options[2], "Vincent van Gogh");
            strcpy(categories[4].questions[0].options[3], "Claude Monet");
            categories[4].questions[0].correct_option = 1;

            strcpy(categories[4].questions[1].question, "Which art movement is characterized by its use of bold colors and geometric shapes?");
            strcpy(categories[4].questions[1].options[0], "Fauvism");
            strcpy(categories[4].questions[1].options[1], "Cubism");
            strcpy(categories[4].questions[1].options[2], "Surrealism");
            strcpy(categories[4].questions[1].options[3], "Dadaism");
            categories[4].questions[1].correct_option = 1;
            break;

        case 5: // Computer Science category
            strcpy(categories[5].questions[0].question, "Which programming language is commonly used for web development and design?");
            strcpy(categories[5].questions[0].options[0], "HTML");
            strcpy(categories[5].questions[0].options[1], "Python");
            strcpy(categories[5].questions[0].options[2], "Java");
            strcpy(categories[5].questions[0].options[3], "CSS");
            categories[5].questions[0].correct_option = 1;

            strcpy(categories[5].questions[1].question, "What is the primary purpose of a compiler?");
            strcpy(categories[5].questions[1].options[0], "Translate high-level code into machine code");
            strcpy(categories[5].questions[1].options[1], "Manage databases");
            strcpy(categories[5].questions[1].options[2], "Execute commands in the terminal");
            strcpy(categories[5].questions[1].options[3], "Create graphical user interfaces");
            categories[5].questions[1].correct_option = 1;
            break;

        default:
        
            break;
    }
}
