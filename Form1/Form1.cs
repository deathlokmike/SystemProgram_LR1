using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;

namespace Form1
{
    public partial class Form1 : Form
    {
        Process Child = null;
        EventWaitHandle EventStart = new EventWaitHandle(false, EventResetMode.AutoReset, "event_start");
        EventWaitHandle EventStop = new EventWaitHandle(false, EventResetMode.AutoReset, "event_stop");
        EventWaitHandle EventConfirm = new EventWaitHandle(false, EventResetMode.AutoReset, "event_confirm");
        EventWaitHandle EventCloseApp = new EventWaitHandle(false, EventResetMode.AutoReset, "event_close");
        public Form1()
        {
            InitializeComponent();
        }

        private bool console()
        {
            if ((Child == null) || (Child.HasExited))
            { 
                listBox1.Items.Clear();
                return false;
            }
            return true;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (console())
            {
                int thread_count = Convert.ToInt32(textBox1.Text);
                for (int j = 0; j < thread_count; j++)
                {
                    EventStart.Set();
                    EventConfirm.WaitOne();
                    listBox1.Items.Add("Поток " + (listBox1.Items.Count + 1).ToString() + " создан");
                }
            }
            else
            {
                Child = Process.Start("Lab1.exe");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (console())
            {   
                int num_last_thread = listBox1.Items.Count;
                EventStop.Set(); // Отправка события в C++
                EventConfirm.WaitOne(); // ожидание события из C++
                if (num_last_thread > 0)
                {
                    listBox1.Items.RemoveAt(num_last_thread - 1); // удаление последней записи из listbox
                }
            }
            else
            {
                Child = null;
                return;
            }
        }

        private void Closing(object sender, FormClosingEventArgs e)
        {
            if (console())
            {
                EventCloseApp.Set();
                EventConfirm.WaitOne();
            }
        }   
    }
}
