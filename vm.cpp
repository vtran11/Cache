
#include <iostream>
#include <string>
#include <bitset>
#include <fstream>

#define RAM_ADDRESS 4
#define LINE_NUM 16

using namespace std;

unsigned int BIT[RAM_ADDRESS];
unsigned int RAM[RAM_ADDRESS];

struct page
{
    int line;
    int position;
    int usebit;
    string address;
    int Ram_full;
    page() {usebit = 0;};
};

int main(int argc, char *argv[])
{
    int i,j;
    int buffer_index = 0;
    int counter = 1;
    int pt = 1;
    int match = 0;
    int position = -1;
    int stop = 0;
    
    page lineBit[LINE_NUM];
    page useBit[RAM_ADDRESS];
    string accept;
    
    ifstream inputFile(argv[1]);
    ofstream output("vm-out.txt");
    
    unsigned int info;
  
    int full;
    
    for(i = 0; i < RAM_ADDRESS; i++)
    {
        RAM[i] = 0;
        BIT[i] = 0;
    }
    
    while(inputFile>>hex>>info)
    {
        position = -1;
        match = 0;
        stop =0;
        
        for(int i = 0; i < LINE_NUM; i++)
        {
            //getline(inputFile, accept);
            lineBit[i].address = accept;
            lineBit[i].usebit = 0;
        }
        
      unsigned int page	= (info & 0xFFFFF000) >> 12;
        
        if(counter == LINE_NUM+1) //first 16 lines
        {
            RAM[0] = page;
            BIT[0] = 1;
            lineBit[counter].usebit = 1;
            output << hex << RAM[0] << "000" << endl;
        }
        
        else if (counter > LINE_NUM+1)
        {
            for(i = 0; i<RAM_ADDRESS;i++)//looking for the same address
            {
                if(RAM[i] == page)
                {
                    match = 1;
                    BIT[i] = 1;
                    
                    if(useBit[i].address.empty())
                        buffer_index = (i + 1)% RAM_ADDRESS;

                    if(pt == RAM_ADDRESS)
                        pt = 0;
                    
                }
            }
            
            if(match == 0)
            {
                while(stop != 1)
                {
                    
                    if(BIT[pt] == 0)
                    {
                        position = pt;
                        pt++;
                        
                        RAM[position] = page;
                        BIT[position] = 1;
                        stop =1;
                    }
                    
                  /* if(lineBit[i].address.empty())
                    {
                        lineBit[i].address = 1;
                        buffer_index = (i + 1)% 4;
 
                    }*/

                    else if(BIT[pt] != 0)
                    {
                        BIT[pt] = 0;
                        pt++;
                        full = 1;
                        
                        for(int i = 0; i < RAM_ADDRESS; i++)
                        {
                            if(BIT[i] == 0)
                            {
                                full = 0;
                                break;
                            }
                        }
                        
                        if(full)
                        {
                            position = -1;//all lines are used
                            stop =1;
                            lineBit[counter].usebit = 0;

                        }
                        
                      
                    }
                    
                   if(useBit[i].address.empty())
                    {
                        lineBit[i].address = 1;
                        buffer_index = (i + 1)% 4;
                    }
 
                    if(pt == RAM_ADDRESS)
                    {
                        pt = 0;
                    }

                    
                }
                
                if(position == -1)//full lines
                {	
                   
                    for(i = 0; i<RAM_ADDRESS; i++)
                    {
                        BIT[i] = 0;
                        useBit[i].usebit =1;
                        lineBit[i].line =1;
                    }
                    
                    RAM[0] = page;
                    BIT[0] = 1;
                    pt = 1;
                }
            }
           
            if (argc< 2)
            {
                cout << "Use bit: " << argv[0]<<endl;
            }
            
            for(j=0 ; j<RAM_ADDRESS; j++)
            {
                if(RAM[j] != 0)
                {
                    
                    if(counter == LINE_NUM+2)
                    {
                        if(j == 1) {output << hex << RAM[j] << "000";}
                        else if (j != 1) {output << hex << RAM[j] << "000 ";}
                        
                        if(useBit[j].position == 1)
                        {
                            useBit[i].usebit = 0;
                            break;
                        }
                    }
                    
                    else if(counter == LINE_NUM+3)
                    {
                        if(j == 2) {output << hex << RAM[j] << "000";}
                        else if (j !=2) {output << hex << RAM[j] << "000 ";}
                        
                        if(useBit[j].position == 1)
                        {
                            useBit[i].usebit = 0;
                        }
                    }
                    
                    else
                    {
                        if(j == 3) {output << hex << RAM[j] << "000";}
                        else if (j != 3) {output << hex << RAM[j] << "000 ";}
                        
                        if(useBit[j].position == 1)
                        {
                            useBit[i].usebit = 1;
                        }
                    }
                }
                
                if(RAM[j] == 0)
                {
                    if(counter == LINE_NUM+2)
                    {
                        if(j != 1)
                        cout << hex << RAM[j] << "000 ";

                    }
                    else if(counter == LINE_NUM+3)
                    {
                        if(j != 2)
                        cout << hex << RAM[j] << "000 ";

                    }
                    
                    else
                    {
                        if(j != 3)
                        cout << hex << RAM[j] << "000 ";
                    }

                }
            }
            
            output << endl;
        }
        //cout << endl;
        counter++;
    }
    
    inputFile.close();
    output.close();
    
    return 0;
}
