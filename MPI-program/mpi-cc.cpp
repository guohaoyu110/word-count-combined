#include <mpi.h>
#include <sys/stat.h>
#include <math.h>
#include <string>
#include <bits/stdc++.h>

using namespace std;
#define MEMORY  1083741824   

size_t lastPos = 0;
long long int buff_size;


struct pairs{
    int count;
    char character;
};


char* readFile(FILE* file, size_t fileSize) {

    long long readsize = min((size_t)buff_size, fileSize - lastPos);    
    if(0 >= readsize) {
        return NULL;
    }

    char* str = new char[readsize+1];
    long start = 0;
    fseek(file, lastPos, SEEK_SET);
    fread(str, 1, readsize, file);

    if(readsize > 50) {
        start = readsize - 50;
    }

    char* currptr = strchr(&str[start], ' '), *pre = NULL;
    
    if(readsize < buff_size || currptr == NULL) {
        lastPos = fileSize;
        str[readsize] = '\0';
        return str;
    }

    while (!(currptr == NULL))
    {
        pre = currptr;
        currptr = strchr(currptr + 1, ' ');
    }
    int rSize = pre - str + 1;
    if(!(pre == NULL)) {
        *pre = '\0';
    }

    lastPos += rSize;
    return str;
}



int main(int argc, char **argv) {

    int nTotalLines = 0, blocks[2] = {1, 1};
    MPI_Aint charex, intex, displacements[2];
    MPI_Datatype obj_type, types[2] = {MPI_INT, MPI_CHAR};
    int max_count = 1, min_count = 1;
    buff_size = MEMORY;
    double startTime = 0;
    int nTasks, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nTasks);
    MPI_Comm_rank( MPI_COMM_WORLD , &rank);
    MPI_Type_extent(MPI_INT, &intex);
    MPI_Type_extent(MPI_CHAR, &charex);
    displacements[0] = (MPI_Aint)(0);
    displacements[1] = intex;
    MPI_Type_struct(2, blocks, displacements, types, &obj_type);
    MPI_Type_commit(&obj_type);


    char *fileBuf;
    long long int *StartId;
    startTime = MPI_Wtime();

    //Read the file

    size_t fileSize;
    FILE* file;

    //work for master node
    if(rank == 0) {
        printf("opening the file\n");
        if(argc >= 2) {
            file = fopen(argv[1], "r+");
        }
        else {
            file = fopen("gutenberg-1G.txt", "r+");
        }
        struct stat filestatus;
        if(argc >= 2) {
            stat(argv[1], &filestatus);
        }
        else {
            stat("gutenberg-1G.txt", &filestatus);
        }
        fileSize = filestatus.st_size;
        printf("opened the file \n");
    }

    double totalTime_noRead = 0, totalTime_NoDist = 0;
    map<char, int> totalHashMap;

    while(true) {
        int status = 1;
        if(rank == 0) {
            StartId = new long long int[buff_size/10];
            fileBuf = readFile(file, fileSize);
            StartId[0] = 0;
            if(fileBuf == NULL) {
                status = 0;
            }
        }
        nTotalLines = 0;
        MPI_Bcast(&status, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("status sent from 0 \n");
        if(status == 0) {
            break;
        }

        //Mapping operation: all other process gets the data through parent process 0

        if(rank == 0) {
            char* currptr = strchr(fileBuf, 10);
            while (!(currptr == NULL))
            {
                StartId[++nTotalLines] = 1+currptr-fileBuf;
                currptr = strchr(currptr+1, 10);
            }
            StartId[nTotalLines+1] = strlen(fileBuf);
        }

        double startTime_noRead = MPI_Wtime();

        char* buffer = NULL;
        int totalChars = 0, portion = 0, startNum = 0, endNum = 0;

        if(rank == 0) {
            startNum = 0;
            portion = nTotalLines / nTasks;
            endNum = portion;
            buffer = new char[StartId[endNum] + 1];
            strncpy(buffer, fileBuf, StartId[endNum]);
            buffer[StartId[endNum]] = '\0';
            for(int i = 1; i <= nTasks-1; i++) {
                int curStartNum = portion*i;
                int curEndNum = portion*(i+1)-1;
                if(i+1 == nTasks) {
                    curEndNum = nTotalLines;
                }
                if(curStartNum < 0) {
                    curStartNum = 0;
                }
                int curLength = StartId[curEndNum+1] - StartId[curStartNum];
                MPI_Send(&curLength, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                if(curLength > 0) {
                    MPI_Send(fileBuf+StartId[curStartNum] , curLength, MPI_CHAR, i, 2, MPI_COMM_WORLD);
                }
            }
            printf("Message sent from 0 \n");
            free(fileBuf);
            free(StartId);
        }
        else {
            MPI_Status status;
            MPI_Recv(&totalChars , 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status); // receive data to process
            if(totalChars > 0) {
                buffer = new char[totalChars + 1];
                MPI_Recv(buffer, totalChars, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
                buffer[totalChars] = '\0';
            }
            printf("message received from 0 to process %d \n", rank);
        }

        pairs* words = NULL;
        int mapSize = 0;
        double startTime_noDist = MPI_Wtime();
        map<char, int> HashMap;

        if((buffer != NULL)) {
            char* word = strtok(buffer, " \r\n\t");
            while (!(word == NULL))
            {
                char NewCharacter;
                for(int j = 0; j < strlen(word); j++) {
                    NewCharacter = word[j];
                    if(HashMap.find(NewCharacter) != HashMap.end()) {
                        HashMap[NewCharacter]++;
                    } else {
                        HashMap[NewCharacter] = 1;
                    }
                }
                word = strtok(NULL, " \r\n\t");
            }
            free(buffer);
            
            mapSize = HashMap.size();

            if(mapSize > 0) {
                words = (pairs*)malloc(mapSize*sizeof(pairs));
                int i = 0;
                for(map<char, int>::iterator it = HashMap.begin(); it != HashMap.end(); it++) {
                    words[i].character = (it->first);
                    words[i].count = it->second;
                    i++;
                }
            }
        }

        printf("HashMap ready in process %d\n", rank);


        if(rank == 0) {
            for(int i = 1; i < nTasks; i++) {
                int leng;
                MPI_Status status;
                MPI_Recv(&leng, 1, MPI_INT, i, 3, MPI_COMM_WORLD, &status);

                if(leng > 0) {
                    pairs* local_words = (pairs*)malloc(leng*sizeof(pairs));
                    MPI_Recv(local_words, leng, obj_type, i, 4, MPI_COMM_WORLD, &status);

                    for(int j = 0; j < leng; j++) {
                        if(totalHashMap.find(local_words[j].character) != totalHashMap.end()) {
                            totalHashMap[local_words[j].character] += local_words[j].count;
                        }
                        else {
                            totalHashMap[local_words[j].character] = local_words[j].count;
                        }
                    }
                    free(local_words);
                }
            }
            printf("HashMap ready in 0 \n");

            for(map<char, int>::iterator it = HashMap.begin(); it != HashMap.end(); it++) {
                totalHashMap[it->first] += it->second;
                if(totalHashMap[it->first] > max_count) {
                    max_count = totalHashMap[it->first];
                }
                if(totalHashMap[it->first] < min_count) {
                    min_count = totalHashMap[it->first];
                }
            }
            printf("final HashMap ready in 0");
        }
        else {
            MPI_Send(&mapSize, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            if(mapSize > 0) {
                MPI_Send(words, mapSize, obj_type, 0, 4, MPI_COMM_WORLD);
            }
        }
        HashMap.clear();
        if(words != NULL && mapSize > 0) {
            delete []words;
        }
        totalTime_noRead += (MPI_Wtime() - startTime_noRead);
        totalTime_NoDist += (MPI_Wtime() - startTime_noDist);
    }

    //print out result.
    if(rank == 0) {
        fclose(file);
        double t = MPI_Wtime();

        for(map<char, int>::iterator it = totalHashMap.begin(); it != totalHashMap.end(); it++) {
            printf(" character %c:%d\n", (it->first), it->second);
        }

        for(map<char, int>::iterator it = totalHashMap.begin(); it != totalHashMap.end(); it++) {
            if(it->second == min_count) {
                printf(" Min character count %c:%d\n", (it->first), it->second);
            }
        }
    
        for(map<char, int>::iterator it = totalHashMap.begin(); it != totalHashMap.end(); it++) {
            if(it->second == max_count) {
                printf(" Min character count %c: %d\n", (it->first), it->second);
            }
        }

        double endTime = MPI::Wtime();
        cout << "Hey there Time taken is: " << totalTime_NoDist + endTime - t << " seconds" << endl;
        totalHashMap.clear();

    }

    MPI_Finalize();
    return 0;

}

