
 #include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <sstream>

using namespace std;

typedef unsigned char byte;
byte RAM[0x10000];    //********************

ofstream output;

class CacheAddress
{
private:
    unsigned short value, offset, set, tag, dirty;
    
public:
    unsigned short get () {return value;}
    unsigned short getTag () {return tag;}
    unsigned short getOffset () {return offset;}
    unsigned short getBlock () {return value & ~7;}
    
    //void setDirty() { dirty = 1; }
    //void clearDirty() { dirty = 0; }
    
    unsigned short getLine () {return set;}
    
    
    CacheAddress (int tag, int set)
    {
        this->tag = tag;
        this->offset = 0;
        this->set = set;
        this->value = (tag << 7) | (set << 3);
    }
    
 
    CacheAddress (unsigned short value)
    {
        this->value = value;
        tag = (value >> 7);
        set = (value >> 3) & 15;
        offset = value & 7;
        
        //cout << "Tag:" << tag << " Set:" << set << " Offset:" << offset << endl << endl;
    }
};


namespace Ops
{
    void dump();  //***********************
    

    class Cache_Line
    {
    public:
        unsigned int  counter;
        byte tag;  //*************** int to char
        
        byte read (int offset) { return value[offset];} //********** int to char
        byte value[8];  //**********char and int

        bool dirty;
        
        //********************************add doan nay vao
            void output(int index, ostream &out)
		{
			out << uppercase << hex << ' ' << setfill(' ') << setw(3) << (int) index << " tag " << setfill('0') << setw(3) << (int) tag << ' ';
			
            for (int i = 7; i >= 0; i--)
				out << setw(2) << uppercase << hex << (int) value[i];
            
            out << ' ' << setw(1) << dirty
            << ", LRU counter " << dec << counter << hex
            << endl;
            
            out <<endl;
		}
        //*************************************
        
    };
    
    
    Cache_Line CACHE[64];
    
    Cache_Line * getLine(CacheAddress &address, bool Read)
    {
        Cache_Line *pos_line = NULL;
        Cache_Line *pos_set = &CACHE[address.getLine()];
        Cache_Line *LRU  = pos_set;

        for (int i = 0; i < 4; i++)
        {
            pos_set[i].counter ++; //count increament for each line

            if (pos_set[i].tag == address.getTag())
            {
                pos_line = &pos_set[i];
            }

            if (pos_set[i].counter > LRU->counter) //LRU line
            {
                LRU = &pos_set[i];
            }
        }
        // output for READ
       if (Read)
            output << uppercase << hex << setw(4) << (int) address.get() << ' ';
        
        int isHit; //***************
        isHit = pos_line ? 1 : 0;
        
        if (!isHit)
            pos_line = LRU;
        
        int getDirty; //***************
        getDirty = pos_line->dirty ? 1 : 0;
        pos_line->counter = 0;

        if (!isHit)
        {

            if (getDirty) {
                CacheAddress writebackAddress(pos_line->tag, address.getLine());
                for (int i = 0; i < 8; i++)
                    RAM[i + writebackAddress.get()] = pos_line->value[i];
                pos_line->dirty = false;
            }
            
            pos_line->tag = address.getTag();
            for (int i = 0; i < 8; i++)
                pos_line->value[i] = RAM[address.getBlock() + i];
        }

        if (Read)
        {
            for (int i = 7; i >= 0; i--)
                output << setfill('0') << setw(2) << hex << (int) pos_line->value[i];

            output << ' ' << setw(1) << (int) isHit << ' ' << setw(1) << (int) getDirty;
            output << endl; //***********
        }
        return pos_line;
    }
   
    byte read (CacheAddress &address)  //********** int to char
    {
        return getLine(address, true)->read(address.getOffset());
    }
    
    void write (CacheAddress &address, byte value) // ********* in value to char
    {
        Cache_Line * pos_line = getLine(address, false);
        pos_line->tag = address.getTag();
        pos_line->dirty = true;
        pos_line->value[address.getOffset()] = value;
    }
    
    //***************************
    void dump ()
	{
		for (int j = 0; j < 64; j++)
				CACHE[j].output(j, cout);
	}

	void dump (int lines)
	{
		for (int j = 0; j < lines; j++)
				CACHE[j].output(j, cout);
	}
      ///********************************
    
}

int main (int argc, char* args[])
{
    if (argc < 2)
    {
        cout << "Usage:\t" << args[0] << " <filename>" << endl;
        return 1;
    }
    
    unsigned short opwrite, data, pos_address;
    
    int count =0; //**********************
    

    //int count = 0;
    
    ifstream file(args[1]);  //*************
 
    
    if (file)
    {
        output.open("sa-out.txt");
   if (output)
        {
            while (!file.eof())
            {
                //read from file
                file >> hex >> pos_address>> hex >> opwrite >> hex >> data;
                
                count ++;  //*******************************
                
                CacheAddress address(pos_address);

                if (opwrite)
                    Ops::write(address, data);

                else//if (!opwrite)
                    Ops::read(address);
           }
            output.close();
        }

        file.close();
    }
}
