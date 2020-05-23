using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;

namespace VFdotnet
{
    public class Vision
    {
        private static int onShow = 0;
        private string fileConfig;
        private string resultTemporary;

        [DllImport("VisionWidgets.dll", EntryPoint = "myDllEntry", CallingConvention = CallingConvention.Cdecl, CharSet =CharSet.Unicode)]
        private static extern int dllshow();
        public Vision() {}
        public void show()
        {
            if(onShow == 0)
            {
                onShow += 1;
                int ret = dllshow();
                if (ret == 0)
                    onShow -= 1;
            }
        }
        [DllImport("VisionWidgets.dll", EntryPoint = "runCppProcess", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern IntPtr runCppProcess(string filename);

        public void loadConfig(string filename)
        {
            fileConfig = filename;
        }
        public void runProcess()
        {
            resultTemporary += Marshal.PtrToStringUni(runCppProcess(fileConfig));
        }
        public string getResult()
        {
            return resultTemporary;
        }
        public void logResult(string filename)
        {
            File.AppendAllText(filename, resultTemporary, Encoding.Unicode);
            resultTemporary.Remove(0);
        }

        
    }
}
