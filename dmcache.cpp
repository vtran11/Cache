
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <cstring>

using namespace std;

//#bits in line = 6, #bits in offset =3, #bits in tag = 6+3 = 9

typedef unsigned char byte;
byte RAM[0x10000];

ofstream output;

class Cache
{
private:
    char tag;
    string databit[16];
    
public:
    Cache()
    {
        tag = -1;
        for (int j =0; j < 16; j++)
            databit[j] = "FF";
    }
};


class Cache_Line {
    
private:
    unsigned int tag;			// 8 bits long
    unsigned int dirty;	// 0 or 1 (boolean)
    string data[4];		// 4 bytes (8 hex values)
    
public:
    // Constructor
    Cache_Line() {
        tag = -1;
        dirty = 0;
        for (int i = 0; i < 4; i++) {
            data[i] = "00";
        }
    }
    
    // Getters
    unsigned int getTag() const { return tag; }
    unsigned int isDirty() const { return dirty; }
    string getOffset(int offset) const { return data[offset]; }
    
    // Setters
    void setTag(unsigned int t) { tag = t; }
    void setDirty() { dirty = 1; }
    void clearDirty() { dirty = 0; }
    void setOffset(int offset, string str) { data[offset] = str; }
    
    // Write
    void write(string data, unsigned int offset) {
        setDirty();
        // Replace the data corresponding to the offset
        this->data[offset] = data;
    }
    
    // Print
    void print(int hit_miss, ostream& output, unsigned int offset) {
        output << getOffset(offset) << " " << hit_miss << " " << dirty << endl;
    }
    
};


class CacheAddress
{
    
private:
    unsigned short value, offset, set, tag, dirty;
    
public:
    unsigned short get () {return value;}
    unsigned short getTag () {return tag;}
    unsigned short getOffset () {return offset;}
    unsigned short ramBlock () {return value & ~7;}
    
    void setDirty() { dirty = 1; }
    void clearDirty() { dirty = 0; }
    
    unsigned short getLine () {return set;}
    
    
    CacheAddress (int tag, int set)
    {
        this->tag = tag;
        this->offset = 0;
        this->set = set;
        this->value = (tag << 9) | (set << 3);
    }
    
    CacheAddress (int offset, int dirty, int set)
    {
        dirty = 0;
        this->offset = 0;
        this->set =0;
    }
    
    // Overrode << operator for debugging
    friend ostream& operator << (ostream& output, const Cache_Line& line) {
        output << "Tag:" << line.getTag() << " Dirty:" << line.isDirty()
        << " Data:" << line.getOffset(0) << line.getOffset(1)
        << line.getOffset(2) << line.getOffset(3) << endl;
        return output;
    }
    
    CacheAddress (unsigned short value)
    {
        this->value = value;
        tag = (value >> 9);
        set = (value >> 3) & 63;
        offset = value & 7;
        
        cout << "Tag:" << tag << " Set:" << set << " Offset:" << offset << endl << endl;
    }
};

struct CLine
{
    int bit;
    unsigned short DATA[8];
};


namespace Ops
{
    void dump();
    
    class CacheLine
    {
    
    public:
        
        unsigned int isDirty() const { return dirty; }
        void setDirty() { dirty = 1; }
        void clearDirty() { dirty = 0; }
        
        byte read (int offset)
        {
            return data[offset];
        }
        
        void print(int index, ostream &output)
        {
            output << uppercase << hex << " " << (int) index << " " << (int) tag;
            
            for (int i = 7; i >= 0; i--)
                output << " " << uppercase << hex << (int) data[i] << " " << dirty << endl;
        }
        
        unsigned int counter; //******************
        
        byte data[8]; //Block size = 8
        bool dirty;
        byte tag;
    };
    
    CacheLine CACHE[64];
    
    CacheLine * getLine(CacheAddress &address, bool isRead)
    {
        if (isRead)
            output << uppercase << hex << (int) address.get() << ' ';
        
        CacheLine * p_line = &CACHE[address.getLine()];
        
        int wasDirty       = p_line->dirty ? 1 : 0;
        int isHit          = p_line->tag == address.getTag() ? 1 : 0;

        if (!isHit) // cache miss
        {
            if (p_line->dirty) // write-back
            {
                CacheAddress writebackAddress(p_line->tag, address.getLine());
                for (int i = 0; i < 8; i++)
                    RAM[i + writebackAddress.get()] = p_line->data[i];
                    p_line->dirty = false;

            }
            
            p_line->tag   = address.getTag();
            for (int i = 0; i < 8; i++)
                p_line->data[i] = RAM[address.ramBlock() + i];
        }
        
        if (isRead)
        {
            for (int i = 8-1; i >= 0; i--)
                output << setfill('0') << setw(2) << hex << (int) p_line->data[i];
                output << ' ' << setw(1) << (int) isHit << ' ' << setw(1) << (int) wasDirty;
                output << endl;
        }
       
        return p_line;
    }
    
    byte read (CacheAddress &address)
    {
        return getLine(address, true)->read(address.getOffset());
    }
    
    void write (CacheAddress &address, byte data)
    {
        CacheLine * p_line = getLine(address, false);
        p_line->data[address.getOffset()] = data;
        p_line->tag = address.getTag();
        p_line->dirty = true;
    }
}

void printCache(Cache_Line* cache, const Cache_Line& line)
{
    for (int i = 0; i < 64; i++)
    {
    }
}

int main (int argc, char* args[])
{
    unsigned int pos_address,optimal, value;
    
    int count =0; //***********************
    
    ifstream input(args[1]);
    
    if (!input)
    {
        cout << "The file has problem!" << endl;
        return -1;
    }
    
  
        
    if (input)
    {
        output.open("dm-out.txt");
        
        if (argc < 2)
        {
            cout << "No file to write!!!" << endl;
            return -1;
        }
        
        if (output)
        {
            while (!input.eof())
            {
                input >> hex >> pos_address>> hex >> optimal >> hex >> value;
                
                count ++;  //**********************
                
                CacheAddress address(pos_address);
                
                if (optimal)
                    Ops::write(address, value);
                if (pos_address == 0)
                    cout << hex << pos_address << " "<<endl;
                else if (!optimal)
                    Ops::read(address);
                if (pos_address == 1)
                    cout <<  hex << pos_address << " "<<endl;
            }
            output.close();
        }
        input.close();
    }
}

