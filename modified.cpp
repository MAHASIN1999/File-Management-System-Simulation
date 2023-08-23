#include <bits/stdc++.h>
#include <unistd.h>
#include <string.h>
#include <chrono>

using namespace std;

#define MAX_FILES 50 // the maximum number of files that can be alllocated
// #define MAX_FILE_SIZE 1000
#define total_blocks 200 // the number of bolocks present in memory

typedef struct // data structure for files
{
    int file_id;
    string name;
    int start_index;
    int specified_size; // the no. of blocks of the file that has to allocated contiguously
    int size;
} File;

typedef struct BlockNode
{
    int loc;
    char ch;
    int free; // 0 if free, 1 if allocated
    struct BlockNode *next;
} Block;

// int memory[MAX_FILE_SIZE];
// int free_blocks[total_blocks]={0};   //0 if free, 1 if allocated

Block blocks[total_blocks];
File file_table[MAX_FILES];
int num_files = 0;
// int next_free_index = 0;

int create_file(string name, string str, int specified_size)
{
    static int file_counter = 0;
    if (num_files >= MAX_FILES)
    {
        printf("Error: Maximum number of files reached\n");
        return 0;
    }

    for (int i = 0; i < num_files; i++)
    {
        if (file_table[i].name == name) // file is being searched by name
        {
            cout << "This file is already allocated to the memory" << endl;
            return 0;
        }
    }

    int size = str.length();
    int next_free_index = 0;
    bool flag = false;

    for (int i = 0; i < total_blocks - specified_size; i++)
    {
        if (blocks[i].free == 0)
        {
            flag = true;
            int j;
            for (j = i; j < specified_size; j++)
            {
                if (blocks[j].free == 1)
                {
                    flag = false;
                    break;
                }
            }

            if (flag == true)
            {
                next_free_index = i;
                break;
            }

            else
                i = j;
        }
    }

    int free_blocks[size];
    int free_blocks_count = 0;
    for (int i = 0; i < specified_size; i++) // first specified size of contiguous memory block are being allocated
    {
        free_blocks[free_blocks_count++] = blocks[next_free_index + i].loc;
    }

    if (flag == true && size > specified_size) // if specified size of contiguous memory block has been found
    {

        flag = false;
        for (int i = 0; i < total_blocks; i++) // this loop finds  memory location where the given file can be accomodated
        {
            if (i == next_free_index)
            {
                i += specified_size - 1;
                continue;
            }

            if (blocks[i].free == 0)
            {
                // cout<<free_blocks_count<<"  "<<size<<endl;
                free_blocks[free_blocks_count++] = i;
            }

            if (free_blocks_count == size)
            {
                flag = true;
                break;
            }
        }
    }

    if (flag == false) // if no such memory location could be found where the file can be accomodated
    {
        cout << "not enough memory to allocate " << name << " file\n";
        return 0;
    }

    int start_index = free_blocks[0];
    // int start_index=next_free_index;
    File file;
    file.file_id = file_counter++;
    file.name = name;
    file.start_index = start_index;
    file.size = size;
    file.specified_size = specified_size;

    blocks[free_blocks[0]].free = 1;
    blocks[free_blocks[0]].ch = str[0];

    for (int i = 1; i < file.size; i++)
    {
        blocks[free_blocks[i - 1]].next = &(blocks[free_blocks[i]]);
        blocks[free_blocks[i]].free = 1;
        blocks[free_blocks[i]].ch = str[i];
    }

    blocks[free_blocks[file.size - 1]].next = NULL;

    file_table[num_files] = file;
    num_files++;

    cout << "file " << name << " (file_id: " << file_table[num_files - 1].file_id << ") has been created\n";
    // sleep(1);
    return 1;
}

void read_file(string name) // reads the given file content
{
    int i;
    for (i = 0; i < num_files; i++)
    {
        if (file_table[i].name == name)
        {
            cout << "file: " << file_table[i].name << " (file_id: " << file_table[i].file_id << ") specified_size: " << file_table[i].specified_size << "\nstart_index: " << file_table[i].start_index << "  size: " << file_table[i].size << endl;
            int j;
            Block *temp = &(blocks[file_table[i].start_index]);
            // int count = file_table[i].start_index;

            while (temp)
            {
                cout << "b" << temp->loc << "[" << temp->ch << "] ";
                temp = temp->next;
            }

            printf("\n");
            // sleep(1);
            return;
        }
    }
    printf("Error: File not found\n");
}

void update_file(string name, string data)
{
    int i;
    for (i = 0; i < num_files; i++)
    {
        if (file_table[i].name == name)
        {

            if (data.length() > file_table[i].size)
            {
                printf("Error: File size limit exceeded\n");
                return;
            }

            /*int j;
            for (j = 0; j < file_table[i].size; j++)
            {

                if (j < data.length())
                    blocks[file_table[i].start_index + j].ch = data[j];
                else
                    blocks[file_table[i].start_index + j].ch = '\0';
            }*/
            Block *temp = &(blocks[file_table[i].start_index]);
            int count = 0;
            while (temp)
            {
                if (count < data.length())
                    temp->ch = data[count++];
                else
                    temp->ch = '\0';

                temp = temp->next;
            }

            cout << name << " (file_id: " << file_table[i].file_id << ") file has been updated succesfully\n";
            // sleep(1);
            return;
        }
    }
    printf("Error: File not found\n");
}

int delete_file(string name)
{
    int i;
    for (i = 0; i < num_files; i++)
    {
        if (file_table[i].name == name)
        {
            cout << name << " (file_id: " << file_table[i].file_id << ") file has been deleted succesfully\n";

            int j;
            Block *temp = &(blocks[file_table[i].start_index]);
            Block *temp1;
            while (temp)
            {
                temp1 = temp;
                temp->ch = '\0';
                temp->free = 0;

                temp = temp->next;
                temp1->next = NULL;
            }

            free(&(file_table[i])); // free the file structure

            for (; i < num_files - 1; i++)
            {
                file_table[i] = file_table[i + 1];
            }
            num_files--;

            // sleep(1);
            return 1;
        }
    }
    printf("Error: File not found\n");
    return 0;
}

int main()
{
    srand(time(NULL));

    for (int i = 0; i < total_blocks; i++)
    {
        blocks[i].loc = i;
        blocks[i].free = 0;
        blocks[i].ch = '\0';
        blocks[i].next = NULL;
    }

    int n;
    int allocated_files = 0; // contains the allocated files to the memory

    cout << "how many files do you want to create?: ";
    cin >> n;

    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now(); // t1 stores the current time of the system
    for (int i = 0; i < n; i++)
    {
        string name, str;
        cout << "enter the name of the file" << i + 1 << ": ";
        cin >> name;

        cout << "enter the content of the file" << i + 1 << ": ";
        cin >> str;

        /* for (int j = 0; j < 5; j++) // file name will always be of size=%+4
             name += char(rand() % 26 + 97);
         name = name + ".txt";

         for (int j = 0; j < (rand() % 30 + 1); j++) // the file content will be of size<=20
             str += char(rand() % 26 + 97);*/

        int size = str.length();
        int specified_size = rand() % size + 1;
        int check = create_file(name, str, specified_size);
        if (check == 1) // if file was created succesfully, then it adds 1 to this variable
            allocated_files++;

        // cout<<endl;
    }
    cout << endl
         << endl;
    for (int i = 0; i < num_files; i++)
    {
        read_file(file_table[i].name); // file is being read
        cout << endl;
    }

    for (int i = 0; i < 5; i++) // here some files are being deleted
    {
        // int x=rand()%n+1;
        int x;
        cin >> x;
        int chk = delete_file(file_table[x].name);
        if (chk == 1)
            allocated_files--;
    }

    cout << endl;

    cout << endl
         << endl
         << "------------------------------------------------------blocks------------------------------------------------\n";
    for (int i = 0; i < total_blocks; i++)
    {
        cout << "b" << i << "[" << blocks[i].ch << "] ";
        if ((i + 1) % 10 == 0)
            cout << endl;
    }

    cout << endl
         << endl;
    // update_file(file_table[1].name,"mahas");

    for (int i = 0; i < 6; i++)
    {
        string name, str;
        cout << "enter the name of the file" << i + 1 << ": ";
        cin >> name;
        cout << "enter the content of the file" << i + 1 << ": ";
        cin >> str;
        /* for (int j = 0; j < 5; j++) // file name will always be of size=%+4
             name += char(rand() % 26 + 97);
         name = name + ".txt";

         for (int j = 0; j < (rand() % 20 + 11); j++) // the file content will be of size<=11
             str += char(rand() % 26 + 97);*/

        int size = str.length();
        int specified_size = rand() % size + 1;
        int check = create_file(name, str, specified_size);
        if (check == 1)
            allocated_files++;

        // cout<<endl;
    }

    cout << endl;
    for (int i = 0; i < num_files; i++)
    {
        read_file(file_table[i].name);
        cout << endl;
    }

    /*cout<<"which file do you want to update (enter index no.):";
    int index;
    cin>>index;
    cout<<"enter the string that you want the file to update with: ";
    string data;
    cin>>data;
    update_file(file_table[index].name,data);*/

    cout << endl
         << endl
         << "------------------------------------------------------blocks--------------------------------------------------\n";
    for (int i = 0; i < total_blocks; i++)
    {
        cout << "b" << i << "[" << blocks[i].ch << "] ";
        if ((i + 1) % 10 == 0)
            cout << endl;
    }

    cout << endl;
    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now(); // t2 stores the current time of the system

    // fclose(fp);

    int free_blocks = 0;
    for (int i = 0; i < total_blocks; i++)
        if (blocks[i].free == 0)
            free_blocks++;

    long long duration = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
    cout << "no. of total allocated files: " << allocated_files << "\nno. of total free memory blocks after allocation: " << free_blocks << endl;
    cout << "total time duration of all the operations:" << duration << " micro seconds" << endl;

    return 0;
}