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
        }

        private void OnMessageRecv(string message)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new MethodInvoker(() => { AppendText(message); }));
            }
            else
            {
                AppendText(message);
            }
        }

        private void AppendText(string message)
        {
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
            Task.Run(() => {
            if (_system.Load(CONFIG_FILE) && _system.Run())
            {
                toolStripMenuItemStart.Enabled = false;
                notifyIconStatus.Icon = Resources.GLight;
                timer.Start();
                }
            });
        }

        private void notifyIconStatus_DoubleClick(object sender, EventArgs e)
        {
            Visible = true;
            if (WindowState == FormWindowState.Minimized)
                WindowState = FormWindowState.Normal;
            BringToFront();
        }

        private void toolStripMenuItemStart_Click(object sender, EventArgs e)
        {
            _system.Run();
            toolStripMenuItemStart.Enabled = false;
            toolStripMenuItemStop.Enabled = true;
            notifyIconStatus.Icon = Resources.GLight;
        }
        private void toolStripMenuItemStop_Click(object sender, EventArgs e)
        {
            _system.Stop();
            toolStripMenuItemStart.Enabled = true;
            toolStripMenuItemStop.Enabled = false;
            notifyIconStatus.Icon = Resources.RLight;
        }

        private void toolStripMenuItemExit_Click(object sender, EventArgs e)
        {

            _exiting = true;

            if (_system.IsRunning)
            {
                _system.Stop();
            }

            notifyIconStatus.Visible = false;

            Application.Exit();
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            timer.Stop();
            this.Close();
        }
    }
}
