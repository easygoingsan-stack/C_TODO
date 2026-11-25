// TODO管理ソフト

#include <stdio.h>
#include <string.h>

#define MENU_COUNT (sizeof(menuName) / sizeof(menuName[0]))
#define MAX_TASKS 100

typedef struct {
    char task[100];
    int done;
} TODO;

typedef struct {
    TODO items[MAX_TASKS];
    int count;
} TODO_LIST;

void showMenu(void);
int choiceTodo(void);

const char* menuName[] = {"終了", "追加", "一覧", "一覧(未完了)",
                          "編集", "検索", "削除", "完了済削除",
                          "完了", "保存", "読込"};
void todoQuit(TODO_LIST*);
void todoAdd(TODO_LIST*);
void todoShow(TODO_LIST*);
void todoShow_unfinished(TODO_LIST*);
void todoEdit(TODO_LIST*);
void todoSearch(TODO_LIST*);
void todoDel(TODO_LIST*);
void todoDel_finished(TODO_LIST*);
void todoDone(TODO_LIST*);
void todoSave(TODO_LIST*);
void todoLoad(TODO_LIST*);

int confirm(void);
int readInt(int*);

void (*task[])(TODO_LIST*) = {
    todoQuit, todoAdd,    todoShow, todoShow_unfinished,
    todoEdit, todoSearch, todoDel,  todoDel_finished,
    todoDone, todoSave,   todoLoad};

int main(void) {
    TODO_LIST list = {{0}, 0};

    printf("====TODO管理ソフト=====\n");

    while (1) {
        int menu = choiceTodo();

        task[menu](&list);
        if (menu == 0) {
            break;
        }
    }
    return 0;
}

void showMenu(void) {
    int menuCount = 0;
    printf("メニューを選んでください\n");
    for (int i = 1; i < MENU_COUNT; i++) {
        printf("%d:%s ", i, menuName[i]);
        menuCount++;
        if (menuCount == 4) {
            printf("\n");
            menuCount = 0;
        }
    }
    printf("%d:%s\n", 0, menuName[0]);  // 終了は最後に表示
}

int choiceTodo(void) {
    int menu_num;
    int ch;

    while (1) {
        showMenu();
        printf("番号(0～%d)->", (int)MENU_COUNT - 1);
        int ret = scanf("%d", &menu_num);
        if (ret == EOF) {
            return 0;  // 終了扱いにする
        }
        if (ret != 1) {
            printf("数値を入力してください\n");
            while ((ch = getchar()) != '\n' && ch != EOF) {
                ;
            }
            continue;
        }

        // 入力行の残りをすべて読み捨てる
        while ((ch = getchar()) != '\n' && ch != EOF) {
            ;
        }

        if (menu_num < 0 || menu_num >= MENU_COUNT) {
            printf("正しい数値を入力してください\n");
            continue;
        }
        break;
    }
    printf("%d %sが選択されました\n", menu_num, menuName[menu_num]);

    return menu_num;
}

void todoAdd(TODO_LIST* list) {
    if (list->count >= MAX_TASKS) {
        printf("タスクの上限に達しました\n");
        return;
    }

    printf("追加するタスクを入力してください\n");
    printf(">");
    if (fgets(list->items[list->count].task,
              sizeof(list->items[list->count].task), stdin) == NULL) {
        printf("入力に失敗しました\n");
        return;
    }

    list->items[list->count]
        .task[strcspn(list->items[list->count].task, "\n")] = '\0';
    printf("%d:%sが追加されました\n", list->count + 1,
           list->items[list->count].task);
    list->items[list->count].done = 0;
    list->count++;
    return;
}

void todoShow(TODO_LIST* list) {
    if (list->count == 0) {
        printf("登録されていません\n");
    } else {
        for (int i = 0; i < list->count; i++) {
            printf("%d:[%s] %s\n", i + 1, list->items[i].done ? "○" : " ",
                   list->items[i].task);
        }
    }
    return;
}

void todoShow_unfinished(TODO_LIST* list) {
    int shown = 0;

    for (int i = 0; i < list->count; i++) {
        if (!(list->items[i].done)) {
            printf("%d:[%s] %s\n", i + 1, " ", list->items[i].task);
            shown++;
        }
    }
    if (shown == 0) {
        printf("未完了のタスクはありません\n");
    }
    return;
}

void todoDel(TODO_LIST* list) {
    if (list->count == 0) {
        printf("削除するタスクがありません\n");
        return;
    }
    printf("削除するタスクを選んでください\n");
    todoShow(list);
    printf("番号->");
    int num;
    if (!readInt(&num)) {
        printf("数値を入力してください\n");
        return;
    }
    if (num < 1 || num > list->count) {
        printf("正しい数値を入力してください\n");
        return;
    }
    printf("%sを削除します。よろしいですか?(Y/n)\n", list->items[num - 1].task);
    printf("->");
    if (!confirm()) {
        printf("中止しました\n");
        return;
    }

    for (int i = num; i < list->count; i++) {
        list->items[i - 1] = list->items[i];
    }
    list->count--;

    printf("削除されました\n");
    return;
}

void todoDel_finished(TODO_LIST* list) {
    int del_count = 0;
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].done) {
            printf("%d:[%s] %s\n", i + 1, "○", list->items[i].task);
            del_count++;
        }
    }
    if (del_count == 0) {
        printf("完了済タスクはありません\n");
        return;
    }
    printf("%d件の完了済タスクを削除します(Y/n)\n", del_count);
    printf("->");
    if (!confirm()) {
        printf("中止しました\n");
        return;
    }

    int dst = 0;
    for (int i = 0; i < list->count; i++) {
        if (!list->items[i].done) {
            list->items[dst++] = list->items[i];
        }
    }
    list->count = dst;

    printf("削除されました\n");
    return;
}

void todoSave(TODO_LIST* list) {
    if (list->count == 0) {
        printf("保存するタスクがありません\n");
        return;
    }
    printf("%d件のタスクを保存します。よろしいですか?(Y/n)\n", list->count);
    printf("->");
    if (!confirm()) {
        printf("中止しました\n");
        return;
    }
    FILE* fp;

    fp = fopen("todo1.dat", "w");
    if (fp == NULL) {
        printf("ファイルをオープンできませんでした\n");
        return;
    }
    for (int i = 0; i < list->count; i++) {
        fprintf(fp, "%d,%s\n", list->items[i].done, list->items[i].task);
    }
    if (fclose(fp) != 0) {
        printf("ファイルの保存に失敗しました\n");
        return;
    }
    printf("保存しました。\n");

    return;
}

void todoEdit(TODO_LIST* list) {
    if (list->count == 0) {
        printf("編集するリストがありません\n");
        return;
    }
    printf("編集するタスクを選んでください(0:中止)\n");
    todoShow(list);
    printf("番号->");
    int num;
    if (!readInt(&num)) {
        printf("数値を入力してください\n");
        return;
    }
    if (num == 0) {
        return;
    }
    if (num < 1 || num > list->count) {
        printf("正しい数値を入力してください\n");
        return;
    }
    printf("%d: %sを編集します\n", num, list->items[num - 1].task);
    printf("->");

    if (fgets(list->items[num - 1].task, sizeof(list->items[num - 1].task),
              stdin) == NULL) {
        printf("入力に失敗しました\n");
        return;
    }

    list->items[num - 1].task[strcspn(list->items[num - 1].task, "\n")] = '\0';
    printf("%d:%sに編集されました\n", num, list->items[num - 1].task);

    return;
}

void todoSearch(TODO_LIST* list) {
    char str[100];
    printf("検索語句を入力してください\n");
    printf("->");
    if (fgets(str, sizeof(str), stdin) == NULL) {
        printf("入力に失敗しました\n");
        return;
    }
    str[strcspn(str, "\n")] = '\0';  // 改行を削除
    if (strlen(str) == 0) {
        printf("入力がありません\n");
        return;
    }

    int found = 0;  // 検索結果の件数

    for (int i = 0; i < list->count; i++) {
        if (strstr(list->items[i].task, str) != NULL) {
            printf("%d:[%s] %s\n", i + 1, list->items[i].done ? "○" : " ",
                   list->items[i].task);
            found++;
        }
    }
    if (found == 0) {
        printf("該当するタスクはありません\n");
        return;
    }
    printf("%d件見つかりました\n", found);

    return;
}

void todoLoad(TODO_LIST* list) {
    printf("ファイルをロードします。よろしいですか?(Y/n)\n");
    printf("->");
    if (!confirm()) {
        printf("中止しました\n");
        return;
    }

    FILE* fp = fopen("todo1.dat", "r");
    if (fp == NULL) {
        printf("ファイルをオープンできませんでした\n");
        return;
    }

    list->count = 0;

    char line[128];
    while (list->count < MAX_TASKS && fgets(line, sizeof(line), fp) != NULL) {
        int done;
        char task[100];
        if (sscanf(line, "%d,%99[^\n]", &done, task) != 2) {
            continue;
        }

        task[strcspn(task, "\n")] = '\0';
        list->items[list->count].done = done;
        strcpy(list->items[list->count].task, task);
        list->count++;
    }

    fclose(fp);
    printf("%d件を読み込みました\n", list->count);
    return;
}

void todoQuit(TODO_LIST*) { return; }

int confirm(void) {
    char buf[16];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    return buf[0] == 'Y' || buf[0] == 'y';
}

// 1行読んで整数に変換する。成功したら1、失敗・EOFなら0を返す
int readInt(int* out) {
    char buf[32];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    if (strchr(buf, '\n') == NULL) {  // 長すぎる入力は残りを読み捨てる
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            ;
        }
        return 0;
    }
    return sscanf(buf, "%d", out) == 1;
}

void todoDone(TODO_LIST* list) {
    if (list->count == 0) {
        printf("タスク1件もありません\n");
        return;
    }
    printf("完了(取消)にするタスクを選んでください(0:中止)\n");
    todoShow(list);
    printf("番号->");
    int num;
    if (!readInt(&num)) {
        printf("数値を入力してください\n");
        return;
    }
    if (num == 0) {
        return;
    }
    if (num < 1 || num > list->count) {
        printf("正しい数値を入力してください\n");
        return;
    }
    list->items[num - 1].done = !(list->items[num - 1].done);
    printf("%sを%sにしました\n", list->items[num - 1].task,
           list->items[num - 1].done ? "完了" : "未完了");
    todoShow(list);
    return;
}
