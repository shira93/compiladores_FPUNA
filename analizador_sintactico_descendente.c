// BNF JSON Grammar functions
void json();
void element();
void array();
void element_list();
void object();
void attributes_list();
void attribute();
void attribute_name();
void attribute_value();

// Variable global para el token actual
Token currentToken;

// Función para avanzar al siguiente token
void nextToken() {
    currentToken = getToken();
}

// Función json -> element eof
void json() {
    // Validar que el archivo comienza con un elemento válido (un objeto o un array)
    if (currentToken != L_LLAVE && currentToken != L_CORCHETE) {
        printf("Error: Se esperaba que el archivo comenzara con '{' o '['.\n");
        return;
    }

    // Procesar el elemento
    element();

    // Comprobar si realmente ha llegado el fin del archivo
    if (currentToken != EOF) {
        printf("Error: Se esperaba el fin de archivo.\n");
    }
}

// Función element -> object | array
void element() {
    if (currentToken == L_LLAVE) {
        object();
    } else if (currentToken == L_CORCHETE) {
        array();
    } else {
        printf("Error: Se esperaba un object o array.\n");
    }
}

// Función element_list -> element-list , element | element
void element_list() {
    element();
    while (currentToken == COMA) { // Coma separa elementos
        nextToken(); // Avanzar después de ','
        element();
    }
}

// Función array -> [element-list] | []
void array() {
    if (currentToken == L_CORCHETE) {
        nextToken(); // Avanzar después de '['
        if (currentToken != R_CORCHETE) {
            element_list();
            if (currentToken != R_CORCHETE) {
                printf("Error: Se esperaba ']' al final del array.\n");
            }
        }
        nextToken(); // Avanzar después de ']'
    }
}

// Función object -> {attributes-list} | {}
void object() {
    if (currentToken == L_LLAVE) {
        nextToken(); // Avanzar después de '{'
        if (currentToken != R_LLAVE) {
            attributes_list();
            if (currentToken != R_LLAVE) {
                printf("Error: Se esperaba '}' al final del object.\n");
            }
        }
        nextToken(); // Avanzar después de '}'
    }
}

// Función attributes_list -> attribute-list , attribute | attribute
void attributes_list() {
    attribute();
    while (currentToken == COMA) { // Coma separa atributos
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
        
        // Modo pánico: avanzar hasta un token seguro para continuar
        while (currentToken != COMA && currentToken != R_LLAVE && currentToken != EOF) {
            nextToken();
        }
    }
}

// Función attribute_name -> string
void attribute_name() {
    if (currentToken == LITERAL_CADENA) {
        nextToken(); // Avanzar después de la cadena
    } else {
        printf("Error: Se esperaba un nombre de atributo (cadena).\n");

        // Modo pánico: avanzar hasta un token seguro para continuar
        while (currentToken != DOS_PUNTOS && currentToken != COMA && currentToken != R_LLAVE && currentToken != EOF) {
            nextToken();
        }
    }
}

// Función attribute_value -> element | string | number | true | false | null
void attribute_value() {
    if (currentToken == LITERAL_CADENA || currentToken == LITERAL_NUM ||
        currentToken == PR_TRUE || currentToken == PR_FALSE || currentToken == PR_NULL) {
        nextToken(); // Avanzar después del valor literal
    } else if (currentToken == L_LLAVE) {
        object();
    } else if (currentToken == L_CORCHETE) {
        array();
    } else {
        printf("Error: Valor de atributo inválido.\n");

        // Modo pánico: avanzar hasta un token seguro para continuar
        while (currentToken != COMA && currentToken != R_LLAVE && currentToken != R_CORCHETE && currentToken != EOF) {
            nextToken();
        }
    }
}

int main() {
    // Inicializar analizador léxico con archivo de entrada
    abrirArchivo("archivo.json"); // Asegúrate de que esta función lea el archivo de entrada
    nextToken(); // Obtener el primer token

    // Comenzar el análisis sintáctico
    json();

    // Finalizar
    printf("Análisis completado.\n");
    cerrarArchivo();
    return 0;
}
