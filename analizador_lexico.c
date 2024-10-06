#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Función para detectar si es un número
int esNumero(char ch) {
    return isdigit(ch) || ch == '.'; // Detectar dígitos o punto decimal
}

// Función principal para procesar el archivo
void procesarArchivo(const char* fuente, const char* salida) {
    FILE *fuente_archivo = fopen(fuente, "r");
    FILE *salida_archivo = fopen(salida, "w");

    if (fuente_archivo == NULL) {
        perror("Error al abrir archivo fuente");
        return;
    }
    if (salida_archivo == NULL) {
        perror("Error al abrir archivo salida");
        return;
    }

    char ch;
    while ((ch = fgetc(fuente_archivo)) != EOF) {
        // Ignorar espacios en blanco
        if (isspace(ch)) {
            continue;
        }

        // Detectar llaves
        if (ch == '{') {
            fprintf(salida_archivo, "L_LLAVE\n");
        } else if (ch == '}') {
            fprintf(salida_archivo, "R_LLAVE\n");
        }
        // Detectar corchetes
        else if (ch == '[') {
            fprintf(salida_archivo, "L_CORCHETE\n");
        } else if (ch == ']') {
            fprintf(salida_archivo, "R_CORCHETE\n");
        }
        // Detectar comas y dos puntos
        else if (ch == ',') {
            fprintf(salida_archivo, "COMA\n");
        } else if (ch == ':') {
            fprintf(salida_archivo, "DOS_PUNTOS\n");
        }
        // Detectar cadenas (entre comillas dobles)
        else if (ch == '"') {
            fprintf(salida_archivo, "STRING ");
            // Leer hasta encontrar la comilla de cierre
            while ((ch = fgetc(fuente_archivo)) != '"' && ch != EOF) {
                // Ignorar contenido de la cadena
            }
            fprintf(salida_archivo, "\n");
        }
        // Detectar números
        else if (esNumero(ch)) {
            fprintf(salida_archivo, "NUMBER ");
            // Leer todos los caracteres del número
            while (esNumero(ch = fgetc(fuente_archivo))) {
                // Continuar hasta el final del número
            }
            ungetc(ch, fuente_archivo); // Devolver el último carácter no numérico
            fprintf(salida_archivo, "\n");
        }
        // Detectar palabras clave (true, false, null)
        else if (strncmp(&ch, "true", 4) == 0 || strncmp(&ch, "TRUE", 4) == 0) {
            fprintf(salida_archivo, "PR_TRUE\n");
            fseek(fuente_archivo, 3, SEEK_CUR); // Avanzar 3 caracteres adicionales
        } else if (strncmp(&ch, "false", 5) == 0 || strncmp(&ch, "FALSE", 5) == 0) {
            fprintf(salida_archivo, "PR_FALSE\n");
            fseek(fuente_archivo, 4, SEEK_CUR); // Avanzar 4 caracteres adicionales
        } else if (strncmp(&ch, "null", 4) == 0 || strncmp(&ch, "NULL", 4) == 0) {
            fprintf(salida_archivo, "PR_NULL\n");
            fseek(fuente_archivo, 3, SEEK_CUR); // Avanzar 3 caracteres adicionales
        }
        // En caso de error léxico
        else {
            fprintf(salida_archivo, "ERROR_LEXICO: %c\n", ch);
        }
    }

    // Cerrar archivos
    fclose(fuente_archivo);
    fclose(salida_archivo);
}

int main() {
    procesarArchivo("fuente.txt", "salida.txt");
    printf("Análisis léxico completado. Revisa el archivo salida.txt.\n");
    return 0;
}
