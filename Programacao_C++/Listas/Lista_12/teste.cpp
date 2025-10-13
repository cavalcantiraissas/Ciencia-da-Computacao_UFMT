#include<fstream>
#include<iostream>
#include<cctype>
#include <cstring>
#include<typeinfo>

using namespace std;



int isKeyword(char buffer[]){
    char keywords[22][10] = {"break","case","char","const","continue","default", "switch",
                            "do","double","else","float","for","if","int","long","return","short",
                            "sizeof","struct","void","while","main"};
    int i, flag = 0;
    
    for(i = 0; i < 22; ++i){
        if(strcmp(keywords[i], buffer) == 0)
        {
            flag = 1;
            break;
        }
    }
    
    return flag;
}
int isSymbol_Punct(char word)
{
    int flag = 0;
    char symbols_punct[] = {'<','>','!','+','-','*','/','%','=',';','(',')','{', '}','.'};
    for(int x= 0; x< 15; ++x)
    {
        if(word==symbols_punct[x])
           {
               flag = 1;
               break;
           }
            
    }
    return flag;
}

int main()
{
    char buffer[15],buffer1[15];
    char identifier[30][10];
    char number[30][10];
    memset(&identifier[0], '\0', sizeof(identifier));
    memset(&number[0], '\0', sizeof(number));
    char word;
    ifstream fin("program.txt");
    if(!fin.is_open())
    {
        cout<<"Error while opening the file"<<endl;
    }
    int i,k,j,l=0;
    while (!fin.eof())
    {
        word  = fin.get();

        if(isSymbol_Punct(word)==1)
        {
            cout<<"<"<<word<<", Symbol/Punctuation>"<<endl;
        }
       
        if(isalpha(word))
        {        
            buffer[j++] = word;
            // cout<<"buffer: "<<buffer<<endl;
        }
        else if((word == ' ' || word == '\n' || isSymbol_Punct(word)==1) && (j != 0))
        {
            buffer[j] = '\0';
            j = 0;
                            
            if(isKeyword(buffer) == 1)
                cout<<"<"<<buffer<<", keyword>"<<endl;
            else
                {
                cout<<"<"<<buffer<<", identifier>"<<endl;
                strcpy(identifier[i],buffer);
                i++;
                }
                    
        } 
           
        else if(isdigit(word))
        {
            buffer1[l++] = word;
            cout<<"buffer: "<<buffer1<<endl;
        }
        else if((word == ' ' || word == '\n' || isSymbol_Punct(word)==1) && (l != 0))
        {
            buffer1[l] = '\0';
            l = 0;
            cout<<"<"<<buffer1<<", number>"<<endl;
            // cout << "Type is: "<<typeid(buffer1).name() << endl;
            strcpy(number[k],buffer1);
            k++;
                        
        } 

    }
    cout<<"Identifier Table"<<endl;
    int z=0;
    while(strcmp(identifier[z],"\0")!=0) 
    {       
        cout <<z<<"\t\t"<< identifier[z]<<endl; 
        z++;
        
    }  
    // cout<<"Number Table"<<endl;
    // int y=0;
    // while(strcmp(number[y],"\0")!=0) 
    // {       
    //     cout <<y<<"\t\t"<< number[y]<<endl; 
    //     y++;
        
    // }   

    
}
