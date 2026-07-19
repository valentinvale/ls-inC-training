#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_PATH 260
#define MAX_FILES 1024

typedef struct {
    char name[260];
    char modes[11];
    unsigned long links;
    long long size;
    char timebuf[64]; 
} FileInfo;

FileInfo files[MAX_FILES];
int count = 0;

int compare_files(const void *a, const void *b) {
    const FileInfo *file_a = a;
    const FileInfo *file_b = b;

    return _stricmp(file_a->name, file_b->name);
}

void mode_string(mode_t mode, char *str) {
    if (S_ISDIR(mode)) str[0] = 'd';
    else if (S_ISCHR(mode)) str[0] = 'c';
    else if (S_ISBLK(mode)) str[0] = 'b';
    else if (S_ISFIFO(mode)) str[0] = 'p';
    else str[0] = '-';

    str[1]  = (mode & S_IRUSR) ? 'r' : '-';
    str[2]  = (mode & S_IWUSR) ? 'w' : '-';
    str[3]  = (mode & S_IXUSR) ? 'x' : '-';

    str[4]  = (mode & S_IRGRP) ? 'r' : '-';
    str[5]  = (mode & S_IWGRP) ? 'w' : '-';
    str[6]  = (mode & S_IXGRP) ? 'x' : '-';

    str[7]  = (mode & S_IROTH) ? 'r' : '-';
    str[8]  = (mode & S_IWOTH) ? 'w' : '-';
    str[9]  = (mode & S_IXOTH) ? 'x' : '-';

    str[10] = '\0';
}

int get_file_info(const char *dir, const char *name, FileInfo *info) {
    char fullpath[MAX_PATH];

    int written = snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, name);

    if (written < 0 || written >= (int)sizeof(fullpath)) {
        fprintf(stderr, "Path too long: %s/%s\n", dir, name);
        return 0;
    }

    struct stat st;
    if (stat(fullpath, &st) < 0) {
        perror(name);
        return 0;
    }
    
    snprintf(info->name, sizeof(info->name), "%s", name);

    mode_string(st.st_mode, info->modes);

    info->links = (unsigned long)st.st_nlink;
    info->size = (long long)st.st_size;

    struct tm *tm_info = localtime(&st.st_mtime);

    if (tm_info == NULL) {
        snprintf(info->timebuf, sizeof(info->timebuf), "?");
    }
    else {
        strftime(info->timebuf, sizeof(info->timebuf), "%b %d %H:%M", tm_info);
    }

    return 1;
}

int show_all = 0;
int long_format = 0;

int main (int argc, char *argv[]) {

    int opt;

    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch(opt) {
            case 'a':
                show_all = 1;
                break;
            case 'l':
                long_format = 1;
                break;
            default:
                fprintf(stderr, "usage: %s [-al] [path]\n", argv[0]);
                return -1;
        }
    }

    const char *path = optind < argc ? argv[optind] : ".";

    DIR *dir = opendir(path);
    if (!dir) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        if (count >= MAX_FILES) {
            fprintf(stderr, "Too many files.\n");
            break;
        }

       if (get_file_info(path, entry->d_name, &files[count])) {
            count++;
       }
    }

    qsort(
        files,
        count,
        sizeof(files[0]),
        compare_files
    );

    int max_links_width = 1;
    int max_size_width = 1;
    
    for (int i = 0; i < count; i++) {
        int links_width = snprintf(NULL, 0, "%lu", files[i].links);
        int size_width = snprintf(NULL, 0, "%lld", files[i].size);
        
        if (max_links_width < links_width) {
            max_links_width = links_width;
        }

        if (max_size_width < size_width) {
            max_size_width = size_width;
        }
    }

    for (int i = 0; i < count; i++) {
        if (long_format) {
            printf(
                "%s %*lu %*lld %s %s\n",
                files[i].modes,
                max_links_width,
                files[i].links,
                max_size_width,
                files[i].size,
                files[i].timebuf,
                files[i].name
            );
        }
        else {
            printf("%s\n", files[i].name);
        }
    }

    closedir(dir);

    return 0;
}