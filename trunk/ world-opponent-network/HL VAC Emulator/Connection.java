import java.io.PrintStream;
import java.net.*;

class Connection extends Thread
    implements Runnable
{

    Connection(vacemu vacemu1, DatagramPacket datagrampacket)
    {
        receivedPacket = null;
        Parent = vacemu1;
        Address = datagrampacket.getAddress();
        Port = datagrampacket.getPort();
        P = datagrampacket;
    }

    void send(DatagramPacket datagrampacket)
        throws Exception
    {
        Parent.S.send(datagrampacket);
    }

    DatagramPacket receive()
        throws Exception
    {
        try
        {
            Thread.sleep(50000L);
            throw new Exception("Receive timed out");
        }
        catch(InterruptedException interruptedexception)
        {
            return receivedPacket;
        }
    }

    void packet(DatagramPacket datagrampacket)
    {
        receivedPacket = datagrampacket;
        interrupt();
    }

    public void run()
    {
        try
        {
            Req req = new Req(P);
            if(!"CHALLENGE".equals(req.Cmd))
            {
                ReqCheckAccess reqcheckaccess = new ReqCheckAccess(P);
                if(((Req) (reqcheckaccess)).legal)
                {
                    System.out.println("                  'Check access'");
                    byte abyte0[] = vacemu.Cmd_AccessGranted(reqcheckaccess.Id, (byte)1);
                    send(new DatagramPacket(abyte0, abyte0.length, P.getAddress(), P.getPort()));
                    Parent.closeConnection(this);
                    return;
                } else
                {
                    byte abyte1[] = vacemu.Cmd_Abort("error");
                    send(new DatagramPacket(abyte1, abyte1.length, P.getAddress(), P.getPort()));
                    throw new Exception("Protocol violation");
                }
            }
            byte abyte2[] = vacemu.Cmd_Accept(Module.Id);
            send(new DatagramPacket(abyte2, abyte2.length, P.getAddress(), P.getPort()));
            abyte2 = new byte[256];
            P = receive();
            ReqGet reqget = new ReqGet(P);
            if(!((Req) (reqget)).legal)
            {
                System.out.println("Invalid GET request from " + P.getAddress() + " :");
                vacemu.Print(P);
                throw new Exception("Protocol violation");
            }
            System.out.print("                  Request for file " + reqget.FileName);
            if(vacemu.TestModuleDir.equals(reqget.TestDir))
                System.out.print(" [beta]");
            System.out.println();
            if(Module.Id != reqget.Id)
            {
                abyte2 = vacemu.Cmd_Abort("Challenge error");
                send(new DatagramPacket(abyte2, abyte2.length, P.getAddress(), P.getPort()));
                throw new Exception("Module order error: request id=" + reqget.Id + " while expecting id=" + Module.Id);
            }
            if(!Character.isLetterOrDigit(reqget.FileName.charAt(0)))
            {
                System.out.println("                ! Attempt to get file " + reqget.FileName);
                Parent.closeConnection(this);
                return;
            }
            Module module = null;
            try
            {
                module = new Module(reqget.FileName);
            }
            catch(Exception exception1)
            {
                abyte2 = vacemu.Cmd_Abort("file not found");
                send(new DatagramPacket(abyte2, abyte2.length, P.getAddress(), P.getPort()));
                System.out.println("                ! File not found: " + reqget.FileName);
                Parent.closeConnection(this);
                return;
            }
            abyte2 = vacemu.Cmd_File(module.Size, module.Header);
            send(new DatagramPacket(abyte2, abyte2.length, P.getAddress(), P.getPort()));
            do
            {
                P = receive();
                ReqNext reqnext = new ReqNext(P);
                if("ABORT".equals(((Req) (reqnext)).Cmd))
                {
                    System.out.println("                  Connection closed");
                    Parent.closeConnection(this);
                    return;
                }
                if(!((Req) (reqnext)).legal || reqnext.Pos < 0 || reqnext.Pos > module.Size)
                {
                    byte abyte3[] = vacemu.Cmd_Abort("error");
                    send(new DatagramPacket(abyte3, abyte3.length, P.getAddress(), P.getPort()));
                    throw new Exception("Protocol violation: " + ((Req) (reqnext)).Cmd);
                }
                if(reqnext.Pos == module.Size)
                {
                    byte abyte4[] = vacemu.Cmd_Finish();
                    send(new DatagramPacket(abyte4, abyte4.length, P.getAddress(), P.getPort()));
                    break;
                }
                int i = module.Size - reqnext.Pos;
                if(i > 1024)
                    i = 1024;
                byte abyte6[] = new byte[i];
                System.arraycopy(module.Data, reqnext.Pos, abyte6, 0, i);
                byte abyte5[] = vacemu.Cmd_Block(abyte6);
                send(new DatagramPacket(abyte5, abyte5.length, P.getAddress(), P.getPort()));
            } while(true);
            System.out.println("                  File " + module.Name + " transfered");
            Parent.closeConnection(this);
        }
        catch(Exception exception)
        {
            exception.printStackTrace();
            Parent.closeConnection(this);
            return;
        }
    }

    vacemu Parent;
    InetAddress Address;
    int Port;
    DatagramPacket receivedPacket;
    DatagramPacket P;
}
