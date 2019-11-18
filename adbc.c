#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LINE_LEN 512
#define ADB_LOCATION "/platform-tools/adb"

struct device {
    char* id;
    char* state;
};

struct device_list {
    struct device* list;
    int count;
};

char* ADB = NULL;

char* get_sdk_path();
void read_adb_path();
char* get_adb_command(char* command);
struct device parse_device(char* s);
struct device_list get_devices();
struct device* select_device(struct device_list devices);
int exec_command(char* id, int argc, char* argv[]);

int main(int argc, char* argv[]) {
    read_adb_path();

    struct device_list devices = get_devices();

    if (devices.count == 0) {
        fprintf(stderr, "no devices attached\n");
        exit(1);
    } else if (devices.count == 1) {
        return exec_command(devices.list[0].id, argc, argv);
    }

    struct device* chosen = select_device(devices);
    if (chosen == NULL) {
        fprintf(stderr, "no device chosen\n");
        exit(1);
    }
    return exec_command(chosen->id, argc, argv);
}

char* get_sdk_path() {
    char* android_home = getenv("ANDROID_HOME");
    if (android_home != NULL && strlen(android_home) > 0) {
        return android_home;
    }  
    char* android_sdk_root = getenv("ANDROID_SDK_ROOT");
    if (android_sdk_root != NULL && strlen(android_sdk_root) > 0) {
        return android_sdk_root;
    }
    return NULL;
}

void read_adb_path() {
    char* sdk = get_sdk_path();
    if (sdk != NULL && strlen(sdk) > 0) {
        ADB = malloc(strlen(sdk) + strlen(ADB_LOCATION) + 1);
        ADB[0] = '\0';
        strcat(ADB, sdk);
        strcat(ADB, ADB_LOCATION);
    } else {
        // hope we have adb available in PATH
        ADB = "adb";
    }
}

char* get_adb_command(char* command) {
    char* result = malloc(strlen(ADB) + strlen(command) + 2);
    result[0] = '\0';
    strcat(result, ADB);
    strcat(result, " ");
    strcat(result, command);
    return result;
}

struct device parse_device(char* s) {
    // get device id
    char* token = strtok(s, "\t");
    char* id = malloc(strlen(token) + 1);
    strncpy(id, token, strlen(token));

    // get device state
    token = strtok(NULL, "\n");
    char* state = malloc(strlen(token) + 1);
    strncpy(state, token, strlen(token));

    struct device d = { id, state };
    return d;
}

struct device_list get_devices() {
    char* command = get_adb_command("devices");
    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "error while executing command '%s'\n", command);
        exit(1);
    }
    struct device* devices = malloc(sizeof(struct device));
    int device_count = 0;
    char line[MAX_LINE_LEN];

    // skip first line which is 'List of devices attached'
    fgets(line, MAX_LINE_LEN, fp);

    // read command output
    while(fgets(line, MAX_LINE_LEN, fp) != NULL) {
        // skip empty lines
        if (strlen(line) < 2) continue;
        struct device d = parse_device(line);
        if (device_count > 0) {
            // extend array
            devices = realloc(devices, sizeof(struct device) * (device_count + 1));
        }
        devices[device_count++] = d;
    }
    int ret = pclose(fp);
    if (ret != 0) {
        exit(1);
    }

    struct device_list result = { devices, device_count };
    return result;
}

struct device* select_device(struct device_list devices) {
    // run select dialog in a separate term window
    FILE* f = fopen("/dev/tty", "r+");
    SCREEN* s = newterm(NULL, f, f);
    set_term(s);
    noecho();
    printw("select a device:\n");
    for(int i = 0; i < devices.count; i++) {
        printw("  %d: %s\t%s\n", i + 1, devices.list[i].id, devices.list[i].state);
    }
    printw("  q: quit\n");
    refresh();
    while(true) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            endwin();
            exit(1);
        }
        int num = ch - '0';
        if (num > 0 && num <= devices.count) {
            endwin();
            return &devices.list[num - 1];
        }
    }
}

/*
    execute a given command with selected id, or just print selected id
*/
int exec_command(char* id, int argc, char* argv[]) {
    if (argc < 2) {
        // no additional arguments: just print selected device id
        printf("%s\n", id);
        return 0;
    }

    char* prefix = "-s ";
    // count length of the resulting command
    int len = strlen(prefix) + strlen(id); // no +1 for space after id, because it will be added in a loop
    for (int i = 1; i < argc; i++) {
        len += strlen(argv[i]) + 1;
    }

    char* command = malloc(len + 1); // +1 for null char
    command[0] = '\0';
    strcat(command, prefix);
    strcat(command, id);
    for(int i = 1; i < argc; i++) {
        strcat(command, " ");
        strcat(command, argv[i]);
    }
    return system(get_adb_command(command));
}