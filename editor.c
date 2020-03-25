#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STR_SIZE 1024
#define MAX_LINES_INSERTED 100
#define MAX_COLUMNS_INSERTED 200
#define INSERT_MODE 0
#define COMMAND_MODE 1
#define OPERATION_INSERT 0
#define OPERATION_BACKSPACE 1
#define OPERATION_DL 2
#define OPERATION_GL 3
#define OPERATION_GC 4
#define OPERATION_D 5
#define OPERATION_REPLACE 6

typedef struct column {
	char val;
	struct column *next;
	struct column *prev;
} Col;

typedef struct line {
	Col *val;
	struct line *next;
	struct line *prev;
} Line;

typedef struct {
	Line *line;
	Col *col;
} Cursor;

typedef struct coordinates {
	int line;
	int col;
} Coord;

typedef struct operation {
	int type;
	int lines;
	char* text;
	char* newtext;
	Coord* coord_before;
	Coord* coord_after;

	struct operation *prev;
	struct operation *next;
} Operation;

typedef struct stack{
	Operation** operationList;
	Operation* top;
} OperationStack;

//functie pentru initializarea stivei
OperationStack* initStack() {
	OperationStack* stack = (OperationStack*)malloc(sizeof(OperationStack)); //memoria pentru stiva
	stack->operationList = (Operation**)malloc(sizeof(Operation*)); //memorie pentru capul listei
	Operation** operationList = stack->operationList;

	*operationList = NULL;
	stack->top = *operationList;

	return stack;
}

//functia adauga in stiva tipul operatiei si modificarile ce au loc in urma ei, in functie de caz
void push(OperationStack* stack, int type, char* text, Coord* coord_before, Coord* coord_after, int lines, char* new_text) {
	Operation* operation = (Operation*)malloc(sizeof(Operation));
	operation->type = type;
	operation->lines = lines;
	if(text != NULL) {
		operation->text = (char*)malloc(MAX_STR_SIZE * MAX_LINES_INSERTED);
		strcpy(operation->text, text);
	}
	if(new_text != NULL) {
		operation->newtext = (char*)malloc(MAX_STR_SIZE * MAX_LINES_INSERTED);
		strcpy(operation->newtext, new_text);
	}
	operation->coord_before = coord_before;
	operation->coord_after = coord_after;

	if(stack->top == NULL) {
		Operation** operationList = stack->operationList;
		*operationList = operation;
		operation->next = NULL;
		operation->prev = NULL;
	}
	else {
		operation->next = NULL;
		operation->prev = stack->top;
		stack->top->next = operation;
	}
	stack->top = operation;
}

//functia returneaza varful stivei
Operation* getTop(OperationStack* stack) {
	Operation* top = stack->top;
	stack->top = top->prev;

	return top;
}

//functie pentru afisarea operatiilor introduse in stiva
void printStack(OperationStack* stack) {
	Operation** operationList = stack->operationList;
	Operation* current = *operationList;
	while(current != NULL) {
		printf("-->TYPE:%d,TEXT:%s,Coord_before:Linia:%d Col:%d, Coord_after:Linia:%d Col:%d\n", 
				current->type, current->text, current->coord_before->line, current->coord_before->col, 
				current->coord_after->line, current->coord_after->col);
		current = current->next;
	}
	printf("\n");
}


/*void clearFromTop(OperationStack* stack) {
	Operation* to_clear = stack->top->next;
	while(to_clear != NULL) {
		free(to_clear->text);
		free(to_clear->newtext);
		Operation* next_operation = to_clear->next;
		free(to_clear);
		to_clear = next_operation;
	}
	stack->top->next = NULL;
}*/

//functia returneaza operatia urmatoare din stiva
Operation* getTopNext(OperationStack* stack) {
	if(stack->top != NULL) {
		stack->top = stack->top->next;
	}
	else {
		Operation** operationList = stack->operationList;
		stack->top = *operationList;
	}

	return stack->top;
}

//functie pentru initializarea listei
Line** initLines(Cursor *cursor) {

	Line **head_lines = (Line**)malloc(sizeof(Line*));
	*head_lines = (Line*)malloc(sizeof(Line));
	Line* line = *head_lines;
	line->next = NULL;
	line->prev = NULL;

	line->val = (Col*)malloc(sizeof(Col));
	Col* column = line->val;
	column->prev = NULL;
	column->next = NULL;

	cursor->col = column;
	cursor->line = line;

	return head_lines;
}

// functia primeste ca parametru o coloana de tipul Col* si creeaza un nod dupa aceasta
// este returnat nodul creat
Col* addColumnAfter(Col* column) {
	Col *new_column = (Col*)malloc(sizeof(Col));
	new_column->next = NULL;
	new_column->prev = column;

	return new_column;
}

/*functia primeste ca parametru o linie de tipul Line* si creeaza o linie dupa aceasta
pentru linia creata se aloca memorie si pentru primul element al ei
functia returneaza linia creata*/
Line* addLineAfter(Line* line) {
	Line *new_line = (Line*)malloc(sizeof(Line));
	new_line->next = NULL;
	new_line->prev = line;
	new_line->val = (Col*)malloc(sizeof(Col));
	Col* column = new_line->val;
	column->prev = NULL;
	column->next = NULL;

	return new_line;
}

//functia returneaza numarul liniei curente pe care se afla cursorul
int currentLinePosition(Line **head, Cursor *cursor) {
	int nb_of_lines = 1;
	Line *line = cursor->line;
	
	while(line != *head) {
		if(line == NULL)
			return -1;
		line = line->prev;
		nb_of_lines++;
	}
	return nb_of_lines;
}

//functia returneaza numarul coloanei pe care se afla cursorul
int currentColPosition(Cursor *cursor) {
	int nb_of_columns = 1;
	Col* first_col = cursor->line->val;
	Col* current_col = cursor->col;

	while(first_col != current_col && first_col->next != NULL) {
		nb_of_columns++;
		first_col = first_col->next;
	}
	return nb_of_columns;
}

/*functia primeste ca parametri capul listei si cursorul
functia intoarce o structura de tipul Coord* in care se salveaza coordonatele cursorului
(linia si coloana pe care se afla cursorul)*/
Coord* coordinates(Line **head, Cursor *cursor) {
	Coord* position = (Coord*)malloc(sizeof(Coord)); //se aloca memorie pentru structura
	position->line = currentLinePosition(head, cursor);
	position->col = currentColPosition(cursor);

	return position; 
}

/*functia primeste ca parametru cursorul, sirul si capul listei
si adauga fiecare caracter al sirului intr-un nod*/
void createLine(Cursor* cursor, char *str, Line** head) {
	int i;
	char existent_value = cursor->col->val;

	if(!existent_value) {
		//cursorul nu se afla pe un nod cu o valoare existenta
		for(i = 0; i < strlen(str) - 1; i++) { //stringul va contine caracterul '\n' la final (folosesc fgets)
			Col* column = cursor->col;
			column->val = str[i];
			column->next = addColumnAfter(column);
			cursor->col = column->next;
		}
		
		Line* line = cursor->line;
		Col* column = cursor->col;
		column->val = str[i];

		line->next = addLineAfter(line);
		Line* new_line = line->next;
		Col* new_col = new_line->val;
		column->next = new_col;
		new_col->prev = column; //leg inceputul noii linii de sfarsitul liniei anterioare

		cursor->line = new_line;
		cursor->col = new_col;
	}
	else { //cursorul se afla pe un nod cu valoare
			Line* line = cursor->line;
			Line* next_line = line->next;
			line->next = addLineAfter(line);
			Line* new_line = line->next;
			new_line->next = next_line;
			if(next_line != NULL)
				next_line->prev = new_line;

			Col* current_col = cursor->col;
			Col* prev_col = current_col->prev;

			new_line->val = current_col; //mut textul de la coloana curenta pe new_line

			for(i = 0; i < strlen(str); i++) {
				Col* column = prev_col;
				Col *new_column = (Col*)malloc(sizeof(Col));
				new_column->prev = column; //leg noul nod de nodul precedent
				if(column != NULL) {
					column->next = new_column;
					if (column->val == '\n') { //s-a mutat tot textul de pe line pe new_line
						line->val = new_column; //new_column va fi prima coloana de pe line
					}
				}
				else //ma aflu pe prima linie
					line->val = new_column;
				new_column->val = str[i];
				prev_col = new_column;
				new_column->next = new_line->val;
				new_line->val->prev = new_column;
			}
			
			cursor->line = new_line;
			cursor->col = new_line->val;
	}
}

//functia printeaza toate caracterele din lista
void print(Line** head) {
	Line* line = *head;
	Col* column = line->val;

	while(column != NULL) {
		printf("%c", column->val);
		column = column->next;
	}
}

// functia primeste ca parametru sirul introdus si modul initial si
// verifica daca acesta este "::i", iar in acest caz schimba modul
int process_line(char string[], int *mode) {
	char str[] = "::i";

	if(strncmp(string, str, strlen(str)) == 0) {
		*mode = !(*mode);
		return 1;
	}
	return 0;
}

//functia printeaza numarul de linii precum si numarul de coloane de pe fiecare linie
void printSize(Line** head) {
	Line* line = *head;
	int count = 0;

	while(line != NULL) {
		int count_col = 0;
		Col* column = line->val;
		while(column->val != '\n' && column->next != NULL) {
			count_col++;
			column = column->next;
		}
		printf("Size col: %d\n", count_col);
		count++;
		line = line->next;
	}
	printf("Size lines: %d\n", count);
}

//functia printeaza valoarea nodului pe care se afla cursorul
void printCursor(Cursor *cursor) {
	printf("cursor:%c", cursor->col->val);
}

/*functia primeste ca parametru cursorul si sterge un caracter dinaintea lui
functia intoarce caracterul sters*/
char* backspace(Cursor* cursor) {
	char* text = (char*)malloc(sizeof(char));

	Col *col = cursor->col->prev; //col este coloana care se sterge | pe col->next se afla cursorul
	text[0] = col->val; //memorez caracterul sters
	
	Line *line = cursor->line;
	if(line->prev == NULL && col == NULL)
		return NULL;

	Col *prev = col->prev;
	Col *next = col->next;
	
	if(prev != NULL)
		prev->next = next;
	if(next != NULL && prev != NULL) 
		next->prev = prev;
	if(prev == NULL && next != NULL) {
		line->val = next;
		next->prev = NULL;
	}

	if(col->val == '\n') {
		Line *prevLine = line->prev;
		Line *nextLine = line->next;

		prevLine->next = nextLine;
		if(nextLine != NULL) {
			nextLine->prev = prevLine;
		}
		line = prevLine;
	}
	if(line->val == col) {
		if(prev != NULL)
			prev->next = next;
		next->prev = prev;
		line->val = next;
	}
	cursor->col = col->next;
	cursor->col->val = col->next->val;
	cursor->line = line;

	return text;
}

/*functia primeste ca parametri numarul de caractere care vor fi sterse si cursorul
functia intoarce caracterele sterse*/
char* deleteCaract(int nrCaract, Cursor* cursor) {
	Col *col = cursor->col; //se sterge inclusiv col
	Col *prevCol = col->prev;
	Line *currline = cursor->line;
	if(currline->next == NULL && col->next == NULL)
		return NULL;

	char* text = (char*)malloc(nrCaract * sizeof(char));
	strcpy(text, "");
	Col* to_delete = cursor->col;
	char character = to_delete->val;
	int i = 0;
	for(i = 0; i < nrCaract; i++) {
		text[i] = character;
		to_delete = to_delete->next;
		character = to_delete->val;
	}

	int count = 1;
	while(count != nrCaract) {
		if(col->val == '\n') { //daca caracterul este newline se va sterge o linie
			Line *line = currline->next;
			currline->next = line->next;
			if(line->next != NULL)
				line->next->prev = currline;
		}
		col = col->next;
		count++;
	}
	if(col->val == '\n') { //daca caracterul care iese din while este tot newline se repeta procedeul
		Line *line = currline->next;
		currline->next = line->next;
		if(line->next != NULL)
			line->next->prev = currline;
	}
	Col *nextCol = col->next;
	
	if(prevCol == NULL) { //ma aflu pe primul caracter al primei linii
		currline->val = nextCol;
		nextCol->prev = NULL;
	}
	else {
		prevCol->next = nextCol;
		nextCol->prev = prevCol;
	}

	cursor->col = nextCol;
	cursor->line = currline;

	return text;
}

/*functia primeste ca parametri numarul liniei pe care se doreste a pozitiona cursorul,
 capul listei si cursorul*/
void goToLine(int lineNumber, Line **head, Cursor* cursor) {

	Line *line = *head;
	int count = 1;

	while(count != lineNumber && line != NULL) {
		line = line->next;
		count++;
	}
	if(line != NULL) {
		cursor->col = line->val;
		cursor->line = line;
	}
}

/*functia primeste ca parametri coloana si linia pe care se doreste a pozitiona cursorul,
 capul listei si cursorul*/
void goToCharacter(int colNumber, int lineNumber, Line **head, Cursor* cursor) {
	Line *line = *head;
	int countLine = 1, countCol = 1;

	while(countLine != lineNumber && line != NULL) {
		line = line->next;
		countLine++;
	}
	if(line != NULL) {
		cursor->line = line;
		Col *col = line->val;
		while(countCol != colNumber && col != NULL) {
			col = col->next;
			countCol++;
		}
		if(col != NULL)
			cursor->col = col;
	}
}

/*functia primeste ca parametri numarul liniei care va fi sterse, capul listei si cursorul
functia returneaza textul de pe linia stearsa*/
char* deleteLine(int lineNumber, Line **head, Cursor* cursor) {
	Line *line = *head;
	int countLine = 1;

	while(countLine != lineNumber && line != NULL) {
		line = line->next;
		countLine++;
	}

	char* text = (char*)malloc(MAX_COLUMNS_INSERTED * sizeof(char));
	strcpy(text, "");
	Col* to_delete = line->val;
	char character = to_delete->val;
	text[0] = character;
	int index = 0;

	if(line->next != NULL) {
		while(character != '\n') {
			text[index] = character;
			to_delete = to_delete->next;
			character = to_delete->val;
			index++;
		}
	}
	else {
		while(to_delete->next != NULL) {
			to_delete = to_delete->next;
			character = to_delete->val;
			text[++index] = character;
		}
	}

	int nrLine = currentLinePosition(head, cursor); //numarul liniei curente
	//daca linia curenta nu este linia pe care vreau sa o sterg => nu modific cursorul
	//daca linia pe care vreau sa o sterg este linia curenta => se modifica cursorul
	Col *col = line->val;
	if(col->prev == NULL) { //daca vreau sa sterg prima linie
		*head = (*head)->next;
		line = *head;
		line->prev = NULL;
		Col *col = line->val;
		col->prev = NULL;
		if(countLine == nrLine) { //updatez cursorul
			cursor->line = line;
			cursor->col = col;
		}
	}
	else if(col->next == NULL) { //daca vreau sa sterg ultima linie
		Line *prevLine = line->prev;
		Col *prevCol = col->prev;
		prevLine->next = NULL;
		prevCol->next = col;
		col->prev = prevCol;
		if(countLine == nrLine) {
			cursor->line = prevLine;
			cursor->col = col;
		}
	}
		else {
			Col *prevCol = col->prev;
			Line *prevLine = line->prev;
			Line *nextLine = line->next;
			if(nextLine != NULL) {
				Col *nextLineCol = nextLine->val;
				prevCol->next = nextLineCol;
				nextLineCol->prev = prevCol;
				prevLine->next = nextLine;
				nextLine->prev = prevLine;
				if(countLine == nrLine) {
					cursor->line = nextLine;
					cursor->col = nextLineCol;
				}
			}
			else {
				prevCol->next = cursor->col;
				cursor->col->prev = prevCol;
				prevLine->next = NULL;
				cursor->line = prevLine;
			}
		}
	return text;
}

//functia salveaza textul din lista intr-un fisier dat ca parametru
void saveFile(FILE* file, Line **head) {
	Line *line = *head;
	Col* column = line->val;

	while(column->next != NULL) {
		fprintf(file ,"%c", column->val);
		column = column->next;
	}
}

void autoSave(char* file, Line **head) {
	FILE* outputFile;
	outputFile = fopen(file, "w");
	saveFile(outputFile, head);
	fclose(outputFile);
}

//functie folosita pentru a realiza salvare automata atunci cand au fost apelate 5 comenzi
void updateCounter(int *saveCounter, char* file, Line **head) {
	(*saveCounter)++;
	if(*saveCounter == 5) {
		*saveCounter = 0;
		autoSave(file, head);
	}
}

/*functia returneaza linia de pe o pozitie data ca parametru*/
Line* getLine(Line **head, int line_number) {
	Line *line = *head;
	int count = 1;

	while(line != NULL && count != line_number) {
		count ++;
		line = line->next;
	}
	return line;
}

/*functia returneaza coloana de pe o coordonatele date ca parametri*/
Col* getCol(Line **head, int line_number, int col_number) {
	Line *line = getLine(head, line_number);
	if(line == NULL)
		return NULL;

	Col *col = line->val;
	int count = 1;

	while(count != col_number) {
		count++;
		col = col->next;
	}
	return col;
}

/*functia primeste ca parametri coordonata dinaintea efectuarii 
operatiei de inserare, coordonata de dupa si capul listei
functia scoate din lista nodurile dintre cele 2 coordonate*/
void undo_insert(Coord* coord_before, Coord* coord_after, Line** head) {
	int line_number_start = coord_before->line;
	int line_number_end = coord_after->line;
	int col_number_start = coord_before->col;
	int col_number_end = coord_after->col;
	Line *startLine;
	Line *endLine;
	Col *startCol;
	Col *endCol;

	/*gasesc linia si coloana de inceput, respectiv linia si coloana de sfarsit,
	folosindu-ma de coordonatele lor numerice*/
	startLine = getLine(head, line_number_start);
	endLine = getLine(head, line_number_end);
	startCol = getCol(head, line_number_start, col_number_start);
	endCol = getCol(head, line_number_end, col_number_end);

	Line* prevLine = startLine->prev;
	Line* nextLine = endLine->next;
	Col* prevCol = startCol->prev;

	if (prevCol == NULL) {
		prevCol = startLine->val;
		endCol->prev = NULL;
	} else {
		endCol->prev = prevCol;
	}
	prevCol->next = endCol;

	if(startLine->val == startCol) {
		if(prevLine != NULL) {
			prevLine->next = endLine;
		}
		else {
			*head = endLine;
		}
		endLine->prev = prevLine;
	}
	else {
		startLine->next = nextLine;
		if(nextLine != NULL) {
			nextLine->prev = startLine;
		}
	}
}

//functia aloca memorie pentru un nou nod de tipul Col* si ii atribuie o valoare
Col* createCol(char character) {
	Col *new_col = (Col*)malloc(sizeof(Col));
	new_col->val = character;

	return new_col;
}

/*functia adauga inapoi in lista caracterul sters dintre cele 2 coordonate*/
void undo_backspace(Coord* coord_before, Coord* coord_after, char* text, Line** head) {
	int line_number_start = coord_before->line;
	int line_number_end = coord_after->line;
	int col_number_end = coord_after->col;
	Line *startLine;
	Line *endLine;
	Col *endCol;

	startLine = getLine(head, line_number_start);
	endLine = getLine(head, line_number_end);
	endCol = getCol(head, line_number_end, col_number_end);
	
	Col *new_col = createCol(text[0]);

	//refac legaturile
	Col* prevCol = endCol->prev;
	if(prevCol != NULL) {
		prevCol->next = new_col;
	}
	new_col->prev = prevCol;
	new_col->next = endCol;
	endCol->prev = new_col;

	if(new_col->val == '\n') {
		Line *new_line = (Line*)malloc(sizeof(Line));
		endLine->next = new_line;
		new_line->prev = endLine;
		new_line->next = startLine;
		if(startLine != NULL)
			startLine->prev = new_line;
		new_line->val = endCol;
	}
	else {
		if(prevCol == NULL) {
			endLine->val = new_col;
		}
	}
}

/*functia adauga inapoi in lista textul de pe linia stearsa*/
void undo_deleteLine(int line_number, char* text, Line **head) {
	Line* line = getLine(head, line_number);
	Line* prevLine;
	Col* col;
	Col* prevCol;
	if(line != NULL) {
		prevLine = line->prev;
		col = line->val;
		prevCol = col->prev;
	}
	else {
		Line* line_aux = getLine(head, line_number - 1);
		prevLine = line_aux;
		col = NULL;
		prevCol = prevLine->val;
		while(prevCol->next != NULL) {
			prevCol = prevCol->next;
		}
	}
	Line* new_line = (Line*)malloc(sizeof(Line));

	if(prevLine != NULL) {
		prevLine->next = new_line;
	}
	else {
		*head = new_line;
	}
	new_line->prev = prevLine;
	new_line->next = line;
	if(line != NULL) {
		line->prev = new_line;
	}

	Col* current_col = (Col*)malloc(sizeof(Col)); //initializez primul element al noii linii
	current_col->val = text[0];
	new_line->val = current_col;
	if(prevCol != NULL) {
		prevCol->next = current_col;
	}
	current_col->prev = prevCol;
	current_col->next = col;
	if(col != NULL) {
		col->prev = current_col;
	}

	int i = 1;
	for(i = 1; i < strlen(text); i++) {
		Col* new_col = (Col*)malloc(sizeof(Col)); //initializare urmator element
		new_col->val = text[i];
		current_col->next = new_col;
		new_col->prev = current_col;
		new_col->next = col;
		if(col != NULL) {
			col->prev = new_col;
		}
		current_col = new_col;
	}
	Col* new_col = (Col*)malloc(sizeof(Col)); //adaug caracterul newline
	new_col->val = '\n';
	current_col->next = new_col;
	new_col->prev = current_col;
	new_col->next = col;
	if(col != NULL) {
		col->prev = new_col;
	}
}

/*functia imi muta cursorul inapoi pe pozitia de dinaintea efectuarii operatiei gl*/
void undo_goToLastPosition(Coord* coord_before, Cursor* cursor, Line** head) {
	int line_number_start = coord_before->line;
	int col_number_start = coord_before->col;
	Line *startLine;
	Col *startCol;

	startLine = getLine(head, line_number_start);
	startCol = getCol(head, line_number_start, col_number_start);

	cursor->col = startCol;
	cursor->line = startLine;
}

//functia adauga inapoi in lista valorile sterse in urma comenzii 'd'
void undo_delete(Coord* coord_after, char* text, Line **head, Cursor* cursor) {
	int line_number_end = coord_after->line;
	int col_number_end = coord_after->col;
	Col *currentCol;

	currentCol = getCol(head, line_number_end, col_number_end);
	Col *prevCol = currentCol->prev;

	int i = 0;
	for(i = 0; i < strlen(text); i++) {
		Col* new_col = (Col*)malloc(sizeof(Col));
		new_col->val = text[i];
		new_col->prev = prevCol;
		if(prevCol != NULL)
			prevCol->next = new_col;
		new_col->next = currentCol;
		if(currentCol != NULL)
			currentCol->prev = new_col;		
		prevCol = new_col;
	}
	cursor->col = prevCol;
}

/*functia inlocuieste un cuvant din lista cu alt cuvant, avand aceleasi dimensiuni*/
void replaceText(Coord* coord_before, Coord* coord_after, char* new_word, Line **head) {
	int line_number_start = coord_before->line;
	int col_number_start = coord_before->col;
	int col_number_end = coord_after->col;
	Col *startCol;

	startCol = getCol(head, line_number_start, col_number_start);

	int i = col_number_start;
	int index = 0;
	while(i < col_number_end) {
		startCol->val = new_word[index];
		index++;
		i++;
		startCol = startCol->next;
	}
}

/*functie care realizeaza operatiile de undo
primeste ca parametri stiva, capul listei si cursorul*/
void undo(OperationStack* stack, Line **head, Cursor* cursor) {
	Operation* operation = getTop(stack);

	switch(operation->type) {
		case OPERATION_INSERT: {
			undo_insert(operation->coord_before, operation->coord_after, head);
			break;
		}
		case OPERATION_BACKSPACE: {
			undo_backspace(operation->coord_before, operation->coord_after, operation->text, head);
			break;
		}
		case OPERATION_DL: {
			undo_deleteLine(operation->lines, operation->text, head);
			break;
		}
		case OPERATION_GL: {
			undo_goToLastPosition(operation->coord_before, cursor, head);
			break;
		}
		case OPERATION_GC: {
			undo_goToLastPosition(operation->coord_before, cursor, head);
			break;
		}
		case OPERATION_D: {
			undo_delete(operation->coord_after, operation->text, head, cursor);
			break;
		}
		case OPERATION_REPLACE: {
			replaceText(operation->coord_before, operation->coord_after, operation->text, head);
			break;
		}
	}
}

/*functiile de mai jos realizeaza inversul operatiei undo al fiecarei comenzi in parte*/

void redo_insert(Coord* coord_before, Coord* coord_after, char* text, Cursor* cursor, Line **head) {
	int line_number_start = coord_before->line;
	int col_number_start = coord_before->col;
	Line *startLine;
	Col *startCol;

	startLine = getLine(head, line_number_start);
	startCol = getCol(head, line_number_start, col_number_start);
	cursor->col = startCol;
	cursor->line = startLine;

	createLine(cursor, text, head);
}

void redo_backspace(Coord* coord_before, Coord* coord_after, char* text, Cursor* cursor, Line **head) {
	int line_number_start = coord_before->line;
	int line_number_end = coord_after->line;
	int col_number_start = coord_before->col;
	int col_number_end = coord_after->col;
	Line *startLine;
	Line *endLine;
	Col *startCol;
	Col *endCol;

	startLine = getLine(head, line_number_start);
	endLine = getLine(head, line_number_end);
	startCol = getCol(head, line_number_start, col_number_start);
	endCol = getCol(head, line_number_end, col_number_end);

	cursor->col = startCol;
	cursor->line = startLine;

	text = backspace(cursor);

	cursor->col = endCol;
	cursor->line = endLine;
}

void redo_goToCharacter(Coord* coord_before, Coord* coord_after, Cursor* cursor, Line **head) {
	int line_number_end = coord_after->line;
	int col_number_end = coord_after->col;
	Line *endLine;
	Col *endCol;

	endLine = getLine(head, line_number_end);
	endCol = getCol(head, line_number_end, col_number_end);

	cursor->col = endCol;
	cursor->line = endLine;
}

void redo_deleteLine(Coord* coord_before, Coord* coord_after, int line_number, char* text, Cursor* cursor, Line **head) {
	int line_number_start = coord_before->line;
	int line_number_end = coord_after->line;
	int col_number_start = coord_before->col;
	int col_number_end = coord_after->col;
	Line *startLine;
	Line *endLine;
	Col *startCol;
	Col *endCol;

	startLine = getLine(head, line_number_start);
	endLine = getLine(head, line_number_end);
	startCol = getCol(head, line_number_start, col_number_start);
	endCol = getCol(head, line_number_end, col_number_end);
	cursor->col = startCol;
	cursor->line = startLine;

	text = deleteLine(line_number, head, cursor);

	cursor->col = endCol;
	cursor->line = endLine;
}

void redo_goToLine(Coord* coord_after, Cursor* cursor, Line **head) {
	int line_number_end = coord_after->line;
	int col_number_end = coord_after->col;
	Line *endLine;
	Col *endCol;

	endLine = getLine(head, line_number_end);
	endCol = getCol(head, line_number_end, col_number_end);

	cursor->col = endCol;
	cursor->line = endLine;
}

void redo(OperationStack* stack, Line **head, Cursor* cursor) {
	Operation* operation = getTopNext(stack);

	switch(operation->type) {
		case OPERATION_INSERT: {
			redo_insert(operation->coord_before, operation->coord_after, operation->text, cursor, head);
			break;
		}
		case OPERATION_BACKSPACE: {
			redo_backspace(operation->coord_before, operation->coord_after, operation->text, cursor, head);
			break;
		}
		case OPERATION_GC: {
			redo_goToCharacter(operation->coord_before, operation->coord_after, cursor, head);
			break;
		}
		case OPERATION_DL: {
			redo_deleteLine(operation->coord_before, operation->coord_after, operation->lines, operation->text, cursor, head);
			break;
		}
		case OPERATION_GL: {
			redo_goToLine(operation->coord_after, cursor, head);
			break;
		}
		case OPERATION_REPLACE: {
			replaceText(operation->coord_before, operation->coord_after, operation->newtext, head);
			break;
		}
	}
}

/*functia realizeaza inlocuirea cuvantului vechi (old_word) cu cel nou (new_word)
intoarce coordonata primului caracter al cuvantului*/
Coord* replace(char* old_word, char* new_word, Cursor* cursor, Line **head) {
	Col* currentCol = cursor->col;
	Coord* coord_before = (Coord*)malloc(sizeof(Coord));
	coord_before->line = currentLinePosition(head, cursor);
	coord_before->col = currentColPosition(cursor);
	int count = 0;
	int i = 0;
	int length = strlen(old_word);

	while(currentCol->next != NULL) {

		/*se verifica daca valoarea nodului curent este egala cu
		primul caracter al cuvantului cautat in linie (old_word)*/
		if(currentCol->val == old_word[0]) {
			count = 1;
			//dupa ce s-a facut match pe primul caracter, se verifica concomitent urmatoarele caractere
			Col* col = currentCol->next;
			for(i = 1; i < length; i++) {
				if(col->val == old_word[i]) {
					count++;
					col = col->next;
				}
			}
			if(count == length) { //s-a gasit cuvantul de inlocuit
				cursor->col = currentCol;
				coord_before->col = currentColPosition(cursor); //salvez coordonata primului caracter al cuvantului
				for(i = 0; i < length; i++) { //realizez inlocuirea caracter cu caracter
					currentCol->val = new_word[i];
					currentCol = currentCol->next;
				}
				return coord_before;
			}	
		}
		currentCol = currentCol->next;
	}
	return coord_before;
}

/*in functie de comanda, functia face modificari asupra listei
functia intoarce 1 cand comanda este quit*/
int process_command(char *string, Cursor* cursor, Line **head, char* file, int *saveCounter, 
							OperationStack* stack, Coord* coord_before, Coord* coord_after) {
	int exit_program = 0;
	int lineNb = currentLinePosition(head, cursor);
	coord_before = coord_after;

	if(strncmp(string, "b", 1) == 0) {
		char* text = backspace(cursor); //caracterul sters in urma operatiei backspace
		coord_after = coordinates(head, cursor); //noua coordonata a cursorului
		push(stack, OPERATION_BACKSPACE, text, coord_before, coord_after, lineNb, NULL); //adaug in stiva informatii
		updateCounter(saveCounter, file, head); //verific daca trebuie realizat autosave
	}
	else if(strncmp(string, "q", 1) == 0) {
		exit_program = 1;
	}
		else if(strncmp(string, "gc", 2) == 0) {
			char *ptr = strtok(string, " \n"); //"gc"
			ptr = strtok(NULL, " \n"); //parametrul 1
			int colNb = atoi(ptr); //convertesc la int
			int lineNb = currentLinePosition(head, cursor); //linia curenta
			ptr = strtok(NULL, " \n");
			if(ptr != NULL) //daca exista parametrul 2
				lineNb = atoi(ptr);
			goToCharacter(colNb, lineNb, head, cursor); //efectuez operatia
			coord_after = coordinates(head, cursor); //noua coordonata
			push(stack, OPERATION_GC, NULL, coord_before, coord_after, lineNb, NULL); //adaug in stiva info
			updateCounter(saveCounter, file, head); //autosave
		}
			else if(strncmp(string, "gl", 2) == 0) {
				char *ptr = strtok(string, " \n");
				ptr = strtok(NULL, " \n");
				int lineNb = atoi(ptr);
				goToLine(lineNb, head, cursor);
				coord_after = coordinates(head, cursor);
				push(stack, OPERATION_GL, NULL, coord_before, coord_after, lineNb, NULL);
				updateCounter(saveCounter, file, head);
			}
				else if(strncmp(string, "dl", 2) == 0) {
					char *ptr = strtok(string, " \n");
					ptr = strtok(NULL, " \n");
					if(ptr != NULL)
						lineNb = atoi(ptr);
					char* text = deleteLine(lineNb, head, cursor);
					coord_after = coordinates(head, cursor);
					push(stack, OPERATION_DL, text, coord_before, coord_after, lineNb, NULL);
					updateCounter(saveCounter, file, head);
				}
					else if(strncmp(string, "d", 1) == 0) {
						char *ptr = strtok(string, " \n");
						int charNb = 1;
						ptr = strtok(NULL, " \n");
						if(ptr != NULL)
							charNb = atoi(ptr);
						char* text = deleteCaract(charNb, cursor);
						coord_after = coordinates(head, cursor);
						push(stack, OPERATION_D, text, coord_before, coord_after, lineNb, NULL);
						updateCounter(saveCounter, file, head);
					}
						else if(strncmp(string, "s", 1) == 0) {
							FILE* outputFile;
							outputFile = fopen(file, "w");
							saveFile(outputFile, head);
							fclose(outputFile);
						}
							else if(strncmp(string, "u", 1) == 0) {
								undo(stack, head, cursor);
							}
							else if(string[0] == 'r' && string[1] == '\n') {
									redo(stack, head, cursor);
								}
									else {
										char* copy = (char*)malloc(sizeof(char));
										strcpy(copy, string);
										copy[2] = '\0';
										char* old_word;
										char* new_word;
										if(strcmp(copy, "re") == 0) {
											char* ptr = strtok(string, " \n");
											ptr = strtok(NULL, " \n");
											old_word = ptr;
											ptr = strtok(NULL, " \n");
											new_word = ptr;
											coord_before = replace(old_word, new_word, cursor, head);
											coord_after->line = coord_before->line;
											coord_after->col = coord_before->col + strlen(old_word);
										}
										push(stack, OPERATION_REPLACE, old_word, coord_before, coord_after, lineNb, new_word);
									}
	//updatez coordonatele cursorului
	coord_after = coordinates(head, cursor);
	coord_before = coord_after;
	return exit_program;
}

int main(int argc, char *argv[]) {

	OperationStack* stack = initStack();
	int* saveCounter = (int*)malloc(sizeof(int));
	*saveCounter = 0;
	int *mode = (int*)malloc(sizeof(int));
	*mode = INSERT_MODE;
	Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
	Line **head = initLines(cursor);
	char *file = argv[1];
	char* string = (char*)malloc(MAX_STR_SIZE);
	char* inserted_lines = (char*)malloc(MAX_STR_SIZE * MAX_LINES_INSERTED);
	strcpy(inserted_lines, "");
	Coord* coord_before = coordinates(head, cursor);
	Coord* coord_after;
	
	while(1) {
		fgets(string, MAX_STR_SIZE, stdin);
		if(!process_line(string, mode)) {
			if(*mode == INSERT_MODE) {
				createLine(cursor, string, head);
				strcat(inserted_lines, string);
				
			}
			if(*mode == COMMAND_MODE) {
				coord_after = coordinates(head, cursor);
				if(process_command(string, cursor, head, file, saveCounter, stack, coord_before, coord_after))
					break;
				coord_before = coord_after;
			}
		}
		else {
			//s-a tastat ::i
			if(*mode == COMMAND_MODE) { //se trece din modul insert in modul comanda
				int lineNb = currentLinePosition(head, cursor);
				coord_after = coordinates(head, cursor);
				if (strlen(inserted_lines) != 0) {
					push(stack, OPERATION_INSERT, inserted_lines, coord_before, coord_after, lineNb, NULL);
				}
				strcpy(inserted_lines, "");
			} else { //se trece din modul comanda in modul insert
				coord_after = coordinates(head, cursor);
				coord_before = coord_after;
			}
		}
	}

	return 0;
}