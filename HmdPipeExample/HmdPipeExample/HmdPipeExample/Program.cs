using System;
using System.IO.Pipes;

namespace HmdPipeExample
{
    class Program
    {
        static void Main(string[] args)
        {
            
            Console.WriteLine("Hello World!");
            NamedPipeServerStream namedPipeServer = new NamedPipeServerStream("HMDPipe");
            
            namedPipeServer.WaitForConnection();
            //namedPipeServer.WriteByte(1);
            Console.WriteLine("Connected");
            
            int pipeNum = 1;

            NamedPipeServerStream[] trackerPipe = new NamedPipeServerStream[pipeNum];

            for(int i = 0; i < pipeNum;i++)
            {
                trackerPipe[i] = new NamedPipeServerStream("TrackPipe" + i);
                trackerPipe[i].WaitForConnection();
                byte[] bytes = System.Text.Encoding.ASCII.GetBytes(pipeNum + " 0\n");
                trackerPipe[i].Write(bytes, 0, bytes.Length);
            }
            
            while (true)
            {
                byte[] buffer = new byte[1024];
                
                int byteFromClient = namedPipeServer.Read(buffer,0,1024);
                var str = System.Text.Encoding.Default.GetString(buffer);
                string[] strVals = str.Split(' ');
                double[] pos = new double[3];
                double[] rot = new double[4];
                for(int i = 0; i < pos.Length;i++)
                {
                    Console.WriteLine(strVals[i]);
                    pos[i] = Convert.ToDouble(strVals[i], System.Globalization.CultureInfo.InvariantCulture);
                }
                for(int i = 0; i < rot.Length;i++)
                {
                    rot[i] = Convert.ToDouble(strVals[i + 3], System.Globalization.CultureInfo.InvariantCulture);
                }
                Console.WriteLine(Convert.ToString(pos[0]) + " " + pos[1] + " " + pos[2]);

                for(int i = 0;i < pipeNum;i++)
                {
                    
                    byte[] bytes = System.Text.Encoding.ASCII.GetBytes("0 0 0 1 0 0 0\n");
                    trackerPipe[i].Write(bytes, 0, bytes.Length);
                    Console.WriteLine("hi");
                }
                
            }
        }
    }
}
