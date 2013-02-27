import java.io.*;

class Module
{

    Module()
    {
    }

    Module(String s)
        throws Exception
    {
        this(new DataInputStream(new FileInputStream(s)));
    }

    Module(DataInputStream datainputstream)
        throws Exception
    {
        Name = datainputstream.readUTF();
        int i = datainputstream.readInt();
        Header = new byte[i];
        datainputstream.read(Header);
        i = datainputstream.readInt();
        Data = new byte[i];
        Size = i;
        datainputstream.read(Data);
        encode(Data, Id);
    }

    void write()
        throws Exception
    {
        int i = Name.lastIndexOf('/');
        if(i != -1)
        {
            String s = Name.substring(0, i);
            (new File(s)).mkdirs();
        }
        write(new DataOutputStream(new FileOutputStream(Name)));
    }

    void write(DataOutputStream dataoutputstream)
        throws Exception
    {
        dataoutputstream.writeUTF(Name);
        dataoutputstream.writeInt(Header.length);
        dataoutputstream.write(Header);
        dataoutputstream.writeInt(Data.length);
        dataoutputstream.write(Data);
    }

    static void decode(byte abyte0[], int i)
    {
        for(int j = 0; j < abyte0.length; j += 1024)
        {
            int k = abyte0.length - j;
            if(k > 1024)
                k = 1024;
            decode(abyte0, i, j, k);
        }

    }

    static void encode(byte abyte0[], int i)
    {
        for(int j = 0; j < abyte0.length; j += 1024)
        {
            int k = abyte0.length - j;
            if(k > 1024)
                k = 1024;
            encode(abyte0, i, j, k);
        }

    }

    static void decode(byte abyte0[], int i, int j, int k)
    {
        int l = ~i;
        int i1 = 0;
        for(int j1 = j; j1 + 3 < j + k; j1 += 4)
        {
            abyte0[j1] ^= i & 0xff;
            abyte0[j1 + 1] ^= i >> 8 & 0xff;
            abyte0[j1 + 2] ^= i >> 16 & 0xff;
            abyte0[j1 + 3] ^= i >> 24 & 0xff;
            abyte0[j1] ^= pattern[i1 + 0 & 0xf] | 0xa5;
            abyte0[j1 + 1] ^= pattern[i1 + 1 & 0xf] | 0xa7;
            abyte0[j1 + 2] ^= pattern[i1 + 2 & 0xf] | 0xad;
            abyte0[j1 + 3] ^= pattern[i1 + 3 & 0xf] | 0xbf;
            byte byte0 = abyte0[j1];
            byte byte1 = abyte0[j1 + 1];
            byte byte2 = abyte0[j1 + 2];
            byte byte3 = abyte0[j1 + 3];
            abyte0[j1] = byte3;
            abyte0[j1 + 1] = byte2;
            abyte0[j1 + 2] = byte1;
            abyte0[j1 + 3] = byte0;
            abyte0[j1] ^= l & 0xff;
            abyte0[j1 + 1] ^= l >> 8 & 0xff;
            abyte0[j1 + 2] ^= l >> 16 & 0xff;
            abyte0[j1 + 3] ^= l >> 24 & 0xff;
            i1++;
        }

    }

    static void encode(byte abyte0[], int i, int j, int k)
    {
        int l = ~i;
        int i1 = 0;
        for(int j1 = j; j1 + 3 < j + k; j1 += 4)
        {
            abyte0[j1] ^= l & 0xff;
            abyte0[j1 + 1] ^= l >> 8 & 0xff;
            abyte0[j1 + 2] ^= l >> 16 & 0xff;
            abyte0[j1 + 3] ^= l >> 24 & 0xff;
            byte byte0 = abyte0[j1];
            byte byte1 = abyte0[j1 + 1];
            byte byte2 = abyte0[j1 + 2];
            byte byte3 = abyte0[j1 + 3];
            abyte0[j1] = byte3;
            abyte0[j1 + 1] = byte2;
            abyte0[j1 + 2] = byte1;
            abyte0[j1 + 3] = byte0;
            abyte0[j1] ^= pattern[i1 + 0 & 0xf] | 0xa5;
            abyte0[j1 + 1] ^= pattern[i1 + 1 & 0xf] | 0xa7;
            abyte0[j1 + 2] ^= pattern[i1 + 2 & 0xf] | 0xad;
            abyte0[j1 + 3] ^= pattern[i1 + 3 & 0xf] | 0xbf;
            abyte0[j1] ^= i & 0xff;
            abyte0[j1 + 1] ^= i >> 8 & 0xff;
            abyte0[j1 + 2] ^= i >> 16 & 0xff;
            abyte0[j1 + 3] ^= i >> 24 & 0xff;
            i1++;
        }

    }

    boolean compareHeader(Module module)
    {
        if(module.Header == null || Header == null || Header.length != module.Header.length || Header.length < 20)
            return false;
        for(int i = 0; i < 20; i++)
            if(Header[i] != module.Header[i])
                return false;

        return true;
    }

    String Name;
    int Size;
    byte Header[];
    byte Data[];
    static int Id = 0x12345678;
    static byte pattern[] = {
        5, 97, 122, -19, 27, -54, 13, -101, 74, -15, 
        100, -57, -75, -114, -33, -96
    };
    static byte pattern2[] = {
        32, 7, 19, 97, 3, 69, 23, 114, 10, 45, 
        72, 12, 74, 18, -87, -75
    };

}
