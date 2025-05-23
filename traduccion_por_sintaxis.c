#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_JSON_SIZE 1024 // Define un tamaño máximo para el JSON 

typedef enum {
    L_CORCHETE, R_CORCHETE, L_LLAVE, R_LLAVE,
    COMA, DOS_PUNTOS, LITERAL_CADENA, LITERAL_NUM,
    PR_TRUE, PR_FALSE, PR_NULL, EOF_TOKEN
} Token;

// Declaraciones de funciones
void json();
void element();
void array();
void element_list();
void object();
void attributes_list();
void attribute();
void attribute_name();
void attribute_value();
void panicMode();
void printIndent(int level);
void nextToken();

// Variable global para el token actual
Token currentToken;
int indentLevel = 0;
char buffer[MAX_JSON_SIZE]; // Buffer para el contenido JSON
char stringValue[256]; // Para almacenar el valor de cadena
char numValue[256];    // Para almacenar el valor numérico
static int pos = 0; // Posición actual en el buffer

// Función para avanzar al siguiente token
void nextToken() {
    char currentChar;

    // Ignorar espacios en blanco
    while ((currentChar = buffer[pos]) != '\0' && isspace(currentChar)) {
        pos++;
    }

    // Si alcanzamos el final del buffer, retornamos EOF_TOKEN
    if (currentChar == '\0') {
        currentToken = EOF_TOKEN;
        return;
    }

    // Leer el siguiente carácter
    currentChar = buffer[pos];

    // Manejar diferentes tipos de tokens
    if (currentChar == '{') {
        currentToken = L_LLAVE;
        pos++;
    } else if (currentChar == '}') {
        currentToken = R_LLAVE;
        pos++;
    } else if (currentChar == '[') {
        currentToken = L_CORCHETE;
        pos++;
    } else if (currentChar == ']') {
        currentToken = R_CORCHETE;
        pos++;
    } else if (currentChar == ',') {
        currentToken = COMA;
        pos++;
    } else if (currentChar == ':') {
        currentToken = DOS_PUNTOS;
        pos++;
    } else if (currentChar == 't') { // true
        if (strncmp(&buffer[pos], "true", 4) == 0) {
            currentToken = PR_TRUE;
            pos += 4;
        } else {
            printf("Error: Token inesperado '%c'\n", currentChar);
            exit(EXIT_FAILURE);
        }
    } else if (currentChar == 'f') { // false
        if (strncmp(&buffer[pos], "false", 5) == 0) {
            currentToken = PR_FALSE;
            pos += 5;
        } else {
            printf("Error: Token inesperado '%c'\n", currentChar);
            exit(EXIT_FAILURE);
        }
    } else if (currentChar == 'n') { // null
        if (strncmp(&buffer[pos], "null", 4) == 0) {
            currentToken = PR_NULL;
            pos += 4;
        } else {
            printf("Error: Token inesperado '%c'\n", currentChar);
            exit(EXIT_FAILURE);
        }
    } else if (currentChar == '\"') { // Literal de cadena
        pos++; // Avanzar para saltar la comilla de apertura
        int start = pos;
        while (buffer[pos] != '\"' && buffer[pos] != '\0') {
            pos++;
        }
        if (buffer[pos] == '\"') {
            currentToken = LITERAL_CADENA;
            strncpy(stringValue, &buffer[start], pos - start);
            stringValue[pos - start] = '\0'; // Terminar la cadena
            pos++; // Saltar la comilla de cierre
        } else {
            printf("Error: Falta la comilla de cierre para la cadena\n");
            exit(EXIT_FAILURE);
        }
    } else if (isdigit(currentChar) || currentChar == '-') { // Literal numérico
        int start = pos;
        while (isdigit(buffer[pos]) || buffer[pos] == '.') {
            pos++;
        }
        currentToken = LITERAL_NUM;
        strncpy(numValue, &buffer[start], pos - start);
        numValue[pos - start] = '\0'; // Terminar el número
    } else {
        printf("Error: Token desconocido '%c'\n", currentChar);
        exit(EXIT_FAILURE); // Termina el programa en caso de error
    }
}

// Función para manejar el inicio de un JSON
void json() {
    if (currentToken == L_LLAVE) {
        object();
    } else if (currentToken == L_CORCHETE) {
        array();
    } else {
        printf("Error: Se esperaba un objeto o un array\n");
        exit(EXIT_FAILURE);
    }
}

// Función element -> object | array
void element() {
    if (currentToken == L_LLAVE) {
        object();
    } else if (currentToken == L_CORCHETE) {
        array();
    } else {
        printf("Error: Se esperaba un objeto o array.\n");
        panicMode();
    }
}

// Función array -> [element-list] | []
void array() {
    if (currentToken == L_CORCHETE) {
        nextToken(); // Avanzar después de '['
        while (currentToken != R_CORCHETE) {
            element_list();
            if (currentToken == COMA) {
                nextToken(); // Avanzar después de ','
            }
        }
        nextToken(); // Avanzar después de ']'
    }
}

// Función element_list -> element-list , element | element
void element_list() {
    indentLevel++;
    element(); // Cada elemento en la lista puede ser un objeto o un array
    indentLevel--;
}

// Función object -> {attributes-list} | {}
void object() {
    printIndent(indentLevel);
    printf("<item>\n");  // Abrir etiqueta XML para el item
    if (currentToken == L_LLAVE) {
        nextToken(); // Avanzar después de '{'
        attributes_list();
        nextToken(); // Avanzar después de '}'
    }
    printIndent(indentLevel);
    printf("</item>\n");  // Cerrar etiqueta XML para el item
}

// Función attributes_list -> attribute-list , attribute | attribute
void attributes_list() {
    attribute();
    while (currentToken == COMA) {
        nextToken(); // Avanzar después de ',' 
        attribute();
    }
}

// Función attribute -> attribute-name : attribute-value
void attribute() {
    attribute_name();
    if (currentToken == DOS_PUNTOS) {
        nextToken(); // Avanzar después de ':' 
        attribute_value();
    } else {
        printf("Error: Se esperaba ':' después de attribute-name.\n");
        panicMode();
    }
}

// Función attribute_name -> string
void attribute_name() {
    if (currentToken == LITERAL_CADENA) {
        printf("<%s>", stringValue); // Usar el nombre extraído
        nextToken(); // Avanzar después de la cadena
    } else {
        printf("Error: Se esperaba un nombre de atributo (cadena).\n");
        panicMode();
    }
}

// Función attribute_value -> element | string | number | true | false | null
void attribute_value() {
    if (currentToken == LITERAL_CADENA) {
        printf("<value>%s</value>\n", stringValue); // Imprimir valor de cadena en XML
        nextToken(); // Avanzar después del valor literal
    } else if (currentToken == LITERAL_NUM) {
        printf("<value>%s</value>\n", numValue); // Imprimir valor numérico en XML
        nextToken();
    } else if (currentToken == PR_TRUE) {
        printf("<value>true</value>\n");
        nextToken();
    } else if (currentToken == PR_FALSE) {
        printf("<value>false</value>\n");
        nextToken();
    } else if (currentToken == PR_NULL) {
        printf("<value>null</value>\n");
        nextToken();
    } else if (currentToken == L_LLAVE) {
        object();
    } else if (currentToken == L_CORCHETE) {
        array();
    } else {
        printf("Error: Valor inesperado.\n");
        panicMode();
    }
}

// Función panicMode
void panicMode() {
    while (currentToken != EOF_TOKEN) {
        if (currentToken == R_LLAVE || currentToken == R_CORCHETE) {
            nextToken();
            return;
        }
        nextToken();
    }
}

// Función para imprimir espacios en blanco para la indentación
void printIndent(int level) {
    for (int i = 0; i < level; i++) {
        printf("\t");
    }
}

int main() {
    // Leer el JSON desde un archivo o desde la entrada estándar
    printf("Ingrese el JSON (máximo %d caracteres):\n", MAX_JSON_SIZE);
    fgets(buffer, MAX_JSON_SIZE, stdin); // Lee la entrada JSON
    nextToken(); // Inicializa el primer token
    json(); // Analiza el JSON
    return 0;
}
