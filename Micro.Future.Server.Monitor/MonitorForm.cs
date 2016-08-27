using Micro.Future.Server.Monitor.Properties;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Resources;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Micro.Future.Server.Monitor
{
    public partial class MonitorForm : Form
    {
        public static string CONFIG_FILE = "system";
        private bool _exiting;
        private MicroFurtureSystemClr _system = MicroFurtureSystemClr.Instance;
        private Task _consoleTask;

        public MonitorForm()
        {
            InitializeComponent();
            Initialize();
        }

        private void Initialize()
        {
            _system.LogCallback.OnMessageRecv += OnMessageRecv;

            var assembly = GetType().Assembly;

            toolStripMenuItemStart.Image = Resources.GLight.ToBitmap();
            toolStripMenuItemStop.Image = Resources.RLight.ToBitmap();
            toolStripMenuItemExit.Image = Resources.Exit.ToBitmap();

            notifyIconStatus.Icon = Resources.RLight;

            _consoleTask = Task.Run(() =>
            {
                string instr;
                for (;;)
                {
                    instr = Console.ReadLine();
                    if (string.Compare(instr, "exit", true) == 0)
                    {
                        Exit();
                        break;
                    }
                    else if (string.Compare(instr, "stop", true) == 0)
                    {
                        StopSystem();
                    }
                    else if (string.Compare(instr, "start", true) == 0)
                    {
                        StartSystem();
                    }
                    else if (string.Compare(instr, "restart", true) == 0)
                    {
                        StopSystem();
                        StartSystem();
                    }
                }
            });
        }

        private void OnMessageRecv(LogSeverityType severity, string message)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new MethodInvoker(() => { AppendText(severity, message); }));
            }
            else
            {
                AppendText(severity, message);
            }
        }

        private void AppendText(LogSeverityType severity, string message)
        {
            richTextBoxLog.AppendText("[" + severity + "] ");
            richTextBoxLog.AppendText(message);
            richTextBoxLog.AppendText(Environment.NewLine);
        }

        private void Form_Closing(object sender, FormClosingEventArgs e)
        {
            if (!_exiting)
            {
                e.Cancel = true;
                Visible = false;
                notifyIconStatus.BalloonTipText = "Log window is hidden.\nDouble click to show it.";
                notifyIconStatus.ShowBalloonTip(10000);
            }
        }

        private void Form_Load(object sender, EventArgs e)
        {
            Task.Run(() =>
            {
                Invoke((MethodInvoker)delegate ()
                {
                    toolStripMenuItemStart.Enabled = false;
                    notifyIconStatus.Icon = Resources.GLight;
                });
                if (_system.Load(CONFIG_FILE) && _system.Start())
                {
                    timer.Start();
                };
            });
        }

        private void notifyIconStatus_DoubleClick(object sender, EventArgs e)
        {
            Visible = true;
            if (WindowState == FormWindowState.Minimized)
                WindowState = FormWindowState.Normal;
            BringToFront();
        }

        public void StartSystem()
        {
            _system.Start();
            Invoke((MethodInvoker)delegate ()
            {
                toolStripMenuItemStart.Enabled = false;
                toolStripMenuItemStop.Enabled = true;
                notifyIconStatus.Icon = Resources.GLight;
            });
        }

        private void toolStripMenuItemStart_Click(object sender, EventArgs e)
        {
            StartSystem();
        }

        public void StopSystem()
        {
            _system.Stop();
            Invoke((MethodInvoker)delegate ()
            {
                toolStripMenuItemStart.Enabled = true;
                toolStripMenuItemStop.Enabled = false;
                notifyIconStatus.Icon = Resources.RLight;
            });
        }

        private void toolStripMenuItemStop_Click(object sender, EventArgs e)
        {
            StopSystem();
        }

        public void Exit()
        {
            _exiting = true;

            _system.Stop();

            Invoke((MethodInvoker)delegate ()
            {
                notifyIconStatus.Visible = false;
            });

            Close();
        }

        private void toolStripMenuItemExit_Click(object sender, EventArgs e)
        {
            Exit();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            timer.Stop();
            this.Close();
        }
    }
}
