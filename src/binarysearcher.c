#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>

struct wordsAndInformation {        /* Firstly All words and words's information stored. */
    char word[255];
    char fileName[255];
    struct wordsAndInformation *next;
};

struct addFileWords{            /* If Add command, so add command's filepath's words stored other linked list */
    char word[255];
    char fileName[255];
    struct addFileWords *next;
};

struct binaryTreeList {         /* for Created binary tree */
    char word[255];
    int *wordCount;
    int *depthWord;
    struct fileNameList *allFileNames;  /* Storing locations a word*/
    struct binaryTreeList *leftChild;
    struct binaryTreeList *rightChild;
};

struct fileNameList{        /* for words location */
    char fileName[255];
    int *fileWordCount;
    struct fileNameList *next;
};

struct commandText{     /* for command text */
    char line[255];
    struct commandText *next;
};

void allDir(char *dirNameRoot,char *dirName,struct wordsAndInformation **start, struct wordsAndInformation **end,FILE *outputFile);
void openfile( const char *dirnameRoot, const char *dirname, char *fileName,struct wordsAndInformation **start, struct wordsAndInformation **end,FILE *outputFile);
void addOpenFile(char *filePath,struct addFileWords **startExtraWord,struct addFileWords **endExtraWord,FILE *outputFile);
void addExtraWordList(struct addFileWords **startExtraWord, struct addFileWords **endExtraWord,char *filePath,char *word);
void addAllWordList(struct wordsAndInformation **start,struct wordsAndInformation **end, char *fileName, char *word);
void insertBinaryTree(struct wordsAndInformation *word,struct binaryTreeList **rootTree);
void preOrderPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile);
void ascPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile);
void dscPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile);
void searchWordInTree(struct binaryTreeList *rootTree,char *word,int *deptCount,FILE *outputFile);
struct binaryTreeList* removeWord(struct binaryTreeList *rootTree,char *word,char *removingWord,FILE *outputFile);
void freeTree(struct binaryTreeList *rootTree);

int main(int argc, char* argv[])
{
    char *dirName = argv[1];        /* Argument directory */
    char *inputFileName = argv[2];  /* Argument command text */
    struct wordsAndInformation *start = NULL, *end = NULL,*searchWords; /* for words linked list */
    struct addFileWords *startExtraWord = NULL , *endExtraWord = NULL ,*searchAddingExtraWord; /* for add command linked list */
    struct binaryTreeList *rootTree = NULL,*searchTree; /* for binary tree  */
    struct commandText *startLine = NULL,*endLine = NULL,*searchLine;   /* for command text */

    FILE *inputFile,*outputFile;
    char line[256];
    char *command,*filePathForAdd,*treeCommand,*ascOrDsc,*word;
    int wordLength,i,*deptWord = 1;
    char *lowerWord;
    struct commandText *tempLine;

    inputFile = fopen(inputFileName, "r");  /* open input and output text */
    outputFile = fopen("output.txt", "w");

    if(!inputFile){ /* if don't command.txt program exit */
        fprintf(outputFile,"File not Found.\n");
        printf("File not Found.\n");
        exit(1);
    }

    while(fgets(line,sizeof(line),inputFile)){  /* Read line by line command.txt */
        tempLine = (struct commandText *) malloc(sizeof(struct commandText));   /* commandText structer create memory  */
        strcpy(tempLine->line,line);
        if(startLine == NULL) { /* adding linked list for command text */
            startLine = tempLine;
            endLine   = tempLine;
        }
        else {
            endLine->next = tempLine;
            endLine = tempLine;
        }
    }

    allDir(dirName,dirName,&start,&end,outputFile);     /* Go allDir directory end Read all directory */

    searchWords = start;
    while(searchWords != NULL){     /* All word  */
        insertBinaryTree(searchWords,&rootTree);    /* Go insertBinaryTree function , then insert node tree */
        searchWords = searchWords->next;
    }

    searchLine = startLine;
    searchTree = rootTree;
    while(searchLine != NULL){          /* All command */
        command = strtok(searchLine->line," ");
        if(strcmp(command,"PRINT") == 0 ){
            treeCommand = strtok(NULL," \n");
            ascOrDsc = strtok(NULL," \n");
            if(ascOrDsc == NULL){   /* PRINT TREE  */
                fprintf(outputFile,"%s %s\n",command,treeCommand);
                printf("%s %s\n",command,treeCommand);
                preOrderPrintTree(rootTree,deptWord,outputFile);  /*  preOrder display word's information*/
            }
            else{
                if(strcmp(ascOrDsc,"ASC")==0){  /* PRINT TREE ASC  */
                    fprintf(outputFile,"%s %s %s\n",command,treeCommand,ascOrDsc);
                    printf("%s %s %s\n",command,treeCommand,ascOrDsc);
                    ascPrintTree(rootTree,deptWord,outputFile);   /* ascending display word's information */
                }
                else if(strcmp(ascOrDsc,"DSC")==0){ /* PRINT TREE DSC */
                    fprintf(outputFile,"%s %s %s\n",command,treeCommand,ascOrDsc);
                    printf("%s %s %s\n",command,treeCommand,ascOrDsc);
                    dscPrintTree(rootTree,deptWord,outputFile);   /* descending display word's information */
                }
            }
        }
        else if(strcmp(command,"SEARCH") == 0 ){    /* Search Word */
            word = strtok(NULL," \n");
            wordLength = strlen(word);
            lowerWord = (char *)malloc(wordLength +1);
            lowerWord[wordLength] = 0;

            for(i =0 ; i<wordLength ; i++){     /* Word change state which lowerCase */
                lowerWord[i] = tolower( word[i]);
            }
            fprintf(outputFile,"%s\n",command);
            printf("%s\n",command);
            searchWordInTree(rootTree,lowerWord,deptWord,outputFile);   /* Find searching word and display word's information */
        }
        else if(strcmp(command,"ADD") == 0){    /* Add text file command */
            filePathForAdd = strtok(NULL," \n");
            fprintf(outputFile,"%s %s\n",command,filePathForAdd);
            printf("%s %s\n",command,filePathForAdd);
            if(strstr(filePathForAdd,".txt")){
                addOpenFile(filePathForAdd,&startExtraWord,&endExtraWord,outputFile);   /* Open Added text file and add word that in this location text */
            }
            searchAddingExtraWord = startExtraWord; /* All extra word that is this location word in text file */
            while(searchAddingExtraWord != NULL){
                insertBinaryTree(searchAddingExtraWord,&rootTree);  /* extra word adding tree */
                searchAddingExtraWord = searchAddingExtraWord->next;
            }
        }
        else if(strcmp(command,"REMOVE") == 0){ /* remove command */
            fprintf(outputFile,"%s ",command);
            printf("%s ",command);
            word = strtok(NULL," \n");
            wordLength = strlen(word);
            lowerWord = (char *)malloc(wordLength +1);
            lowerWord[wordLength] = 0;

            for(i =0 ; i<wordLength ; i++){     /* Word change state which lowerCase */
                lowerWord[i] = tolower( word[i]);
            }
            rootTree = removeWord(rootTree,lowerWord,lowerWord,outputFile);
            fprintf(outputFile,"\n");
            printf("\n");
            //removeWord(&rootTree,lowerWord,outputFile);   /* removing word on tree */

        }
        searchLine = searchLine->next;
    }
    fclose(inputFile);  /* close input file */
    fclose(outputFile); /* close output file */

    /* All created memory , Free now */
    struct wordsAndInformation *tempWordForFree;
    searchWords = start;
    while(searchWords != NULL){
        tempWordForFree = searchWords;
        searchWords = searchWords->next;
        free(tempWordForFree);
    }
    start = NULL;

    struct addFileWords *tempFileWordForFree;
    searchAddingExtraWord = startExtraWord;
    while(searchAddingExtraWord != NULL){
        tempFileWordForFree = searchAddingExtraWord;
        searchAddingExtraWord = searchAddingExtraWord->next;
        free(tempFileWordForFree);
    }
    startExtraWord = NULL;

    struct commandText *tempCommandTextForFree;
    searchLine = startLine;
    while(searchLine != NULL){
        tempCommandTextForFree = searchLine;
        searchLine = searchLine->next;
        free(tempCommandTextForFree);
    }
    startLine = NULL;

    freeTree(rootTree);
    rootTree = NULL;

    return 0;
}

void freeTree(struct binaryTreeList *rootTree){
    if(rootTree != NULL){
        freeTree(rootTree->leftChild);
        freeTree(rootTree->rightChild);
        free(rootTree);
    }
}

struct binaryTreeList* removeWord(struct binaryTreeList *rootTree,char *word,char *removingWord,FILE *outputFile){
    if(rootTree == NULL){   /* If root is null, tree is free */
        return rootTree;
    }
    if(strcmp(rootTree->word,word) > 0){    /* if word smaller root's word, so go left child */
        rootTree->leftChild = removeWord(rootTree->leftChild,word,removingWord,outputFile);
    }
    else if(strcmp(rootTree->word,word) < 0){   /* if word greater root's word, so go right child */
        rootTree->rightChild = removeWord(rootTree->rightChild,word,removingWord,outputFile);
    }
    else{
        if(rootTree->leftChild == NULL && rootTree->rightChild == NULL){    /* Root haven't any child */
            fprintf(outputFile,"%s",removingWord);
            printf("%s",removingWord);
            free(rootTree); /* node free */
            rootTree = NULL;
            return rootTree;
        }
        if(rootTree->leftChild == NULL){    /* node haven't left child but have right child */
            fprintf(outputFile,"%s",removingWord);
            printf("%s",removingWord);
            struct binaryTreeList *tempNode = rootTree->rightChild;
            free(rootTree);
            return tempNode;
        }
        else if(rootTree->rightChild == NULL){  /* node haven't right child but have left child */
            fprintf(outputFile,"%s",removingWord);
            printf("%s",removingWord);
            struct binaryTreeList *tempNode = rootTree->leftChild;
            free(rootTree);
            return tempNode;
        }
        else{       /* If node have two child */
            struct binaryTreeList *tempNode = rootTree->rightChild;
            while( tempNode->leftChild != NULL){    /* The right child of the element to be deleted  finding min value node */
            tempNode = tempNode->leftChild;
            }
            strcpy(rootTree->word,tempNode->word);  /* rootTree information change min value node */
            rootTree->allFileNames = tempNode->allFileNames;
            rootTree->rightChild = removeWord(rootTree->rightChild,tempNode->word,removingWord,outputFile);
        }
    }
    return rootTree;
}

void searchWordInTree(struct binaryTreeList *rootTree,char *word,int *deptCount,FILE *outputFile)
{
    if(!rootTree){
        return; /* if node is null, don't anything and return */
    }
    int dept = 0;
        dept = deptCount;
        dept++;
    if(strcmp(rootTree->word,word) == 0){   /* found node */
        struct fileNameList *tempFile;
        int sumCount = 0;
        int fileCount = 0;
        tempFile = rootTree->allFileNames;
        while(tempFile != NULL){    /* Finding word count */
            fileCount = tempFile->fileWordCount;
            sumCount = sumCount + fileCount;
            tempFile = tempFile->next;
        }
        /* printing word's information */
        fprintf(outputFile,"%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        printf("%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){    /* finding word's location all text and display */
            fprintf(outputFile,"\t%s %d\n",tempFile->fileName,tempFile->fileWordCount);
            printf("\t%s %d\n",tempFile->fileName,tempFile->fileWordCount); /* printing word's fileName information */
            tempFile = tempFile->next;
        }
        return;
    }

    if(strcmp(rootTree->word,word) > 0){        /* don't find removing node and if searching node less this node , so left child */
        searchWordInTree(rootTree->leftChild,word,dept,outputFile);
    }else{                                       /* don't find removing node and if searching node less this node , so right child */
        searchWordInTree(rootTree->rightChild,word,dept,outputFile);
    }
}

void dscPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile)
{
    if(rootTree != NULL){   /* Node is not null */
        int dept = 0;
        dept = deptCount;
        dept++;
        dscPrintTree(rootTree->rightChild, dept,outputFile);     /* don't find removing node and if searching node less this node , so right child */
        struct fileNameList *tempFile;
        int sumCount = 0;
        int fileCount = 0;
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){    /* Finding word count */
            fileCount = tempFile->fileWordCount;
            sumCount = sumCount + fileCount;
            tempFile = tempFile->next;
        }
        /* printing word's  information */
        fprintf(outputFile,"%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        printf("%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){    /* finding word's location all text and display */
            fprintf(outputFile,"\t%s %d\n",tempFile->fileName,tempFile->fileWordCount);
            printf("\t%s %d\n",tempFile->fileName,tempFile->fileWordCount); /* printing word's fileName information */
            tempFile = tempFile->next;
        }
        dscPrintTree(rootTree->leftChild, dept,outputFile);  /* don't find removing node and if searching node less this node , so left child */
    }
}

void ascPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile)
{
    if(rootTree != NULL){   /* Node is not null */
        int dept = 0;
        dept = deptCount;
        dept++;
        ascPrintTree(rootTree->leftChild,dept,outputFile);  /* don't find removing node and if searching node less this node , so left child */
        struct fileNameList *tempFile;
        int sumCount = 0;
        int fileCount = 0;
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){     /* Finding word count */
            fileCount = tempFile->fileWordCount;
            sumCount = sumCount + fileCount;
            tempFile = tempFile->next;
        }
        /* printing word's information */
        fprintf(outputFile,"%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        printf("%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){     /* finding word's location all text and display */
            fprintf(outputFile,"\t%s %d\n",tempFile->fileName,tempFile->fileWordCount);
            printf("\t%s %d\n",tempFile->fileName,tempFile->fileWordCount);     /* printing word's fileName information */
            tempFile = tempFile->next;
        }
        ascPrintTree(rootTree->rightChild,dept,outputFile); /* don't find removing node and if searching node less this node , so right child */
    }
}

void preOrderPrintTree(struct binaryTreeList *rootTree, int *deptCount,FILE *outputFile)
{
    if(rootTree != NULL){   /* Node is not null */
        int dept = 0;
        dept = deptCount;
        dept++;
        struct fileNameList *tempFile;
        int sumCount = 0;
        int fileCount = 0;
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){    /* Finding word count */
            fileCount = tempFile->fileWordCount;
            sumCount = sumCount + fileCount;
            tempFile = tempFile->next;
        }
        /* Print word's information */
        fprintf(outputFile,"%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        printf("%s\n\t%d\n\t%d\n",rootTree->word,sumCount,deptCount);
        tempFile = rootTree->allFileNames;

        while(tempFile != NULL){     /* finding word's location all text and display */
            fprintf(outputFile,"\t%s %d\n",tempFile->fileName,tempFile->fileWordCount);
            printf("\t%s %d\n",tempFile->fileName,tempFile->fileWordCount); /* printing word's fileName information */
            tempFile = tempFile->next;
        }
        preOrderPrintTree(rootTree->leftChild,dept,outputFile);     /* don't find removing node and if searching node less this node , so left child return */
        preOrderPrintTree(rootTree->rightChild,dept,outputFile);    /* don't find removing node and if searching node less this node , so right child return */
    }
}

void insertBinaryTree(struct wordsAndInformation *word,struct binaryTreeList **rootTree)
{
    struct binaryTreeList *tempList = *rootTree, *newBinaryNode;
    int Count = 0;
    tempList = *rootTree;       /* Every calling this function, tempList equal rootTree always */
    newBinaryNode = (struct binaryTreeList *) malloc(sizeof(struct binaryTreeList));    /* Create new word and adding information */
    newBinaryNode->depthWord = 1;
    newBinaryNode->wordCount = 1;
    strcpy(newBinaryNode->word,word->word);
    newBinaryNode->leftChild = NULL;
    newBinaryNode->rightChild = NULL;

    if(tempList == NULL){   /* If there is no word, so first element linked list and root */
        *rootTree = newBinaryNode;  /* root equal new node */
        struct fileNameList *firstFileName;
        firstFileName = (struct fileNameList *) malloc(sizeof(struct fileNameList));    /* create and add first file name  */
        firstFileName->fileWordCount = 1;
        strcpy(firstFileName->fileName, word->fileName);
        (*rootTree)->allFileNames = firstFileName;
        return;
    }

    int treeDep = 2;
    while(tempList != NULL){    /* If there is a word, so new element adding tree */
        if(strcmp(tempList->word, word->word) > 0){ /* new element smaller previous element, so this left child */
            if(tempList->leftChild == NULL) {   /* If previous left child not null , control next left child and added */
                newBinaryNode->depthWord = treeDep; /* dept increasing */
                tempList->leftChild = newBinaryNode;    /* adding left child on tree */
            }else{
              tempList = tempList->leftChild;
            }
        }else if(strcmp(tempList->word, word->word) < 0){   /* new element smaller previous element, so this right child */
            if(tempList->rightChild == NULL) {   /* If previous right child not null , control next right child and added */
                newBinaryNode->depthWord = treeDep; /* dept increasing */
                tempList->rightChild = newBinaryNode;    /* adding right child on tree */
            }else{
              tempList = tempList->rightChild;
            }
        }
        else if(strcmp(tempList->word, word->word) == 0){   /* If the same word already exists , word's count increasing and new fileName adding */
            Count = tempList->wordCount;
            Count++;
            tempList->wordCount = Count;
            struct fileNameList *tempFileNames = tempList->allFileNames;

            if(tempFileNames == NULL){
                struct fileNameList *firstFileName;
                firstFileName = (struct fileNameList *) malloc(sizeof(struct fileNameList));    /* create new file */
                firstFileName->fileWordCount = 1;
                strcpy(firstFileName->fileName, word->fileName);
                tempList->allFileNames = firstFileName;
            }
            else{
                bool isFounded = false;
                while(tempFileNames != NULL){   /* this Word's  all fileName*/
                    if(strcmp(tempFileNames->fileName, word->fileName) == 0){   /* If the same file name already exists, file name count increasing */
                        isFounded = true;
                        int fileCount = 0;
                        fileCount = tempFileNames->fileWordCount;
                        fileCount++;
                        tempFileNames->fileWordCount = fileCount;
                        break;
                    }
                    tempFileNames = tempFileNames->next;
                }

                if(isFounded == false){ /* If the same file name not , new adding file name */
                    struct fileNameList *firstFileName;
                    firstFileName = (struct fileNameList *) malloc(sizeof(struct fileNameList));
                    firstFileName->fileWordCount = 1;
                    strcpy(firstFileName->fileName, word->fileName);
                    tempFileNames = tempList->allFileNames;

                    while(tempFileNames->next != NULL){ /* Finding last file name, and next element new file name */
                        tempFileNames = tempFileNames->next;
                    }
                    tempFileNames->next = firstFileName;
                }
            }
            break;
        }
        treeDep++;      /* increase tree dept */
    }
}

void addAllWordList(struct wordsAndInformation **start,struct wordsAndInformation **end, char *fileName, char *word)
{
    int i;
    struct wordsAndInformation *tempWord;
    tempWord = (struct wordsAndInformation *) malloc(sizeof(struct wordsAndInformation));   /* Create new word */
    int wordLength = strlen(word);
    char *lowerWord = (char *)malloc(wordLength +1);
    lowerWord[wordLength] = 0;

    for(i =0 ; i<wordLength ; i++){     /* Word change state which lowerCase */
        lowerWord[i] = tolower( word[i]);
    }
    strcpy(tempWord->word,lowerWord);
    strcpy(tempWord->fileName,fileName);
    if(*start == NULL) {    /* Adding Word in linked list */
        *start = tempWord;
        *end   = tempWord;
    }
    else {
        (*end)->next = tempWord;
        *end = tempWord;
    }
    free(lowerWord);
}

void addExtraWordList(struct addFileWords **startExtraWord, struct addFileWords **endExtraWord,char *filePath,char *word)
{
    int i;
    char *fileName ;
    struct addFileWords *tempWord;
    tempWord = (struct addFileWords *) malloc(sizeof(struct addFileWords)); /* Create new extra word  */
    int wordLength = strlen(word);
    char *lowerWord = (char *)malloc(wordLength +1);
    lowerWord[wordLength] = 0;

    for(i =0 ; i<wordLength ; i++){      /* Word change state which lowerCase */
        lowerWord[i] = tolower( word[i]);
    }
    strcpy(tempWord->word,lowerWord);
    strcpy(tempWord->fileName,filePath);
    if(*startExtraWord == NULL) {   /* adding word in extra word linked list */
        *startExtraWord = tempWord;
        *endExtraWord   = tempWord;
    }
    else {
        (*endExtraWord)->next = tempWord;
        *endExtraWord = tempWord;
    }
    free(lowerWord);
}

void addOpenFile(char *filePath,struct addFileWords **startExtraWord,struct addFileWords **endExtraWord,FILE *outputFile)
{
    FILE *inputFile;
    char line[2056];
    char *command;
    char *fileName;
    inputFile = fopen( filePath, "r" ); /* open add extra file */

    if(!inputFile){ /* Don't find add command file path , return main function */
        fprintf(outputFile,"File not Found.\n");
        printf("File not Found.\n");
        return;
    }
    while(fgets(line,sizeof(line),inputFile)){  /* read all line and split, then go extra word list function */
        command = strtok(line,".,;:()\" \r\n\t");
        while( command != NULL){
             addExtraWordList(startExtraWord,endExtraWord,filePath,command);    /* all word in a line reading and go function */
            command = strtok(NULL,".,;:()\" \r\n\t");
        }
    }
}

void openfile( const char *dirnameRoot, const char *dirname, char *fileName,struct wordsAndInformation **start, struct wordsAndInformation **end,FILE *outputFile)
{
    char pathname[1024];
    FILE *inputFile;
    char line[2056];
    char *command;
    sprintf( pathname, "%s/%s/%s",dirnameRoot, dirname , fileName); /* path name create information */
    inputFile = fopen( pathname, "r" ); /* open this file */

    if(!inputFile){ /* Don't find add command file path , return allDir function */
        fprintf(outputFile,"File not Found.\n");
        printf("File not Found.\n");
        return;
    }
    while(fgets(line,sizeof(line),inputFile)){  /* read all line and split, then go word list function */
        command = strtok(line,".,;:()\" \r\n\t");
        while( command != NULL){    /* all word in a line reading and go function */
             addAllWordList(start,end,fileName,command);
            command = strtok(NULL,".,;:()\" \r\n\t");
        }
    }
 }

void allDir(char *dirNameRoot,char *dirName,struct wordsAndInformation **start, struct wordsAndInformation **end,FILE *outputFile)
{
    DIR *dirPath;
    struct dirent *ent;
    struct stat statBuffer;

    if((dirPath = opendir(dirName)) == NULL) {  /* If don't open directory , return main function */
        fprintf(outputFile,"Does not open directory %s\n",dirName);
        printf("Does not open directory %s\n",dirName);
        return;
    }
    chdir(dirName); /* Validate the dirName */
    while((ent = readdir(dirPath)) != NULL) {   /* Read dirPath */
        lstat(ent->d_name,&statBuffer); /* If the path is a link, the link is not the file it refers to. */
        if(S_ISDIR(statBuffer.st_mode)) {   /* statBuffer is a directory */
            if(strcmp(".",ent->d_name) == 0 || strcmp("..",ent->d_name) == 0){  /* file name is . or .. pass */
                continue;
            }
            if(!strstr(dirNameRoot,dirName))    /* Change directory for text file */
                sprintf( dirNameRoot, "%s/%s", dirNameRoot, dirName);

            allDir(dirNameRoot,ent->d_name,start,end,outputFile);   /* Next directory go function for read */
        }
        else{
            if(strstr(ent->d_name,".txt")){ /* If a text file */
                if(strcmp(dirNameRoot, dirName) == 0){  /* text file in first directory, read this text file */
                    openfile(dirNameRoot,"",ent->d_name,start,end,outputFile);
                }else{  /*  text file not in first directory, read this text file   */
                    openfile(dirNameRoot,dirName,ent->d_name,start,end,outputFile);
                }
            }
        }
    }
    chdir("..");    /* Validate the .. */
    closedir(dirPath);
}
