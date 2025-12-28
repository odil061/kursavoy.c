#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#define MAX_NAME 50
#define MAX_INTERFACE 20
#define MAX_CONNECTION 20
#define MAX_RESOLUTION 20
#define MAX_CAMERAS 100
#define FILENAME "webcams.dat"
typedef struct {
    char model[MAX_NAME];               
    int megapixels;                     
    char resolution[MAX_RESOLUTION];   
    char interface[MAX_INTERFACE];      
    char connection_type[MAX_CONNECTION]; 
    int viewing_angle;                 
    int has_microphone;                 
} WebCamera;
WebCamera cameras[MAX_CAMERAS];
int camera_count = 0;
void print_menu();
void create_record();
void search_records();
void save_to_file();
void load_from_file();
void print_all_records();
void modify_record();
void add_to_file();
void sort_by_resolution();
void sort_by_field(int field_choice);
int compare_resolution(const void* a, const void* b);
void print_camera(WebCamera cam);
void clear_input_buffer();
int get_valid_int(const char* prompt, int min, int max);
float get_valid_float(const char* prompt, float min, float max);
void get_valid_string(const char* prompt, char* buffer, int max_len);
int validate_resolution(const char* resolution);
int main() {
    setlocale(LC_ALL, "RUS");

    printf("========================================\n");
    printf("     БАЗА ДАННЫХ ВЕБ-КАМЕР\n");
    printf("========================================\n\n");
    load_from_file();
    int choice;
    do {
        print_menu();
        printf("Выберите действие: ");
        scanf("%d", &choice);
        clear_input_buffer();
        switch (choice) {
        case 1:
            create_record();
            break;
        case 2:
            search_records();
            break;
        case 3:
            save_to_file();
            break;
        case 4:
            print_all_records();
            break;
        case 5:
            modify_record();
            break;
        case 6:
            add_to_file();
            break;
        case 7:
            printf("Завершение работы программы...\n");
            break;
        default:
            printf("Ошибка: неверный выбор. Попробуйте снова.\n");
        }
        printf("\n");
    } while (choice != 7);
    return 0;
}
void print_menu() {
    printf("=== ГЛАВНОЕ МЕНЮ ===\n");
    printf("1. Создать новую запись\n");
    printf("2. Поиск записей\n");
    printf("3. Сохранить все данные в файл\n");
    printf("4. Просмотр всех записей (с сортировкой)\n");
    printf("5. Изменить запись\n");
    printf("6. Добавить новые записи в файл\n");
    printf("7. Выход\n");
    printf("====================\n");
}
void create_record() {
    if (camera_count >= MAX_CAMERAS) {
        printf("Ошибка: достигнуто максимальное количество записей (%d)\n", MAX_CAMERAS);
        return;
    }
    printf("\n=== СОЗДАНИЕ НОВОЙ ЗАПИСИ ===\n");
    WebCamera new_cam;
    get_valid_string("Введите модель камеры: ", new_cam.model, MAX_NAME);
    new_cam.megapixels = get_valid_int("Введите число мегапикселей (1-50): ", 1, 50);
    do {
        get_valid_string("Введите разрешение (формат: 1920x1080): ",
            new_cam.resolution, MAX_RESOLUTION);
        if (!validate_resolution(new_cam.resolution)) {
            printf("Ошибка: неверный формат разрешения!\n");
        }
    } while (!validate_resolution(new_cam.resolution));
    get_valid_string("Введите интерфейс подключения (USB, HDMI, Wi-Fi): ",
        new_cam.interface, MAX_INTERFACE);
    get_valid_string("Введите тип подключения (проводное/беспроводное): ",
        new_cam.connection_type, MAX_CONNECTION);
    new_cam.viewing_angle = get_valid_int("Введите угол обзора (30-180 градусов): ", 30, 180);

    new_cam.has_microphone = get_valid_int("Наличие микрофона (0 - нет, 1 - есть): ", 0, 1);
    cameras[camera_count] = new_cam;
    camera_count++;
    printf("\nЗапись успешно создана! Всего записей: %d\n", camera_count);
}
void search_records() {
    if (camera_count == 0) {
        printf("База данных пуста. Сначала добавьте записи.\n");
        return;
    }
    printf("\n=== ПОИСК ЗАПИСЕЙ ===\n");
    printf("Поиск по:\n");
    printf("1. Углу обзора\n");
    printf("2. Интерфейсу подключения\n");
    printf("3. По обоим параметрам\n");
    printf("Выберите тип поиска: ");
    int search_type;
    scanf("%d", &search_type);
    clear_input_buffer();
    int viewing_angle = 0;
    char interface[MAX_INTERFACE] = "";
    int found_count = 0;
    if (search_type == 1 || search_type == 3) {
        printf("Введите угол обзора для поиска: ");
        scanf("%d", &viewing_angle);
        clear_input_buffer();
    }
    if (search_type == 2 || search_type == 3) {
        printf("Введите интерфейс подключения для поиска: ");
        fgets(interface, MAX_INTERFACE, stdin);
        interface[strcspn(interface, "\n")] = 0;
    }
    printf("\n=== РЕЗУЛЬТАТЫ ПОИСКА ===\n");
    for (int i = 0; i < camera_count; i++) {
        int match = 1;
        if (search_type == 1 || search_type == 3) {
            if (cameras[i].viewing_angle != viewing_angle) {
                match = 0;
            }
        }
        if (search_type == 2 || search_type == 3) {
            if (strcmp(cameras[i].interface, interface) != 0) {
                match = 0;
            }
        }
        if (match) {
            printf("\nЗапись #%d:\n", i + 1);
            print_camera(cameras[i]);
            found_count++;
        }
    }
    if (found_count == 0) {
        printf("Записи не найдены.\n");
    }
    else {
        printf("\nНайдено записей: %d\n", found_count);
    }
}
void save_to_file() {
    FILE* file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("Ошибка открытия файла для записи!\n");
        return;
    }
    fwrite(&camera_count, sizeof(int), 1, file);
    fwrite(cameras, sizeof(WebCamera), camera_count, file);
    fclose(file);
    printf("Данные успешно сохранены в файл '%s'. Записей: %d\n", FILENAME, camera_count);
}
void load_from_file() {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Файл '%s' не найден. Будет создан новый файл при сохранении.\n", FILENAME);
        return;
    }
    fread(&camera_count, sizeof(int), 1, file);
    if (camera_count > MAX_CAMERAS) {
        printf("Ошибка: файл содержит слишком много записей!\n");
        camera_count = 0;
        fclose(file);
        return;
    }
    fread(cameras, sizeof(WebCamera), camera_count, file);
    fclose(file);
    printf("Данные успешно загружены из файла '%s'. Загружено записей: %d\n",
        FILENAME, camera_count);
}
void print_all_records() {
    if (camera_count == 0) {
        printf("База данных пуста.\n");
        return;
    }
    printf("\n=== ВЫБОР ПОЛЯ ДЛЯ СОРТИРОВКИ ===\n");
    printf("1. Разрешение (по умолчанию)\n");
    printf("2. Модель\n");
    printf("3. Число мегапикселей\n");
    printf("4. Интерфейс\n");
    printf("5. Тип подключения\n");
    printf("6. Угол обзора\n");
    printf("7. Наличие микрофона\n");
    printf("Выберите поле для сортировки: ");
    int sort_choice;
    scanf("%d", &sort_choice);
    clear_input_buffer();
    WebCamera sorted_cameras[MAX_CAMERAS];
    memcpy(sorted_cameras, cameras, sizeof(WebCamera) * camera_count);
    sort_by_field(sort_choice);
    printf("\n=== ВСЕ ЗАПИСИ (ОТСОРТИРОВАННЫЕ) ===\n");
    printf("Всего записей: %d\n\n", camera_count);
    for (int i = 0; i < camera_count; i++) {
        printf("Запись #%d:\n", i + 1);
        print_camera(cameras[i]);
        printf("----------------------------------------\n");
    }
}
void modify_record() {
    if (camera_count == 0) {
        printf("База данных пуста.\n");
        return;
    }
    printf("\n=== ИЗМЕНЕНИЕ ЗАПИСИ ===\n");
    printf("Всего записей: %d\n", camera_count);
    printf("Введите номер записи для изменения (1-%d): ", camera_count);
    int record_num;
    scanf("%d", &record_num);
    clear_input_buffer();
    if (record_num < 1 || record_num > camera_count) {
        printf("Ошибка: неверный номер записи!\n");
        return;
    }
    int index = record_num - 1;
    printf("\nТекущие данные записи #%d:\n", record_num);
    print_camera(cameras[index]);
    printf("\nВведите новые данные:\n");
    get_valid_string("Введите модель камеры: ", cameras[index].model, MAX_NAME);
    cameras[index].megapixels = get_valid_int("Введите число мегапикселей (1-50): ", 1, 50);
    do {
        get_valid_string("Введите разрешение (формат: 1920x1080): ",
            cameras[index].resolution, MAX_RESOLUTION);
        if (!validate_resolution(cameras[index].resolution)) {
            printf("Ошибка: неверный формат разрешения!\n");
        }
    } while (!validate_resolution(cameras[index].resolution));
    get_valid_string("Введите интерфейс подключения: ",
        cameras[index].interface, MAX_INTERFACE);
    get_valid_string("Введите тип подключения: ",
        cameras[index].connection_type, MAX_CONNECTION);
    cameras[index].viewing_angle = get_valid_int("Введите угол обзора (30-180): ", 30, 180);
    cameras[index].has_microphone = get_valid_int("Наличие микрофона (0-1): ", 0, 1);
    printf("\nЗапись успешно изменена!\n");
}
void add_to_file() {
    printf("\n=== ДОБАВЛЕНИЕ ЗАПИСЕЙ В ФАЙЛ ===\n");
    int num_to_add;
    printf("Сколько записей вы хотите добавить? ");
    scanf("%d", &num_to_add);
    clear_input_buffer();
    if (num_to_add <= 0) {
        printf("Ошибка: неверное количество!\n");
        return;
    }
    if (camera_count + num_to_add > MAX_CAMERAS) {
        printf("Ошибка: превышение максимального количества записей!\n");
        printf("Доступно для добавления: %d\n", MAX_CAMERAS - camera_count);
        return;
    }
    for (int i = 0; i < num_to_add; i++) {
        printf("\nЗапись #%d:\n", camera_count + 1);
        create_record();
    }
    printf("\nДобавлено %d новых записей. Всего записей: %d\n",
        num_to_add, camera_count);
}
void sort_by_field(int field_choice) {
    switch (field_choice) {
    case 1: 
        qsort(cameras, camera_count, sizeof(WebCamera), compare_resolution);
        break;
    case 2:
        for (int i = 0; i < camera_count - 1; i++) {
            for (int j = i + 1; j < camera_count; j++) {
                if (strcmp(cameras[i].model, cameras[j].model) > 0) {
                    WebCamera temp = cameras[i];
                    cameras[i] = cameras[j];
                    cameras[j] = temp;
                }
            }
        }
        break;
    case 3: 
        for (int i = 0; i < camera_count - 1; i++) {
            for (int j = i + 1; j < camera_count; j++) {
                if (cameras[i].megapixels > cameras[j].megapixels) {
                    WebCamera temp = cameras[i];
                    cameras[i] = cameras[j];
                    cameras[j] = temp;
                }
            }
        }
        break;
    default:
        qsort(cameras, camera_count, sizeof(WebCamera), compare_resolution);
    }
}
int compare_resolution(const void* a, const void* b) {
    WebCamera* cam1 = (WebCamera*)a;
    WebCamera* cam2 = (WebCamera*)b;
    return strcmp(cam1->resolution, cam2->resolution);
}
void print_camera(WebCamera cam) {
    printf("Модель: %s\n", cam.model);
    printf("Мегапиксели: %d Мп\n", cam.megapixels);
    printf("Разрешение: %s\n", cam.resolution);
    printf("Интерфейс: %s\n", cam.interface);
    printf("Тип подключения: %s\n", cam.connection_type);
    printf("Угол обзора: %d°\n", cam.viewing_angle);
    printf("Микрофон: %s\n", cam.has_microphone ? "Есть" : "Нет");
}
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
int get_valid_int(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            clear_input_buffer();
            return value;
        }
        printf("Ошибка: введите целое число от %d до %d!\n", min, max);
        clear_input_buffer();
    }
}
void get_valid_string(const char* prompt, char* buffer, int max_len) {
    printf("%s", prompt);
    fgets(buffer, max_len, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    while (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == ' ') {
        buffer[strlen(buffer) - 1] = '\0';
    }
}
int validate_resolution(const char* resolution) {
    int x_count = 0;
    for (int i = 0; resolution[i]; i++) {
        if (resolution[i] == 'x' || resolution[i] == 'X') {
            x_count++;
        }
        else if (!isdigit(resolution[i])) {
            return 0;
        }
    }
    return x_count == 1;
}