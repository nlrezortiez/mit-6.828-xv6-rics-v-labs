#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

/* find searches the directory tree rooted at each given starting-point by evaluating the
 * given expression from left to right. If no starting-point provided, "." is assumed.
 *  find [starting-point] -name <template>  
 *  find [starting-point] -iname <template> - case insensetive
 *  find [starting-point] -type <file_type>, file_type is one of: 
 *  f - regular file, d - directory, l - link, s - socket, 
 *  b - block type file, c - symbol-type file, p - named pipe
 */

enum { ENTRSZ = 512 };

void find(const char* path, const char* template) {
  char buf[ENTRSZ], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: open error\n");
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch (st.type) {
  case T_DEVICE:
  case T_FILE:
    printf("%s\n", path);
    break;
  case T_DIR:
    if (!template) {
      printf("%s\n", path);
    }
    
    if (strlen(path) + 1 + DIRSIZ + 1 >= ENTRSZ) {
      printf("find: path is too long\n");
      break;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof de) == sizeof de) {
      if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
        continue;
      }

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      
      if (st.type == T_DIR) {
        find(buf, template);
      } else {
        if (template) {
#ifdef WITH_STRSTR
          if (strstr(template, de.name)) {
#else
          if (strcmp(template, de.name) == 0) {
#endif
            printf("%s\n", buf);
          }
        } else {
          printf("%s\n", buf);
        }
      }
    }

    break;
  default:
    break;
  }
}

int main(int argc, char* argv[]) {
  int template_flag = 0, template_idx;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-name") == 0) {
      template_flag = 1;
      
      if (i == argc - 1) {
        fprintf(2, "template must be provided after \"-name\" ");
        exit(EXIT_FAILURE);
      }

      template_idx = i + 1;

      break;
    }
  }
    
  if (argc == 1) {
    find(".", 0);
    exit(EXIT_SUCCESS);
  }

  if (template_flag == 0) {
    for (int i = 1; i < argc; ++i) {
      find(argv[i], 0);
    }
  } else {
    // find [starting-point] -[name, iname] <template>
    for (int i = 1; i < template_idx - 1; ++i) {
      find(argv[i], argv[template_idx]);
    }
  }

  exit(EXIT_SUCCESS);
}
