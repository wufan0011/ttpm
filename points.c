#include "points_self.h"
#include "points.h"



int startMenu(void)
{
    struct playerInfo *p_playerInfo[PLAYERS_NUM_MAX];
    int playerNum;
    char startMenuSelection;
    int isQuit = NO;

    playerNum = readPlayerInfo(p_playerInfo);

    while (isQuit == NO) {
        system("clear");
        printf("[a]  增删选手\n");
        printf("[b]  赛前抽签\n");
        printf("[r]  录入成绩\n");
        printf("[e]  修改成绩\n");
        printf("[c]  计    算\n");
        printf("[p]  打    印\n");
        printf("[q]  离    开\n");
        printf("你的选择: ");

        scanf("%c", &startMenuSelection);
        while (getchar() != '\n')
            ;
        switch(startMenuSelection) {
            case 'a':
                while (addPlayer(p_playerInfo, &playerNum) == 0)
                    ;
                break;
            case 'b':
                ballot(p_playerInfo, playerNum);
                break;
            case 'r':
                recordOneGameResult(p_playerInfo, playerNum);
                break;
            case 'e':
                break;
            case 'c':
                break;
            case 'p':
                printInfo(p_playerInfo, playerNum);
                break;
            case 'q':
            default:
                isQuit = YES;
                break;
        }
    }

    writePlayerInfo(p_playerInfo, playerNum);
    freePlayerInfoStruct(p_playerInfo, playerNum);
    return 0;
}



int addPlayer(struct playerInfo* p_playerInfo[], int *p_playerNum)
{
    struct playerInfo *p_tmp;
    int addPlayerSelection;
    int playerNum = *p_playerNum;
    int deletePlayerNum;
    int i;
    char c;

    /* print players */
    printf("\n当前参赛选手:\n");
    printAllPlayerName(p_playerInfo, playerNum);

    /* select to add or delete a player */
    printf("[a]  添加选手\n");
    printf("[d]  删除选手\n");
    printf("[q]  离    开\n");
    printf("你的选择: ");
    addPlayerSelection = getchar();
    if (addPlayerSelection != '\n')
        while (getchar() != '\n')
            ;

    switch (addPlayerSelection) {
        case 'a':
            p_tmp = (struct playerInfo *)malloc(sizeof(struct playerInfo));
            /* read name */
            printf("姓名: ");
            scanf("%s", p_tmp->name);
            while (getchar() != '\n')
                ;
            /* read sex */
            printf("性别[m]/f: ");
            scanf("%c", &(p_tmp->sex));
            while (getchar() != '\n')
                ;
            /* init the new player */
            p_tmp->num = playerNum + 1;
            p_tmp->startWeek = getCurrentGameWeek();
            p_tmp->stopWeek = 0;
            p_tmp->level = DEFAULT_LEVEL;
            p_tmp->winNum = 0;
            p_tmp->failNum = 0;
            p_tmp->point = 0;
            p_tmp->rate = 0;
            p_tmp->rank = playerNum + 1;
            p_tmp->weekNum = 0;
            p_tmp->p_oneGameInfo = NULL;
            p_playerInfo[playerNum] = p_tmp;
            ++(*p_playerNum);
            break;
        case 'd':
            printf("序号: ");
            scanf("%d", &deletePlayerNum);
            while (getchar() != '\n')
                ;
            for (i = 0; i < playerNum; ++i)
                if (p_playerInfo[i]->num == deletePlayerNum)
                    break;
            if (i == playerNum) {
                printf("未找到序号为 %d 的选手\n", deletePlayerNum);
                printf("Enter键返回...");
                while (getchar() != '\n')
                    ;
            }
            else {
                printf("确定要删除序号为 %d 的选手y/[n]: ", deletePlayerNum);
                scanf("%c", &c);
                while (getchar() != '\n')
                    ;
                if (c == 'y' || c == 'Y')
                    p_playerInfo[i]->stopWeek = getCurrentGameWeek();
            }
            break;
        case 'q':
        default:
            return -1;
            break;
    }

    return 0;
}



int ballot(struct playerInfo *p_playerInfo[], int playerNum)
{
    int *p_ballotArray;
    int ballotPlayerNum = 0;
    int eachPlayerTimes;
    int anotherPlayerLine;
    int isFail = YES;
    int onePositionFail;
    int lines;
    int cols;
    int i;
    int j;
    int k;
    int l;
    int tryTimes;

    printf("当次比赛选手数: ");
    scanf("%d", &ballotPlayerNum);
    while (getchar() != '\n')
        ;
    printf("每位选手的比赛场数");
    if (ballotPlayerNum % 2 != 0)
        printf("(偶数)");
    printf(": ");
    scanf("%d", &eachPlayerTimes);
    while (getchar() != '\n')
        ;
    if (ballotPlayerNum % 2 != 0 && eachPlayerTimes % 2 != 0) {
        printf("当选手数是奇数时，每位选手的比赛场数必须是偶数...\n");
        printf("Enter键返回...");
        while (getchar() != '\n')
            ;
        return -2;
    }

    if (ballotPlayerNum > 0 && eachPlayerTimes > 0 &&
            eachPlayerTimes < ballotPlayerNum) {
        lines = ballotPlayerNum;
        cols = eachPlayerTimes + 1;
        p_ballotArray = (int *)calloc(cols * lines, sizeof(int));
    }
    else
        return -3;

    printf("\n全部参赛选手:\n");
    printPlayerName(p_playerInfo, playerNum);

    printf("输入当次比赛参赛选手序号，以空格分隔: ");
    for (i = 0; i < lines; ++i)
        scanf("%d", p_ballotArray + i*cols);
    while (getchar() != '\n')
        ;

    /* make sure everyone exist */
    for (i = 0; i < lines; ++i) {
        for (l = 0; l < playerNum; ++l)
            if (p_playerInfo[l]->num == *(p_ballotArray + i*cols))
                break;
        if (l == playerNum) {
            printf("\n\n%d号选手不存在...\n", *(p_ballotArray + i*cols));
            free(p_ballotArray);
            printf("Enter键返回...");
            while (getchar() != '\n')
                ;
            return -3;
        }
    }

    /* more random sort */
    srand(time(NULL));
    for (l = 0; l < RANDOM_LOOP * lines; ++l) {
        i = rand() % lines;
        j = rand() % lines;
        swapTwoIntNum(p_ballotArray + i*cols, p_ballotArray + j*cols);
    }

    /* ballot */
    while (isFail == YES) {
        /* clean ballot array */
        for (i = 0; i < lines; ++i)
            for (j = 1; j < cols; ++j)
                *(p_ballotArray + i*cols + j) = -1;
        /* try ballot */
        onePositionFail = NO;
        for (j = 1; j < cols; ++j) {
            for (i = 0; i < lines; ++i) {
                if (*(p_ballotArray + i*cols + j) == -1) {
                    tryTimes = 0;
                    anotherPlayerLine = rand() % lines;
                    while (tryTimes < lines) {
                        if (++anotherPlayerLine == lines)
                            anotherPlayerLine = 0;

                        /* not myself */
                        if (anotherPlayerLine == i) {
                            ++tryTimes;
                            continue;
                        }

                        /* have not play with */
                        for (k = 1; k < j; ++k)
                            if (*(p_ballotArray + i*cols + k) ==
                                    anotherPlayerLine)
                                break;
                        if (k != j) {
                            ++tryTimes;
                            continue;
                        }

                        if (*(p_ballotArray + anotherPlayerLine*cols + j)
                                == -1) {
                            *(p_ballotArray + anotherPlayerLine*cols + j) = i;
                            *(p_ballotArray + i*cols + j) = anotherPlayerLine;
                            break;
                        }
                        else if (j < cols-1 && *(p_ballotArray +
                                    anotherPlayerLine*cols + (j+1)) == -1) {
                            *(p_ballotArray + anotherPlayerLine*cols + (j+1)) =
                                    i;
                            *(p_ballotArray + i*cols + j) = anotherPlayerLine;
                            break;
                        }
                        else {
                            ++tryTimes;
                            continue;
                        }
                    }
                    if (tryTimes == lines) {
                        onePositionFail = YES;
                        break;
                    }
                }
            }
            if (onePositionFail == YES)
                break;
        }
        if (i == lines && j == cols)
            isFail = NO;
    }

    /* simplify the array and print the ballot result */
    for (i = 0; i < lines; ++i)
        for (j = 1; j < cols; ++j) {
            anotherPlayerLine = *(p_ballotArray + i*cols + j);
            /* simplify the array */
            if (anotherPlayerLine != -1) {
                for (k = 1; k < cols; ++k)
                    if (*(p_ballotArray + anotherPlayerLine*cols + k) == i) {
                        *(p_ballotArray + anotherPlayerLine*cols + k) = -1;
                        break;
                    }
            }
        }

    printBallotResult(p_playerInfo, playerNum, p_ballotArray, lines, cols);

    free(p_ballotArray);

    printf("\n");
    printf("Enter键返回...");
    while (getchar() != '\n')
        ;

    return 0;
}



int recordOneGameResult(struct playerInfo *p_playerInfo[], int playerNum)
{
    int oneGameResult[PLAYERS_NUM_MAX][3];
    int resultNum;
    char recordSourceSelection;

    printf("比赛场次从记录上次抽签结果的文件读入[y]/n: ");
    scanf("%c", &recordSourceSelection);
    if (recordSourceSelection == 'y' || recordSourceSelection == 'Y')
        resultNum = addOneGameResultFromBallotFile(p_playerInfo, playerNum,
                oneGameResult);
    else
        resultNum = addOneGameResult(p_playerInfo, playerNum, oneGameResult);
    if (resultNum == -2) {
        printf("输入有误\n\n");
        return -2;
    }
    calcFromOneNewGameResult(p_playerInfo, playerNum, oneGameResult, resultNum);
    printAllPlayerMainInfo(p_playerInfo, playerNum);

    return 0;
}



int printInfo(struct playerInfo *p_playerInfo[], int playerNum)
{
    char printInfoSelection;

    printf("[c]  最新信息\n");
    printf("[n]  最近单周\n");
    printf("[w]  单周信息\n");
    printf("[m]  单月信息\n");
    printf("[s]  单季信息\n");
    printf("[p]  单名选手\n");
    printf("[q]  离    开\n");
    printf("你的选择: ");
    scanf("%c", &printInfoSelection);
    while (getchar() != '\n')
        ;

    switch (printInfoSelection) {
        case 'c':
            printPlayerInfoByType(p_playerInfo, playerNum, PRINT_TYPE_CURRENT);
            break;
        case 'n':
            break;
        case 'w':
            break;
        case 'm':
            break;
        case 's':
            break;
        case 'p':
            break;
        case 'q':
        default:
            return -1;
            break;
    }

    return 0;
}



int printBallotResult(struct playerInfo *p_playerInfo[],
        int playerNum, int *p_ballotArray, int lines, int cols)
{
    FILE *fp_swp;
    char text[TEXT_LEN_MAX];
    int anotherPlayerLine;
    char c;
    int i;
    int j;
    int k;

    /* print ballot result and write to file */
    fp_swp = fopen(BALLOT_SWP_FILE_PATH, "w");
    printf("请输入本次抽签标识字串，比如 20111201 : ");
    scanf("%s", text);
    while (getchar() != '\n')
        ;
    fprintf(fp_swp, " %s ", text);
    fprintf(fp_swp, " %d", lines*(cols-1)/2);
    printf("抽签结果:\n");
    for (i = 0; i < lines; ++i)
        for (j = 1; j < cols; ++j) {
            anotherPlayerLine = *(p_ballotArray + i*cols + j);
            if (anotherPlayerLine == -1)
                continue;
            for (k = 0; k < playerNum; ++k)
                if (p_playerInfo[k]->num == *(p_ballotArray + i*cols)) {
                    fprintf(fp_swp, " %d", p_playerInfo[k]->num);
                    printf("(%c)%s", p_playerInfo[k]->level,
                            p_playerInfo[k]->name);
                    break;
                }
            printf("    VS    ");
            for (k = 0; k < playerNum; ++k)
                if (p_playerInfo[k]->num ==
                        *(p_ballotArray + anotherPlayerLine*cols)) {
                    fprintf(fp_swp, " %d", p_playerInfo[k]->num);
                    printf("%s(%c)", p_playerInfo[k]->name,
                            p_playerInfo[k]->level);
                    break;
                }
            printf("\n");
        }
    fprintf(fp_swp, "\n");
    fclose(fp_swp);

    printf("\n保存本次抽签结果到文件，覆盖上次抽签结果[y]/n: ");
    scanf("%c", &c);
    while (getchar() != '\n')
        ;
    if (c == 'y' || c == 'Y') {
        remove(BALLOT_FILE_PATH);
        rename(BALLOT_SWP_FILE_PATH, BALLOT_FILE_PATH);
    }

    return 0;
}



int calcFromOneNewGameResult(struct playerInfo *p_playerInfo[],
        int playerNum, int oneGameResult[][3], int resultNum)
{
    struct calcRankStruct calcRankStruct[PLAYERS_NUM_MAX];
    struct calcOneGameRankStruct calcOneGameRankStruct[PLAYERS_NUM_MAX];
    struct oneGameInfo *p_tmp;
    int eachLevelLowestRank[LEVEL_NUM-1];
    char anotherPlayerLevel;
    int actualPlayerNum = 0;
    int thisWeek;
    int found;
    int i;
    int j;
    int k = 0;
    int l;
    int m = 0;

    thisWeek = getCurrentGameWeek();
    for (i = 0; i < playerNum; ++i) {
        if (isThisWeekValid(thisWeek, p_playerInfo[i]->startWeek,
                    p_playerInfo[i]->stopWeek) == 0)
            continue;
        ++actualPlayerNum;
        found = 0;
        for (j = 0; j < resultNum; ++j) {
            if (oneGameResult[j][0] == p_playerInfo[i]->num
                    || oneGameResult[j][1] == p_playerInfo[i]->num) {
                /* create a new oneGameInfo */
                if (found == 0) {
                    p_tmp = p_playerInfo[i]->p_oneGameInfo;
                    if (p_tmp == NULL) {
                        p_playerInfo[i]->p_oneGameInfo = (struct oneGameInfo *)
                                malloc(sizeof(struct oneGameInfo));
                        p_tmp = p_playerInfo[i]->p_oneGameInfo;
                    }
                    else {
                        while (p_tmp->next != NULL)
                            p_tmp = p_tmp->next;
                        p_tmp->next = (struct oneGameInfo *)
                                malloc(sizeof(struct oneGameInfo));
                        p_tmp = p_tmp->next;
                    }
                    /* init the new oneGameInfo */
                    p_tmp->week = thisWeek;
                    p_tmp->levelBefore = p_playerInfo[i]->level;
                    p_tmp->winNum = 0;
                    p_tmp->failNum = 0;
                    p_tmp->pointBefore = p_playerInfo[i]->point;
                    p_tmp->point = 0;
                    p_tmp->rateBefore = p_playerInfo[i]->rate;
                    p_tmp->rankBefore = p_playerInfo[i]->rank;
                    p_tmp->rank = 0;
                    p_tmp->next = NULL;
                    ++(p_playerInfo[i]->weekNum);
                }
                ++found;
                /* calculate winner's point */
                if (oneGameResult[j][0] == p_playerInfo[i]->num) {
                    ++(p_tmp->winNum);
                    for (l = 0; l < playerNum; ++l)
                        if (p_playerInfo[l]->num == oneGameResult[j][1]) {
                            anotherPlayerLevel = p_playerInfo[l]->level;
                            break;
                        }
                    if (oneGameResult[j][2] == 2)
                        p_tmp->point += GIVE_UP_BASE_POINT;
                    else if (p_tmp->levelBefore > anotherPlayerLevel)
                        p_tmp->point += (WIN_BASE_POINT + WIN_STEP_POINT
                                * (p_tmp->levelBefore - anotherPlayerLevel));
                    else
                        p_tmp->point += WIN_BASE_POINT;
                }
                /* calculate loser's point */
                else {
                    ++(p_tmp->failNum);
                    /* if give up */
                    if (oneGameResult[j][2] != 0)
                        p_tmp->point += GIVE_UP_BASE_POINT;
                    else
                        p_tmp->point += FAIL_BASE_POINT;
                }
            }
        }
        /* update playerInfo */
        if (found != 0) {
            p_playerInfo[i]->winNum += p_tmp->winNum;
            p_playerInfo[i]->failNum += p_tmp->failNum;
            p_playerInfo[i]->point += p_tmp->point;
            if (p_playerInfo[i]->winNum + p_playerInfo[i]->failNum != 0)
                p_playerInfo[i]->rate = (double)p_playerInfo[i]->winNum /
                        (double)(p_playerInfo[i]->winNum +
                        p_playerInfo[i]->failNum);
            else
                p_playerInfo[i]->rate = 0;
        }
        /* for one game rank calculation */
        if (found != 0) {
            calcOneGameRankStruct[k].score = (double)p_tmp->point *
                    ((double)p_tmp->winNum / (double)(p_tmp->winNum +
                    p_tmp->failNum));
            calcOneGameRankStruct[k].p_oneGameInfo = p_tmp;
            ++k;
        }
        /* for rank calculation */
#if CUSTOM_SCORE
        calcRankStruct[m].score = customScore(p_playerInfo[i]->point,
                p_playerInfo[i]->rate);
#else
        calcRankStruct[m].score = defaultScore(p_playerInfo[i]->point,
                p_playerInfo[i]->rate);
#endif
        calcRankStruct[m].p_playerInfo = p_playerInfo[i];
        ++m;
    }
    /* calculate one game rank */
    qsort(calcOneGameRankStruct, k, sizeof(struct calcOneGameRankStruct),
            oneGameRankCmpFunction);
    for (l = 0; l < k; ++l)
        if (l > 0 && calcOneGameRankStruct[l].score ==
                calcOneGameRankStruct[l-1].score)
            calcOneGameRankStruct[l].p_oneGameInfo->rank =
                    calcOneGameRankStruct[l-1].p_oneGameInfo->rank;
        else
            calcOneGameRankStruct[l].p_oneGameInfo->rank = l+1;
    /* calculate rank */
    qsort(calcRankStruct, actualPlayerNum, sizeof(struct calcRankStruct),
            rankCmpFunction);
    for (l = 0; l < m; ++l)
        if (l > 0 && calcRankStruct[l].score ==
                calcRankStruct[l-1].score)
            calcRankStruct[l].p_playerInfo->rank =
                    calcRankStruct[l-1].p_playerInfo->rank;
        else
            calcRankStruct[l].p_playerInfo->rank = l+1;
    for (i = 0; i < playerNum; ++i)
        if (isThisWeekValid(thisWeek, p_playerInfo[i]->startWeek,
                    p_playerInfo[i]->stopWeek) == 0)
            p_playerInfo[i]->rank = -1;

    /* calculate level */
    for (j = 0; j < LEVEL_NUM-1; ++j)
        eachLevelLowestRank[j] =
                (int)((j+1) * (1.0/LEVEL_NUM) * actualPlayerNum);
    for (i = 0; i < playerNum; ++i) {
        if (p_playerInfo[i]->rank == -1)
            continue;
        for (j = 0; j < LEVEL_NUM-1; ++j)
            if (p_playerInfo[i]->rank <= eachLevelLowestRank[j])
                break;
        p_playerInfo[i]->level = 'A' + j;
    }

    return 0;
}



int addOneGameResult(struct playerInfo *p_playerInfo[], int playerNum,
        int oneGameResult[][3])
{
    FILE *fp;
    FILE *fp_weekDate;
    char c;
    int week;
    int year;
    int month;
    int day;
    int i;
    int resultNum;

    /* get xxxx-xx-xx */
    printf("年: ");
    scanf("%d", &year);
    while (getchar() != '\n')
        ;
    printf("月: ");
    scanf("%d", &month);
    while (getchar() != '\n')
        ;
    printf("日: ");
    scanf("%d", &day);
    while (getchar() != '\n')
        ;

    /* get result */
    for (i = 0; ; ++i) {
        system("clear");
        printPlayerName(p_playerInfo, playerNum);
        printf("第 %d 场\n", i+1);
        printf("胜方: ");
        scanf("%d", &oneGameResult[i][0]);
        while (getchar() != '\n')
            ;
        printf("负方: ");
        scanf("%d", &oneGameResult[i][1]);
        while (getchar() != '\n')
            ;
        printf("是否存在弃权y/[n]: ");
        scanf("%c", &c);
        while (getchar() != '\n')
            ;
        if (c == 'y' || c == 'Y') {
            printf("[a] -负方弃权    [c] -双方弃权: ");
            scanf("%c", &c);
            while (getchar() != '\n')
                ;
            switch (c) {
                case 'a':
                    oneGameResult[i][2] = 1;
                    break;
                case 'c':
                    oneGameResult[i][2] = 2;
                    break;
                default:
                    return -2;
                    break;
            }
        }
        else
            oneGameResult[i][2] = 0;
        printf("继续[y]/n: ");
        scanf("%c", &c);
        while (getchar() != '\n')
            ;
        if (c == 'n' || c == 'N')
            break;
    }
    resultNum = i+1;

    fp_weekDate = fopen(WEEK_DATE_FILE_PATH, "a");
    fp = fopen(DATA_FILE_PATH, "a");

    /* write week, xxxx-xx-xx */
    week = getCurrentGameWeek() + 1;
    if (fp_weekDate != NULL)
        fprintf(fp_weekDate, " %d %d %d %d \n", week, year, month, day);
    /* write result */
    if (fp != NULL) {
        fprintf(fp, " %d %d", week, resultNum);
        for (i = 0; i < resultNum; ++i)
            fprintf(fp, "  %d %d %d", oneGameResult[i][0],
                    oneGameResult[i][1], oneGameResult[i][2]);
        fprintf(fp, "\n");
    }

    fclose(fp_weekDate);
    fclose(fp);

    return resultNum;
}



int addOneGameResultFromBallotFile(struct playerInfo *p_playerInfo[],
        int playerNum, int oneGameResult[][3])
{
    FILE *fp;
    FILE *fp_weekDate;
    FILE *fp_ballot;
    char text[TEXT_LEN_MAX];
    int resultNum;
    int year;
    int month;
    int day;
    int week;
    int playerA;
    int playerB;
    char winPlayer;
    int i;
    int j;

    /* get xxxx-xx-xx */
    printf("年: ");
    scanf("%d", &year);
    while (getchar() != '\n')
        ;
    printf("月: ");
    scanf("%d", &month);
    while (getchar() != '\n')
        ;
    printf("日: ");
    scanf("%d", &day);
    while (getchar() != '\n')
        ;

    fp_ballot = fopen(BALLOT_FILE_PATH, "r");
    if (fp_ballot != NULL) {
        fscanf(fp_ballot, "%s", text);
        printf("现在使用标识字串为 %s 的抽签结果...\n", text);
        printf("  a -前者胜    A -前者胜，后者弃权    C -均弃权\n");
        printf("    a/A                b/B\n");
        fscanf(fp_ballot, "%d", &resultNum);
        for (i = 0; i < resultNum; ++i) {
            fscanf(fp_ballot, "%d %d", &playerA, &playerB);
            for (j = 0; j < playerNum; ++j)
                if (p_playerInfo[j]->num == playerA)
                    break;
            printf("[%2d]%s", playerA, p_playerInfo[j]->name);
            printf("    VS    ");
            for (j = 0; j < playerNum; ++j)
                if (p_playerInfo[j]->num == playerB)
                    break;
            printf("%s[%2d]", p_playerInfo[j]->name, playerB);
            printf("        胜方a/b/A/B/C: ");
            scanf("%c", &winPlayer);
            while (getchar() != '\n')
                ;
            switch (winPlayer) {
                case 'a':
                    oneGameResult[i][0] = playerA;
                    oneGameResult[i][1] = playerB;
                    oneGameResult[i][2] = 0;
                    break;
                case 'b':
                    oneGameResult[i][0] = playerB;
                    oneGameResult[i][1] = playerA;
                    oneGameResult[i][2] = 0;
                    break;
                case 'A':
                    oneGameResult[i][0] = playerA;
                    oneGameResult[i][1] = playerB;
                    oneGameResult[i][2] = 1;
                    break;
                case 'B':
                    oneGameResult[i][0] = playerB;
                    oneGameResult[i][1] = playerA;
                    oneGameResult[i][2] = 1;
                    break;
                case 'C':
                    oneGameResult[i][0] = playerA;
                    oneGameResult[i][1] = playerB;
                    oneGameResult[i][2] = 2;
                    break;
                default:
                    return -2;
                    break;
            }
        }
    }
    else {
        printf("\n不存在已保存的抽签结果\n\n");
        printf("Enter键返回...");
        while (getchar() != '\n')
            ;
    }
    fclose(fp_ballot);

    fp_weekDate = fopen(WEEK_DATE_FILE_PATH, "a");
    fp = fopen(DATA_FILE_PATH, "a");

    /* write week, xxxx-xx-xx */
    week = getCurrentGameWeek() + 1;
    if (fp_weekDate != NULL)
        fprintf(fp_weekDate, " %d %d %d %d \n", week, year, month, day);
    /* write result */
    if (fp != NULL) {
        fprintf(fp, " %d %d", week, resultNum);
        for (i = 0; i < resultNum; ++i)
            fprintf(fp, "  %d %d %d", oneGameResult[i][0],
                    oneGameResult[i][1], oneGameResult[i][2]);
        fprintf(fp, "\n");
    }

    fclose(fp_weekDate);
    fclose(fp);

    return resultNum;
}



int editOneGameResult(void)
{
}



void printPlayerName(struct playerInfo *p_playerInfo[], int playerNum)
{
    int i;

    for (i = 0; i < playerNum; ++i)
        if (p_playerInfo[i]->stopWeek == 0)
            printf("[%2d]    %s\n", p_playerInfo[i]->num,
                    p_playerInfo[i]->name);
}



void printAllPlayerName(struct playerInfo *p_playerInfo[], int playerNum)
{
    int i;

    for (i = 0; i < playerNum; ++i) {
        if (p_playerInfo[i]->stopWeek != 0)
            printf("%2d x", p_playerInfo[i]->stopWeek);
        else
            printf("    ");
        printf("[%2d]    %s\n", p_playerInfo[i]->num, p_playerInfo[i]->name);
    }
}



void printAllPlayerMainInfo(struct playerInfo *p_playerInfo[], int playerNum)
{
    int i;

    printf("序号");
    printf("    姓名");
    printf("  性别");
    printf("   胜");
    printf("   负");
    printf("  积分");
    printf("    胜率");
    printf("  积分X胜率");
    printf("  排名");
    printf("  级别");
    printf("\n\n");

    for (i = 0; i < playerNum; ++i) {
        printf("[%2d]", p_playerInfo[i]->num);
        printf("  %s", p_playerInfo[i]->name);
        if (p_playerInfo[i]->sex == 'm' || p_playerInfo[i]->sex == 'M')
            printf("    男");
        else
            printf("    女");
        printf("%5d", p_playerInfo[i]->winNum);
        printf("%5d", p_playerInfo[i]->failNum);
        printf("%6d", p_playerInfo[i]->point);
        printf("%7.1lf%%", p_playerInfo[i]->rate * 100);
        printf("%11.2lf", p_playerInfo[i]->point * p_playerInfo[i]->rate);
        printf("%6d", p_playerInfo[i]->rank);
        printf("%6c", p_playerInfo[i]->level);
        printf("\n");
    }
}



void printPlayerInfoByType(struct playerInfo *p_playerInfo[], int playerNum,
        int type)
{
    struct playerInfo *p_tmp[PLAYERS_NUM_MAX];
    char sortSelection;
    int firstWeek;
    int lastWeek;
    int actualPlayerNum = 0;
    int j;

    if (type == PRINT_TYPE_PLAYER) {
    }
    else {
        /* which week, month, or season */
        switch (type) {
            case PRINT_TYPE_WEEK:
                printf("[a]  周次\n");
                break;
            case PRINT_TYPE_MONTH:
                printf("[b]  月份\n");
                break;
            case PRINT_TYPE_SEASON:
                printf("[c]  季度\n");
                break;
            default:
                break;
        }

        /* get sort method */
        printf("[a]  默认排序\n");
        printf("[b]  积分排序\n");
        printf("[c]  胜率排序\n");
        printf("你的选择: ");
        scanf("%c", &sortSelection);
        while (getchar() != '\n')
            ;

        /* sort */
        switch (type) {
            case PRINT_TYPE_CURRENT:
                firstWeek = getCurrentGameWeek();
                lastWeek = firstWeek;
                actualPlayerNum = createActualPlayerInfoPointArray(
                        p_playerInfo, playerNum, p_tmp, firstWeek, lastWeek);
                switch (sortSelection) {
                    case 'a':
                        qsort(p_tmp, actualPlayerNum,
                                sizeof(struct playerInfo *),
                                currentInfoDefaultSortCmpFunction);
                        break;
                    case 'b':
                        qsort(p_tmp, actualPlayerNum,
                                sizeof(struct playerInfo *),
                                currentInfoPointSortCmpFunction);
                        break;
                    case 'c':
                        qsort(p_tmp, actualPlayerNum,
                                sizeof(struct playerInfo *),
                                currentInfoRateSortCmpFunction);
                        break;
                    default:
                        break;
                }
                break;
            case PRINT_TYPE_CURRENT_WEEK:
                break;
            case PRINT_TYPE_WEEK:
                break;
            case PRINT_TYPE_MONTH:
                break;
            case PRINT_TYPE_SEASON:
                break;
            default:
                break;
        }

        /* print table head */
        printf("序号");
        printf("    姓名");
        printf("  性别");
        printf("   胜");
        printf("   负");
        printf("  积分");
        printf("    胜率");
        printf("    成绩");
        printf("  排名");
        printf("  级别");
        printf("\n\n");

        /* print table */
        switch (type) {
            case PRINT_TYPE_CURRENT:
                for (j = 0; j < actualPlayerNum; ++j) {
                    printf("[%2d]", p_tmp[j]->num);
                    printf("  %s", p_tmp[j]->name);
                    if (p_tmp[j]->sex == 'm' || p_tmp[j]->sex == 'M')
                        printf("    男");
                    else
                        printf("    女");
                    printf("%5d", p_tmp[j]->winNum);
                    printf("%5d", p_tmp[j]->failNum);
                    printf("%6d", p_tmp[j]->point);
                    printf("%7.1lf%%", p_tmp[j]->rate * 100);
#if CUSTOM_SCORE
                    printf("%8.2lf", customScore(p_tmp[j]->point,
                                p_tmp[j]->rate));
#else
                    printf("%8.2lf", defaultScore(p_tmp[j]->point,
                                p_tmp[j]->rate));
#endif
                    printf("%6d", p_tmp[j]->rank);
                    printf("%6c", p_tmp[j]->level);
                    printf("\n");
                }
                break;
            case PRINT_TYPE_CURRENT_WEEK:
                break;
            case PRINT_TYPE_WEEK:
                break;
            case PRINT_TYPE_MONTH:
                break;
            case PRINT_TYPE_SEASON:
                break;
            default:
                break;
        }
    }

    printf("\n");
    printf("Enter键返回...");
    while (getchar() != '\n')
        ;
}



int readPlayerInfo(struct playerInfo *p_playerInfo[])
{
    FILE *fp;
    int i = -1;
    int j;
    struct oneGameInfo *p_tmp;

    fp = fopen(PLAYERS_FILE_PATH, "r");
    if (fp != NULL) {
        while (fgetc(fp) != EOF) {
            p_playerInfo[++i] = 
                    (struct playerInfo *)malloc(sizeof(struct playerInfo));
            fscanf(fp, "%d %d %d %s %c %c %d %d %d %lf %d %d",
                    &(p_playerInfo[i]->num),
                    &(p_playerInfo[i]->stopWeek),
                    &(p_playerInfo[i]->stopWeek),
                    p_playerInfo[i]->name,
                    &(p_playerInfo[i]->sex),
                    &(p_playerInfo[i]->level),
                    &(p_playerInfo[i]->winNum),
                    &(p_playerInfo[i]->failNum),
                    &(p_playerInfo[i]->point),
                    &(p_playerInfo[i]->rate),
                    &(p_playerInfo[i]->rank),
                    &(p_playerInfo[i]->weekNum));
            if (p_playerInfo[i]->weekNum != 0) {
                p_playerInfo[i]->p_oneGameInfo = (struct oneGameInfo *)
                        malloc(sizeof(struct oneGameInfo));
                p_tmp = p_playerInfo[i]->p_oneGameInfo;
                for (j = 0; j < p_playerInfo[i]->weekNum; ++j) {
                    if (j != 0) {
                        p_tmp->next = (struct oneGameInfo *)
                                malloc(sizeof(struct oneGameInfo));
                        p_tmp = p_tmp->next;
                    }
                    fscanf(fp, "%d %c %d %d %d %d %lf %d %d",
                            &(p_tmp->week),
                            &(p_tmp->levelBefore),
                            &(p_tmp->winNum),
                            &(p_tmp->failNum),
                            &(p_tmp->pointBefore),
                            &(p_tmp->point),
                            &(p_tmp->rateBefore),
                            &(p_tmp->rankBefore),
                            &(p_tmp->rank));
                    p_tmp->next = NULL;
                }
            }
            else
                p_playerInfo[i]->p_oneGameInfo = NULL;
            while (fgetc(fp) != '\n')
                ;
        }
    }
    fclose(fp);

    return i+1;
}



void writePlayerInfo(struct playerInfo *p_playerInfo[], int playerNum)
{
    FILE *fp;
    struct oneGameInfo *p_tmp;
    int i;
    int j;

    fp = fopen(PLAYERS_SWP_FILE_PATH, "w");
    if (fp != NULL)
        for (i = 0; i < playerNum; ++i) {
            fprintf(fp, " %d %d %d %s %c %c %d %d %d %lf %d %d",
                    p_playerInfo[i]->num,
                    p_playerInfo[i]->startWeek,
                    p_playerInfo[i]->stopWeek,
                    p_playerInfo[i]->name,
                    p_playerInfo[i]->sex,
                    p_playerInfo[i]->level,
                    p_playerInfo[i]->winNum,
                    p_playerInfo[i]->failNum,
                    p_playerInfo[i]->point,
                    p_playerInfo[i]->rate,
                    p_playerInfo[i]->rank,
                    p_playerInfo[i]->weekNum);
            if (p_playerInfo[i]->weekNum != 0) {
                p_tmp = p_playerInfo[i]->p_oneGameInfo;
                for (j = 0; j < p_playerInfo[i]->weekNum; ++j) {
                    fprintf(fp, " %d %c %d %d %d %d %lf %d %d",
                            p_tmp->week,
                            p_tmp->levelBefore,
                            p_tmp->winNum,
                            p_tmp->failNum,
                            p_tmp->pointBefore,
                            p_tmp->point,
                            p_tmp->rateBefore,
                            p_tmp->rankBefore,
                            p_tmp->rank);
                    p_tmp = p_tmp->next;
                }
            }
            fprintf(fp, "\n");
        }
    fclose(fp);
    remove(PLAYERS_FILE_PATH);
    rename(PLAYERS_SWP_FILE_PATH, PLAYERS_FILE_PATH);
}



void freePlayerInfoStruct(struct playerInfo *p_playerInfo[], int playerNum)
{
    struct oneGameInfo *p_tmp;
    struct oneGameInfo *p_tmpNext;
    int i;

    for (i = 0; i < playerNum; ++i) {
        if (p_playerInfo[i]->p_oneGameInfo != NULL) {
            p_tmp = p_playerInfo[i]->p_oneGameInfo;
            while (p_tmp->next != NULL) {
                p_tmpNext = p_tmp->next;
                free(p_tmp);
                p_tmp = p_tmpNext;
            }
            free(p_tmp);
        }
        free(p_playerInfo[i]);
    }
}



int createActualPlayerInfoPointArray(struct playerInfo *p_playerInfo[],
        int playerNum, struct playerInfo *p_actualPlayerInfo[],
        int firstWeek, int lastWeek)
{
    int i;
    int j;

    for (i = 0, j = 0; i < playerNum; ++i) {
        if (isThisWeekValid(lastWeek, p_playerInfo[i]->startWeek,
                p_playerInfo[i]->stopWeek) != 0)
            p_actualPlayerInfo[j++] = p_playerInfo[i];
    }

    return j;
}



int getCurrentGameWeek(void)
{
    FILE *fp;
    int week = 0;

    fp = fopen(WEEK_DATE_FILE_PATH, "r");
    if (fp != NULL)
        while (fgetc(fp) != EOF) {
            fscanf(fp, "%d", &week);
            while (fgetc(fp) != '\n')
                ;
        }
    fclose(fp);

    return week;
}



int rankCmpFunction(const void *p1, const void *p2)
{
    double p1_score = ((struct calcRankStruct *)p1)->score;
    double p2_score = ((struct calcRankStruct *)p2)->score;
    if (fabs(p1_score - p2_score) < 1e-5)
        return 0;
    else
        return (p1_score < p2_score) ? 1 : -1;
}



int oneGameRankCmpFunction(const void *p1, const void *p2)
{
    double p1_score = ((struct calcOneGameRankStruct *)p1)->score;
    double p2_score = ((struct calcOneGameRankStruct *)p2)->score;
    if (fabs(p1_score - p2_score) < 1e-5)
        return 0;
    else
        return (p1_score < p2_score) ? 1 : -1;
}



int currentInfoDefaultSortCmpFunction(const void *p1, const void *p2)
{
#if CUSTOM_SCORE
    double p1_score = customScore((*(struct playerInfo **)p1)->point,
            (*(struct playerInfo **)p1)->rate);
    double p2_score = customScore((*(struct playerInfo **)p2)->point,
            (*(struct playerInfo **)p2)->rate);
#else
    double p1_score = defaultScore((*(struct playerInfo **)p1)->point,
            (*(struct playerInfo **)p1)->rate);
    double p2_score = defaultScore((*(struct playerInfo **)p2)->point,
            (*(struct playerInfo **)p2)->rate);
#endif
    if (fabs(p1_score - p2_score) < 1e-5)
        return 0;
    else
        return (p1_score < p2_score) ? 1 : -1;
}



int currentInfoPointSortCmpFunction(const void *p1, const void *p2)
{
    int p1_point = (*(struct playerInfo **)p1)->point;
    int p2_point = (*(struct playerInfo **)p2)->point;
    return p2_point - p1_point;
}



int currentInfoRateSortCmpFunction(const void *p1, const void *p2)
{
    double p1_rate = (*(struct playerInfo **)p1)->rate;
    double p2_rate = (*(struct playerInfo **)p2)->rate;
    if (fabs(p1_rate - p2_rate) < 1e-5)
        return 0;
    else
        return (p1_rate < p2_rate) ? 1 : -1;
}



int isThisWeekValid(int thisWeek, int startWeek, int stopWeek)
{
    if (thisWeek <= startWeek || (stopWeek != 0 && thisWeek > stopWeek))
        return 0;
    else
        return 1;
}



void swapTwoIntNum(int *i, int *j)
{
    if (*i == *j)
        return;
    *i = *i ^ *j;
    *j = *i ^ *j;
    *i = *i ^ *j;
}



double defaultScore(int point, double rate)
{
    return point * rate;
}



double customScore(int point, double rate)
{
    return point * sqrt(rate);
}
