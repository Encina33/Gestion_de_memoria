#include <windows.h>
#include <stdio.h>
#include <string.h>

#define SIZE 4096

int main() {
    // Crear un archivo de mapeo de memoria
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,   // Usar memoria del sistema
        NULL,                   // Seguridad predeterminada
        PAGE_READWRITE,         // Permisos de lectura y escritura
        0,                      // Tamaño máximo (alto)
        SIZE,                   // Tamaño máximo (bajo)
        "SharedMemoryExample"); // Nombre del objeto de memoria compartida

    if (hMapFile == NULL) {
        fprintf(stderr, "Error al crear la memoria compartida: %ld\n", GetLastError());
        return 1;
    }

    // Mapeo de memoria en el proceso padre
    char *shared_memory = (char *)MapViewOfFile(
        hMapFile,           // Manejador del objeto de memoria compartida
        FILE_MAP_ALL_ACCESS,// Permisos de acceso
        0,                  // Offset alto
        0,                  // Offset bajo
        SIZE);              // Tamaño a mapear

    if (shared_memory == NULL) {
        fprintf(stderr, "Error al mapear la memoria compartida: %ld\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // Crear un proceso hijo
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if (!CreateProcess(
            NULL,                       // Nombre del ejecutable
            "child_process",            // Línea de comandos (nombre del proceso hijo)
            NULL,                       // Seguridad del proceso
            NULL,                       // Seguridad del hilo
            FALSE,                      // No heredar manejadores
            0,                          // Opciones de creación
            NULL,                       // Heredar entorno
            NULL,                       // Directorio actual
            &si,                        // Información de inicio
            &pi)) {                     // Información del proceso
        fprintf(stderr, "Error al crear el proceso hijo: %ld\n", GetLastError());
        UnmapViewOfFile(shared_memory);
        CloseHandle(hMapFile);
        return 1;
    }

    // Escribir en la memoria compartida
    strcpy(shared_memory, "Hello, child process!");

    // Esperar a que el proceso hijo termine
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Limpieza
    UnmapViewOfFile(shared_memory);
    CloseHandle(hMapFile);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
