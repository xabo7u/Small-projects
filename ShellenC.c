#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

// Esta función crea un directorio con el nombre especificado.
// Recibe como parámetro 'dir_name', que es el nombre del directorio a crear.
// Los permisos predeterminados para el nuevo directorio son 0755 (lectura, escritura y ejecución para el propietario, solo lectura y ejecución para grupo y otros).
// En caso de error, utiliza perror para mostrar el mensaje de error.
void create_directory(char *dir_name) {
    if (mkdir(dir_name, 0755) == -1) {
        perror("mkdir");
    }
}

// Esta función elimina un directorio con el nombre especificado.
// Recibe como parámetro 'dir_name', que es el nombre del directorio a eliminar.
// Solo puede eliminar directorios vacíos. Si el directorio no está vacío o hay otros errores, mostrará el mensaje de error utilizando perror.
void delete_directory(char *dir_name) {
    if (rmdir(dir_name) == -1) {
        perror("rmdir");
    }
}

// Esta función crea un archivo con el nombre especificado.
// Recibe como parámetro 'file_name', que es el nombre del archivo a crear.
// Utiliza los flags O_CREAT (para crear el archivo si no existe) y O_WRONLY (para abrir el archivo en modo escritura).
// Los permisos predeterminados para el nuevo archivo son 0644 (lectura y escritura para el propietario, solo lectura para grupo y otros).
// En caso de error, utiliza perror para mostrar el mensaje de error.
void create_file(char *file_name) {
    int fd = open(file_name, O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        perror("open");
    } else {
        close(fd);
    }
}
// Esta función lista el contenido de un directorio con el nombre especificado.
// Recibe como parámetro 'dir_name', que es el nombre del directorio del cual se desea listar el contenido.
// Abre el directorio con opendir y verifica si se abrió correctamente.
// Itera sobre cada entrada del directorio con readdir y muestra el nombre de cada entrada.
// Cierra el directorio con closedir al finalizar.
// En caso de error al abrir el directorio, utiliza perror para mostrar el mensaje de error.
void list_directory_contents(char *dir_name) {
    DIR *d;
    struct dirent *dir;
    d = opendir(dir_name);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    } else {
        perror("opendir");
    }
}

// Esta función muestra el contenido de un archivo con el nombre especificado.
// Recibe como parámetro 'file_name', que es el nombre del archivo del cual se desea mostrar el contenido.
// Abre el archivo en modo lectura con open y verifica si se abrió correctamente.
// Lee el contenido del archivo en bloques de 1024 bytes con read y lo escribe en la salida estándar con write.
// Cierra el archivo con close al finalizar.
// En caso de error al abrir o leer el archivo, utiliza perror para mostrar el mensaje de error.
void show_file_contents(char *file_name) {
    int fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        perror("open");
    } else {
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }
        if (bytes_read == -1) {
            perror("read");
        }
        close(fd);
    }
}

// Esta función muestra un mensaje de ayuda con los comandos disponibles.
// Imprime los comandos disponibles en la consola utilizando printf.
// No recibe parámetros ni utiliza llamadas a sistemas.
void show_help() {
    printf("Available commands:\n");
    printf("mkdir <directory_name> - Create a directory\n");
    printf("rmdir <directory_name> - Delete a directory\n");
    printf("touch <file_name> - Create a file\n");
    printf("ls <directory_name> - List directory contents\n");
    printf("cat <file_name> - Show file contents\n");
    printf("chmod <permissions> <file_name> - Change file permissions\n");
    printf("help - Show this help message\n");
}

// Esta función cambia los permisos de un archivo con el nombre especificado.
// Recibe como parámetros 'permissions', que es una cadena que representa los nuevos permisos en formato octal (por ejemplo, "644"),
// y 'file_name', que es el nombre del archivo al cual se le cambiarán los permisos.
// Convierte la cadena 'permissions' a un modo numérico válido.
// Aplica los nuevos permisos al archivo con chmod.
// En caso de error, utiliza perror para mostrar el mensaje de error.
void change_permissions(char *permissions, char *file_name) {
    mode_t mode = 0;
    if (strlen(permissions) == 3) {
        mode = (permissions[0] - '0') * 64 + (permissions[1] - '0') * 8 + (permissions[2] - '0');
    } else {
        fprintf(stderr, "Invalid permissions format\n");
        return;
    }
    if (chmod(file_name, mode) == -1) {
        perror("chmod");
    }
}

// La función principal funciona como un intérprete de comandos básico para simular operaciones de sistema de archivos. 
// Utiliza un bucle infinito para esperar comandos del usuario desde la entrada estándar. 
// El programa continúa ejecutándose hasta que el usuario decide terminarlo. 
int main() {
    char command[256];
    char *args[3];
    while (1) {
        printf("> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;

        int i = 0;
        args[i] = strtok(command, " ");
        while (args[i] != NULL && i < 2) {
            args[++i] = strtok(NULL, " ");
        }

        if (args[0] == NULL) {
            continue;
        } else if (strcmp(args[0], "mkdir") == 0 && args[1] != NULL) {
            create_directory(args[1]);
        } else if (strcmp(args[0], "rmdir") == 0 && args[1] != NULL) {
            delete_directory(args[1]);
        } else if (strcmp(args[0], "touch") == 0 && args[1] != NULL) {
            create_file(args[1]);
        } else if (strcmp(args[0], "ls") == 0 && args[1] != NULL) {
            list_directory_contents(args[1]);
        } else if (strcmp(args[0], "cat") == 0 && args[1] != NULL) {
            show_file_contents(args[1]);
        } else if (strcmp(args[0], "chmod") == 0 && args[1] != NULL && args[2] != NULL) {
            change_permissions(args[1], args[2]);
        } else if (strcmp(args[0], "help") == 0) {
            show_help();
        } else {
            fprintf(stderr, "Unknown command\n");
        }
    }

    return 0;
}
