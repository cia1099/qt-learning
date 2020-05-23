using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using VFdotnet;

namespace TestForm
{
    public partial class Form1 : Form
    {
        private List<string> configList;
        private Vision vision;
        public Form1()
        {
            InitializeComponent();
            vision = new Vision();
            configList = new List<string>();
            listView1.View = View.Details;
            listView1.LabelEdit = false;
            listView1.FullRowSelect = true;
            listView1.Columns.Add("配置文件",listView1.Width-1);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            vision.show();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.Filter = "CSV File|*.csv";
            saveDialog.Title = "輸出日誌路徑";
            saveDialog.ShowDialog();
            if(saveDialog.FileName != "")
            {
                textBox1.Text = saveDialog.FileName;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            string filename = textBox1.Text;
            if(filename.Length != 0 && filename.Substring(filename.LastIndexOf('.')) == ".csv")
            {
                vision.logResult(filename);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            OpenFileDialog openDialog = new OpenFileDialog();
            openDialog.Filter = "Config File|*.dat";
            openDialog.Title = "載入配置檔";
            openDialog.ShowDialog();
            string filename = openDialog.FileName;
            if(filename.Length!=0 && filename.Substring(filename.LastIndexOf('.')) == ".dat")
            {
                configList.Add(filename);
                string itemName = filename.Substring(filename.LastIndexOf('\\')+1);
                itemName = itemName.Remove(itemName.LastIndexOf('.'));
                var item = new ListViewItem(itemName);
                listView1.Items.Add(itemName);
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            foreach(ListViewItem item in listView1.SelectedItems)
            {
                int idx = item.Index;
                listView1.Items.RemoveAt(idx);
                configList.RemoveAt(idx);
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            int i = 0;
            for(; i != configList.Count; ++i)
            {
                vision.loadConfig(configList[i]);
                vision.runProcess();
            }
            if (i != 0)
                richTextBox1.AppendText(vision.getResult());
        }
    }
}
